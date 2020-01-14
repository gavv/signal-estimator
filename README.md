# Signal Estimator

[![Build Status](https://travis-ci.org/gavv/signal-estimator.svg?branch=master)](https://travis-ci.org/gavv/signal-estimator)

## Features

*Work in progress!*

`signal-estimator` is a small command-line tool allowing to measure different characteristics of the signal looped back from audio output to audio input.

Features:

- [x] send test signal to output device
- [x] receive looped back signal from input device
- [x] dump output and input streams to text files
- [x] measure signal latency (the shift between output and input signal)
- [x] measure signal loss ratio (number of glitches per second in the input signal)
- [ ] measure mean opinion score (MOS)

## Use cases

*TODO*

## Supported platforms

* Linux / ALSA

## Dependencies

* C++17 compiler
* CMake >= 3.0
* libasound (ALSA devel)

## Installation

Install dependencies:

```
sudo apt install libasound2-dev
```

Clone repo:

```
git clone --recurse-submodules https://github.com/gavv/signal-estimator.git
cd signal-estimator
```

Build:

```
mkdir build
cd build
cmake ..
make -j4
```

## Command-line options

*TODO*

## Measure latency

*TODO*

## Measure loss ratio

*TODO*

## Measure MOS

*TODO*

## Manually inspect streams

*TODO*

## Acknowledgments

This tool was initially developed for a freelance project. Big thanks to my customer Samuel Blum at Boring Technologies, who sponsored the development and has kindly allowed to make it open-source!

## Authors

See [here](https://github.com/gavv/signal-estimator/graphs/contributors).

## License

[MIT](LICENSE)
