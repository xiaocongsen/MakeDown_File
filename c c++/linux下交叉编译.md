# linux交叉编译

## 安装编译工具

### ubuntu

- amd64
    sudo apt install gcc-mips64-linux-gnu g++-mips64-linux-gnu
    > 编译openssl
    ./Configure linux-generic64 --cross-compile-prefix=x86_64-linux-gnu- no-shared no-asm

- arm64
    sudo apt-get install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu
    > 编译openssl
    ./Configure linux-generic64 --cross-compile-prefix=aarch64-linux-gnu- no-shared no-asm

- mips64
    sudo apt-get install gcc-mips64el-linux-gnuabi64 g++-mips64el-linux-gnuabi64
    > 编译openssl
    ./Configure linux-generic64 --cross-compile-prefix=mips64el-linux-gnuabi64- no-shared no-asm
