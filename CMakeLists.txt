cmake_minimum_required(VERSION 4.0)
project(InitialGL)

set(CMAKE_CXX_STANDARD 20)

# 依赖根路径
set(DEPENDENCIES_DIR ${CMAKE_SOURCE_DIR}/Dependencies)
# 头文件路径
set(GLEW_INCLUDE_DIR ${DEPENDENCIES_DIR}/GLEW/include)
set(GLFW_INCLUDE_DIR ${DEPENDENCIES_DIR}/GLFW/include)
set(STB_IMAGE_INCLUDE_DIR ${DEPENDENCIES_DIR}/OTHER/IMAGE)
# GLM 放在 OTHER/MATH（其下含 glm.hpp、gtc、gtx、detail 等）
set(GLM_INCLUDE_DIR ${DEPENDENCIES_DIR}/OTHER/MATH)
# 新增 KHR 与 EGL 头文件目录（供 eglew.h 使用）

set(UTILITIES_DIR ${CMAKE_SOURCE_DIR}/src/cxx/main/util)

set(SHADER_DIR ${CMAKE_SOURCE_DIR}/src/cxx/main/shader)

add_executable(InitialGL
        src/cxx/main/camera/Camera.cpp
        src/cxx/main/shader/VertexShader.cpp
        src/cxx/main/util/Utilities.cpp
        src/cxx/main/shader/FragmentShader.cpp
        src/cxx/main/program/Program.cpp
        src/cxx/main/main.cpp
        src/cxx/main/call_back/CallBack.h
        src/cxx/main/call_back/CallBack.cpp
        src/cxx/main/model/Model.cpp
        src/cxx/main/light/Light.cpp
        src/cxx/main/light/LightManager.cpp)

# 定义资源路径变量
set(RESOURCE_PATH "${CMAKE_SOURCE_DIR}/src/cxx/Resources")
set(GLSL_PATH "${CMAKE_SOURCE_DIR}/src/glsl")
set(CONFIG_DIR ${CMAKE_SOURCE_DIR}/config)
# 生成头文件，把路径写进去
configure_file(
        ${CONFIG_DIR}/config.h.in
        ${CONFIG_DIR}/config.h
)



# 添加包含目录（仅对本目标可见）
target_include_directories(InitialGL PRIVATE
        ${GLEW_INCLUDE_DIR}
        ${GLFW_INCLUDE_DIR}
        ${STB_IMAGE_INCLUDE_DIR}
        ${GLM_INCLUDE_DIR}
        ${KHR_INCLUDE_DIR}
        ${EGL_INCLUDE_DIR}
        ${UTILITIES_DIR}
        ${SHADER_DIR}
        ${DEPENDENCIES_DIR}
        ${CMAKE_SOURCE_DIR}
)

# 使用 GLEW 静态库时需要该宏
target_compile_definitions(InitialGL PRIVATE GLEW_STATIC)

# 链接库（Windows/MSVC，使用预编译的 VC2022 静态库）
if(WIN32 AND MSVC)
    set(GLEW_LIB_DIR ${DEPENDENCIES_DIR}/GLEW/lib)
    set(GLFW_LIB_DIR ${DEPENDENCIES_DIR}/GLFW/lib-vc2022)

    set(GLEW_STATIC_LIB ${GLEW_LIB_DIR}/glew32s.lib)
    # 选择与默认 /MD 运行库匹配的静态库
    set(GLFW_STATIC_LIB ${GLFW_LIB_DIR}/glfw3.lib)

    # 系统库（GLFW/Win32 依赖）
    set(WIN_SYS_LIBS
        opengl32.lib
        gdi32.lib
        user32.lib
        shell32.lib
        kernel32.lib
        winmm.lib
        ws2_32.lib
        bcrypt.lib
        ole32.lib
        uuid.lib
        advapi32.lib
    )

    target_link_libraries(InitialGL PRIVATE
        ${GLEW_STATIC_LIB}
        ${GLFW_STATIC_LIB}
        ${WIN_SYS_LIBS}
    )
else()
    message(WARNING "Prebuilt libs under Dependencies target MSVC/Windows; adjust linking for your toolchain if not using MSVC on Windows.")
endif()
