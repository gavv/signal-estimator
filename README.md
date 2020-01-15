# Signal Estimator

[![Build Status](https://travis-ci.org/gavv/signal-estimator.svg?branch=master)](https://travis-ci.org/gavv/signal-estimator)

## Features

`signal-estimator` is a small command-line tool allowing to measure different characteristics of the signal looped back from audio output to audio input.

Features:

* send test signal to output device and receive looped back signal from input device
* measure signal latency (the shift between output and input signal)
* measure signal loss ratio (number of glitches per second in the input signal)
* dump output and input streams to text files and plot them using matplotlib

## Example use cases

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

## Supported platforms

* Linux / ALSA

## Dependencies

* C++17 compiler
* CMake >= 3.0
* libasound (ALSA devel)

## Installation

Install dependencies:

```
$ sudo apt install libasound2-dev
```

Clone repo:

```
$ git clone --recurse-submodules https://github.com/gavv/signal-estimator.git
$ cd signal-estimator
```

Build:

```
$ mkdir build
$ cd build
$ cmake ..
$ make -j4
$ cd ..
```

## Command-line options

```
$ ./bin/signal-estimator --help
Measure characteristics of a looped back signal
Usage:
  signal-estimator [OPTION...]

 General options:
  -h, --help          Print help message and exit
  -m, --mode arg      Mode: noop|latency|losses (default: latency)
  -o, --output arg    Output device
  -i, --input arg     Input device
  -r, --rate arg      Sample rate (default: 48000)
  -c, --chans arg     Number of channels (default: 2)
  -p, --periods arg   Number of periods in buffer (default: 2)
  -l, --latency arg   Input or output latency, us (default: 8000)
  -d, --duration arg  Test duration, seconds (default: 10.000000)
  -v, --volume arg    Test signal volume, from 0 to 1 (default: 0.500000)

 Reporting options:
      --sma arg  Simple moving average window in reports (default: 5)

 Latency estimation options:
      --strike-period arg     Strike period, seconds (default: 1.000000)
      --strike-length arg     Strike length, seconds (default: 0.100000)
      --strike-window arg     Strike detection running maximum window, in
                              samples (default: 96)
      --strike-threshold arg  Strike detection threshold, from 0 to 1
                              (default: 0.400000)

 Loss ratio estimation options:
      --glitch-window arg     Glitch detection running maximum window, in
                              samples (default: 32)
      --glitch-threshold arg  Glitch detection threshold, from 0 to 1
                              (default: 0.050000)

 File dumping options:
  -O, --dump-output arg    File to dump output stream
  -I, --dump-input arg     File to dump input stream
      --dump-frame arg     Frame size (default: 64)
      --dump-rounding arg  Rounding (default: 10)
```

## Measure latency

*TODO*

## Measure loss ratio

*TODO*

## Manually inspect streams

*TODO*

## Acknowledgments

This tool was initially developed for a freelance project. Big thanks to my customer Samuel Blum at Boring Technologies, who sponsored the development and has kindly allowed to make it open-source!

## Authors

See [here](https://github.com/gavv/signal-estimator/graphs/contributors).

## License

[MIT](LICENSE)
