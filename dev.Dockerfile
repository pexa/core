# Build stage for BerkeleyDB
FROM alpine as berkeleydb

RUN apk --no-cache add autoconf automake build-base

ENV BERKELEYDB_VERSION=db-4.8.30.NC
ENV BERKELEYDB_PREFIX=/opt/${BERKELEYDB_VERSION}

RUN wget https://download.oracle.com/berkeley-db/${BERKELEYDB_VERSION}.tar.gz
RUN tar -xzf *.tar.gz
RUN sed s/__atomic_compare_exchange/__atomic_compare_exchange_db/g -i ${BERKELEYDB_VERSION}/dbinc/atomic.h
RUN mkdir -p ${BERKELEYDB_PREFIX}

WORKDIR /${BERKELEYDB_VERSION}/build_unix

RUN ../dist/configure --enable-cxx --disable-shared --with-pic --prefix=${BERKELEYDB_PREFIX}
RUN make -j$(nproc)
RUN make install
RUN rm -rf ${BERKELEYDB_PREFIX}/docs

# Build stage for Pexa Core
FROM alpine as pexa-core

COPY --from=berkeleydb /opt /opt

RUN apk --no-cache add autoconf \
  automake \
  boost-dev \
  build-base \
  chrpath \
  file \
  gnupg \
  libevent-dev \
  libtool \
  linux-headers \
  protobuf-dev \
  zeromq-dev \
  cmake

RUN set -ex \
  && for key in \
    90C8019E36C2E964 \
  ; do \
    gpg --batch --keyserver keyserver.ubuntu.com --recv-keys "$key" || \
    gpg --batch --keyserver pgp.mit.edu --recv-keys "$key" || \
    gpg --batch --keyserver keyserver.pgp.com --recv-keys "$key" || \
    gpg --batch --keyserver ha.pool.sks-keyservers.net --recv-keys "$key" || \
    gpg --batch --keyserver hkp://p80.pool.sks-keyservers.net:80 --recv-keys "$key" ; \
  done

ENV PEXA_PREFIX=/opt/pexa

COPY . /pexa

WORKDIR /pexa

# Install alt-integration-cpp
RUN export VERIBLOCK_POP_CPP_VERSION=$(awk -F '=' '/\$\(package\)_version/{print $NF}' $PWD/depends/packages/veriblock-pop-cpp.mk | head -n1); \
    (\
     cd /opt; \
     wget https://github.com/VeriBlock/alt-integration-cpp/archive/${VERIBLOCK_POP_CPP_VERSION}.tar.gz; \
     tar -xf ${VERIBLOCK_POP_CPP_VERSION}.tar.gz; \
     cd alt-integration-cpp-${VERIBLOCK_POP_CPP_VERSION}; \
     mkdir build; \
     cd build; \
     cmake .. -DCMAKE_BUILD_TYPE=Debug -DTESTING=OFF; \
     make -j$(nproc) install \
    )

RUN ./autogen.sh
RUN ./configure LDFLAGS=-L`ls -d /opt/db*`/lib/ CPPFLAGS=-I`ls -d /opt/db*`/include/ \
    --disable-tests \
    --disable-bench \
    --disable-gmock \
    --disable-ccache \
    --disable-man \
    --without-gui \
    --with-libs=no \
    --with-daemon \
    --prefix=${PEXA_PREFIX}

RUN make -j$(nproc) install

# Build stage for compiled artifacts
FROM alpine

RUN apk --no-cache add \
  boost \
  boost-program_options \
  libevent \
  libzmq \
  su-exec \
  valgrind \
  curl

ENV DATA_DIR=/home/pexa/.pexa
ENV PEXA_PREFIX=/opt/pexa
ENV PATH=${PEXA_PREFIX}/bin:$PATH

COPY --from=pexa-core /opt /opt

RUN mkdir -p ${DATA_DIR}
RUN set -x \
    && addgroup -g 1001 -S pexa \
    && adduser -u 1001 -D -S -G pexa pexa
RUN chown -R 1001:1001 ${DATA_DIR}
USER pexa
WORKDIR $DATA_DIR
