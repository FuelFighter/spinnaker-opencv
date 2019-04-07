# spinnaker-opencv
Convert images captured on Flir / Point Grey camera to OpenCV via Spinnaker SDK

## Docker based build:

Either build the docker image yourself:
```
./docker/build_docker.sh
```

Or download it:
```
docker pull fuelfighter/reodor-environment
```

Then run the docker container:
```
./docker/run_docker.sh
```

Inside the docker container:

build:

```
./make.sh
```

run:
```
./run.sh
```
