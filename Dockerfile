##### BUILD-ENV #####
FROM gcc:14 as gcc14_build

RUN apt-get update && \
    apt-get install -y --no-install-recommends cmake libssl-dev libsodium-dev lcov && \
	apt-get autoclean && \
	apt-get autoremove && \
    apt-get clean && \
	rm -rf /var/lib/apt/lists/*

RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y

##### BUILD debug #######
FROM gcc14_build as debug_build

ENV DOCKER_WORKDIR="/code"
WORKDIR ${DOCKER_WORKDIR}

COPY . ${DOCKER_WORKDIR}

RUN mkdir build
WORKDIR ${DOCKER_WORKDIR}/build
RUN cmake .. && cmake . && make -j$(nproc) GradidoBlockchain
