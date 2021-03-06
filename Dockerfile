# Build stage for BerkeleyDB
FROM alpine as berkeleydb

RUN sed -i 's/http\:\/\/dl-cdn.alpinelinux.org/https\:\/\/alpine.global.ssl.fastly.net/g' /etc/apk/repositories
RUN apk --no-cache add autoconf automake build-base cmake libressl

ENV BERKELEYDB_VERSION=db-4.8.30.NC
ENV BERKELEYDB_PREFIX=/opt/${BERKELEYDB_VERSION}

RUN wget https://download.oracle.com/berkeley-db/${BERKELEYDB_VERSION}.tar.gz
RUN tar -xzf *.tar.gz
RUN sed s/__atomic_compare_exchange/__atomic_compare_exchange_db/g -i ${BERKELEYDB_VERSION}/dbinc/atomic.h
RUN mkdir -p ${BERKELEYDB_PREFIX}

WORKDIR /${BERKELEYDB_VERSION}/build_unix

RUN ../dist/configure --enable-cxx --disable-shared --with-pic --prefix=${BERKELEYDB_PREFIX}
RUN make -j4
RUN make install
RUN rm -rf ${BERKELEYDB_PREFIX}/docs

# Build stage for Pexa Core
FROM alpine as pexa-core

COPY --from=berkeleydb /opt /opt

RUN sed -i 's/http\:\/\/dl-cdn.alpinelinux.org/https\:\/\/alpine.global.ssl.fastly.net/g' /etc/apk/repositories
RUN apk --no-cache add autoconf \
  automake \
  boost-dev \
  build-base \
  chrpath \
  file \
  gnupg \
  libevent-dev \
  libressl \
  libressl-dev \
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

ENV PEXA_VERSION=1.7.0
ENV PEXA_PREFIX=/opt/pexa-${PEXA_VERSION}

COPY . /pexa-${PEXA_VERSION}

WORKDIR /pexa-${PEXA_VERSION}

RUN sed -i '/AC_PREREQ/a\AR_FLAGS=cr' src/univalue/configure.ac
RUN sed -i '/AX_PROG_CC_FOR_BUILD/a\AR_FLAGS=cr' src/secp256k1/configure.ac
RUN sed -i s:sys/fcntl.h:fcntl.h: src/compat.h
RUN ./autogen.sh
RUN ./configure LDFLAGS=-L`ls -d /opt/db*`/lib/ CPPFLAGS=-I`ls -d /opt/db*`/include/ \
    --prefix=${PEXA_PREFIX} \
    --mandir=/usr/share/man \
    --disable-tests \
    --disable-bench \
    --disable-ccache \
    --with-gui=no \
    --with-utils \
    --with-libs \
    --with-daemon
RUN make -j4
RUN make install
RUN strip ${PEXA_PREFIX}/bin/pexa-cli
RUN strip ${PEXA_PREFIX}/bin/pexad
RUN strip ${PEXA_PREFIX}/lib/libpexaconsensus.a
RUN strip ${PEXA_PREFIX}/lib/libpexaconsensus.so.0.0.0

# Build stage for compiled artifacts
FROM alpine

LABEL maintainer.0="Ryan Hein (@ryanmhein)"

RUN sed -i 's/http\:\/\/dl-cdn.alpinelinux.org/https\:\/\/alpine.global.ssl.fastly.net/g' /etc/apk/repositories
RUN apk --no-cache add \
  boost \
  boost-program_options \
  libevent \
  libressl \
  libzmq \
  su-exec \
  curl

ENV DATA_DIR=/home/pexa/.pexa
ENV PEXA_VERSION=1.7.0
ENV PEXA_PREFIX=/opt/pexa-${PEXA_VERSION}
ENV PATH=${PEXA_PREFIX}/bin:$PATH
ENV DOCKERIZE_VERSION v0.6.1

RUN wget https://github.com/jwilder/dockerize/releases/download/$DOCKERIZE_VERSION/dockerize-alpine-linux-amd64-$DOCKERIZE_VERSION.tar.gz \
    && tar -C /usr/local/bin -xzvf dockerize-alpine-linux-amd64-$DOCKERIZE_VERSION.tar.gz \
    && rm dockerize-alpine-linux-amd64-$DOCKERIZE_VERSION.tar.gz

COPY ./.docker/config /tmp
COPY --from=pexa-core /opt /opt
COPY docker-entrypoint.sh /entrypoint.sh

RUN mkdir -p ${DATA_DIR}
RUN set -x \
    && addgroup -g 1001 -S pexa \
    && adduser -u 1001 -D -S -G pexa pexa
RUN chown -R 1001:1001 ${DATA_DIR}
USER pexa
WORKDIR $DATA_DIR

EXPOSE 8235 8769 18235 2300

ENTRYPOINT ["/entrypoint.sh"]`

CMD ["pexad"]