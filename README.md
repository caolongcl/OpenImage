# “开图” 相机

## 项目介绍

一个基于 Android 平台，Camera 2 API 作为摄像采集接口，使用 EGL/OpenGL ES（3.1） 渲染的相机应用。

目标是希望成为实时图像处理、视觉应用等的移动实验平台，以便可以快速验证想法和方案。欢迎大家一起参与！

目前实现了如下功能

- 相机预览
  - 前置、后置切换
  - 分辨率切换
- 拍照、录制
- 文字渲染
- 着色器管道
  - 滤镜效果
- 实时图像处理管道
  - 摄像头标定
  - 人脸检测
  - 基于标签的 AR
- ...

（未充分测试，也未适配多机型（调试手机型号是 Redmi k30 5G），所以欢迎提交 bug 修改。）

## 应用界面

整体开发重点在 native 层（渲染，滤镜，文字，实时图像处理等），所以只有一个稍显简陋的测试 UI。随着项目进展，根据交互特点可以设计更友好的界面。（右上角是弹出菜单；锤子是摄像头标定。）

![app](./app_full.png)

## 开发

使用 Android Studio 开发，直接下载代码、修改、提交就行，同时希望提交的代码能和项目代码风格一致。

> 由于 ffmpeg 及 OpenCV 的库较大，需要使用 git lfs clone / git lfs pull 拉取

### 调试测试用文件

- 人脸检测的人脸图片 `face_detector_samples.png`
- 相机校正棋盘格图片 `chessboard.bmp`
  - 行内点数为 6，列内点数为 8
- 基于标签的 AR 所用的 Marker 图片，目前代码只支持 marker code 为 0，1，2，3，4 的
  - `marker_detect.png` 共有 4 个 marker
  - `marker_detect_3.png` 对应 marker code 为 3

### 相机标定校正步骤

- 准备：
  - 将棋盘格图片打印出来，或者在电脑屏幕上显示，然后测量棋盘格每个格子的宽高，以 mm 为单位；
  - 将 Marker 图片打印出来，或者在电脑屏幕上显示，然后测量 Marker 的宽高，以 mm 为单位
- 切换到校正模式
- 长按校正按钮，在弹出框依次填入如下，并点击确认按钮保存设置
  - 棋盘格行内点数
  - 棋盘格列内点数
  - 棋盘格格子实际测量的宽度（mm）
  - 棋盘格格子实际测量的高度（mm）
  - Marker 实际测量的宽度（mm）
  - Marker 实际测量的高度（mm）
- 从各个角度拍摄棋盘格 5 张图片，然后短按校正按钮，开始标定
- 切换到拍照模式，可以从弹出菜单中开启 “opencv AR”。将摄像头对准 Marker 图片，正常情况下会有一个彩色立方体位于 Marker 图片上

## 测试 UI

1. 录像、拍照、校正是模式，下面图标按钮是和模式配合使用的
   - 拍照和录像模式：拍照、录像、切换前置后置相机。校正不可用。
   - 校正模式：不可切换前置后置相机。拍照后，点击校正按钮会进行摄像头标定，生成标定文件。
2. 弹出菜单
   - “3:4”、“9:16”、“original size” 是设置预览宽高比，也是拍摄和录制校正功能图像的分辨率。
   - “h flip”、“v flip” 是画面水平镜像和垂直镜像切换。
   - “color invert” 反色效果，点击按钮切换。
   - “grayscale” 转换画面为灰度图，点击按钮切换。
   - “time color” 采集画面和随机颜色混合产生的效果，点击按钮切换。
   - “blur” 模糊效果，点击按钮切换。
   - “mosaic” 马赛克效果，点击按钮切换。
   - “whirlpool” 漩涡效果，点击按钮切换。
   - “exposure” 调节曝光度，长按相应按钮将启用或关闭此滤镜。开启后，点击将出现相应不同的效果。
   - “hue” 调节色调，长按相应按钮将启用或关闭此滤镜。开启后，点击将出现相应不同的效果。
   - “brightness”、“contrast”、“saturation” 调节亮度，对比度，饱和度，长按其中任何一个相应按钮将启用或关闭此滤镜。开启后，点击将出现相应不同的效果。
   - “face detect” 人脸检测，点击按钮开启和关闭。
   - “opencv AR” 基于标签的 AR，点击按钮开启和关闭。

## 架构说明

### Java 层
Java 层主要负责相机预览的开启、相机切换、相机关闭，及获取 native 层的纹理 id 来创建预览 Surface 、将数据填充到预览 Surface 上等；主要功能逻辑、参数等都会委托给 native 进行处理，`jniBridge.cpp` 文件是 Java 层和 native 层的桥梁。

### native 层

整体是一个多线程架构
- `Flow` 线程创建者，可以创建 `GLThread` 类型的线程，默认创建了渲染线程 `render`，资源加载线程 `shared`
   - `render` 是渲染线程，负责将相机采集的图像绘制到纹理上，然后经过渲染管道进行滤镜处理，最终输出到预览 Surface 、录制 Surface 或拍照的 Surface 上；除此之外，`render` 线程也会将相机采集的图像纹理输入到实时图像处理管道中，读取纹理像素，通过 OpenCV 进行图像处理，如人脸检测，基于 Marker 的 AR 效果等。
  - `shared` 是共享 `render` 线程 EGL 的任务线程，主要用于与 `render` 线程共享纹理资源等，如加载矢量字体到纹理上、加载扩展 shader 等。
- `WorkerFlow` 工作线程池 用来处理实时图像处理任务等。

## 关键组件

### 线程组件
共有三种线程
- Java 线程，需要依赖 `JNIEnv` 时，必须使任务在 Java 线程中执行，如 native 层回调数据到 Java 层
- `GLThread` 线程，是拥有 `EGLContext` 的线程，可以使用 OpenGL ES API，渲染、操作纹理、shader 等；同时这种线程也支持消息机制，可以互相发送同步或异步消息
- `std::thread` 线程，普通线程，用来做运算密集任务

### EGL / OpenGL ES 环境
`GLThread` 是拥有 `EGLCore` 的线程，`EGLCore` 是封装了 Android 平台用来构建 OpengGL ES 环境逻辑的对象。通过切换不同的 `EGLCore` 可以在不同线程使用 OpengGL ES 的 API。

### PreviewController
Java 层相机预览在 native 层的代理，是整个 native 层逻辑的顶层对象。

### GLRender
`GLRender` 是整个渲染的核心，是滤镜渲染管道 `FilterPipe` 及图像处理管道 `ProcessPipe` 纹理的生产者，它接受相机预览帧的通知，将从相机获得的图像纹理输入到滤镜渲染管道和图像处理管道中进行渲染或处理。

### ResManager
`ResManager` 是用来加载纹理、字体、读取编译构建 shader、配置文件、人脸检测模型文件等资源的管理组件。

### PixelReaderPbo
读取纹理像素组件，读出的像素数据经过缩放后通过图像处理管道进行处理。

### Printer
通过加载的矢量字体纹理，绘制文字，支持配置文字位置、颜色、背景色、阴影的特性。

### PolygonDrawer / BaseModel
`PolygonDrawer` 支持绘制多边形，比如人脸检测框，Marker 检测框；`BaseModel` 支持绘制基本的几何体，如 Marker AR 中的立方体。

### FaceDetector
人脸检测组件，调用 OpenCV API `cv::CascadeClassifier` 进行人脸检测和性能统计。

### CalibrateCamera / Marker / MarkerAR
`CalibrateCamera` 相机校正组件，校正结果可以获得相机的内参矩阵和畸变参数，进一步可以计算出相机视图矩阵（View，结合 Model、 Projection 矩阵就可以构建模拟真实世界的三维环境了，这是实现 AR 的关键一步）

`Marker` 组件用来对 Marker 图像进行识别、解析

`MarkerAR` 是在 `CalibrateCamera` 和 `Marker` 结果之上，通过识别 `Marker`，将三维物体模拟放置在真实三维环境中，实现 AR 效果。
