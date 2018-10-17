The Makefile
===========

Introduction
----------

The Apollo Project has a single makefile. This makefile is designed to be
extendable for every archetechture.

The makefile requires a TARGET be set to compile, otherwise it'll fail.
Current targets are: x86

That's it for now.

Current options for the makefile are:

* `all`     - Comiles the current TARGET and the documentation.
* `doc`     - Compiles the documentation. 
* `clean`   - Removes the object files.

Each target requires a seperate makefile stub. This has to be in the format
"target-<TARGET>.mk" and has to contain the following:

### Variables
* BIN -  Binary file name in the format "<TARGET>-sys.mod"
* TARGET_DEFS - Target specific flags for gcc
* TARGET_LDFLAGS - Target specific flags to pass to the linker
* CSOURCES - A list of the C sources. This must also include the variable $(CSOURCES_TI) which is platform agnostic sources.


### Optional additions
* CC - variable containing the name of the c compiler for the platform
* any additional target building (such as target for assembly)

[Go Up](../)

