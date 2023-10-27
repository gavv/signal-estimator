# Changelog

## [v0.0.8][v0.0.8] - ???

* Overhaul hardware latency calculation and improve its precision
* Implement new `io_jitter` measurement mode ([gh-53][gh-53])
* Support multiple input devices and `--diff` mode ([gh-51][gh-51])
* Support different channel counts for output and input devices ([gh-50][gh-50])
* Support different period size and count for output and input devices ([gh-52][gh-52])
* Rename `-v` (`--volume`) to `-g` (`--gain`)
* Add `-v` (`--verbose`) option, reduce verbosity by default
* Add `--no-rt` option
* Rename `--step-period` / `--impulse-period` to `--step-interval` / `--impulse-interval` to avoid confusion with ALSA periods
* Take `--gain` into account in `latency_corr` mode (earlier it was ignored)
* Saturate instead of overflowing when `--gain`-ed signal is high
* Enable `--warmup` by default
* Disable `--dump-compression` by default
* Use wallclock (absolute system time) time in csv dumps ([gh-60][gh-60])
* GUI: Display measurement results ([gh-13][gh-13])
* GUI: Fix mode names
* GUI: Reduce default dump compression
* Optimization: avoid frame allocations in real-time path
* Optimization: avoid using double-precision floats when they're not needed
* Optimization: move computations to separate thread in `latency_step` mode
* Optimization: make computation threads non-realtime
* Switch to spdlog and enable colored logging ([gh-38][gh-38])
* Switch from cxxopts to CLI11 ([gh-26][gh-26])
* Switch vendored dependencies to latest stable tags
* Add new dependency: intrusive_shared_ptr
* Enable and fix more warnings
* Refactoring
* Improve documentation

[v0.0.8]: https://github.com/gavv/signal-estimator/releases/tag/v0.0.8

[gh-13]: https://github.com/gavv/signal-estimator/issues/13
[gh-26]: https://github.com/gavv/signal-estimator/issues/26
[gh-38]: https://github.com/gavv/signal-estimator/issues/38
[gh-50]: https://github.com/gavv/signal-estimator/issues/50
[gh-51]: https://github.com/gavv/signal-estimator/issues/51
[gh-52]: https://github.com/gavv/signal-estimator/issues/52
[gh-53]: https://github.com/gavv/signal-estimator/issues/53
[gh-60]: https://github.com/gavv/signal-estimator/issues/60

## [v0.0.7][v0.0.7] - 19 Sep 2023

* Support cross-compilation (see README)
* Bump minimal cmake version to 3.5
* Improve documentation
* Improve CI

[v0.0.7]: https://github.com/gavv/signal-estimator/releases/tag/v0.0.7

## [v0.0.6][v0.0.6] - 15 May 2023

* Add debug checks, sanitizers, and assertions
* Fix buffer overflow in ImpulseGenerator ([gh-25][gh-25])
* Fix use after free ([gh-25][gh-25])
* Fix memory leak in FFTConvolution ([gh-25][gh-25])
* Fix crash in plotting code ([gh-25][gh-25])
* Fix hang in alsa_close ([gh-5][gh-5])

[v0.0.6]: https://github.com/gavv/signal-estimator/releases/tag/v0.0.6

[gh-25]: https://github.com/gavv/signal-estimator/issues/25
[gh-5]: https://github.com/gavv/signal-estimator/issues/5

## [v0.0.5][v0.0.5] - 11 Feb 2023

* Fix broken build for Qt 5.15.5 ([gh-23][gh-23])
* Implement convolution-based latency estimation ([gh-3][gh-3])
* Major refactoring
* Refinements in command-line options
* Add `--warmup` option

[v0.0.5]: https://github.com/gavv/signal-estimator/releases/tag/v0.0.5

[gh-23]: https://github.com/gavv/signal-estimator/issues/23
[gh-3]: https://github.com/gavv/signal-estimator/issues/3

## [v0.0.4][v0.0.4] - 05 Jun 2022

* Fix timestamps on 32-bits platforms ([gh-21][gh-21])
* Improve Qwt plot GUI ([gh-19][gh-19])
* Fix leaks in GUI
* Fix compiler warnings
* Improve documentation

[v0.0.4]: https://github.com/gavv/signal-estimator/releases/tag/v0.0.4

[gh-19]: https://github.com/gavv/signal-estimator/issues/19
[gh-21]: https://github.com/gavv/signal-estimator/issues/21

## [v0.0.3][v0.0.3] - 24 Aug 2021

* Fix build on Arch ([gh-18][gh-18])
* Fixes in dumper
* Fix compiler warnings
* Improve documentation
* Improve CI

[v0.0.3]: https://github.com/gavv/signal-estimator/releases/tag/v0.0.3

[gh-18]: https://github.com/gavv/signal-estimator/issues/18

## [v0.0.2][v0.0.2] - 22 Aug 2021

* Add JSON output support ([gh-2][gh-2])
* Add GUI ([gh-6][gh-6])
* Add makefile
* Fix error messages
* Fix compiler warnings
* Improve documentation

[v0.0.2]: https://github.com/gavv/signal-estimator/releases/tag/v0.0.2

[gh-2]: https://github.com/gavv/signal-estimator/issues/2
[gh-6]: https://github.com/gavv/signal-estimator/issues/6

## [v0.0.1][v0.0.1] - 03 Mar 2020

* Initial release

[v0.0.1]: https://github.com/gavv/signal-estimator/releases/tag/v0.0.1
