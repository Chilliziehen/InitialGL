# InitialGL — OpenGL 学习与示例工程（Windows/MSVC）

本项目提供一个开箱即用的 OpenGL + CMake + MSVC 环境，已集成常用依赖：GLEW、GLFW、stb_image、GLM，并内置最小示例与资源/着色器路径配置。

## 许可证

- 本项目使用 MIT 许可证，详见根目录 LICENSE。
- 所含第三方依赖遵循其各自的开源许可证（见文末“依赖与链接”）。

## 项目结构

```
2025_autumn_cg/
├─ CMakeLists.txt
├─ LICENSE
├─ README.MD
├─ vcpkg.json                       # 可选；当前未声明依赖（使用本地 Dependencies）
├─ vcpkg-configuration.json         # 可选；如需启用 vcpkg
├─ config/
│  ├─ config.h.in                   # CMake configure_file 模板（写入资源与 GLSL 路径）
│  └─ config.h                      # 由 CMake 生成（当前配置写回源码目录，亦可生成到构建目录）
├─ Dependencies/                    # 预编译/头文件依赖（MSVC/Windows）
│  ├─ GLEW/
│  │  ├─ include/GL/*.h
│  │  └─ lib/glew32s.lib            # 静态库（与 GLEW_STATIC 搭配）
│  ├─ GLFW/
│  │  ├─ include/GLFW/*.h
│  │  └─ lib-vc2022/glfw3.lib       # /MD 运行库版本（若用 /MT，请改为 glfw3_mt.lib）
│  └─ OTHER/
│     ├─ IMAGE/stb_image.h
│     ├─ MATH/                      # GLM 头文件（glm.hpp、gtc/、gtx/、detail/）
│     └─ MODEL/                     # 示例模型或资源
├─ src/
│  ├─ cxx/
│  │  ├─ main/
│  │  │  ├─ main.cpp
│  │  │  ├─ camera/Camera.cpp
│  │  │  ├─ program/Program.cpp
│  │  │  ├─ shader/VertexShader.cpp
│  │  │  ├─ shader/FragmentShader.cpp
│  │  │  ├─ model/Model.cpp
│  │  │  ├─ util/Utilities.cpp
│  │  │  └─ call_back/CallBack.{h,cpp}
│  │  ├─ Resources/                 # 运行期资源（纹理、模型等）
│  │  └─ test/                      # 测试/示例（可选）
│  └─ glsl/
│     ├─ vertex.vsh
│     ├─ fragment.fsh
│     ├─ gPass/                     # 几何通道着色器（若有）
│     └─ lPass/                     # 光照通道着色器（若有）
└─ cmake-build-*/                   # 构建输出目录（由 CMake 生成）
```

## 构建与部署

- 操作系统：Windows 10/11（64 位）
- 工具链：
  - CMake 4.0+（建议 4.1+）
  - Visual Studio 2022（MSVC 工具集）或“x64 Native Tools for VS 2022”控制台
  - 可选 Ninja 构建器
- 图形环境：建议 GPU 驱动支持 OpenGL 3.3+。

推荐使用 MSVC（已提供预编译 .lib，开箱即用）：

- 方式 A：Visual Studio 生成器
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

- 方式 B：Ninja + MSVC（在“x64 Native Tools for VS 2022”环境中）
  1) 生成：
     ```bat
     cmake -S . -B cmake-build-msvc -G Ninja
     ```
  2) 编译与运行：
     ```bat
     cmake --build cmake-build-msvc
     .\cmake-build-msvc\InitialGL.exe
     ```

提示：本仓库的 Dependencies 下第三方库为 MSVC/Windows 预编译版本，不能直接被 MinGW 使用。

### 使用 MinGW（可选）

若要使用 MinGW-w64，请通过 MSYS2 安装或从源码编译得到对应的 .a 库，并在 CMakeLists.txt 的非 MSVC 分支中配置：
- 包含路径：GLFW、GLEW、stb、GLM 头文件
- 链接库：-lglfw3、-lGLEW、-lopengl32 等（以实际安装为准）

## 配置参数（config）

CMake 会把资源与着色器根路径写入一个头文件，供运行时代码引用：

- 模板文件：config/config.h.in
- 生成文件：config/config.h（当前脚本写回源码目录）

可用宏：
- RESOURCE_PATH：资源根目录，默认 "${CMAKE_SOURCE_DIR}/src/cxx/Resources"
- GLSL_PATH：着色器根目录，默认 "${CMAKE_SOURCE_DIR}/src/glsl"

如何使用：
```cpp
#include "config.h"
#include <string>

std::string tex = std::string(RESOURCE_PATH) + "/textures/xxx.png";
std::string vsh = std::string(GLSL_PATH) + "/vertex.vsh";
```

如何修改：
- 直接在 CMakeLists.txt 顶部修改：
  - set(RESOURCE_PATH "${CMAKE_SOURCE_DIR}/src/cxx/Resources")
  - set(GLSL_PATH "${CMAKE_SOURCE_DIR}/src/glsl")
- 重新 CMake 配置并编译；config.h 将被重新生成。

注意：把生成文件写回源码目录虽便于 include，但也可改为写入构建目录并通过 target_include_directories 暴露给目标。

## 运行时注意

- 使用 glew32s.lib 时已自动定义 GLEW_STATIC；初始化顺序需先创建有效的 OpenGL 上下文（GLFW），再调用 glewInit。
- GLFW 默认链接的是 /MD 运行库版本的 glfw3.lib；若工程改为 /MT，请切换为 glfw3_mt.lib 并全局统一运行库。
- Windows 下还会链接 opengl32、user32、gdi32、shell32、winmm 等系统库（已在 CMake 中配置）。

## 依赖与链接（第三方）

- GLEW — The OpenGL Extension Wrangler Library（BSD-3-Clause）
  - https://github.com/nigels-com/glew
  - 许可：https://github.com/nigels-com/glew/blob/master/LICENSE.txt
- GLFW — A multi-platform library for OpenGL（zlib/libpng License）
  - https://www.glfw.org/
  - https://github.com/glfw/glfw
  - 许可：https://github.com/glfw/glfw/blob/master/LICENSE.md
- stb_image.h（Public Domain or MIT）
  - https://github.com/nothings/stb
  - 许可：https://github.com/nothings/stb#license
- GLM — OpenGL Mathematics（MIT License）
  - https://github.com/g-truc/glm
  - 许可：https://github.com/g-truc/glm/blob/master/copying.txt

可选/保留位：
- KHR/EGL 头文件（如后续引入 eglew.h 或 EGL 环境，可在 CMake 中添加 KHR_INCLUDE_DIR/EGL_INCLUDE_DIR）。

## 常见问题

- LNK2038 或运行库不一致（/MD vs /MT）：请统一工程运行库并选择匹配的 GLFW 静态库文件。
- 无法解析 OpenGL 符号：确保链接 opengl32.lib（MSVC）或 -lopengl32（MinGW）。
- 找不到头文件：请通过 CMake 打开工程（而非单文件编译）；包含路径已由 target_include_directories 配置。

## 贡献

欢迎以 PR 的形式提交改进（示例、工具链适配、CMake 优化、文档修正等）。在提交前请确认构建通过并遵循项目许可证。
