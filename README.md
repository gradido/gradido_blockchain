# Gradido Blockchain
lib for all gradido blockchain related stuff in c++

git submodule update --init --recursive

## Dependencies
- cmake 
- rust

## Build with CMake in Release Mode
Without parameter is version without test, http and iota rust client

```bash
cd gradido_blockchain
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

- ENABLE_IOTA_RUST_CLIENT: switch for include rust iota client into build, needed for sending transaction to iota
- ENABLE_HTTP: switch for include http based classes into build, will be enabled if ENABLE_IOTA_RUST_CLIENT is enabled
- ENABLE_TEST: switch for include test into build config

## Install for Linux
```bash
mkdir build
cd build
cmake ..
# build protoc 
make -j$(nproc) protoc
# parse protoc to c++
cmake ..
# build lib
make -j$(nproc) GradidoBlockchain
```
