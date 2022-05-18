#!env python3
# Generate M-sequence,
# Interpolate it such that bit rate of m-sequence is fraction of Fs,
# Shift it to some Intermediate Frequency in order to work in less noisy range,
# Fade In and Out with some arbitrary choosen window.

import sys
import os
from os.path import splitext, join

import os
import matplotlib.pyplot as plt
from scipy.io.wavfile import write, read
from scipy.signal import max_len_seq, resample_poly, correlate
from scipy.signal.windows import hamming
from scipy.fft import fft
import numpy as np


if len(sys.argv) <= 1:
    print("Please provide path to the target wav file for probe impulse")
    sys.exit(1)
else:
    print(f'Store result to "{sys.argv[1]}"')
###############################################################################

def spectrum(s, Fs=48000):
    Ftest = fft(s, norm="forward")
    xf = np.linspace(0.0, Fs/2.0, Ftest.shape[-1]//2.0)
    return xf, 20*np.log10(np.abs(Ftest[0:Ftest.shape[-1]//2]))


###############################################################################

Fs = 48000 # SPS

seq_len = 2048 # The length of M-Sequence
bit_rate = Fs / 4 # With what frequency bits change in m-sequence.
f0 = 8000   # Intermediate frequency.
t_long = bit_rate * seq_len # The length of impulse in seconds
nbits = int(round(np.log2(round(seq_len)))) # num of bits in shift-register generating m-sequence.

Nupsample = round(Fs / bit_rate) # Coefficient of interpolation.

###############################################################################

impulse = max_len_seq(nbits)[0].astype(np.float64) - 0.5
# Interpolation
probe_rec = resample_poly(impulse, Nupsample, 1)

# Intermediate freqeuncy.
modulator = np.sin(np.arange(probe_rec.shape[0]) * 2*np.pi / Fs * f0)
probe_rec = probe_rec * modulator
probe_rec = np.pad(probe_rec, (0, 8192 - probe_rec.shape[0]))

# Fade In n Out
win = hamming(256)
nwin_half = win.shape[0] // 2
win_raise = win[:nwin_half]
win_fall = win[nwin_half:]
probe_rec[:nwin_half] *= win_raise
probe_rec[-nwin_half:] *= win_fall

# Make probe signal stereo, but occupy only left channel.
zeros_to_right = np.zeros(probe_rec.shape[0], dtype=np.float32)
output = np.stack((probe_rec, zeros_to_right), axis=1)
write(sys.argv[1], Fs, output.astype(np.float32))

###############################################################################
# Generate header

header_path = splitext(sys.argv[1])[0] + ".h"
with open(header_path, "w", encoding = 'utf-8') as f:
    f.write("// This is a generated file")
    f.write("// Pls refer to make_probe_wav.py")
    f.write("#include <array>\n\n")
    f.write(f"std::array<float, {output.shape[0]}> impulse = " + "{")
    for i in range(output.shape[0]):
        f.write(f"{output[i, 0]}, ")
    f.write("\n};\n")

###############################################################################
# Check

plt.subplot(211)
plt.semilogx(*spectrum(probe_rec)); plt.grid(True);
plt.grid(True, "both")
plt.xlim([10, 20e3])

delta = correlate(probe_rec, probe_rec, "full")

plt.subplot(212)
N = probe_rec.shape[-1]
t = np.linspace(-N/Fs, N/Fs, delta.shape[0])
plt.plot(t, delta)
plt.xlim([-0.005, 0.005])
plt.grid(True)
plt.show()
