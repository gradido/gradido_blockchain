##### BUILD-ENV #####
FROM gcc:14 AS gcc14_build

RUN apt-get update && \
    apt-get install -y --no-install-recommends cmake libssl-dev libsodium-dev lcov && \
	  apt-get autoclean && \
	  apt-get autoremove && \
    apt-get clean && \
	  rm -rf /var/lib/apt/lists/*

RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y

##### BUILD debug #######
FROM gcc14_build AS debug_build

ENV DOCKER_WORKDIR="/code"
WORKDIR ${DOCKER_WORKDIR}

COPY ./src ./src 
COPY ./include ./include
COPY ./test ./test
COPY ./dependencies ./dependencies
COPY ./CMakeLists.txt ./CMakeLists.txt 
COPY ./packaging ./packaging
COPY ./cmake ./cmake 

RUN mkdir build
WORKDIR ${DOCKER_WORKDIR}/build
RUN cmake -DBUILD_SHARED_LIBS=Off .. && cmake . && make -j$(nproc) GradidoBlockchain
RUN ls -lah

# To keep the container running for debugging
# CMD ["sh"]

##### TEST debug #####
FROM debug_build AS test_debug

ENV DOCKER_WORKDIR="/code"
WORKDIR ${DOCKER_WORKDIR}/build
RUN cmake -DBUILD_SHARED_LIBS=Off .. && cmake . && make -j$(nproc) GradidoBlockchainTest

CMD ["ctest"]

