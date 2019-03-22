#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
. $DIR/config.sh

CONTAINER_NAME="$PROJECT_NAME"
[ $# -gt 0 ] && CONTAINER_NAME="$1"

xhost +local:root >/dev/null
docker run \
  --net=host \
  --privileged \
  --rm \
  -v $(realpath $DIR/..):$WORKDIR \
  -e DISPLAY \
  -v /tmp/.X11-unix:/tmp/.X11-unix:rw \
  --name $CONTAINER_NAME \
  -it $DOCKER_REPOSITORY_NAME:latest
xhost -local:root >/dev/null
