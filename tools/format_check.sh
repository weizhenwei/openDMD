#!/bin/sh

# ATTENTION: This script should be run at the root dir of openDMD project.

find ./src -name *.h -o -name *.cpp | xargs ./tools/cpplint.py

