#pragma once

#include <boost/iterator/counting_iterator.hpp>

#include "escargot/algorithm/affinity_watershed.hxx"
#include "escargot/tools/threadpool.hxx"
#include "escargot/tools/tools.hxx"


namespace escargot {
namespace algorithm {


template<class value_type> bool
merge_subblocks(marray::View<value_type> & labeling1, marray::View<value_type> & labeling2,
        std::vector<size_t> const & range1, std::vector<size_t> const & range2,
        marray::View<value_type> & global_labeling) {
        
        // check if we have overlap in x
        if( range2[0] == range1[1]- 2 && ( range1[2] == range2[2] && range1[3] == range2[3] )) {

            // get the number of unique segments and initialize ufd
            value_type max1 = *std::max_element(labeling1.begin(), labeling1.end() ) ;
            value_type n_segs = max1 + *std::max_element(labeling2.begin(), labeling2.end() );
            
            tools::Ufd<value_type> ufd(n_segs);

            // add max of block 1 to block 2 to keep ids unique, only for non - zero entries
            for( auto it = labeling1.begin(); it < labeling1.end(); it++)
                *it += (*it != 0) ? max1 : 0;

            // merge along the x overlap
            //leave out the overlapping y coordinates
            size_t y_start = (range1[2] != 0) ? range1[2] + 1 : range1[2];
            size_t y_stop  = (range1[3] != global_labeling.shape(2)-1) ? range1[3]-1 : range1[3];
            
            for( size_t y = 0; y < y_stop - y_start; y++) {

                // get the labels of the overlapping vertices in both blocks
                value_type l1_1 = labeling1(range1[1] - 1, y);
                value_type l1_2 = labeling1(range1[1] - 2, y);
                value_type l2_1 = labeling2(0,y);
                value_type l2_2 = labeling2(1,y);

                // see if these vertices are connected / corresponding edges on or off
                bool e_1 = l1_1 == l1_2;
                bool e_2 = l2_1 == l2_2;

                //  one edge on, one off   -> merge
                
                if( e_1 && !e_2 ) {
                    // edge is connected in labeling1 and not connected in labeling2
                    
                    // 0's are unconnected, hence we don't merge them in the ufd
                    if( l2_1 == 0 )
                        labeling2(0,y) = ufd.find(l1_1);
                    else
                        ufd.merge(l2_1, l1_1);
                    
                    if( l2_2 == 0 )
                        labeling2(1,y) = ufd.find(l1_1);
                    else
                        ufd.merge(l2_2, l2_1);

                }

                else if( !e_1 && e_2 ) {
                    // edge is not connected in s1 and connected in s2
                    
                    // 0's are unconnected, hence we don't merge them in the ufd
                    if( l1_1 == 0 )
                        labeling1(range1[1] - 1,y) = ufd.find(l2_1);
                    else
                        ufd.merge(l1_2, l2_1);
                    
                    if( l1_2 == 0 )
                        labeling1(range1[1] - 2,y) = ufd.find(l2_1);
                    else
                        ufd.merge(l1_2, l2_1);

                }

                // both edges off -> merge
                // TODO in his thesis, Zlateski describes some further merging steps in this case, which I don'y get yet
                else if ( e_1 && e_2)
                    ufd.merge(l1_1, l1_2);

                // both edges on -> do nothing
            }

            // get new labeling and write it back to the volumes
            std::map<value_type, value_type> labels_new;
            ufd.representativeLabeling(labels_new);

            for( size_t x = range1[0]; x < range1[1] - 1; x++ ) {

                for ( size_t y = y_start; y < y_stop; y++ )  {
                    
                    value_type l_new = labels_new[ ufd.find( labeling1(x-range1[0],y-y_start) ) ];
                    global_labeling(x,y) = l_new;
                    labeling1(x-range1[0],y-y_start) = l_new;

                }
            }
            
            for( size_t x = range2[0] + 1; x < range2[1]; x++ ) {

                for ( size_t y = y_start; y < y_stop; y++ )  {

                    value_type l_new = labels_new[ ufd.find( labeling2(x-range2[0],y-y_start) ) ];
                    global_labeling(x,y) = l_new;
                    labeling2(x-range2[0],y-y_start) = l_new;

                }
            }

            std::cout << "Finished Merge" << std::endl;

            return true;

        }


        // check if we have overlap in y
        else if( range2[2] == range1[3]- 2 && ( range1[0] == range2[0] && range1[1] == range2[1] )) {
            throw std::runtime_error("Shouldn't come here right now");

            return true;
        }

        else
            return false;
}

template<class value_type, class label_type> void
graphWatershed2dBlockwise(marray::View<value_type> & edge_weights,
        value_type const upper_threshold, value_type const lower_threshold,
        value_type const size_threshold, value_type const region_threshold, 
        std::vector<size_t> const block_shape, marray::View<label_type> & ret) {
    
    if( lower_threshold > upper_threshold )
        throw( std::runtime_error("Thresholds inverted!") );

    size_t shape[] = {edge_weights.shape(0), edge_weights.shape(1)};

    marray::Marray<value_type> node_weights( shape, shape + 2);
    
    nodeWeightsFromEdgeWeights<value_type>(edge_weights, lower_threshold, node_weights, true);
    thresholdEdgeWeights(edge_weights, upper_threshold);
    
    size_t s_x = block_shape[0];
    //assert s_x <= affinities.shape[0], str(s_x) + " , " + str(affinities.shape[0])
    size_t s_y = block_shape[1];
    //assert s_y <= affinities.shape[1], str(s_y) + " , " + str(affinities.shape[1])

    size_t n_x = static_cast<size_t>( ceil( ( (float) edge_weights.shape(0) / (float) s_x ) )) ;
    size_t n_y = static_cast<size_t>( ceil( ( (float) edge_weights.shape(1) / (float) s_y ) )) ;

    // 1 pixel overlaps
    size_t o_x = 1;
    size_t o_y = 1;

    size_t n_blocks = n_x * n_y;

    std::vector<std::vector<size_t>> slicings;

    size_t start_x, start_y, end_x, end_y;

    for( size_t x = 0; x < n_x; x++ ) {

        // X range
        start_x = x * s_x;
        if( start_x != 0 )
            start_x -= o_x;
        
        end_x = (x + 1) * s_x + o_x;
        
        if( end_x > edge_weights.shape(0) )
            end_x = edge_weights.shape(0);

        for( size_t y = 0; y < n_y; y++ ) {

            // Y range
            start_y = y * s_y;
            
            if( start_y != 0 )
                start_y -= o_y;

            end_y = (y + 1) * s_y + o_y;
            
            if( end_y > edge_weights.shape(1) )
                end_y = edge_weights.shape(1);

            slicings.push_back( std::vector<size_t>{ start_x, end_x, start_y, end_y} );
        }
    }

    // run waterhseds on the subblocks in parallel

    // initialize the subblock labelings
    std::vector<marray::Marray<label_type>> sub_labelings;
    for( auto s : slicings ) {
        size_t shape[] = {s[1] - s[0], s[3] - s[2]};
        sub_labelings.push_back(marray::Marray<label_type>( shape, shape + 2, 0) );
    }

    // serial for debugging
    //for( size_t i = 0; i < slicings.size(); i++) {
    //    auto s = slicings[i];
    //    // create subviews to edge weights and node weights
    //    size_t sub_base[]  = {s[0], s[2]};
    //    size_t sub_shape[] = {s[1] - s[0], s[3] - s[2]};
    //    // TODO does this work or do we have to take care of the extra channel axis?
    //    marray::View<value_type> sub_weights = edge_weights.view(sub_base, sub_shape);
    //    marray::View<value_type> sub_nodes = node_weights.view(sub_base, sub_shape);
    //    // run watershed and write result to corresponding labeling
    //    runGraphWatershed2d<value_type, label_type>(sub_weights, sub_nodes, sub_labelings[i], true);
    //}

    // parallel
    tools::parallel_foreach( -1, slicings.size(),
        [&](size_t thread_id, int id) {
            
            auto s = slicings[id];

            // create subviews to edge weights and node weights
            size_t sub_base[]  = {s[0], s[2]};
            size_t sub_shape[] = {s[1] - s[0], s[3] - s[2]};
            // TODO does this work or do we have to take care of the extra channel axis?
            marray::View<value_type> sub_weights = edge_weights.view(sub_base, sub_shape);
            marray::View<value_type> sub_nodes = node_weights.view(sub_base, sub_shape);
            // run watershed and write result to corresponding labeling
            runGraphWatershed2d<value_type, label_type>(sub_weights, sub_nodes, sub_labelings[id], true);
            
        } );

    // TODO check whether the output makes any sense here!


    // TODO apply modified size filter on the subblock labelings
    // on second thought: why should we, if there is a final size filtering anyway ?!

    // generate all the 2 combinations of pairs we need
    
    auto pairs = tools::get_combinations(boost::counting_iterator<size_t>(0),
            boost::counting_iterator<size_t>(slicings.size()), 2);
    
    for( auto p : pairs  ) {
        size_t i1 = p[0], i2 = p[1];
        auto s1 = slicings[i1], s2 = slicings[i2];
        
        bool have_ovlp = merge_subblocks( sub_labelings[i1], sub_labelings[i2], s1, s2, ret );

        std::cout << i1 << " " << i2 << std::endl;
        if( have_ovlp ) {
            std::cout <<  "Have overlap" << std::endl;
            std::cout <<  "New number of segments:" << std::endl;
            std::cout <<  *std::max_element(ret.begin(), ret.end()) << std::endl;
        }

        else
            std::cout << "No overlap" << std::endl;
    }

    //# TODO apply final size filter

}


} // namespace algorithm
} // namespace escargot
