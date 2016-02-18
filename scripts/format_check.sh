#!/bin/sh

# ATTENTION: This script should be run at the root dir of openDMD project.

find ./unittest -name *.h -o -name *.cpp -o -name *.m -o -name *.mm | xargs ./scripts/cpplint.py
find ./src -name *.h -o -name *.cpp -o -name *.m -o -name *.mm | xargs ./scripts/cpplint.py

