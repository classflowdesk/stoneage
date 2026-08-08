FROM ubuntu:24.04

RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
        build-essential \
        default-libmysqlclient-dev \
        mariadb-client \
        netcat-openbsd \
        zlib1g-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /opt/stoneage
COPY vendor/stone-age-2.5/ ./

RUN find saac gmsv -name makefile -type f \
        -exec sed -i 's/CFLAGS=-O3 -w /CFLAGS=-O3 -w -fcommon /' {} + \
    && make -C saac clean \
    && make -C gmsv clean \
    && make -C saac \
    && make -C gmsv \
    && mkdir -p /state/char /state/char_sleep /state/log /state/mail \
        /state/data/family /state/data/fmpointdir /state/data/fmsmemodir \
        /state/data/wklog /state/gmsv-log

COPY config/acserv.cf /opt/stoneage/saac/acserv.cf
COPY config/setup.cf /opt/stoneage/gmsv/setup.cf
COPY scripts/start-saac.sh /usr/local/bin/start-saac
COPY scripts/start-gmsv.sh /usr/local/bin/start-gmsv

RUN chmod +x /usr/local/bin/start-saac /usr/local/bin/start-gmsv
