PROJ_ROOT="$(cd "`dirname "$0"`" && pwd)"
PROTO_PATH="${PROJ_ROOT}/proto"
PROTOBUF_PATH="${PROJ_ROOT}/3rd/protobuf"
LLBC_PATH="${PROJ_ROOT}/3rd/llbc"
PROTO_SRC_PATH="${PROTOBUF_PATH}/src"
PROTOC_PATH="${PROTO_SRC_PATH}/protoc"
LLBC_PATH="${PROJ_ROOT}/llbc"

# build protobuf lib function
build_protobuf() {
    echo "Building protobuf"
    git submodule update --init --recursive
    cd $PROTOBUF_PATH
    git checkout tags/v3.20.3
    ./autogen.sh
    ./configure
    make -j$(nproc)
    echo "Building protobuf done"
    cd -
}

# build llbc lib function
build_llbc() {
    # sudo yum install libuuid libuuid-devel
    echo "Building llbc"
    git submodule update --init --recursive
    cd $LLBC_PATH
    make core_lib -j$(nproc)
    echo "Building llbc done"
    cd -
}

# build proto
build_proto() {
    echo "Building proto"
    cd $PROTO_PATH 
    if [ ! -d "stub" ]; then
        mkdir stub
    fi
    ( $PROTOC_PATH  *.proto --cpp_out=./stub )
    echo "Building proto done"
}

# 根据参数调用相应的函数
if [[ ${1} == "protobuf" ]]; then 
    build_protobuf
    exit 0
elif [[ ${1} == "llbc" ]]; then 
    build_llbc
    exit 0
elif [[ ${1} == "proto" ]]; then 
    build_proto
    exit 0
else 
    echo "Usage: $0 |[protobuf|llbc|proto]"
    exit 1
fi