# Stage 1: Base image and essential dependencies
FROM ubuntu:mantic AS base

RUN apt-get -y update
RUN apt-get -y install gnupg curl
RUN echo "deb http://apt.llvm.org/mantic/ llvm-toolchain-mantic-17 main" \
    >> /etc/apt/sources.list.d/llvm.list
RUN echo "deb http://apt.llvm.org/mantic/ llvm-toolchain-mantic-18 main" \
    >> /etc/apt/sources.list.d/llvm.list

RUN curl -sSL https://apt.llvm.org/llvm-snapshot.gpg.key -o /etc/apt/trusted.gpg.d/apt.llvm.org.asc

# Install required dependencies
RUN apt-get -y update \
    && apt-get -y install build-essential vim \
                           pandoc \
                           git \
                           ccache \
                           libboost-all-dev \
                           libdpdk-dev \
                           ca-certificates \
                           lsb-release \
                           wget \
                           clang-18 \
                           clang++-18

# Set up compiler alternatives
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 13 \
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 13 \
    && update-alternatives --install /usr/bin/clang clang /usr/bin/clang-18 18 \
    && update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-18 18

# Stage 2: Arrow repository setup and installation
FROM base AS arrow

# Set up Arrow repository and install Arrow dependencies
RUN wget https://apache.jfrog.io/artifactory/arrow/$(lsb_release --id --short | tr 'A-Z' 'a-z')/apache-arrow-apt-source-latest-$(lsb_release --codename --short).deb
RUN apt -y update && apt install -y -V ./apache-arrow-apt-source-latest-$(lsb_release --codename --short).deb
RUN apt -y update && apt install -y -V libarrow-dev \
                      libarrow-glib-dev \
                      libarrow-dataset-dev \
                      libarrow-dataset-glib-dev \
                      libarrow-acero-dev \
                      libarrow-flight-dev \
                      libarrow-flight-glib-dev \
                      libarrow-flight-sql-dev \
                      libarrow-flight-sql-glib-dev \
                      libgandiva-dev \
                      libgandiva-glib-dev \
                      libparquet-dev \
                      libparquet-glib-dev

# Stage 3: Seastar setup and installation
FROM arrow AS seastar

# Install Seastar Debug
RUN git clone https://github.com/googom/seastar.git /seastar

WORKDIR /seastar

RUN git submodule update --init --recursive

RUN bash install-dependencies.sh

# Run cooking script with additional parameters
RUN bash cooking.sh -i dpdk -t Release -- -DCMAKE_CXX_STANDARD=20 -DCMAKE_CXX_FLAGS="-fcoroutines"

# Build Seastar
RUN ninja -C build install

WORKDIR /

# Final Stage: Consolidating stages into a final image
FROM seastar AS final

WORKDIR /



