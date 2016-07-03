from escargot_module import affinity_watershed_2d

def affinity_watershed_2d_blockwise(affinities, upper_threshold, lower_threshold, size_threshold, region_threshold, block_shape):

    import numpy as np
    from concurrent import futures
    from escargot_module import get_node_weights, run_graph_watershed, apply_size_filter

    node_weights = get_node_weights(affinities, upper_threshold, lower_threshold)

    s_x = block_shape[0]
    assert s_x < affinities.shape[0], str(s_x) + " , " + str(affinities.shape[0])
    s_y = block_shape[1]
    assert s_y < affinities.shape[1], str(s_y) + " , " + str(affinities.shape[1])

    n_x = int( np.ceil( float( affinities.shape[0] ) / s_x ) )
    n_y = int( np.ceil( float( affinities.shape[1] ) / s_y ) )

    # 1 pixel overlaps
    o_x = 1
    o_y = 1

    n_blocks = n_x * n_y

    slicings = []
    for x in xrange(n_x):

        # X range
        start_x = x * s_x
        if x != 0:
            start_x -= o_x
        end_x = (x + 1) * s_x + o_x
        if end_x > affinities.shape[0]:
            end_x = affinities.shape[0]

        for y in xrange(n_y):

            # Y range
            start_y = y * s_y
            if y != 0:
                start_y -= o_y
            end_y = (y + 1) * s_y + o_y
            if end_y > affinities.shape[1]:
                end_y = affinities.shape[1]

             slicings.append( np.s_[start_x:end_x,start_y:end_y] )

    with futures.ThreadPoolExecutor(max_workers=20) as executor:
        tasks = []
        for s in slicings:
            tasks.append( executor.submit( run_graph_watershed, affinities[s], node_weights[s] )
        block_ws = [task.result() for task in tasks]

    # TODO merging and size filter
