FROM ubuntu:18.04
MAINTAINER Gamze Gursoy

RUN apt-get update && apt-get install -y \
	build-essential \
	python3.7 \
	cmake	\
	git \
	libgtk2.0-dev \
	pkg-config \
	libavcodec-dev \
	libavformat-dev \
	libswscale-dev\
	libtbb2 \
	libtbb-dev \
	libjpeg-dev \
	libpng-dev \
	libtiff-dev \
	libeigen3-dev \
	liblapack-dev \
	libatlas-base-dev \
	libgomp1 \
	vim  

ENV NAME VAR1
ENV NAME VAR2
COPY run_idash.sh /run_idash.sh
COPY Gerstein_MoMA /idash
WORKDIR /idash/
RUN make compile
WORKDIR / 
CMD ["/bin/sh", "./run_idash.sh"]
