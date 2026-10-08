# GAMES101 作业记录

闫令琪老师《现代计算机图形学入门》（GAMES101）的作业实现与学习笔记。

## 目录结构

| 目录 | 内容 |
|---|---|
| **`原题/`** | 课程官方发布的原始作业包（未改动的代码框架 + 作业 PDF），从官网下载的 zip 解出，只做了目录摊平。⚠️ **hw3 例外** —— 官方包里没有 Assignment3，来源见 [`原题/hw3/说明.md`](原题/hw3/说明.md) |
| **`我的作业/`** | 我在框架上完成的实现，以及移植到 Windows / MSVC 的 `CMakeLists.txt`（官方原文件保留为 `CMakeLists.txt.linux-original`） |
| **`笔记/`** | 学习笔记 `Games101_Homework.md` 与配图 |

## 各次作业

| 编号 | 内容 | 原题 | 我的作业 |
|---|---|---|---|
| hw1 | 旋转与投影矩阵（含提高项：绕任意轴旋转） | ✅ | ✅ |
| hw2 | 三角形光栅化 + Z-Buffer（含提高项：MSAA） | ✅ | ✅ |
| hw3 | 管线与着色：Blinn-Phong / 纹理 / bump / displacement（含提高项：双线性插值） | ✅ [第三方来源](原题/hw3/说明.md) | ✅ |
| hw4 | Bézier 曲线（de Casteljau，含提高项：反走样） | ✅ | ✅ |
| hw5 | 光线与三角形求交（Möller–Trumbore） | ✅ | ✅ |
| hw6 | BVH 加速（含提高项：SAH） | ✅ | ✅ |
| hw7 | 路径追踪（含作业要求的多线程渲染） | ✅ | ✅ |
| hw8 | 质点弹簧绳子模拟（显式/半隐式欧拉、Verlet、全局阻尼） | ✅ | ✅ |

> ⚠️ **hw3 的原题不是官方包**：课程发放的作业包只有 pa0、Assignment1、2、4、5、PA6、PA7-1、assignment8，唯独缺 Assignment3。
> `原题/hw3` 取自 Gitee 上的第三方仓库，实现代码**已全部清空、只保留官方 TODO 注释**，另对 `Texture.hpp` 的纹理坐标边界检查
> 做了一处修复（框架原版用 Debug 跑 `texture` 会断言崩溃）。完整的来源、与原版框架的差异、以及附带中文文档的性质，
> 都写在 **[`原题/hw3/说明.md`](原题/hw3/说明.md)** 里。

## 环境

- **Windows** + **Visual Studio 2022**（MSVC 19.38）+ **CMake 3.15 以上**
- 各作业的外部依赖不同：
  - `hw1` ~ `hw4`：**OpenCV**（`hw3` 另需 **Eigen**）
  - `hw5` ~ `hw7`：无外部依赖，框架自带 OBJ 读取；`hw7` 用 **OpenMP** 做多线程
  - `hw8`：**CGL** 图形库（GLEW / GLFW 的源码已随作业包附带）+ **FreeType**

> ⚠️ 各 `CMakeLists.txt` 里的第三方库路径是我本机的绝对路径（OpenCV、Eigen、FreeType），换机器需要改成你自己的。
> `hw7` / `hw8` 的路径分别在各自顶层的 `CMakeLists.txt` 里。

## 构建与运行

每个作业流程相同（以 hw7 为例）：

```powershell
cmake -S "我的作业/hw7" -B "我的作业/hw7/build" -G "Visual Studio 17 2022" -A x64
cmake --build "我的作业/hw7/build" --config Release
```

| 作业 | 运行方式 |
|---|---|
| `hw1` ~ `hw3` | 命令行：`Rasterizer.exe <输出图.png> [shader名]` |
| `hw4` | 交互窗口，鼠标点四个控制点后自动输出曲线图 |
| `hw5` ~ `hw7` | 直接运行 `RayTracing.exe`，输出 `binary.ppm`（工作目录要用 `build/`，模型走 `../models/` 相对路径） |
| `hw8` | 直接运行 `ropesim.exe`，弹出 OpenGL 窗口显示两条绳子 |

## 说明

- 作业框架、讲义与 `Assignment*.pdf` 的版权归 **GAMES101 课程组**所有，本仓库仅作个人学习记录，未用于任何商业用途。
- 仓库中**不包含任何第三方的作业答案**（`.gitignore` 里也排除了 `*.answer`）。
- 提交历史里的每一版都是我自己的实现，欢迎参考思路，但请自己动手完成作业。
