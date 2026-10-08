set(GLEW_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(GLEW_SOURCE_DIR ${GLEW_ROOT_DIR}/src)
set(GLEW_HEADER_DIR ${GLEW_ROOT_DIR}/include)

include_directories(${GLEW_HEADER_DIR})
add_library(glew 
${GLEW_SOURCE_DIR}/glew.c
${GLEW_SOURCE_DIR}/glewinfo.c
${GLEW_SOURCE_DIR}/visualinfo.c)

# [Windows 移植] 静态链接：不定义 GLEW_STATIC 的话，glew.h 会把 API 声明成 __declspec(dllimport)，
# 使用侧就去找 __imp_glewInit 这类 DLL 导入符号，而这里编的是静态库 → LNK2019。
if(MSVC)
  target_compile_definitions(glew PRIVATE GLEW_STATIC)
endif()

set(GLEW_LIBRARIES glew PARENT_SCOPE)
set(GLEW_INCLUDE_DIRS ${GLEW_HEADER_DIR} PARENT_SCOPE)

install(TARGETS glew DESTINATION lib)
install(DIRECTORY ${GLEW_HEADER_DIR}/GL DESTINATION include)
