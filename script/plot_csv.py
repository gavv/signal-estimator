#!env python3
import argparse
import csv
import os
import pylab
import sys

def load(filename, device, stip_device, direction):
    arr_x = []
    arr_y = []
    with open(filename) as fp:
        reader = csv.reader(fp, dialect='unix')
        for row in reader:
            if row[0] != direction:
                continue
            row = row[1:]
            if device and row[0] != device:
                continue
            if device or stip_device:
                row = row[1:]
            ts, amp = map(float, row[:2])
            if ts == 0:
                continue
            arr_x.append(ts / 1000000)
            arr_y.append(amp)
    arr_x, arr_y = zip(*sorted(zip(arr_x, arr_y)))
    return arr_x, arr_y

parser = argparse.ArgumentParser(description='plot dump(s) in csv format')

parser.add_argument('-d,--device', dest='device', type=str, required=False,
                    help='filter by device name (for multi-input setup)')

parser.add_argument('dump', nargs='+',
                    help='one or two dump files (output and input)')

args = parser.parse_args()

out_dump = args.dump[0]
if len(args.dump) > 1:
    in_dump = args.dump[1]
else:
    in_dump = out_dump
if len(args.dump) > 2:
    print('Too many arguments', file=sys.stderr)
    exit(1)

stip_device = bool(args.device)

out_x, out_y = load(out_dump, None, stip_device, 'o')
in_x, in_y = load(in_dump, args.device, stip_device, 'i')

fig = pylab.figure()
fig.canvas.mpl_connect('close_event', lambda ev: os._exit(0))

pylab.plot(out_x, out_y, '-o', label='output')
pylab.plot(in_x, in_y, '-o', label='input')

pylab.legend()
pylab.show()
