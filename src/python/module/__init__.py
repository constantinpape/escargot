from escargot_module import affinity_watershed_2d, affinity_watershed_2d_blockwise


#def affinity_watershed_2d_blockwise(affinities, upper_threshold, lower_threshold, size_threshold, region_threshold, block_shape):
#
#    import numpy as np
#    from concurrent import futures
#    from escargot_module import get_node_weights, get_edge_weights, run_graph_watershed, merge_subblocks#, apply_size_filter
#
#    node_weights = get_node_weights(affinities, lower_threshold, False)
#    affinities   =  get_edge_weights(affinities, upper_threshold)
#
#    # extract the blocking corresponding to the block shape
#
#    s_x = block_shape[0]
#    assert s_x <= affinities.shape[0], str(s_x) + " , " + str(affinities.shape[0])
#    s_y = block_shape[1]
#    assert s_y <= affinities.shape[1], str(s_y) + " , " + str(affinities.shape[1])
#
#    n_x = int( np.ceil( float( affinities.shape[0] ) / s_x ) )
#    n_y = int( np.ceil( float( affinities.shape[1] ) / s_y ) )
#
#    # 1 pixel overlaps
#    o_x = 1
#    o_y = 1
#
#    n_blocks = n_x * n_y
#
#    slicings = []
#    for x in xrange(n_x):
#
#        # X range
#        start_x = x * s_x
#        if start_x != 0:
#            start_x -= o_x
#        end_x = (x + 1) * s_x + o_x
#        if end_x > affinities.shape[0]:
#            end_x = affinities.shape[0]
#
#        for y in xrange(n_y):
#
#            # Y range
#            start_y = y * s_y
#            if start_y != 0:
#                start_y -= o_y
#            end_y = (y + 1) * s_y + o_y
#            if end_y > affinities.shape[1]:
#                end_y = affinities.shape[1]
#
#            slicings.append( np.s_[start_x:end_x,start_y:end_y] )
#
#    #for s in slicings:
#    #    print s
#    #quit()
#
#    # run waterhseds on the subblocks in parallel
#
#    print "Post slicing / pre BlockWs"
#
#    ## serial for debugging
#    #block_ws = []
#    #for s in slicings:
#    #    block_ws.append( run_graph_watershed(affinities[s], node_weights[s], True) )
#
#    # parallel
#    with futures.ThreadPoolExecutor(max_workers=20) as executor:
#        tasks = []
#        for s in slicings:
#            tasks.append( executor.submit( run_graph_watershed, affinities[s], node_weights[s], False ) )
#        block_ws = [task.result() for task in tasks]
#
#    assert len(block_ws) == len(slicings)
#
#    ## debug
#    #for i, seg in enumerate(block_ws):
#    #    print seg.shape
#    #    print seg.max()
#    #    from volumina_viewer import volumina_n_layer
#    #    volumina_n_layer( [affinities[slicings[i]], seg.astype(np.uint32)] )
#    #    quit()
#
#    # TODO apply modified size filter on the subblock labelings
#    # on second thought: why should we, if there is coming a final size filtering anyway ?!
#
#    # merge the subblock results
#    ws = np.zeros_like(affinities, dtype = np.uint32)
#
#    # iterate over all pairs of blocks that have common overlap
#    print "Pre Merge / post BlockWs"
#
#    from itertools import combinations
#    for pair in combinations( xrange( len(slicings) ), 2 ):
#
#        i1, i2 = pair[0], pair[1]
#        s1, s2 = slicings[i1], slicings[i2]
#
#        print  pair
#
#        have_ovlp = merge_subblocks( block_ws[i1], block_ws[i2],
#                # need to give the slicings as list
#                [s1[0].start, s1[0].stop, s1[1].start, s1[1].stop],
#                [s2[0].start, s2[0].stop, s2[1].start, s2[1].stop],
#                ws )
#
#        if have_ovlp:
#            print "Have overlap"
#            print "New number of segments:"
#            print ws.max()
#
#        else:
#            print "No overlap"
#
#
#    print "Post Merge"
#
#    # TODO apply final size filter
#
#    return ws
#
