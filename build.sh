#!/bin/sh

usage() { 
    echo "Usage: $0 [-b build ][ -h ] [ -t compiler_ ]" 1>&2
    exit 1; 
}


while getopts b:ht: cli
do
    case $cli in
        -b ) build=$OPTARG ;;
        -h ) usage ;;
        -t ) toolchain=$OPTARG ;;
    esac
done

# now "$@" contains the rest of the arguments
shift "$((OPTIND - 1))"

if [ -z ${build+x} ]; then
    echo "Error: Build path needs to be specified!"
    usage
fi

mkdir -p $build
