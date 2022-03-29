FROM trikset/checker-runtime
MAINTAINER Iakov Kirilenko <Iakov.Kirilenko@trikset.com>
ENV DEBIAN_FRONTEND noninteractive

ENV TRIK_GCCVERSION 9
ENV TRIK_QTIFW_VERSION 4.0.1
ENV TRIK_LIBUSB_TAG v1.0.24
ENV TRIK_QTVERSION 5.12.10

RUN bash -uc 'echo ${TRIK_PYTHON3_VERSION_MINOR}'
# Build image with two huge layers, this is the 1st one.
# Do not upgrade packages to match trikset/checker-runtime
RUN apt-get update \
    && apt-get install --no-install-recommends -y lsb-release software-properties-common \
    && add-apt-repository -y ppa:jonathonf/gcc \
    && add-apt-repository -y ppa:ubuntu-toolchain-r/test \
    && add-apt-repository -y ppa:git-core/ppa \
    && add-apt-repository -y ppa:beineri/opt-qt-${TRIK_QTVERSION}-$(lsb_release -sc) \
    && apt-get update \
    && apt-get install -y --no-install-recommends \
		    wget curl rsync sshpass openssh-client build-essential g++-${TRIK_GCCVERSION} gdb \
		    python3.${TRIK_PYTHON3_VERSION_MINOR}-dev python3.${TRIK_PYTHON3_VERSION_MINOR}-venv \
		    pkg-config libboost-system-dev libboost-wave-dev \
                tcl vera++ xvfb mesa-common-dev libgl1-mesa-dri \
		libglu1-mesa-dev libpng-dev \
		zlib1g-dev xbitmaps xterm git ccache \
#qt
		qt512base qt512multimedia qt512script qt512svg qt512tools qt512wayland qt512serialport \
#for libusb build
                automake libtool-bin libudev-dev \
#for TRIK Studio build and tests
                dbus libxkbcommon-x11-0 libxcb-xkb1 libpulse-dev \
#for PythonQt
                realpath \
#set gcc/g++ version ${TRIK_GCCVERSION} as default
    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-${TRIK_GCCVERSION} 60 --slave /usr/bin/g++ g++ /usr/bin/g++-${TRIK_GCCVERSION} \
#Cleanup after installation
    && apt-get purge -y software-properties-common \
    && apt-get autoremove -y \
    && apt-get clean \
    && rm -rf /opt/qt512/examples \
            /var/lib/apt/lists/* \
            /var/cache/debconf/* \
            /usr/share/doc \
            /usr/share/man \
            /usr/share/locale/?? \
            /usr/share/locale/??_?? \
&& { [ -r /etc/machine-id ] || { dbus-uuidgen | tee /etc/machine-id ; } ; }

#Build fresh libusb from sources
RUN git clone --single-branch --depth 1 --branch ${TRIK_LIBUSB_TAG} https://github.com/libusb/libusb \
    && cd libusb \
    && ./autogen.sh --enable-udev --disable-examples-build --disable-tests-build --disable-static \
    && make install -j 4 \
    && cd .. \
    && rm -rf libusb \
    && echo pkg-config libusb-1.0 is v$(pkg-config --modversion libusb-1.0)

#ENV QTDIR "/Qt"
#ENV QTBIN "${QTDIR}/5.12.7/gcc_64/bin"
#ENV QTBIN /opt/qt512/bin
#COPY qt_scriptinstall.qs /tmp
#COPY install_qt.sh /tmp
# Split docker image into two main layers, this is the second one
#RUN /tmp/install_qt.sh $QTDIR \
# Cleanup once again
#    && rm -rf /tmp/* \
#    && rm -rf $QTDIR/Docs $QTDIR/Examples $QTDIR/Tools/QtCreator \
#              $QTDIR/MaintenanceTool* \
#	      $QTBIN/qgltf \
#	      $QTBIN/qdoc \
#	      $QTBIN/linguist \
#	      $QTBIN/assistant \
#	      $QTBIN/designer \
#	      $QTBIN/../lib/*.a

RUN ln -svt /etc/profile.d/ /opt/qt512/bin/qt512-env.sh
ENV BASH_ENV /etc/profile
ADD qt_scriptinstall.qs /tmp/
RUN curl -L -o /tmp/qtifw.run https://download.qt.io/official_releases/qt-installer-framework/${TRIK_QTIFW_VERSION}/QtInstallerFramework-linux-x64.run \
    && chmod +x /tmp/qtifw.run \
    && env TRIK_QT_INSTALL_DIR=/opt/qtifw /tmp/qtifw.run -platform minimal --no-force-installations --verbose --script /tmp/qt_scriptinstall.qs \
    && rm  /tmp/qtifw.run \
    && find /opt/qtifw/ -type f -maxdepth 1 -delete -print \
    && rm -rf /opt/qtifw/{doc,examples,installerResources}/

#Build QtIFW from sources
#RUN git clone --single-branch --depth 1 --branch ${TRIK_QTIFW_TAG} https://github.com/qtproject/installer-framework.git qtifw \
#    && cd qtifw \
#    && qmake CONFIG+=release CONFIG+=static \
#    && make install -j 4 \
#    && cd .. \
#    && rm -rf qtifw

#ENV PATH "${QTBIN}:/usr/lib/ccache:${PATH}"
#RUN python3 -m pip install --upgrade pip codecov

RUN echo Internal image disk usage: $(du -sch /* 2>/dev/null | sort -h)
ENTRYPOINT ["/bin/bash", "-c"]
CMD ["bash"]
