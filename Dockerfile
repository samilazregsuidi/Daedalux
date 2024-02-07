# # Stage 0 : Fetch daikon
# FROM amazoncorretto:latest as daikon-fetcher

# # Install wget
# RUN yum install -y wget tar gzip

# WORKDIR /usr/src/daikon
# RUN wget http://plse.cs.washington.edu/daikon/download/daikon-5.8.18.tar.gz
# RUN tar zxf daikon-5.8.18.tar.gz

# # Stage 1: Build Daikon
# FROM amazoncorretto:latest as daikon-builder

# # Install build tools
# RUN yum install -y make git graphviz netpbm-progs texlive texinfo ctags perl zlib1g-dev gcc
# # Install aclocal
# RUN yum install -y automake autoconf libtool zlib-devel binutils-devel

# # Copy daikon from the daikon-fetcher stage
# WORKDIR /usr/src/daikon
# COPY --from=daikon-fetcher /usr/src/daikon/daikon-5.8.18 /usr/src/daikon/daikon-5.8.18
# # Add DAIKONDIR to ENV
# ENV DAIKONDIR=/usr/src/daikon/daikon-5.8.18
# # RUN source $DAIKONDIR/scripts/daikon.bashrc
# # Build Daikon
# RUN make -C daikon-5.8.18 rebuild-everything

# # Update perl 
# RUN yum update -y perl
# # Install perl modules
# RUN yum install -y perl-CPAN
# # Install cpanm
# RUN yum install -y perl-App-cpanminus
# # Install Text::CSV
# RUN cpanm Text::CSV
# RUN perl -MCPAN -e'install Text::CSV'
# # Reload perl modules
# RUN source $DAIKONDIR/scripts/daikon.bashrc

# WORKDIR /usr/local/lib64/perl5
# RUN yum install -y wget
# RUN wget https://raw.githubusercontent.com/plume-lib/html-tools/master/checkargs.pm

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

# Build Daedalux
WORKDIR /usr/src/daedalux/build
RUN cmake ..
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
FROM fedora:latest as final

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

# Copy the models into the image
COPY ./models /usr/src/daedalux/models

# Copy the test_files into the image
COPY ./test_files /usr/src/daedalux/test_files

# Copy the python scripts
COPY --from=app-builder /usr/src/daedalux/test_scripts /usr/src/daedalux/python_scripts

# Copy the daikon jar
# COPY --from=daikon-builder /usr/src/daikon/daikon-5.8.18/daikon.jar /usr/src/daedalux

# Copy your application from the app-builder stage
COPY --from=app-builder /usr/src/daedalux/build/ /usr/src/daedalux/

# Set working directory
WORKDIR /usr/src/daedalux

# Set entry point or CMD as needed
# CMD ["./daedalux"]
ENTRYPOINT ["tail", "-f", "/dev/null"]

# Label the image
LABEL Name=daedalux Version=0.0.1


# Stage 5: Test image
# FROM final as test

# CMD [ "" ]
