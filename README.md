# Gradido Blockchain
lib for all gradido blockchain related stuff in c++

git submodule update --init --recursive

## Dependencies
### Windows
- conan 
- rust



## Build with Conan
```bash`
cd gradido_blockchain
conan install . --output-folder=build --build=missing
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

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
## Install for Windows