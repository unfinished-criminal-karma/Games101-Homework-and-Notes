cmake_minimum_required(VERSION 3.15)
project(BezierCurve CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ==================== 只需要改这一处路径 ====================
# OpenCV：解压 opencv-4.x.x-windows.exe 后得到的那个 opencv 目录，指向它的 build
set(OpenCV_DIR "G:/OpenCV/opencv/build")
# =============================================================
# 注：本作业不用 Eigen（main.cpp 只 include OpenCV），所以没配 EIGEN_ROOT

find_package(OpenCV REQUIRED)
if(NOT OpenCV_FOUND)
    message(FATAL_ERROR "找不到 OpenCV，检查上面的 OpenCV_DIR")
endif()

message(STATUS "OpenCV 版本   : ${OpenCV_VERSION}")
message(STATUS "OpenCV 头文件 : ${OpenCV_INCLUDE_DIRS}")

add_executable(BezierCurve
    main.cpp
        main.cpp
        main.cpp
)

target_include_directories(BezierCurve PRIVATE
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(BezierCurve PRIVATE ${OpenCV_LIBS})

# 让 exe 生成在 build/ 而不是 build/Release/
# （`my_bezier_curve.png` 是相对路径写出的，工作目录就是 exe 所在目录）
set_target_properties(BezierCurve PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY         "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}"
)

# Windows：编译完只把 opencv_world 运行时 DLL 拷到 exe 旁边（不拷 PDB，省几百 MB）
if(WIN32)
    file(GLOB OPENCV_RUNTIME_DLLS CONFIGURE_DEPENDS "${OpenCV_DIR}/x64/vc16/bin/opencv_world*.dll")
    add_custom_command(TARGET BezierCurve POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${OPENCV_RUNTIME_DLLS}
                "$<TARGET_FILE_DIR:BezierCurve>"
        COMMENT "拷贝 opencv_world*.dll"
    )
endif()
