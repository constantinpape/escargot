// affinity watershed, implementation based on
// http://ieeexplore.ieee.org/xpls/abs_all.jsp?arnumber=4564470&tag=1
// and http://dspace.mit.edu/handle/1721.1/66820

#pragma once

#include <algorithm>
#include <deque>
#include <stdexcept>
#include <map>
#include <utility>
#include <vector>

#include "escargot/marray/marray.hxx"
#include "escargot/tools/ufd.hxx"
#include "escargot/tools/tools.hxx"

// TODO make everything actually graph based, once we have grid graphs

namespace escargot {
namespace algorithm {

typedef std::pair<size_t,size_t> coordinate_type;

// TODO does it make sense to call a view by reference? :
// marray::View & vs marray::Marray &
template<class value_type> void
nodeWeightsFromEdgeWeights(marray::View<value_type> const & edge_weights,
        value_type const upper_threshold, value_type const lower_threshold,
        marray::View<value_type> & out, bool const ignore_border = false) {

    value_type infinity = *std::max_element(edge_weights.begin(), edge_weights.end()) + .1;

    for( size_t x = 0; x < out.shape(0); x++) {
        
        for( size_t y = 0; y < out.shape(1); y++) {
            std::vector<value_type> incoming_weights;

            // TODO make sure that this conforms with the CNN conventions
            if( x > 0 )
                incoming_weights.push_back(edge_weights(x-1,y,0));
            if( x < out.shape(0) - 1)
                incoming_weights.push_back(edge_weights(x,y,0));
            if( y > 0 )
                incoming_weights.push_back(edge_weights(x,y-1,1));
            if( y < out.shape(1) - 1)
                incoming_weights.push_back(edge_weights(x,y,1));

            value_type max_weight = *std::max_element(incoming_weights.begin(), incoming_weights.end() ); 
            out(x,y) = (max_weight > lower_threshold ) ? infinity : max_weight;

            if( ignore_border && (x == 0 || y == 0 || x == out.shape(0) - 1 || y == out.shape(1) -1) )
                out(x,y) = infinity;
        }
    
    }

    for( size_t x = 0; x < out.shape(0); x++) {
        for( size_t y = 0; y < out.shape(1); y++) {
            
            if( edge_weights(x,y,0) > upper_threshold )
                edge_weights(x,y,0) = infinity;
            
            if( edge_weights(x,y,1) > upper_threshold )
                edge_weights(x,y,1) = infinity;
        }
    }
}


template<class value_type, class label_type> inline 
label_type stream(coordinate_type const pixel, 
        marray::View<value_type> const & edge_weights,
        marray::View<value_type> const & node_weights,
        marray::View<label_type> const & labels, 
        std::vector< std::pair<size_t,size_t> > & stream_coordinates ) {
    
    // add pixel to the stream coordinates
    stream_coordinates.push_back(pixel);

    // initialize pixel queue
    std::deque<coordinate_type> queue;
    queue.push_back(pixel);
    
    size_t x_size = edge_weights.shape(0);
    size_t y_size = edge_weights.shape(1);

    while( ! queue.empty() ) {
       
        coordinate_type p = queue.front();
        queue.pop_front(); 
        
        // get neighbors and weights for this pixel
        std::vector< std::pair<size_t, size_t> > coordinates;
        std::vector<value_type> weights;

        // TODO better limit check
        if( p.first < x_size - 1 ) {
            
            coordinates.push_back( coordinate_type(p.first + 1, p.second) );
            weights.push_back( edge_weights(p.first, p.second, 0) );
        }
        
        if( p.first > 0 ) {
            
            coordinates.push_back( coordinate_type(p.first - 1, p.second) );
            weights.push_back( edge_weights(p.first - 1, p.second, 0) );
        }
        
        if( p.second < y_size - 1 ) {
            
            coordinates.push_back( coordinate_type(p.first, p.second + 1) );
            weights.push_back( edge_weights(p.first, p.second, 1) );
        }
        
        if( p.second > 0 ) {
            
            coordinates.push_back( coordinate_type(p.first, p.second -1) );
            weights.push_back( edge_weights(p.first, p.second - 1, 1) );
        }
        
        value_type w_max = node_weights(p.first,p.second); 

        auto it_weights = weights.begin();
        auto it_coordinates = coordinates.begin();

        for(;it_weights != weights.end(); it_weights++, it_coordinates++) {
            
            // only consider a pixel, if it is not in the stream yet and if its weight is equal to the nodes max-weight
            // TODO more robust comparison
            if( std::find(stream_coordinates.begin(), stream_coordinates.end(), *it_coordinates) == stream_coordinates.end() && *it_weights == w_max) {

                // if we hit a labeled pixel, return the stream 
                if( labels(it_coordinates->first,it_coordinates->second) != 0 )
                    return labels(it_coordinates->first,it_coordinates->second);

                // if the node weight of the considered pixel is smaller, we start depth first search from it 
                else if( node_weights(it_coordinates->first,it_coordinates->second) < w_max ) {
                    
                    stream_coordinates.push_back(*it_coordinates);
                    queue.clear();
                    queue.push_back(*it_coordinates);
                }
                else {
                    stream_coordinates.push_back(*it_coordinates);
                    queue.push_back(*it_coordinates);
                }
            }
        }
    }
    
    // return 0, if we have not found a labeled pixel in the stream
    return 0;
}



template<class value_type, class label_type>  void
runGraphWatershed2d(marray::View<value_type> const & edge_weights,
        marray::Marray<value_type> const & node_weights,
        marray::View<label_type> & out, bool const ignore_border = false ) {
    
    label_type next_label = 1;

    // iterate over all pixel
    for( size_t x = 0; x < out.shape(0); x++) {
        for( size_t y = 0; y < out.shape(1); y++) {

            if( ignore_border && (x == 0 || y == 0 || x == out.shape(0) - 1 || y == out.shape(1) -1) )
                continue;
            
            // if the pixel is already labeled, continue
            if( out(x,y) != 0 )
                continue;

            // call stream -> finds the stream belonging to the current label and pixel coordinates belonging to the stream
            std::vector< coordinate_type > stream_coordinates;
            label_type label = stream( coordinate_type(x, y), edge_weights, node_weights, out, stream_coordinates);

            // if stream returns 0, we found a new stream
            if( label == 0 )
                label = next_label++;
            
            // update labels
            for( auto coord : stream_coordinates)
                out(coord.first,coord.second) = label;

        }
    }
}


// TODO proper graph instead
template<class value_type, class label_type> std::map<coordinate_type, value_type>
get_region_weights(marray::View<value_type> const & edge_weights,
        marray::View<label_type> const & labels) {

    // unfortunately unordered map does not work because coordinate_type can not be hashed (would probably be possible, if we implemented a proper hash function)
    //std::unordered_map<coordinate_type, value_type> ret;
    std::map<coordinate_type, value_type> ret;

    size_t size_x = edge_weights.shape(0);
    size_t size_y = edge_weights.shape(1);

    for( size_t x = 0; x < size_x; x++ ) {
        for( size_t y = 0; y < size_y; y++) {
            
            label_type l = labels(x,y);

            if( x != size_x - 1 )
            {
                label_type l_x = labels(x + 1, y);

                if( l_x != l )
                {
                    value_type w_x = edge_weights(x,y,0);
                    coordinate_type edge_x( std::min(l,l_x), std::max(l,l_x) );

                    if( ret.find(edge_x) != ret.end() )
                        ret.insert( std::pair<coordinate_type, value_type>(edge_x, w_x) );
                    
                    else
                        ret[edge_x] = std::max(w_x, ret[edge_x]);
                }
            }
            
            if( y != size_y - 1 ) {
                
                label_type l_y = labels(x, y + 1);

                if( l_y != l ) {
                    value_type w_y = edge_weights(x,y,1);
                    coordinate_type edge_y( std::min(l,l_y), std::max(l,l_y) );
                    
                    if( ret.find(edge_y) != ret.end() )
                        ret.insert( std::pair<coordinate_type, value_type>(edge_y, w_y) );
                    
                    else
                        ret[edge_y] = std::max(w_y, ret[edge_y]);
                }
            }
        }
    }
    return ret;
}


    


template<class value_type, class label_type> void 
apply_size_filter(std::map<coordinate_type, value_type> const & region_weights,
        size_t const size_threshold, value_type const region_threshold,
        marray::View<label_type> & labels ) {
    
    // dump map into a vector and sort it by value
    auto region_weights_vec = std::vector<std::pair<coordinate_type,value_type> >(region_weights.begin(), region_weights.end());

    std::sort( region_weights_vec.begin(), region_weights_vec.end(), &tools::sortByValDecreasing<value_type> );
    
    // + 1 because 0 label is reserved
    size_t n_regions = *std::max_element(labels.begin(), labels.end()) + 1;

    // find sizes of regions
    std::vector<size_t> sizes(n_regions);
    for( size_t x = 0; x < labels.shape(0); x++) {
        for( size_t y = 0; y < labels.shape(1); y++)
            sizes[labels(x,y)]++;
    }
    
    tools::Ufd<label_type> ufd(n_regions);

    // merge regions
    for( auto e_and_w : region_weights_vec ) {
        
        // if we have reached the value threshold, we stop filtering
        if( e_and_w.second < region_threshold )
            break;
        
        coordinate_type edge = e_and_w.first;
        
        label_type s1 = ufd.find(edge.first);
        label_type s2 = ufd.find(edge.second);

        // merge two regions, if at least one of them is below the size threshold
        if( s1 != s2 && (sizes[s1] < size_threshold || sizes[s2] < size_threshold) ) {
            
            size_t size = sizes[s1] + sizes[s2];
            sizes[s1] = 0;
            sizes[s2] = 0;
            ufd.merge(s1,s2);
            sizes[ufd.find(s1)] = size;
        }
    }
    
    std::map<label_type, label_type> new_label_map;
    ufd.representativeLabeling(new_label_map);
    
    // filter out small regions
    label_type next_label = 1;
    for( auto it = new_label_map.begin(); it != new_label_map.end(); ++it) {
        
        if( sizes[it->first] < size_threshold )
            it->second = 0;
        else
            it->second = next_label++;
    }
    
    
    // write the new labels
    for( size_t x = 0; x < labels.shape(0); x++) {
        
        for( size_t y = 0; y < labels.shape(1); y++) {
            labels(x,y) = new_label_map[ ufd.find( labels(x,y) ) ];
        }
    }
}

// toplevel function for the watershed 
// TODO check that shapes match
// FIXME call by reference of edge weights here is a little tricky, because they are changed!
// TODO workaround with using pyview internally...
template<class value_type, class label_type> void 
graphWatershed2d(marray::View<value_type> & edge_weights,
        value_type const upper_threshold, value_type const lower_threshold,
        value_type const size_threshold, value_type const region_threshold, marray::View<label_type> & ret) {

    if( lower_threshold > upper_threshold )
        throw( std::runtime_error("Thresholds inverted!") );

    size_t shape[] = {edge_weights.shape(0), edge_weights.shape(1)};

    marray::Marray<value_type> node_weights( shape, shape + 2);
    
    nodeWeightsFromEdgeWeights<value_type>(edge_weights, upper_threshold, lower_threshold, node_weights);

    runGraphWatershed2d<value_type, label_type>(edge_weights, node_weights, ret);
    
    // TODO call by reference instead of returnvalue
    auto region_weights = get_region_weights(edge_weights, ret);
    
    apply_size_filter(region_weights, size_threshold, region_threshold, ret);

}


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


} // namespace algorithm
} // namespace escargot
