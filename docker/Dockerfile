FROM ubuntu:14.04

MAINTAINER Iakov Kirilenko <Iakov.Kirilenko@trikset.com>
RUN apt update
RUN apt -y full-upgrade
RUN apt install software-properties-common -y
RUN add-apt-repository ppa:beineri/opt-qt563-trusty -y
RUN apt update
RUN apt install -y rsync build-essential g++-4.8 pkg-config libboost-system-dev libboost-wave-dev \
        tcl vera++ xvfb mesa-common-dev libglu1-mesa-dev libpng-dev zlib1g-dev xbitmaps \
        xterm python2.7-dev git-core

#RUN apt-get install -y qdbus qmlscene qt5-qmake \
#        qtbase5-dev-tools qtchooser qtdeclarative5-dev libqt5svg5-dev \
#    qtscript5-dev qt5-default qtbase5-private-dev qtmultimedia5-dev

RUN apt install -y qt56-meta-minimal qt56svg qt56script qt56multimedia
#RUN apt install -y python-pip
#RUN pip install --user codecov
RUN echo 'source /opt/qt56/bin/qt56-env.sh' >> /root/.bashrc
ENV QTDIR="/opt/qt56"
ENV PATH="${QTDIR}/bin:${PATH}"
#RUN ln -svt /etc/profile.d/ /opt/qt56/bin/qt56-env.sh
#ENTRYPOINT ["/bin/bash", "-lc"]
