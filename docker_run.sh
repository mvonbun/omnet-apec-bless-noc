#!/bin/bash
# Depending on your configuration, a sudo is required to run Docker
# containers. Comment/Uncomment the line that reflects your setup.

# change to script dir
pushd "$(dirname "$0")"
# run simulation
#   docker_run.sh -c CONFIG -r RUNNUMBER
# sudo docker run -v "$(pwd)":/sim mvonbun/omnetpp:5.0 bash -c "cd simulations && ./run -u Cmdenv $*"
docker run -v "$(pwd)":/sim mvonbun/omnetpp:5.0 bash -c "cd simulations && ./run -u Cmdenv $*"
popd
