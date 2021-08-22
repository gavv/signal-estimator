Signal Estimator
================

[![Build](https://github.com/gavv/signal-estimator/workflows/build/badge.svg)](https://github.com/gavv/signal-estimator/actions)

<!-- toc -->

- [Features](#features)
- [Example use cases](#example-use-cases)
- [Supported platforms](#supported-platforms)
- [Dependencies](#dependencies)
- [Installation](#installation)
- [Graphical interface](#graphical-interface)
- [Command-line options](#command-line-options)
- [Measuring latency](#measuring-latency)
- [Measuring losses](#measuring-losses)
- [JSON output](#json-output)
- [Dumping streams](#dumping-streams)
- [ALSA parameters](#alsa-parameters)
- [Real-time scheduling policy](#real-time-scheduling-policy)
- [Acknowledgments](#acknowledgments)
- [Authors](#authors)
- [License](#license)

<!-- tocstop -->

Features
--------

`signal-estimator` is a small command-line and GUI tool allowing to measure different characteristics of the signal looped back from audio output to audio input.

Features:

* send test signal to output device and receive looped back signal from input device
* measure signal latency (the shift between output and input signal)
* measure signal loss ratio (number of glitches per second in the input signal)
* measurements output in JSON format for easy parsing
* dump output and input streams to text files and plot them using matplotlib

Example use cases
-----------------

* Measure local hardware latency.

  *Example setups:*

  * connect computer's audio output to its audio input using a jack cable
  * place computer's speakers near the its microphone

* Measure total latency and signal quality of an external hardware + software system.

  *Example setups:*

  * measure Raspberry Pi hardware + software latency

    connect computer's audio output to a Raspberry Pi's input and vice versa, and run a program on Raspberry Pi that loops back signal from input to output

  * measure hardware + software + network latency / quality of the two Raspberry Pies connected via a network transport

    connect computer's audio output to the first Raspberry Pi's input, and computer's audio input to the second Raspberry Pi's output, and connect the two Raspberry Pies using some network audio transport

  * measure the hardware + software + bluetooth latency / quality of a mobile phone with bluetooth microphone and headphones

    place computer's speakers near bluetooth microhpone connected to a mobile phone, place computer's microhpone near bluetooth headphones conencted to the phone, and run a mobile app that loops back signal from bluetooth microphone to bluetooth headphones

  In these examples, you'll need to measure the latency of your local harware first, and then subtract it ftom the measurements results, to get the actual latency of the external system being tested.

Supported platforms
-------------------

* Linux / ALSA

Dependencies
------------

* C++17 compiler
* CMake >= 3.0
* libasound (ALSA devel)
* Qt5 (for GUI)
* Qwt (for GUI)

Installation
------------

Install dependencies:

```
sudo apt install libasound2-dev qtbase5-dev libqwt-qt5-dev
```

Clone repo:

```
git clone --recurse-submodules https://github.com/gavv/signal-estimator.git
cd signal-estimator
```

Build:

```
make
```

Build without GUI (in this case Qt and Qwt are not needed):

```
make no_gui
```

Install into the system (optionally):

```
sudo make install
```

Graphical interface
-------------------

The tools comes with a simple GUI (`signal-estimator-gui`) that plots the signal in the real-time. It can be especially useful for troubleshooting problems with measurements.

It uses Qt and Qwt libraries. Internally, it invokes the command-line tool (`signal-estimator`) and parses its output.

Here is how it looks like:

<img src="https://github.com/gavv/signal-estimator/blob/main/doc/screenshot.png" width="600" />

Command-line options
--------------------

```
$ ./bin/signal-estimator --help
Measure characteristics of a looped back signal
Usage:
  signal-estimator [OPTION...]

 General options:
  -h, --help          Print help message and exit
  -f, --format arg    Output Format: json|text (default: text)
  -m, --mode arg      Mode: noop|latency|losses (default: latency)
  -o, --output arg    Output device name, required
  -i, --input arg     Input device name, required
  -r, --rate arg      Sample rate (default: 48000)
  -c, --chans arg     Number of channels (default: 2)
  -v, --volume arg    Signal volume, from 0 to 1 (default: 0.500000)
  -p, --periods arg   Number of periods in ring buffer (default: 2)
  -l, --latency arg   Ring buffer size, microseconds (default: 8000)
  -d, --duration arg  Measurement duration, seconds (default: 10.000000)

 Reporting options:
      --sma arg  Simple moving average window in reports (default: 5)

 Latency estimation options:
      --strike-period arg       Strike period, seconds (default: 1.000000)
      --strike-length arg       Strike length, seconds (default: 0.100000)
      --strike-detection-window arg
                                Strike detection running maximum window, in
                                samples (default: 96)
      --strike-detection-threshold arg
                                Strike detection threshold, from 0 to 1
                                (default: 0.400000)

 Loss ratio estimation options:
      --signal-detection-window arg
                                Signal detection running maximum window, in
                                samples (default: 48)
      --signal-detection-threshold arg
                                Signal detection threshold, from 0 to 1
                                (default: 0.150000)
      --glitch-detection-window arg
                                Glitch detection running maximum window, in
                                samples (default: 32)
      --glitch-detection-threshold arg
                                Glitch detection threshold, from 0 to 1
                                (default: 0.050000)

 File dumping options:
      --dump-output arg    File to dump output stream
      --dump-input arg     File to dump input stream
      --dump-frame arg     Dump only one maximum value per frame (default:
                           64)
      --dump-rounding arg  Round values before dumping and don't dump
                           duplicates (default: 10)
```

Measuring latency
-----------------

In the latency estimation mode, the tool generates short periodic beeps ("strikes") and calculates the shift between each sent and received strike.

```
$ ./bin/signal-estimator -m latency -o hw:1,0 -i hw:1,0 -d 5
opening alsa writer for device hw:1,0
suggested_latency: 8000 us
suggested_buffer_size: 384 samples
selected_buffer_time: 8000 us
selected_buffer_size: 384 samples
selected_period_time: 4000 us
selected_period_size: 192 samples
opening alsa reader for device hw:1,0
suggested_latency: 8000 us
suggested_buffer_size: 384 samples
selected_buffer_time: 8000 us
selected_buffer_size: 384 samples
selected_period_time: 4000 us
selected_period_size: 192 samples
can't enable real-time scheduling policy
can't enable real-time scheduling policy
latency:  sw+hw  13.145ms  hw   2.934ms  hw_avg5   2.934ms
latency:  sw+hw  12.465ms  hw   2.924ms  hw_avg5   2.929ms
latency:  sw+hw  13.171ms  hw   2.968ms  hw_avg5   2.942ms
latency:  sw+hw  12.510ms  hw   2.962ms  hw_avg5   2.947ms
latency:  sw+hw  12.536ms  hw   3.008ms  hw_avg5   2.959ms
```

Notation:

* `sw+hw` - total software + hardware latency, including ALSA ring buffer

  computed as the time interval beginning when the first audio *frame* of the strike was sent to the output ring buffer, and ending when the first frame of the strike was received from the input ring buffer

* `hw` - an estimation of hardware latency, excluding ALSA ring buffer

  computed as the time interval beginning when the first audio *sample* of the strike was sent from ring buffer to DAC, and ending when the first sample of the strike was received from ADC and placed to ring buffer

* `hw_avg5` - moving average of last 5 `hw` values

`sw+hw` latency is affected by the `--latency` parameter, which defines the size of the ALSA ring buffer. You may need to select a higher latency if you're experiencing underruns or overruns.

`hw` latency, on the other hand, should not be affected by it and should depend only on your hardware and the way how the signal is looped back from output to input.

If you're having troubles, you may also need to configure signal volume, strike period and length, and strike detection parameters. Note that strike period has to be greater than latency of your loopback.

Measuring losses
----------------

In the loss estimation mode, the tool generates continuous beep and counts for glitches and gaps in the received signal.

```
$ ./bin/signal-estimator -m losses -o hw:1,0 -i hw:1,0 -d 10
opening alsa writer for device hw:1,0
suggested_latency: 8000 us
suggested_buffer_size: 384 samples
selected_buffer_time: 8000 us
selected_buffer_size: 384 samples
selected_period_time: 4000 us
selected_period_size: 192 samples
opening alsa reader for device hw:1,0
suggested_latency: 8000 us
suggested_buffer_size: 384 samples
selected_buffer_time: 8000 us
selected_buffer_size: 384 samples
selected_period_time: 4000 us
selected_period_size: 192 samples
can't enable real-time scheduling policy
can't enable real-time scheduling policy
losses:  rate   0.0/sec  rate_avg5   0.0/sec  ratio   0.00%
losses:  rate   6.0/sec  rate_avg5   3.0/sec  ratio   0.26%
losses:  rate   3.0/sec  rate_avg5   3.0/sec  ratio   0.20%
losses:  rate   0.0/sec  rate_avg5   2.3/sec  ratio   0.00%
losses:  rate   1.0/sec  rate_avg5   2.0/sec  ratio   0.01%
```

Notation:

* `rate` - rough estimation of the number of losses (glitches) per second

  a glitch is defined as a spike in the received signal gradient; we rely on the fact that the original signal is a smooth continuous sine wave

* `rate_avg5` - moving average of last 5 `rate` values

* `ratio` - rough estimation of the loss ratio (percentage of the lost frames)

  a loss is defined as a frame of the received signal, with all samples having small amplitude (volume); we rely on the fact that the original signal is a loud continuous sine wave

These numbers may be rough enough.

If you're having troubles, you may need to configure signal volume, and signal and glitch detection parameters.

JSON output
-----------

JSON output can be enabled by passing the `--format json` or `-f json` flag. By default, output is displayed in text format.

Sample JSON output format for measuring latency is shown below.

```
[
  {"sw_hw": 17.288373, "hw": 10.339908, "hw_avg5": 10.339908},
  {"sw_hw": 20.710353, "hw": 10.966634, "hw_avg5": 10.653271},
  {"sw_hw": 21.708660, "hw": 12.604020, "hw_avg5": 11.303521}
]
```

Sample JSON output format for measuring losses is shown below.

```
[
  {"rate": 0.000000, "rate_avg5": 0.000000, "ratio": 0.000000},
  {"rate": 0.000000, "rate_avg5": 0.000000, "ratio": 3.501563},
  {"rate": 0.000000, "rate_avg5": 0.000000, "ratio": 2.626626}
]
```

Note: Here `sw_hw` means `sw+hw` - total software + hardware latency, including ALSA ring buffer. Except `sw_hw`, all the notations are the same as mentioned in the measuring latency and losses section. All time units are in milliseconds.

Dumping streams
---------------

In any mode, including `noop` mode, you can specify `--dump-output` and `--dump-input` options to dump output and input samples and their timestamps to text files or stdout (use `-`).

To reduce the file size, we dump only one maximum value per frame. To reduce the file size even more, we round every dumped value, and drop value if it's the same as the previois one. The frame size and rounding factor are configurable via command-line.

The timestamps in the dumped files correspond to the estimate time, in nanoseconds, when the sample was written to DAC or read from ADC.

```
$ ./bin/signal-estimator -m noop -o hw:1,0 -i hw:1,0 -d 5 \
    --volume 1.0 --dump-output output.txt --dump-input input.txt
opening alsa writer for device hw:1,0
suggested_latency: 8000 us
suggested_buffer_size: 384 samples
selected_buffer_time: 8000 us
selected_buffer_size: 384 samples
selected_period_time: 4000 us
selected_period_size: 192 samples
opening alsa reader for device hw:1,0
suggested_latency: 8000 us
suggested_buffer_size: 384 samples
selected_buffer_time: 8000 us
selected_buffer_size: 384 samples
selected_period_time: 4000 us
selected_period_size: 192 samples
can't enable real-time scheduling policy
can't enable real-time scheduling policy
```

The command above will produce two files:

```
$ ls -lh *.txt
-rw-r--r-- 1 user user  13K Jan 15 16:22 output.txt
-rw-r--r-- 1 user user 118K Jan 15 16:22 input.txt
```

We also provide a helper script that plots the files using matplotlib. You can use it to manually inspect the signal:

```
$ ./script/plot_signal.py output.txt input.txt
```

![](./doc/plot_edited.png)

In this example we were measuring the latency of an Android phone with AirPods connected via Bluetooth, and the measured latency was about 238ms.

ALSA parameters
---------------

ALSA output and input device names are the same as passed to `aplay` and `arecord` tools.

You may need to configure sample rate and the number of channels. The selected values should be supported by both output and input devices.

You may also need to configure ALSA ring buffer size and the number of periods in the ring buffer. These parameters affect software latency and output / input robustness.

Real-time scheduling policy
---------------------------

If you run the tool under the `root` user, or with `CAP_SYS_NICE` and `CAP_SYS_ADMIN` capabilities, it will automatically enable `SCHED_RR` scheduling policy for output and input threads. This may help to avoid glitches introduced by the tool itself (not by the hardware or sofware being measured) on a loaded system.

Acknowledgments
---------------

This tool was initially developed for a freelance project. Big thanks to my customer Samuel Blum at Boring Technologies, who sponsored the development and has kindly allowed to make it open-source!

And of course, thanks to everyone who contributed to the project!

Authors
-------

See [here](https://github.com/gavv/signal-estimator/graphs/contributors).

License
-------

[MIT](LICENSE)
