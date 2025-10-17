# Gradido Blockchain

lib for all gradido blockchain related stuff in c++

git submodule update --init --recursive

## Dependencies

- cmake
- zig

## Install in Debian 12

```bash
sudo apt install libsodium-dev libssl-dev
```

## Build with CMake in Release Mode

Without parameter is version without test, http and iota rust client

```bash
cd gradido_blockchain
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

## test and https on windows

```bash
cd gradido_blockchain
mkdir build
cd build
cmake --preset windows-x64 -DENABLE_TEST=ON -DENABLE_HTTPS=ON ..
ctest
```

on windows, vcpkg (submodule) is used to download and build openssl for https support

- ENABLE_IOTA_RUST_CLIENT: switch for include rust iota client into build, needed for sending transaction to iota
- ENABLE_HTTP: switch for include http based classes into build, will be enabled if ENABLE_IOTA_RUST_CLIENT is enabled
- ENABLE_TEST: switch for include test into build config
- ENABLE_HTTPS: switch for include openssl for https support

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

## Cross Compile for arm64 on Linux

```bash
mkdir build_arm64 && cd build_arm64
cmake .. -DTARGET=aarch64-linux-gnu
make -j$(nproc) GradidoBlockchain
```
