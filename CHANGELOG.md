# Changelog
All notable changes to this project will be documented in this file.
The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to
[Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
### Added
- MIT License tag for README.md
- Testing autoload just like modules

### Changed
- Changelog spec to 1.0.0 from 0.3.0. (Visibility changes)
## [0.0.1] - 2018-12-08
### Added
- Semantic Versioning starting at 0.0.1
- This CHANGELOG file
- CONTRIBUTING.md for contributors to follow our guides

## [0.0.0] - Before 2018-12-08
This is documenting the state of the kernel before CHANGELOG was implemented

### Added
- Makefile system that allows for multiple platforms and targets
- x86 target for kernel
    - Drivers for PIC, PIT, UARTS, and a VGA Text Console
    - Modules for Interrupts, the GDT, and I/O ports
- hosted target for kernel
    - Driver for Text Console
- Core System
    - Module auto loading system
    - Hardware Abstraction Layer API declaration
    - Console Driver
- Testing System implemented by port of
[Unity](http://www.throwtheswitch.org/unity)
- Standard Library
    - Implemented most of `string.c`
    - Implemented `printf()`, `putchar()`, and `puts()` from stdio
- Abstract Data Types
    - Ring Buffer
