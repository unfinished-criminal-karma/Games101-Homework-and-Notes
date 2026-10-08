/* Windows 兼容层：MSVC 里没有 POSIX 的 <unistd.h>。
 *
 * main.cpp 用了 `#include <unistd.h>` + getopt()/optarg/optind，
 * 这里把它转发到 GLFW 自带的那份 getopt 实现
 * （CGL/deps/glfw/deps/getopt.h + getopt.c，BSD 许可），
 * 这样作业原文件一个字都不用改。
 *
 * 两个要点：
 *   1) getopt.c 是 C 文件、按 C 链接编译，所以这里必须用 extern "C" 包住声明，
 *      否则 C++ 端按名字修饰去找符号会链接失败。
 *   2) 这个目录只在 WIN32 构建时被 CMake 加进 include 路径，
 *      Linux/macOS 上不会遮蔽系统的 unistd.h。
 */
#pragma once

#ifdef _WIN32

#ifdef __cplusplus
extern "C" {
#endif

#include "getopt.h"

#ifdef __cplusplus
}
#endif

#endif /* _WIN32 */
