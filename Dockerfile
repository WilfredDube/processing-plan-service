FROM frolvlad/alpine-gxx

# Install dependences
RUN apk add --no-cache make cmake \
    build-base \
    boost boost-dev \
    tcl \
    freetype \
    tcl-dev freetype-dev tk tk-dev

WORKDIR /opt/
COPY ./third_party/OCCT /opt/
RUN mkdir -p build \
    && cd build \
    && cmake .. \
    && make \
    && make install \
    && make clean \
    && rm -rf *

# RUN mkdir WtApp && cd WtApp
WORKDIR /Fxtract
COPY ./* /Fxtract/

# RUN mkdir -p third_party \
#     && cd third_party \
#     && git clone https://github.com/Open-Cascade-SAS/OCCT.git \
#     && ls \
#     && cd OCCT \
#     && mkdir -p build \
#     && cd build \
#     && cmake .. \
#     && make \
#     && make install \
#     && make clean


# COPY ./third_party/wt /opt
# RUN git submodule update --init --recursive
# RUN apk add --no-cache boost-dev
# RUN cd wt && mkdir build && cd build && cmake ../ && make && make install && make clean
# RUN rm -rf wt
# These commands copy your files into the specified directory in the image
# and set that as the working location
# COPY ./third_party/opencascade-7.4.0.tgz /opt/
# WORKDIR /opt/docker_test
# COPY ./hello-docker.cpp /opt/docker_test
# COPY ./Makefile /opt/docker_test
# COPY ./CMakeLists.txt /opt/docker_test

# This command compiles your app using GCC, adjust for your source code
# RUN g++ -o hello-docker hello-docker.cpp -lwthttp -lwt

# This command runs your application, comment out this line to compile only
# CMD ["./hello-docker"]

# LABEL Name=hellogrpc Version=0.0.1
