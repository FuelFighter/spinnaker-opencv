From fuelfighter/ff-opencv:latest

ENV DEBIAN_FRONTEND noninteractive
ENV APT "apt-get -yqq -o=Dpkg::Use-Pty=0"

RUN $APT update && \
  $APT install --no-install-recommends apt-utils && \
  $APT upgrade && \
  $APT install build-essential tmux vim htop iputils-ping

# SDL
RUN $APT update && \
  $APT install libsdl2-dev freeglut3-dev libglew-dev libeigen3-dev libsoil-dev

# Debug
RUN $APT update && \
  $APT install gdb valgrind

## Building PCL 
#RUN apt-get install -y software-properties-common
#RUN apt-get update && apt-get install -y \
#git build-essential linux-libc-dev \
#cmake cmake-gui \
#libusb-1.0-0-dev libusb-dev libudev-dev \
#mpi-default-dev openmpi-bin openmpi-common \
#libflann1.8 libflann-dev \
#libeigen3-dev \
#libboost-all-dev \
#libvtk5.10-qt4 libvtk5.10 libvtk5-dev \
#libqhull* libgtest-dev \
#freeglut3-dev pkg-config \
#libxmu-dev libxi-dev \
#qt-sdk openjdk-8-jdk openjdk-8-jre \
#openssh-client


# PCL - build from source and install
#RUN cd /opt \
#&& git clone https://github.com/PointCloudLibrary/pcl.git pcl-trunk \
#&& ln -s /opt/pcl-trunk /opt/pcl \
#&& cd /opt/pcl && git checkout pcl-1.8.0 \
#&& mkdir -p /opt/pcl-trunk/release \
#&& cd /opt/pcl/release && cmake -DCMAKE_BUILD_TYPE=None -DBUILD_GPU=ON -DBUILD_apps=ON -DBUILD_examples=ON .. \
#&& cd /opt/pcl/release && make -j3 \
#&& cd /opt/pcl/release && make install \
#&& cd /opt/pcl/release && make clean

## For lidar
RUN $APT update && \
  $APT install libeigen3-dev \
  libvtk6-dev

RUN $APT update && \
  $APT install \
  ros-kinetic-tf2-geometry-msgs \
  ros-kinetic-tf \
  ros-kinetic-tf-conversions \
  ros-kinetic-geodesy ros-kinetic-pcl-ros ros-kinetic-nmea-msgs ros-kinetic-libg2o \
  ros-kinetic-rviz
  #ros-kinetic-pcl-ros \
  #ros-kinetic-pcl-conversions \
  #ros-kinetic-pcl-msgs \
  #python-vtk \
  #tcl-vtk

ADD spinnaker-1.13.0.31-amd64 /root/spinnaker-debs
RUN cd /root/spinnaker-debs && ./install_spinnaker.sh

ADD config.sh /root/config.sh
ENTRYPOINT /bin/bash -c '. /ros_entrypoint.sh && . /root/config.sh && cd $WORKDIR && /bin/bash'
