FROM ubuntu:18.04

LABEL maintainer="Karl Syvert Løland <karl@trollcode.no>"

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update \
    && apt-get -y install --no-install-recommends apt-utils 2>&1 \
    # 
    # Verify git, process tools, lsb-release (useful for CLI installs) installed
    && apt-get -y install git procps lsb-release \
    #
    # Install Clang tools
    && apt-get -y install clang build-essential gdb gnu-efi lld mtools lldb make\
    #Install Tools to build qemu
    && apt-get -y install wget gcc g++ zlib1g-dev pkg-config libglib2.0-dev \
        binutils-dev libboost-all-dev autoconf libtool libssl-dev libpixman-1-dev \
        libpython-dev libgtk-3-dev libsdl2-dev \
    # Download QEMU
    && cd /tmp/ \
    && wget https://download.qemu.org/qemu-4.0.0.tar.xz \
    && tar -xf qemu-4.0.0.tar.xz \
    && cd qemu-4.0.0 \
    # Build QEMU
    && ./configure --enable-sdl \
    && make \
    && make install \
    # Clean up
    && cd .. \
    && rm -rf qemu-4.0.0 \
    && apt-get remove -y gcc g++ zlib1g-dev pkg-config libglib2.0-dev \
        binutils-dev libboost-all-dev autoconf libtool libssl-dev libpixman-1-dev \
        libpython-dev libgtk-3-dev libsdl2-dev \
    # Add runtime versions of libs
    && apt-get -y install libxml2 libpulse0 libsdl2-2.0-0 libgtk-3-0 libnuma1 \
    && apt-get autoremove -y \
    && apt-get clean -y \
    && rm -rf /var/lib/apt/lists/*

ENV DEBIAN_FRONTEND=dialog
ENV SHELL /bin/bash