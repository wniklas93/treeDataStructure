#!/bin/sh

usage() { 
    echo "Usage: $0 [-b build ][ -h ] [ -p profile ]" 1>&2
    exit 1; 
}

while getopts b:hp: cli
do
    case $cli in
        b) build=$OPTARG;; 
        h) usage;;
        p) profile=$OPTARG;;
    esac
done


# now "$@" contains the rest of the arguments
shift "$((OPTIND - 1))"

if [ -z ${build+x} ]; then
    echo "Error: Build path needs to be specified!"
    usage
fi

if [ -z ${profile+x} ]; then
    profile=debug
fi

mkdir -p $build/$profile

if [ "$profile" = "debug" ]; then
    cmake -S . -B $build/$profile -DCMAKE_BUILD_TYPE=DEBUG
else 
    cmake -S . -B $build/$profile -DCMAKE_BUILD_TYPE=RELEASE
fi

