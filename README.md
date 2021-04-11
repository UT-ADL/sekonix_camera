<upper>*Inspired by [NVIDIA dw_ros](https://github.com/NVIDIA/dw-ros) & [leo-drive drivers](https://gitlab.com/leo-drive/Drivers/sekonix_camera).* </upper>

## How to build on host

#### Required steps
- [install NVIDIA DRIVE™ OS 5.2.0 and DriveWorks 3.5 (Linux)](https://github.com/nvidia/dw-ros#install-nvidia-drive-os-520-and-driveworks-35-linux)
- [cross compile ROS](https://github.com/nvidia/dw-ros#cross-compile-ros)
- [cross compile nv_sensors](https://github.com/nvidia/dw-ros#cross-compile-nv_sensors)
- [run on the target system](https://github.com/nvidia/dw-ros#run-on-the-target-system)

#### Install NVIDIA DRIVE™ OS 5.2.0 and DriveWorks 3.5 (Linux)
- Follow the download [page](https://developer.nvidia.com/drive/downloads) to install NVIDIA DRIVE™ OS 5.2.0 and DriveWorks 3.5 (Linux). Follow the 'DriveWorks 3.5 Installation Guide'.

#### Prepare a cross-compilation sysroot

```
SYSROOT=~/nvidia/nvidia_sdk/DRIVE_OS_5.2.0_SDK_Linux_OS_DDPX/DRIVEOS/drive-t186ref-linux/targetfs
cd $SYSROOT

sudo apt install qemu-user-static
sudo cp /usr/bin/qemu-aarch64-static usr/bin
sudo cp -b /etc/resolv.conf etc
sudo mount -o bind /dev dev
sudo mount -o bind /proc proc
sudo mount -o bind /sys sys

sudo LC_ALL=C chroot .
# apt update
# apt install libboost-all-dev libtinyxml-dev libtinyxml2-dev liblz4-dev libbz2-dev libapr1 libaprutil1 libconsole-bridge-dev libpoco-dev libgpgme-dev python-defusedxml python-rospkg python-catkin-pkg python-netifaces liblog4cxx-dev libopenblas-dev libgflags-dev libglew-dev libopencv-dev
# exit

sudo umount sys proc dev 
sudo rm usr/bin/qemu-aarch64-static
sudo mv etc/resolv.conf~ etc/resolv.conf
sudo rm -rf var/lib/apt/lists/*
sudo rm -rf dev/*
sudo rm -rf var/log/*
sudo rm -rf var/tmp/*
sudo rm -rf var/cache/apt/archives/*.deb
sudo rm -rf tmp/*
```

#### Fix broken symlinks

The broken symlinks can be fixed temporarily with overlays, using commands similar to the following:
```
sudo mkdir /lib/aarch64-linux-gnu
sudo mkdir /tmp/ros-cc-overlayfs
sudo mount -t overlay -o lowerdir=$SYSROOT/lib/aarch64-linux-gnu,upperdir=/lib/aarch64-linux-gnu,workdir=/tmp/ros-cc-overlayfs overlay /lib/aarch64-linux-gnu
```

#### Ros Prerequisites
Follow this [page](http://wiki.ros.org/melodic/Installation/Source) to install all prerequisites and then run below commands to download source of ROS Melodic Morenia (Ubuntu 18.04 is the target root file system of DRIVE OS Linux 5.2.0)

```
mkdir -p ~/ros_catkin_ws/src && cd ~/ros_catkin_ws
rosinstall_generator ros_comm sensor_msgs camera_info_manager cv_bridge image_transport nodelet roscpp std_msgs --rosdistro melodic --deps --tar > melodic-ros_comm.rosinstall
vcs import src < melodic-ros_comm.rosinstall
```

#### Clone UT Sekonix driver
```bash
git clone git@gitlab.cs.ut.ee:autonomous-driving-lab/autoware.ai/local/sekonix_camera_ut.git src/sekonix_camera_ut
```

#### Extract ros dependancies

```bash
rosdep install -si --reinstall --from-path src
```
Install the displayed dependencies on the emulated sysroot with `apt install`.

#### Crossbuild ros 

```bash
SYSROOT=~/nvidia/nvidia_sdk/DRIVE_OS_5.2.0_SDK_Linux_OS_DDPX/DRIVEOS/drive-t186ref-linux/targetfs
```
```bash
src/catkin/bin/catkin_make_isolated -DVIBRANTE_PDK:STRING=$HOME/nvidia/nvidia_sdk/DRIVE_OS_5.2.0_SDK_Linux_OS_DDPX/DRIVEOS/drive-t186ref-linux -DTRT_VERSION:STRING=6.3.1.3 -DCMAKE_TOOLCHAIN_FILE=$HOME/ros_catkin_ws/src/sekonix_camera_ut/Toolchain-V5L.cmake -DCMAKE_EXE_LINKER_FLAGS="${CMAKE_EXE_LINKER_FLAGS} -L/usr/local/driveworks/targets/aarch64-Linux/lib -Wl,-rpath,/usr/local/driveworks/targets/aarch64-Linux/lib -L$SYSROOT/usr/local/cuda-10.2/targets/aarch64-linux/lib -Wl,-rpath,$SYSROOT/usr/local/cuda-10.2/targets/aarch64-linux/lib -L$SYSROOT/usr/lib/aarch64-linux-gnu/openblas -Wl,-rpath,$SYSROOT/usr/lib/aarch64-linux-gnu/openblas"
```

Replace with the current installation path with the binary installation path so we can run any binary installed packages on the target.

```bash
sed -i "s#$HOME/ros_catkin_ws/install_isolated#/opt/ros/melodic#g" install_isolated/_setup_util.py
```

## How to run on target

#### Install ROS dependencies and ROS

Follow http://wiki.ros.org/melodic/Installation/Ubuntu to install ROS necessary apt packages via below command
```
sudo apt install ros-melodic-ros-base ros-melodic-image-view
```

Install the same dependancies as we have installed on the emulated sysroot :

```
apt install libboost-all-dev libtinyxml-dev libtinyxml2-dev liblz4-dev libbz2-dev libapr1 libaprutil1 libconsole-bridge-dev libpoco-dev libgpgme-dev python-defusedxml python-rospkg python-catkin-pkg python-netifaces liblog4cxx-dev libopenblas-dev libgflags-dev libglew-dev libopencv-dev
```

Also install the dependancies with rosdep :

```bash
rosdep install --from-paths src --ignore-src -r -y
```

#### Remove old library versions\
*(The detailed reason is detailed in this [post](https://forums.developer.nvidia.com/t/libgdal-so-has-undefined-symbol/110239/5)).*
```bash
rm /usr/lib/libxerces-c*.
```

#### Set up ros environment
```
source ~/install_isolated/setup.bash
```