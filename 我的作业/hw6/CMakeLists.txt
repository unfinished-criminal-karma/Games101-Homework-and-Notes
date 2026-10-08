# GAMES101 作业 6 · BVH 加速（Windows / MSVC 版）
# 官方原文件已备份为 CMakeLists.txt.linux-original
cmake_minimum_required(VERSION 3.15)
project(RayTracing CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 本作业不用 OpenCV / 不用 Eigen：模型走自带的 OBJ_Loader.hpp，输出仍是 binary.ppm
add_executable(RayTracing
    main.cpp
    Vector.cpp
    Scene.cpp
    BVH.cpp
    Renderer.cpp
)

# exe 输出到 build/ 根下 —— main.cpp 里是 "../models/bunny/bunny.obj"，
# 工作目录必须是 build/ 才找得到模型
set_target_properties(RayTracing PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY         "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}"
)

if(MSVC)
    # /utf-8：MSVC 默认按 GBK(ACP=936) 读源码，中文注释的末字节可能吞掉下一行代码
    target_compile_options(RayTracing PRIVATE /utf-8 /EHsc)
elseif(MINGW)
    # MinGW-w64 不带 libubsan：少了这条会链接失败（ld: cannot find -lubsan）
    target_compile_options(RayTracing PRIVATE -Wall -Wextra -pedantic -Wshadow -Wreturn-type)
else()
    # 官方原始 flag（Linux / GCC）
    target_compile_options(RayTracing PRIVATE -Wall -Wextra -pedantic -Wshadow -Wreturn-type -fsanitize=undefined)
    target_link_libraries(RayTracing PRIVATE -fsanitize=undefined)
endif()
