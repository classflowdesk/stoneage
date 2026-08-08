FROM ubuntu@sha256:019e8eb29a85e74d64925745884f2ec79aa27e3feab36353d24656f4d6b89467

RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
        build-essential \
        default-libmysqlclient-dev \
        gcc-multilib \
        libc6-dev-i386 \
        mariadb-client \
        netcat-openbsd \
        patch \
        zlib1g-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /opt/stoneage
COPY vendor/stone-age-2.5/ ./
COPY patches/ /tmp/patches/

RUN if ! grep -q 'static int \*QuizTable = NULL;' gmsv/npc/npc_quiz.c; then \
        patch -p1 </tmp/patches/npc-quiz-persistent-table.patch; \
    fi \
    && find saac -name makefile -type f \
        -exec sed -i -E 's/^CFLAGS=-O3 -w .*\$\(INCFLAGS\).*$/CFLAGS=-O3 -w -fcommon -fgnu89-inline -fno-stack-protector -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0 $(INCFLAGS)/' {} + \
    && find gmsv -name makefile -type f \
        -exec sed -i -E 's/^CFLAGS=-O3 -w .*\$\(INCFLAGS\).*$/CFLAGS=-O2 -m32 -w -fcommon -fgnu89-inline -fno-stack-protector -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0 -D_FILE_OFFSET_BITS=64 $(INCFLAGS)/' {} + \
    && sed -i 's/$(OBJ) $(LIBS) $(LDFLAGS)/$(OBJ) -Wl,--start-group $(LIBS) -Wl,--end-group $(LDFLAGS)/' gmsv/makefile \
    && make -C saac clean \
    && make -C gmsv clean \
    && (make -C saac || make -C saac || make -C saac) \
    && (make -C gmsv || make -C gmsv || make -C gmsv || make -C gmsv || make -C gmsv) \
    && mkdir -p /state/char /state/char_sleep /state/log /state/mail \
        /state/data/family /state/data/fmpointdir /state/data/fmsmemodir \
        /state/data/wklog /state/gmsv-log

COPY config/acserv.cf /opt/stoneage/saac/acserv.cf
COPY config/setup.cf /opt/stoneage/gmsv/setup.cf
COPY scripts/start-saac.sh /usr/local/bin/start-saac
COPY scripts/start-gmsv.sh /usr/local/bin/start-gmsv

RUN chmod +x /usr/local/bin/start-saac /usr/local/bin/start-gmsv
