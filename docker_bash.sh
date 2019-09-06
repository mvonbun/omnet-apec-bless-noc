#!/bin/bash
# Send bash commands to Docker container (useful to mv/rm simulation results)
#  ./docker_bash.sh "cd simulations && rm -R results/Test"

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
# sudo docker run -v "$(pwd)":/sim mvonbun/omnetpp:5.0 bash -c "$*"
docker run -v "$(pwd)":/sim mvonbun/omnetpp:5.0 bash -c "$*"
popd
