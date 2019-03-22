#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
. $DIR/config.sh
docker run \
  --net=host \
  --privileged \
  --rm \
  -v $(realpath $DIR/..):$WORKDIR \
  --name $PROJECT_NAME \
  -it $DOCKER_REPOSITORY_NAME:latest
