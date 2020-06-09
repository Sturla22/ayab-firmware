#!/bin/bash
set -e
OPTIND=1
verbose=0
clean=0
# parse arguments
while getopts "vc" opt; do
    case $opt in
        v)   verbose=1
             ;;
        c)   clean=1
             ;;
    esac
done
shift $((OPTIND-1))
[ "${1:-}" = "--" ] && shift

# change to script directory
parent_path=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd "$parent_path"

# Remove old coverage data

mkdir -p build
cd build
cmake ..

make_verbose=""
ctest_verbose=""
if [[ $verbose -eq 1 ]]; then
  make_verbose="VERBOSE=1"
  ctest_verbose="-V"
fi

if [[ $clean -eq 1 ]]; then
  rm -f CMakeFiles/**/*.gc{da,no}
  make $make_verbose clean
fi

make -j $(nproc) $make_verbose
GTEST_COLOR=1 ctest $ctest_verbose --output-on-failure .

cd ../..

GCOVR_ARGS="--exclude-unreachable-branches --exclude-throw-branches \
  -e test_* -e libraries* -j $(nproc)"
GCOVR_SEARCH_PATH="test/build/CMakeFiles"

gcovr $GCOVR_ARGS --html-details -o ./test/build/coverage.html $GCOVR_SEARCH_PATH
gcovr $GCOVR_ARGS --branches $GCOVR_SEARCH_PATH
gcovr -d $GCOVR_ARGS --fail-under-line 100 --fail-under-branch 100 $GCOVR_SEARCH_PATH
