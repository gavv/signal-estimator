#!env python3
import pylab
import sys
import os

fig = pylab.figure()
fig.canvas.mpl_connect('close_event', lambda ev: os._exit(0))

def load(filename):
    arr_x = []
    arr_y = []
    with open(filename) as fp:
        for line in fp.readlines():
            x, y = map(float, line.split()[1:])
            arr_x.append(float(int(x / 1000000)))
            arr_y.append(abs(y))
    return arr_x, arr_y

wr_x, wr_y = load(sys.argv[1])
rd_x, rd_y = load(sys.argv[2])

pylab.plot(wr_x, wr_y, '-o', label='writer')
pylab.plot(rd_x, rd_y, '-o', label='reader')

pylab.legend()
pylab.show()
