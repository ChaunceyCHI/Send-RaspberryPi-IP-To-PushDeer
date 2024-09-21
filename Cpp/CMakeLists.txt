add_definitions(-DCPPHTTPLIB_OPENSSL_SUPPORT)

cmake_minimum_required(VERSION 3.10)
project(sendIpToPushdeer)

set(CMAKE_CXX_STANDARD 20)

# 指定目标系统类型
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# 指定交叉编译的编译器
set(CMAKE_C_COMPILER /usr/bin/aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER /usr/bin/aarch64-linux-gnu-g++)

# 设置交叉编译工具链的根目录
set(CMAKE_FIND_ROOT_PATH /usr/aarch64-linux-gnu)

# 确保 CMake 优先在目标架构的路径中查找库
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# 设置 Boost 路径
set(BOOST_ROOT /home/chi/boost)
set(Boost_INCLUDE_DIR ${BOOST_ROOT}/include)
set(Boost_LIBRARY_DIR ${BOOST_ROOT}/lib)
set(CMAKE_PREFIX_PATH ${BOOST_ROOT} ${NLOHMANN_JSON})
# 查找 Boost
find_package(Boost REQUIRED)
# 包含 Boost 头文件
include_directories(${Boost_INCLUDE_DIR})

# 设置 nlohmann_json 路径
include_directories(${NLOHMANN_JSON}/include)

# 设置 OpenSSL 安装路径
set(OPENSSL_ROOT_DIR /home/chi/OpenSSL_aarch64)
set(OPENSSL_INCLUDE_DIR /home/chi/OpenSSL_aarch64/include)
set(OPENSSL_SSL_LIBRARY /home/chi/OpenSSL_aarch64/lib/libssl.a)  # 明确设置 OpenSSL SSL 库路径
set(OPENSSL_CRYPTO_LIBRARY /home/chi/OpenSSL_aarch64/lib/libcrypto.a)  # 明确设置 OpenSSL Crypto 库路径
# 查找 OpenSSL
find_package(OpenSSL REQUIRED)

# 启用 cpp-httplib 的 SSL 支持
set(CPPHTTPLIB_OPENSSL_SUPPORT ON)
set(HTTPLIB_USE_SSL TRUE)
add_subdirectory(include/cpp_httplib)  # 假设 cpp-httplib 在 include/cpp_httplib 目录下

# 引入头文件
include_directories(include/cpp_httplib)

# 添加源代码
add_executable(sendIpToPushdeer sendIpToPushdeer.cpp)

# 链接库
target_link_libraries(sendIpToPushdeer 
    Boost::boost 
    pthread 
    OpenSSL::SSL 
    OpenSSL::Crypto 
    httplib::httplib  # 链接 cpp-httplib
)
