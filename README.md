# Gradido Blockchain
lib for all gradido blockchain related stuff in c++

## Build with Conan
```bash`
cd gradido_blockchain
conan install . --output-folder=build --build=missing
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