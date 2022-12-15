FROM alpine

RUN apk add clang wget tar make build-base

RUN apk add linux-headers
WORKDIR /
RUN wget http://files.portaudio.com/archives/pa_stable_v190700_20210406.tgz
RUN tar -xvf pa_stable_v190700_20210406.tgz
WORKDIR /portaudio
RUN ./configure
RUN make
RUN make install

RUN apk add ncurses-dev zlib-dev bzip2-dev
RUN apk add ffmpeg-libs ffmpeg-dev
RUN apk add ffmpeg
RUN apk add bash
ADD . /asciiplayer
