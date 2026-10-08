cmake_minimum_required(VERSION 3.15)
project(Rasterizer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ==================== 只需要改下面两处路径 ====================
# 1) Eigen：直接复用 UE 源码里自带的（纯头文件，无需安装）
set(EIGEN_ROOT "F:/epic/UE_5.7/Engine/Source/ThirdParty/Eigen")

# 2) OpenCV：解压后的 opencv 目录下的 build
set(OpenCV_DIR "G:/OpenCV/opencv/build")
# =============================================================

find_package(OpenCV REQUIRED)

message(STATUS "OpenCV version : ${OpenCV_VERSION}")
message(STATUS "Eigen root     : ${EIGEN_ROOT}")

add_executable(Rasterizer
    main.cpp
    rasterizer.cpp
    Triangle.cpp
)

target_include_directories(Rasterizer PRIVATE
    ${EIGEN_ROOT}
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(Rasterizer PRIVATE ${OpenCV_LIBS})

# exe 直接生成在 build/，工作目录与输出目录统一
set_target_properties(Rasterizer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY         "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}"
)

# Windows：编译完只把 opencv_world 运行时 DLL 拷到 exe 旁边（不拷 PDB，省几百 MB）
if(WIN32)
    file(GLOB OPENCV_RUNTIME_DLLS CONFIGURE_DEPENDS "${OpenCV_DIR}/x64/vc16/bin/opencv_world*.dll")
    add_custom_command(TARGET Rasterizer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${OPENCV_RUNTIME_DLLS}
                "$<TARGET_FILE_DIR:Rasterizer>"
        COMMENT "copy opencv_world*.dll"
    )
endif()
