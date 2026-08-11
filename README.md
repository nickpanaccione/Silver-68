# Silver '68
[![Build](https://github.com/nickpanaccione/Silver-68/actions/workflows/build.yml/badge.svg?branch=main)](https://github.com/nickpanaccione/Silver-68/actions/workflows/build.yml)

Circuit-modelled guitar amplifier.

## Description

Silver '68 is an audio plugin (VST3/AU) that models the Fender Bassman AC568 stage by stage. 

> **Note:** This project is in early development.

## Getting Started

### Dependencies

* CMake 3.22+
* A C++20 compiler (on macOS, the Xcode Command Line Tools)
* JUCE (included as a git submodule)

Optional, for the circuit-validation workflow:

* [LTspice](https://www.analog.com/en/resources/design-tools-and-calculators/ltspice-simulator.html) (simulate the reference schematics in `spice/`)
* Python 3 with NumPy and Matplotlib (analysis scripts in `scripts/`)

### Installing

```bash
git clone --recurse-submodules https://github.com/nickpanaccione/Silver-68
cd Silver-68
```

### Executing program

Configure and build with CMake:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

This produces VST3, AU, and Standalone targets, and installs the plugin into
the system plug-in folders automatically. 
> AU is macOS-only and is skipped on other platforms.

## License

This project is licensed under the AGPLv3 License - see the
[LICENSE](./LICENSE) file for details.

## Acknowledgments

* Built with [JUCE](https://juce.com/)
* Tone stack discretization follows Yeh & Smith, ["Discretization of the '59 Fender Bassman Tone Stack"](https://www.dafx.de/paper-archive/2006/papers/p_001.pdf) (DAFx-06)
* Vacuum tube SPICE models by [Norman Koren](https://www.normankoren.com/Audio/Tubemodspice_article.html)
