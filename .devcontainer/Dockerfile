FROM mcr.microsoft.com/devcontainers/base:bookworm


# Install C/C++ tools
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
    build-essential \
    gdb \
    cmake \
    valgrind \
    && apt-get autoremove -y && apt-get clean -y && rm -rf /var/lib/apt/lists/*