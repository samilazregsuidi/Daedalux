FROM mcr.microsoft.com/devcontainers/cpp:latest

# Install additional packages
RUN sudo apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
    autoconf   \
    automake   \
    libtool-bin   \
    libtool   \
    g++        
 