# UML 图一览（PlantUML）

位于本目录的 .puml 文件可直接用 PlantUML 渲染。

- class-diagram.puml：核心类图（Shader/Program/Model/Camera/CallBack/Light/LightManager 及关系）。
- package-diagram.puml：包图（camera/shader/program/model/call_back/light 与外部依赖、glsl 分组）。
- component-diagram.puml：组件图（GPass/LPass/ShadowPass、GBuffer/SBuffer、Model/Camera 与渲染器关系）。
- sequence-rendering.puml：每帧渲染时序图（Shadow→Geometry→Lighting 三通道）。
- dataflow-fbo.puml：G-Buffer/阴影 FBO 纹理数据流示意。
- state-camera.puml：相机输入/状态机（移动、旋转、缩放）。
- shader-uniforms.puml：三通道程序的关键 Uniform 与采样器绑定一览。

渲染示例（VSCode 配合 PlantUML 扩展，或命令行）：

```bash
java -jar plantuml.jar class-diagram.puml
java -jar plantuml.jar package-diagram.puml
java -jar plantuml.jar component-diagram.puml
java -jar plantuml.jar sequence-rendering.puml
java -jar plantuml.jar dataflow-fbo.puml
java -jar plantuml.jar state-camera.puml
java -jar plantuml.jar shader-uniforms.puml
```

渲染细节要点（着色器关系与帧缓冲）
- 着色器关系：Program 管理并链接 Shader（Vertex/Fragment）。工程含三套 Program：
  - GPass：写入 G-Buffer（三个颜色附件：Position/Normal/AlbedoSpec）。
  - ShadowPass：从光源视角写入 S-Buffer（深度纹理 + 增益贴图）。
  - LPass：读取上述纹理合成最终颜色（默认帧缓冲）。
- 关键 Uniform 契约（详见 shader-uniforms.puml）：
  - GPass：umat4View/Proj/Model；texture_diffuse1=0，texture_specular1=1。
  - ShadowPass：umat4LightView/Proj、umat4Model。
  - LPass：gPosition=0、gNormal=1、gAlbedoSpec=2、shadowTexture=3、gainTexture=4；viewPos、numLights、lights[16]、光源矩阵。
- 帧缓冲与附件：
  - G-Buffer：
    - gPosition: GL_RGB16F（颜色附件0）
    - gNormal: GL_RGB16F（颜色附件1）
    - gAlbedoSpec: GL_RGBA8（颜色附件2）
    - 深度：Renderbuffer（DEPTH24）
  - S-Buffer：
    - depthTexture: GL_DEPTH_COMPONENT（深度附件）
    - lGainTexture: GL_RGBA（颜色附件0）
- 渲染流程：ShadowPass → GPass → LPass；前两步启用深度测试并写入各自 FBO，最后一步关闭深度测试绘制全屏 Quad。
- 常见问题：
  - 采样器绑定单位与 Uniform 不匹配；FBO 不完整（缺附件或尺寸不一致）；Uniform 名称与着色器不一致。

提示：若使用 VSCode，安装 “PlantUML” 扩展后可在文件内直接预览。
