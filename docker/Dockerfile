FROM ubuntu:xenial
MAINTAINER Iakov Kirilenko <Iakov.Kirilenko@trikset.com>
ENV DEBIAN_FRONTEND noninteractive
# Build image with two huge layers, this is the 1st one.
RUN apt-get update \
    && apt-get -y upgrade \
    && apt-get install --no-install-recommends -y software-properties-common \
    && add-apt-repository -y ppa:jonathonf/gcc \
    && add-apt-repository -y ppa:ubuntu-toolchain-r/test \
    && apt-get update && apt -y upgrade \
    && apt-get install -y --no-install-recommends \
		    wget curl rsync sshpass openssh-client build-essential g++-8 gdb python3-dev \
		    pkg-config libboost-system-dev libboost-wave-dev \
                tcl vera++ xvfb mesa-common-dev libgl1-mesa-dri \
		libglu1-mesa-dev libpng-dev \
		zlib1g-dev xbitmaps xterm git-core ccache \
#for libusb build
                automake libtool-bin libudev-dev \
#for TRIK Studio build and tests
                dbus libxkbcommon-x11-0 libxcb-xkb1 libpulse-dev \
#for PythonQt
                realpath \
#set gcc/g++ version 8 as default
    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 60 --slave /usr/bin/g++ g++ /usr/bin/g++-8 \
#Cleanup after installation
    && apt-get purge -y software-properties-common \
    && apt-get autoremove -y \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* \
            /var/cache/debconf/* \
            /usr/share/doc \
            /usr/share/man \
            /usr/share/locale/?? \
            /usr/share/locale/??_?? \
&& { [ -r /etc/machine-id ] || { dbus-uuidgen | tee /etc/machine-id ; } ; }

#Build fresh libusb from sources
RUN git clone https://github.com/libusb/libusb && cd libusb && git checkout v1.0.22 && ./autogen.sh --enable-udev --disable-examples-build --disable-tests-build --disable-static && make install && cd .. && rm -rf libusb && echo pkg-config libusb-1.0 is v$(pkg-config --modversion libusb-1.0)

ENV QTDIR "/Qt"
ENV QTBIN "${QTDIR}/5.12.6/gcc_64/bin"
COPY qt_scriptinstall.qs /tmp
COPY install_qt.sh /tmp
# Split docker image into two main layers, this is the second one
RUN /tmp/install_qt.sh $QTDIR \
# Cleanup once again
    && rm -rf /tmp/* \
    && rm -rf $QTDIR/Docs $QTDIR/Examples $QTDIR/Tools/QtCreator \
              $QTDIR/MaintenanceTool* \
	      $QTBIN/qgltf \
	      $QTBIN/qdoc \
	      $QTBIN/linguist \
	      $QTBIN/assistant \
	      $QTBIN/designer \
	      $QTBIN/../lib/*.a


ENV PATH "${QTBIN}:/usr/lib/ccache:${PATH}"
ENV bash -ic 'python3 -m pip install --upgrade pip codecov'

RUN echo Internal image disk usage: $(du -sch /* 2>/dev/null | sort -h)
#RUN ln -svt /etc/profile.d/ /opt/qt56/bin/qt56-env.sh
#ENTRYPOINT ["/bin/bash", "-lc"]
