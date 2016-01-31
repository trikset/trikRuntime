FROM ubuntu:15.10

MAINTAINER Yurii Litvinov <yurii.litvinov@gmail.com>

RUN apt-get update
RUN apt-get install build-essential -y
RUN apt-get install git -y
RUN apt-get install libboost-system-dev libboost-wave-dev tcl vera++ -y
RUN apt-get install xvfb -y
RUN apt-get install mesa-common-dev libglu1-mesa-dev -y
RUN apt-get install qdbus qmlscene qt5-qmake qtbase5-dev-tools qtchooser qtdeclarative5-dev xbitmaps xterm libqt5svg5-dev qtscript5-dev qt5-default -y
RUN Xvfb :0 &
RUN export DISPLAY=:0
RUN cd ~
