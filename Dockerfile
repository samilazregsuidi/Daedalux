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

# Stage 3: Final image
FROM fedora:latest

# Copy CUDD from the cudd-builder stage
COPY --from=cudd-builder /usr/src/cudd-cudd-3.0.0 /usr/src/cudd-cudd-3.0.0

# Copy your application from the app-builder stage
COPY --from=app-builder /usr/src/daedalux/build/daedalux /usr/src/daedalux/build/

# Set working directory
WORKDIR /usr/src/daedalux/build

# Set entry point or CMD as needed
# CMD ["./daedalux"]
ENTRYPOINT ["tail", "-f", "/dev/null"]

# Label the image
LABEL Name=daedalux Version=0.0.1