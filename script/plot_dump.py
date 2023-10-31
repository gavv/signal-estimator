#!env python3
import argparse
import csv
import numpy
import os
import pylab
import sys

def load(filename, direction, has_device, filter_device):
    if filter_device:
        print(f'Loading {direction} signal from {filename} for device {filter_device} ...',
              file=sys.stderr)
    else:
        print(f'Loading {direction} signal from {filename} ...',
              file=sys.stderr)
    x_vals = []
    y_vals = []
    with open(filename) as fp:
        for row in csv.reader(fp, dialect='unix'):
            if len(row) < 2:
                continue
            if row[0] != direction[0]:
                continue
            row = row[1:]
            if has_device:
                if filter_device and row[0] != filter_device:
                    continue
                row = row[1:]
            if len(row) < 2:
                continue
            try:
                ts = float(row[0])
                amp = float(row[1])
            except:
                continue
            if ts <= 0:
                continue
            x_vals.append(ts)
            y_vals.append(amp)
    return numpy.array([x_vals, y_vals]).T

parser = argparse.ArgumentParser(description='plot dump in csv format')

parser.add_argument('-d,--device', dest='device', type=str, required=False,
                    help='filter by device name (for multi-input setup)')
parser.add_argument('dump_file', nargs=1, help='csv dump file')

args = parser.parse_args()

has_device = bool(args.device)

out_sig = load(args.dump_file[0], 'output', has_device, None)
in_sig = load(args.dump_file[0], 'input', has_device, args.device)

out_sig[:,0] /= 1000000
in_sig[:,0] /= 1000000

base = min(out_sig[0,0], in_sig[0,0])
out_sig[:,0] -= base
in_sig[:,0] -= base

print('Plotting ...', file=sys.stderr)

fig = pylab.figure()
fig.canvas.mpl_connect('close_event', lambda ev: os._exit(0))

pylab.plot(out_sig[:,0], out_sig[:,1], '-o', label='output signal')
pylab.plot(in_sig[:,0], in_sig[:,1], '-o',
           label=(f'input signal ({args.device})' if has_device else 'input signal'))

pylab.grid()
pylab.xlabel('time, milliseconds')
pylab.ylabel('amplitude')

pylab.legend()
pylab.show()
