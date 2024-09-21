要在 Windows 上将上述 C++ 代码编译成适用于 Raspberry Pi 4B 的可执行文件，您需要使用 **交叉编译**（cross-compilation）。这意味着在 Windows 上编译出适用于不同架构（在这种情况下是 Raspberry Pi 4B 的 ARM 架构）的二进制文件。以下是具体步骤。

## 步骤 1：安装交叉编译工具链

为了在 Windows 上进行交叉编译，您可以使用 **CMake** 作为构建系统，并配置适用于 ARM 的 **GCC 交叉编译工具链**。可以通过 **WSL2 (Windows Subsystem for Linux)** 安装这些工具，或直接使用一些交叉编译环境。

我们将使用 **WSL2** 和 **交叉编译工具链**。如果您不想使用 WSL2，也可以直接在 Windows 上安装交叉编译器，但 WSL2 提供了更加接近 Linux 环境的体验。

### 1.1 安装 WSL2

1. 打开 PowerShell 以管理员身份运行，并输入以下命令以安装 WSL 和 Ubuntu 发行版：

    ```powershell
    wsl --install
```

2. 安装完 WSL 后，重新启动计算机。

3. 打开 WSL Ubuntu 终端，更新并安装必要的工具：
```bash
sudo apt update 
sudo apt upgrade
```

### 1.2 安装交叉编译工具链

在 WSL Ubuntu 中，安装适用于 Raspberry Pi 4B 的交叉编译工具链。

1. 安装 ARM 交叉编译工具链：

```bash
sudo apt-get install g++-aarch64-linux-gnu
```

2. 安装 CMake：

```bash
sudo apt install cmake
```

## 步骤 2：交叉编译arrch64的Boost
### 1. 下载 Boost 源码

你可以从 Boost 的官方网站下载最新的源码包：

```bash
wget https://boostorg.jfrog.io/artifactory/main/release/1.78.0/source/boost_1_78_0.tar.gz tar -xzf boost_1_78_0.tar.gz cd boost_1_78_0
```

### 2. 配置 Boost

使用 `b2` 工具来配置 Boost，并为 aarch64 编译。首先需要运行 bootstrap.sh 生成 b2 可执行文件：

```bash
./bootstrap.sh
```

然后，使用 `b2` 编译 Boost：

```bash
CC=/usr/bin/aarch64-linux-gnu-gcc CXX=/usr/bin/aarch64-linux-gnu-g++ ./b2 --prefix=/home/chi/boost install

```
- `CC` 显示指定完整的gcc路径
- `CXX` 显示指定完整的g++路径
- `--prefix` 指定 Boost 安装路径，确保这个路径存在且具有写权限。

## 步骤 3：交叉编译arrch64的nlohmann_json

### 1. 下载 nlohmann_json 源码

首先，你可以从 GitHub 上下载 `nlohmann_json` 的源代码：

```bash
git clone https://github.com/nlohmann/json.git 
cd json
```

### 2. 创建一个交叉编译工具链文件

在项目目录下创建一个工具链文件，例如 `aarch64-toolchain.cmake`，内容如下：

```cmake
set(CMAKE_SYSTEM_NAME Linux) 
set(CMAKE_SYSTEM_PROCESSOR aarch64)  

set(CMAKE_C_COMPILER /usr/bin/aarch64-linux-gnu-gcc) 
set(CMAKE_CXX_COMPILER /usr/bin/aarch64-linux-gnu-g++)
```

### 3. 编译 nlohmann_json

使用 CMake 编译 `nlohmann_json`：

```bash
mkdir build 
cd build 
cmake .. -DCMAKE_TOOLCHAIN_FILE=../aarch64-toolchain.cmake  -DCMAKE_INSTALL_PREFIX=/home/chi/nlohmann_json
make
```

### 4. 安装 nlohmann_json

如果编译成功，可以使用以下命令安装：

```bash
make install
```

## 步骤4：交叉编译arrch64的OpenSSL
### 1. 下载并安装 OpenSSL 3.0.0+

从 OpenSSL 的官方网站下载最新版本（3.0.0 或更高）：

```bash
wget https://www.openssl.org/source/openssl-3.0.0.tar.gz tar -xzf openssl-3.0.0.tar.gz cd openssl-3.0.0
```

### 2. 配置并编译 OpenSSL

为 `aarch64` 进行编译：

```bash
./Configure linux-aarch64 --prefix=/home/chi/OpenSSL_aarch64 
make  
make install
```

这会将 OpenSSL 安装到 `/home/chi/OpenSSL_aarch64`。