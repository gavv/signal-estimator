#!env python3
import pylab

def load(filename):
    arr_x = []
    arr_y = []
    with open(filename) as fp:
        for line in fp.readlines():
            x, y = map(float, line.split())
            arr_x.append(float(int(x / 1000000)))
            arr_y.append(abs(y))
    return arr_x, arr_y

wr_x, wr_y = load('writer.txt')
rd_x, rd_y = load('reader.txt')

pylab.plot(wr_x, wr_y, '-o', label='writer')
pylab.plot(rd_x, rd_y, '-o', label='reader')

pylab.legend()
pylab.show()
