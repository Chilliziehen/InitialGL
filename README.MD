# OpenGL 初始环境

本项目旨在帮助开发者在 Windows 上快速搭建可用的 OpenGL+CMake 开发环境，并集成常用依赖：GLEW、GLFW、stb_image、GLM。

## 目录结构

```
InitialGL/
├─ CMakeLists.txt              # CMake 构建脚本（已接入 Dependencies）
├─ main.cpp                    # 示例入口（控制台输出）
├─ Dependencies/               # 本地三方依赖（免安装）
│  ├─ GLEW/
│  │  ├─ include/GL/*.h        # glew.h 等头文件
│  │  └─ lib/                  # glew32s.lib（静态），glew32.lib（动态）
│  ├─ GLFW/
│  │  ├─ include/GLFW/*.h
│  │  └─ lib-vc2022/           # glfw3.lib（/MD 静态），glfw3_mt.lib（/MT 静态），glfw3dll.lib 等
│  └─ OTHER/
│     ├─ IMAGE/stb_image.h     # stb_image 头文件
│     └─ MATH/                 # GLM 头文件（glm.hpp、gtc/、gtx/…）
└─ cmake-build-*/              # 构建输出目录（由 CMake 生成）
```

## 已接入的依赖

- GLEW：用于加载 OpenGL 扩展；默认以静态库方式链接（glew32s.lib），并定义 GLEW_STATIC。
- GLFW：跨平台窗口与上下文创建；默认链接静态库 glfw3.lib（与 /MD 运行库匹配）。
- stb_image：头文件库，用于图像加载（仅包含目录）。
- GLM：头文件库，OpenGL 风格的数学库（仅包含目录）。

注意：当前仓库中提供的二进制库为 MSVC/Windows 预编译版本，无法直接用于 MinGW。若使用 MinGW，请参考下文“MinGW 构建”。

## 环境准备

- 操作系统：Windows 10/11
- 构建工具：CMake（建议 3.26+），可选 Ninja
- 编译器（两选一）：
  - Visual Studio 2022（MSVC 工具集）
  - MinGW-w64（需自行获取对等库或用包管理器安装）

## 构建与运行

默认 CMake 已将 Dependencies 添加到包含与链接路径中：
- 头文件包含：GLEW/include、GLFW/include、OTHER/IMAGE（stb_image）、OTHER/MATH（GLM）
- MSVC 下链接：glew32s.lib、glfw3.lib 以及 opengl32、user32、gdi32、shell32 等系统库

推荐使用 MSVC 构建（开箱即用）：

- Visual Studio 生成器
  1) 生成工程：
     ```pwsh
     cmake -S . -B cmake-build-msvc -G "Visual Studio 17 2022" -A x64
     ```
  2) 编译：
     ```pwsh
     cmake --build cmake-build-msvc --config Debug
     ```
  3) 运行：
     ```pwsh
     .\cmake-build-msvc\Debug\InitialGL.exe
     ```

- Ninja + MSVC（在“x64 Native Tools for VS 2022”环境中）
  1) 生成：
     ```bat
     cmake -S . -B cmake-build-msvc -G Ninja
     ```
  2) 编译与运行：
     ```bat
     cmake --build cmake-build-msvc
     .\cmake-build-msvc\InitialGL.exe
     ```

### MinGW 构建（可选）

仓库内的 .lib 为 MSVC 版本，MinGW 无法直接使用。若使用 MinGW，请：
- 通过 MSYS2/MinGW 包管理安装对应库，例如：glfw、glew、glm、stb
- 或手动从源码编译获得 .a 静态库
- 然后在 CMakeLists.txt 的非 MSVC 分支中，添加 MinGW 对应的链接库（如 -lglfw3、-lGLEW、-lopengl32 等）

当前示例 main.cpp 不依赖 OpenGL API，因此即使不链接上述库也可编译运行。但在你的代码中开始使用 OpenGL/GLFW/GLEW 接口前，请完成 MinGW 链接配置。

## 在代码中使用依赖

- 头文件示例：
  ```cpp
  #include <GL/glew.h>
  #include <GLFW/glfw3.h>
  #include <stb_image.h>
  #include <glm/glm.hpp>
  ```
- 初始化顺序提示（MSVC 静态 GLEW）：
  1) 调用 glfwInit 并创建窗口与上下文
  2) 调用 glewInit 以加载函数指针（需在上下文有效后）

## 运行库与切换说明

- 默认链接 glfw3.lib（/MD）。若你的项目使用 /MT 运行库，请：
  - 将 CMake 中 GLFW 库切换为 glfw3_mt.lib
  - 或统一配置工程的运行库为 /MD
- GLEW 默认使用 glew32s.lib（静态）。如需改用动态（glew32.lib）：
  - 移除/关闭 GLEW_STATIC 宏
  - 部署同目录下的 glew32.dll 到可执行文件旁或系统路径

## 常见问题（FAQ）

- LNK2038 运行库不匹配（/MD vs /MT）：确保使用与工程一致的 GLFW 库（glfw3.lib 或 glfw3_mt.lib），或统一运行库设置。
- 找不到 OpenGL 符号：确保链接了 opengl32.lib（MSVC）或 -lopengl32（MinGW）。
- IDE 报找不到头文件：请通过 CMake 打开工程而非单文件编译；包含与链接路径已在 CMakeLists.txt 中配置。
- MinGW 不能用 MSVC 的 .lib：需安装 MinGW 版本的库或自行编译生成 .a，并在 CMake 非 MSVC 分支中链接。

## 维护与扩展

- 依赖根目录：CMake 中以 `${CMAKE_SOURCE_DIR}/Dependencies` 约定
- 若更新/替换依赖：保持 include 与 lib 目录结构或同步更新 CMake 路径
- 建议将渲染/初始化逻辑拆分到独立源文件，并逐步增加单元测试

---

如需我为 MinGW 增补完整链接分支或添加一段最小可运行的 GLFW+GLEW 初始化示例，请告知你的目标编译器与期望运行库（/MD 或 /MT）。
