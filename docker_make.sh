#!/bin/bash
# Depending on your configuration, a sudo is required to run Docker
# containers. Comment/Uncomment the line that reflects your setup.

# change to script dir
pushd "$(dirname "$0")"
echo "(apec) build simulation using Docker"
# sudo docker run -v "$(pwd)":/sim mvonbun/omnetpp:5.0 bash -c "cd src && make"
docker run -v "$(pwd)":/sim mvonbun/omnetpp:5.0 bash -c "cd src && make"
popd
