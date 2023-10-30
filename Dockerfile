# Stage 4: Build Daikon
FROM amazoncorretto:latest as daikon-builder

# Install wget
# RUN yum install -y wget
# # Install Make
# RUN yum install -y make
# # Install tar
# RUN yum install -y tar

# WORKDIR /usr/src/daikon
# RUN wget http://plse.cs.washington.edu/daikon/download/daikon-5.8.18.tar.gz
# RUN tar -xvf daikon-5.8.18.tar.gz
# # Add DAIKONDIR to ENV
# RUN EXPORT DAIKONDIR=/usr/src/daikon/daikon-5.8.18
# RUN source $DAIKONDIR/scripts/daikon.bashrc
# # Build Daikon
# RUN make -C daikon-5.8.18 rebuild-everything

# Stage 1: Build CUDD
FROM gcc:latest as cudd-builder

WORKDIR /usr/src
RUN apt-get update && apt-get install -y wget
RUN wget https://github.com/ivmai/cudd/archive/refs/tags/cudd-3.0.0.tar.gz
RUN tar -xvf cudd-3.0.0.tar.gz
RUN rm cudd-3.0.0.tar.gz
WORKDIR /usr/src/cudd-cudd-3.0.0
RUN ./configure --enable-shared --enable-dddmp --enable-obj --enable-const --enable-arith --enable-epd --enable-cuddlib --enable-silent-rules --enable-verbose --enable-cplusplus 
RUN make
RUN make check

# Stage 2: Build your application
FROM gcc:latest as app-builder

# Copy everyfile in the current directory to the image
COPY . /usr/src/daedalux

WORKDIR /usr/src/daedalux

RUN apt-get update && apt-get install -y cmake

WORKDIR /usr/src/daedalux/build
RUN cmake ../
RUN cmake --build .

# Stage 3: SPIN build
FROM gcc:latest as spin-builder

# Install SPIN
RUN apt-get update 
RUN apt-get install build-essential

# Download SPIN
WORKDIR /usr/src/spin
RUN wget https://github.com/nimble-code/Spin/archive/refs/tags/version-6.5.2.tar.gz
RUN tar -xvf version-6.5.2.tar.gz

# Build SPIN
WORKDIR /usr/src/spin/Spin-version-6.5.2/Src
RUN apt-get install byacc flex -y
RUN make

# Stage 5: Final image
FROM fedora:latest

# Install Python 3.0
RUN dnf install -y python3
# Install Pip
RUN dnf install -y python3-pip
# Install Java
RUN dnf install -y java-11-openjdk-devel
# Install CPP
RUN dnf install -y gcc-c++

# Copy SPIN from the spin-builder stage
COPY --from=spin-builder /usr/src/spin/Spin-version-6.5.2/Src/spin /usr/bin/spin

# Copy CUDD from the cudd-builder stage
COPY --from=cudd-builder /usr/src/cudd-cudd-3.0.0 /usr/src/cudd-cudd-3.0.0

# Copy the models
COPY --from=app-builder /usr/src/daedalux/models /usr/src/daedalux/models

# Copy the python scripts
COPY --from=app-builder /usr/src/daedalux/test_scripts /usr/src/daedalux/python_scripts

# Copy the daikon jar
# COPY -from=daikon-builder /usr/src/daikon/daikon-5.8.18/daikon.jar /usr/src/daedalux/

# Copy your application from the app-builder stage
COPY --from=app-builder /usr/src/daedalux/build/daedalux /usr/src/daedalux/

# Set working directory
WORKDIR /usr/src/daedalux

# Set entry point or CMD as needed
# CMD ["./daedalux"]
ENTRYPOINT ["tail", "-f", "/dev/null"]

# Label the image
LABEL Name=daedalux Version=0.0.1