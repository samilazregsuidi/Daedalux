# GCC support can be specified at major, minor, or micro version
# (e.g. 8, 8.2 or 8.2.0).
# See https://hub.docker.com/r/library/gcc/ for all supported GCC
# tags from Docker Hub.
# See https://docs.docker.com/samples/library/gcc/ for more on how to use this image
FROM gcc:latest

# Copy everyfile in the current directory to the image
ADD . /usr/src/daedalux

# Install any needed packages specified requirements

# Flex and Bison
RUN apt-get update && apt-get install -y flex bison

# CUDD
RUN apt-get update && apt-get install -y wget

WORKDIR /usr/src
RUN wget https://github.com/ivmai/cudd/archive/refs/tags/cudd-3.0.0.tar.gz
RUN tar -xvf cudd-3.0.0.tar.gz
# Delete tar.gz file
RUN rm cudd-3.0.0.tar.gz
WORKDIR /usr/src/cudd-cudd-3.0.0
RUN ./configure --enable-shared --enable-dddmp --enable-obj --enable-const --enable-arith --enable-epd --enable-cuddlib --enable-silent-rules --enable-verbose
RUN make
RUN make check

# CMAKE
RUN apt-get update && apt-get install -y cmake

# # # These commands copy your files into the specified directory in the image
# # # and set that as the working location
WORKDIR /usr/src/daedalux/build
# This command compiles your app using GCC, adjust for your source code
RUN cmake ../
RUN cmake --build . 

# # This command runs your application, comment out this line to compile only
CMD ["./daedalux"] 

LABEL Name=daedalux Version=0.0.1

# ENTRYPOINT ["tail", "-f", "/dev/null"]