# dscrd

dscrd is a C library for Discord API. Library is not to be considered in any way as completed and it probably will never be.

## Build

Project uses `premake5` for building. To compile you also need: `libmagic-dev` `libwebsockets-dev` (and `rest-client-c` which is included)

* Run `premake5 gmake`

* Run `make dscrd` or `make` to build with examples.


## Code structure

`docs/html` - generated documentation by using `doxygen` (or `doxygen Doxyfile_i` for dev)

`dscrd_daemon` - a program that allows other programs to access auth info through dscrd library

`examples` - various examples showing usage of `libdscrd` and `libjts` (remember to put your token in `src/auth_data.h`)

`lib` - libdscrd source files

`lib/con` - code that wraps up REST and websockets connections

`lib/jts` - `jsonToStruct` library for simpler processing of json data

`rest-client-c` - [source-code of rest-client-c](https://github.com/EMCECS/rest-client-c)

`scripts` - scripts to concatenate headers into one (`dscrd.h` and `dscrd_i.h`)



