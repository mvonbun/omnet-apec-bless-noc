#!/bin/bash
# Run simulation using Docker
#   docker_run.sh -c CONFIG -r RUNNUMBER

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
# sudo docker run -v "$(pwd)":/sim mvonbun/omnetpp:5.0 bash -c "cd simulations && ./run -u Cmdenv $*"
docker run -v "$(pwd)":/sim mvonbun/omnetpp:5.0 bash -c "cd simulations && ./run -u Cmdenv $*"
popd
