# Changelog
All notable changes to this project will be documented in this file.
The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to
[Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Documentation for existing Code

### Removed
- Remove `make doc` functionality.

### Changed
- Rewrite of docs/ subdirectory

## [0.0.5] - 2022-09-10
### Added
- Raspberry Pi 4 Support (In Progress)
- Driver Subsystem
- Resource Management API
- CircleCI Artifacts and Releases

### Changed
- Serial driver (16550) rewrite
- Change existing driver code to be platform agnostic
- Makefile Changes to facilitate multi-arch
- Markdown to HTML Generation with `markdown` instead of `pandoc`

### Bugfixes
- x86 IDT code working properly

## [0.0.4] - 2022-07-30
### Added
- Fancy VGA support (8x8 Font)
- iso creation in makefile
- kmain() Now displays system information
- `debug=y` parsing turns on kernel debugging messages

### Changed
- HAL: `get_num_processors()` now reports 1 as default
- Standardized naming of kernel (always kernel.mod now)
- VGA Console code cleanup
- IDT Triple fault fix

### Removed
- pre-compiled binaries and images from bin/ directory

## [0.0.3] - 2022-04-30
### Added
- ipxe iso image
- Kconfig files
- Modular Makefile system
- Qemu-specific shutdown driver
- Shutdown function in HAL
### Changed
- Rewrote Multiboot code
- Modified Code Struture
- Changed build system entirely

## [0.0.2] - 2018-12-12
### Added
- MIT License tag for README.md
- Testing autoload just like modules
- Tests for Ringbuffer and Interrupts
- Menu for iPXE boot
### Changed
- Moved Testing into a seperate directory
- Testing is seamlessly ran now thanks to core/testing.c and macro magic

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

[Unreleased]: https://github.com/primis/apollo/compare/v0.0.5...HEAD
[0.0.5]: https://github.com/primis/apollo/compare/0.0.4...v0.5.0
[0.0.4]: https://github.com/primis/apollo/compare/0.0.3...0.0.4
[0.0.3]: https://github.com/primis/apollo/compare/0.0.2...0.0.3
[0.0.2]: https://github.com/primis/apollo/compare/0.0.1...0.0.2
[0.0.1]: https://github.com/primis/apollo/compare/0.0.0...0.0.1
[0.0.0]: https://github.com/primis/apollo/releases/tag/0.0.0
