#!/bin/sh

# exit when command return value != 0.
set -e

# avoid ctrl + c impact on this script;
# trap '' INT

# echo "Checking code format"
# ./tools/format_check.sh;

echo "Root workspace directory: `pwd`"

cd build
echo "Working on directory: `pwd`"
./buildcmdline.sh Debug

cd cmdline
echo "Working on directory: `pwd`"
make VERBOSE=1
./src/openDMD
make test

cd ../..
echo "Returning directory: `pwd`"

