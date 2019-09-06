#!/bin/bash
# Setup simulation model using Docker.
#   ./docker_setup.sh

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
echo "(apec) setup projects make"
# sudo docker run -v "$(pwd)":/sim mvonbun/omnetpp:5.0 ./makemake
docker run -v "$(pwd)":/sim mvonbun/omnetpp:5.0 ./makemake
echo "(apec) build simulation"
# sudo docker run -v "$(pwd)":/sim mvonbun/omnetpp:5.0 bash -c "cd src && make"
docker run -v "$(pwd)":/sim mvonbun/omnetpp:5.0 bash -c "cd src && make"
popd
