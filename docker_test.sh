#!/bin/bash
# Run an entire test of the build and simulation system using Docker.
#   ./docker_test.sh

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

echo -n "(apec) setup project's make... "
# sudo docker run -v "$(pwd)":/sim mvonbun/omnetpp:5.0 ./makemake
docker run -v "$(pwd)":/sim mvonbun/omnetpp:5.0 ./makemake &>/dev/null
if [ $? = 0 ]; then echo "done."; else echo "FAILED!"; fi

echo -n "(apec) build simulation model... "
# sudo docker run -v "$(pwd)":/sim mvonbun/omnetpp:5.0 bash -c "cd src && make"
docker run -v "$(pwd)":/sim mvonbun/omnetpp:5.0 bash -c "cd src && make" &>/dev/null
if [ $? = 0 ]; then echo "done."; else echo "FAILED!"; fi

echo -n "(apec) run simulation... "
# sudo docker run -v "$(pwd)":/sim mvonbun/omnetpp:5.0 bash -c "cd simulations && ./run -u Cmdenv -c Test" &>/dev/null
docker run -v "$(pwd)":/sim mvonbun/omnetpp:5.0 bash -c "cd simulations && ./run -u Cmdenv -c Test" &>/dev/null
if [ $? = 0 ]; then echo "done."; else echo "FAILED!"; fi

popd
