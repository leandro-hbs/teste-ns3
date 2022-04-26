#!/bin/bash
#
# This is a scripz file based by https://github.com/wyllianbs/ns3install .
# It has been modified to download and build the (at the time) newest 
# version of NS3 in a non-interactive manner
#
# Federal University of Santa Catarina - UFSC
# (c) Prof. Wyllian Bezerra da Silva <wyllian.bs@ufsc.br>
#
# Check out README.md for more details available at URL:
# https://github.com/wyllianbs/ns3install

export DEBIAN_FRONTEND=noninteractive

echo "Installing all dependencies for Debian-based Distro (via apt)"
sudo apt-get update;
sudo -E apt-get install -y g++ python3 python3-dev pkg-config sqlite3 git python3-setuptools qt5-default mercurial gdb valgrind gir1.2-goocanvas-2.0 python-gi python-gi-cairo python3-gi python3-gi-cairo python3-pygraphviz gir1.2-gtk-3.0 ipython3 openmpi-bin openmpi-common openmpi-doc libopenmpi-dev autoconf cvs bzr unrar uncrustify doxygen graphviz imagemagick texlive texlive-extra-utils texlive-latex-extra texlive-font-utils dvipng latexmk python3-sphinx dia  gsl-bin libgsl-dev libgsl23 libgslcblas0 tcpdump sqlite sqlite3 libsqlite3-dev libxml2 libxml2-dev cmake libc6-dev libc6-dev-i386 libclang-6.0-dev llvm-6.0-dev automake python3-pip libgtk-3-dev vtun lxc uml-utilities libboost-dev libboost-filesystem-dev wget libyaml-cpp-dev vim > apt.log 2>&1
python3 -m pip install --user cxxfilt > cxxfilt.log 2>&1
python3 -m pip install pyyaml > pyyaml.log 2>&1
python3 -m pip install nympy > numpy.log 2>&1
python3 -m pip install matplotlib > matplotlib.log 2>&1

# Ns3 Folder
URLfile="https://www.nsnam.org/releases/ns-allinone-3.32.tar.bz2"
file=$(echo "${URLfile##*/}")
NSallinonedir=$(echo "${file%.*.*}");
NSversion=$(echo "${NSallinonedir##*-}"); 
NSdir=ns-$NSversion; 

echo -e "\n::: Downloading the NS3 version: \"$URLfile\"..."; 
wget $URLfile ; 

echo -e "\n::: Extracting file: \"$file\"..."; 
tar xvjf $file ; 

echo -e "\n::: Accessing the path: \"$NSallinonedir\"..."; 
cd $NSallinonedir; 

echo -e "\n::: Building the NS3 [version $NSallinonedir]..."; 
./build.py --enable-examples --enable-tests > build.log 2>&1; 

echo -e "\n::: Accessing the directory: \"$NSdir\"..."; 
cd $NSdir ; 

echo -e "\n::: Testing processing..."; 
./test.py > test.log 2>&1;

echo -e "\n::: Enabling YAML-CPP..."; 
CXFLAGS_EXTRA="-I/usr/include/yaml-cpp" LDFLAGS="-lyaml-cpp" ./waf configure --enable-example --enable-tests > wafconf-yaml.log 2>&1;
./waf build > wafbuild-yaml.log 2>&1;

echo -e "\n::: Configuring control layer..."; 
WORKDIR='/ns3/ns-allinone-3.32/ns-3.32'

cd $WORKDIR/src/applications/model

cp udp-echo-client.h control-layer-client.h
cp udp-echo-client.cc control-layer-client.cc
cp udp-echo-server.h control-layer-server.h
cp udp-echo-server.cc control-layer-server.cc

sed -i 's/UdpEcho/ControlLayer/g' control-layer-client.h
sed -i 's/UdpEcho/ControlLayer/g' control-layer-client.cc
sed -i 's/UdpEcho/ControlLayer/g' control-layer-server.h
sed -i 's/UdpEcho/ControlLayer/g' control-layer-server.cc

sed -i 's/UDP_ECHO/CONTROL_LAYER/g' control-layer-client.h
sed -i 's/UDP_ECHO/CONTROL_LAYER/g' control-layer-client.cc
sed -i 's/UDP_ECHO/CONTROL_LAYER/g' control-layer-server.h
sed -i 's/UDP_ECHO/CONTROL_LAYER/g' control-layer-server.cc

sed -i 's/udp-echo/control-layer/g' control-layer-client.h
sed -i 's/udp-echo/control-layer/g' control-layer-client.cc
sed -i 's/udp-echo/control-layer/g' control-layer-server.h
sed -i 's/udp-echo/control-layer/g' control-layer-server.cc

sed -i 's/udpecho/controllayer/g' control-layer-client.h
sed -i 's/udpecho/controllayer/g' control-layer-client.cc
sed -i 's/udpecho/controllayer/g' control-layer-server.h
sed -i 's/udpecho/controllayer/g' control-layer-server.cc

sed -i 's/Udp Echo/Control Layer/g' control-layer-client.h
sed -i 's/Udp Echo/Control Layer/g' control-layer-client.cc
sed -i 's/Udp Echo/Control Layer/g' control-layer-server.h
sed -i 's/Udp Echo/Control Layer/g' control-layer-server.cc

cd $WORKDIR/src/applications

sed -i -e '/udp-echo-client.h/p' -e '0,/udp-echo-client.h/s//control-layer-client.h/' wscript
sed -i -e '/udp-echo-client.cc/p' -e '0,/udp-echo-client.cc/s//control-layer-client.cc/' wscript
sed -i -e '/udp-echo-server.h/p' -e '0,/udp-echo-server.h/s//control-layer-server.h/' wscript
sed -i -e '/udp-echo-server.cc/p' -e '0,/udp-echo-server.cc/s//control-layer-server.cc/' wscript

cd $WORKDIR
./waf build > controllayer.log 2>&1

cd $WORKDIR/src/applications/helper

cp udp-echo-helper.h control-layer-helper.h
cp udp-echo-helper.cc control-layer-helper.cc

sed -i 's/UdpEcho/ControlLayer/g' control-layer-helper.h
sed -i 's/UdpEcho/ControlLayer/g' control-layer-helper.cc

sed -i 's/UDP_ECHO/CONTROL_LAYER/g' control-layer-helper.h
sed -i 's/UDP_ECHO/CONTROL_LAYER/g' control-layer-helper.cc

sed -i 's/udp-echo/control-layer/g' control-layer-helper.h
sed -i 's/udp-echo/control-layer/g' control-layer-helper.cc

sed -i 's/udpecho/controllayer/g' control-layer-helper.h
sed -i 's/udpecho/controllayer/g' control-layer-helper.cc

sed -i 's/Udp Echo/Control Layer/g' control-layer-helper.h
sed -i 's/Udp Echo/Control Layer/g' control-layer-helper.cc

cd $WORKDIR/src/applications

sed -i -e '/udp-echo-helper.h/p' -e '0,/udp-echo-helper.h/s//control-layer-helper.h/' wscript
sed -i -e '/udp-echo-helper.cc/p' -e '0,/udp-echo-helper.cc/s//control-layer-helper.cc/' wscript

cd $WORKDIR
./waf > controllayer-helper.log 2>&1

cd $WORKDIR/examples/udp

cp udp-echo.cc control-layer.cc

sed -i 's/UdpEcho/ControlLayer/g' control-layer.cc
sed -i 's/UDP_ECHO/CONTROL_LAYER/g' control-layer.cc
sed -i 's/udp-echo/control-layer/g' control-layer.cc
sed -i 's/udpecho/controllayer/g' control-layer.cc
sed -i 's/Udp Echo/Control Layer/g' control-layer.cc

sed -i -e '/program/{N;p}' -e '0,/udp-echo/s//control-layer/' -e '0,/udp-echo.cc/s//control-layer.cc/' wscript

cd $WORKDIR
./waf > controllayer-example.log 2>&1
