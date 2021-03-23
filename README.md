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

（未充分测试，所以欢迎提交 bug 修改。）

## 应用界面

整体开发重点在 native 层（渲染，滤镜，文字，实时图像处理等），所以只有一个稍显简陋的测试 UI。随着项目进展，根据交互特点可以设计更友好的界面。（右上角是弹出菜单；锤子是摄像头标定。）

![app](./app_full.png)

## 开发

使用 Android Studio 开发，直接下载代码、修改、提交就行，同时希望提交的代码能和项目代码风格一致。

## 测试 UI

1. 录像、拍照、校正是模式，下面图标按钮是和模式配合使用的
   - 拍照和录像模式：拍照、录像、切换前置后置相机。校正不可用。
   - 校正模式：不可切换前置后置相机。拍照后，点击校正按钮会进行摄像头标定，生成标定文件（标定使用棋盘格，需要在代码中设置棋盘格大小参数等）。
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