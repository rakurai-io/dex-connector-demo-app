FROM ubuntu:22.04

LABEL Name=rakurai-dex-connector-demo Version=0.0.1 Maintainer=saad@rakurai.io

ARG DEBIAN_FRONTEND=noninteractive
# Install dependencies
RUN apt-get update
RUN apt-get install tzdata -y
RUN apt-get install --no-install-recommends apt-utils -y
RUN apt-get install vim nano -y
RUN apt-get install build-essential cmake cmake-curses-gui -y
RUN apt-get install git-all -y
RUN apt-get install doxygen python3-sphinx graphviz -y
RUN apt-get install python3 python3-pip python3-venv -y && ln -sf /usr/bin/python3 /usr/bin/python
RUN apt-get install libgtest-dev -y
RUN apt-get install libssl-dev -y
RUN apt-get install libasio-dev -y
RUN apt-get install zlib1g-dev -y
RUN apt-get install net-tools -y
RUN apt-get install gdb gdbserver -y
RUN apt-get install pkg-config -y
RUN apt-get install curl -y
RUN apt-get install libsodium-dev -y
RUN apt-get install libpq-dev -y


WORKDIR /dex-connector-demo-app
