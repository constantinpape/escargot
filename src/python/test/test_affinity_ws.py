from escargot import affinity_watershed_2d, affinity_watershed_2d_blockwise

#import vigra
import numpy as np
import time

def test_affinity_ws_2d(data):

    t_high = 0.94
    t_low  = 0.8
    t_size = 40
    t_reg  = 0.12

    ws = affinity_watershed_2d(data, t_high, t_low, t_size, t_reg)

    print "passed"


def time_affinity_ws_2d(data):
    t_high = 0.94
    t_low  = 0.8
    t_size = 40
    t_reg  = 0.12

    times = []

    for rep in xrange(5):
        t_rep = []
        for _ in xrange(100):
            t = time.time()
            affinity_watershed_2d(data, t_high, t_low, t_size, t_reg)
            t_rep.append(time.time() - t )
        times.append(min(t_rep))

    print np.mean(times)



def test_affinity_ws_2d_blockwise(data):
    blockshape = [512,512]

    t_high = 0.94
    t_low  = 0.8
    t_size = 40
    t_reg  = 0.12

    ws = affinity_watershed_2d_blockwise(data, t_high, t_low, t_size, t_reg, blockshape)



def test_and_time_affinity_ws_2d(data):
    pass



if __name__ == '__main__':

    #affgraph = vigra.readHDF5("/home/consti/cremi_wsgraph/cremi_sampleA_affinities_googleV4.h5","data")[:,:,0,:2]
    #np.save("./affinity_graph_2d.npy", affgraph[:,:,0,:2])

    affgraph = np.load("./affinity_graph_2d.npy")

    test_affinity_ws_2d_blockwise(affgraph[:,:512,:])
    #test_affinity_ws_2d(affgraph[:64,:64,:])
    #time_affinity_ws_2d(affgraph[:64,:64,:])
