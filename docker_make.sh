#!/bin/bash
# Build the simulation model using Docker.
#  ./docker_make.sh

# Depending on your configuration, a sudo is required to run Docker
# containers. Comment/Uncomment the line that reflects your setup.

# silent pushd/popd
pushd () {
    command pushd "$@" > /dev/null
}
popd () {
    command popd "$@" > /dev/null
}

# change to script dir
pushd "$(dirname "$0")"
echo "(apec) build simulation using Docker"
# sudo docker run -v "$(pwd)":/sim mvonbun/omnetpp:5.0 bash -c "cd src && make"
docker run -v "$(pwd)":/sim mvonbun/omnetpp:5.0 bash -c "cd src && make"
popd
