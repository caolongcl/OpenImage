cmake_minimum_required(VERSION 3.4.1)
cmake_policy(SET CMP0012 NEW)
project(avcore)

set(CMAKE_VERBOSE_MAKEFILE on)
set(MODULE_CORE "core")

add_definitions("
-DGL_GLEXT_PROTOTYPES
-DEGL_EGLEXT_PROTOTYPES
")

# 开启native LOG
option(WITH_LOG "Build with output logs" ON)
if (WITH_LOG)
  add_definitions(-D__LOG__)
endif (WITH_LOG)

option(WITH_DEBUG "Build with debug logic" ON)
if (WITH_DEBUG)
  add_definitions(-D__CLT_DEBUG__)
endif (WITH_DEBUG)

set(root "${CMAKE_SOURCE_DIR}/src/main/cpp")
set(libs "${CMAKE_SOURCE_DIR}/src/main/jniLibs")

include_directories(${root}/core/include)
include_directories(${root}/core/3rdparty)
include_directories(${root}/core/3rdparty/ffmpeg)
include_directories(${root}/core/3rdparty/freetype2)

add_library(libade STATIC IMPORTED)
set_target_properties(libade PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libade.a")
add_library(libcpufeatures STATIC IMPORTED)
set_target_properties(libcpufeatures PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libcpufeatures.a")
add_library(libIlmImf STATIC IMPORTED)
set_target_properties(libIlmImf PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libIlmImf.a")
add_library(libittnotify STATIC IMPORTED)
set_target_properties(libittnotify PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libittnotify.a")
add_library(liblibjasper STATIC IMPORTED)
set_target_properties(liblibjasper PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/liblibjasper.a")
add_library(liblibjpeg-turbo STATIC IMPORTED)
set_target_properties(liblibjpeg-turbo PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/liblibjpeg-turbo.a")
add_library(liblibpng STATIC IMPORTED)
set_target_properties(liblibpng PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/liblibpng.a")
add_library(liblibprotobuf STATIC IMPORTED)
set_target_properties(liblibprotobuf PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/liblibprotobuf.a")
add_library(liblibtiff STATIC IMPORTED)
set_target_properties(liblibtiff PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/liblibtiff.a")
add_library(liblibwebp STATIC IMPORTED)
set_target_properties(liblibwebp PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/liblibwebp.a")
add_library(libquirc STATIC IMPORTED)
set_target_properties(libquirc PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libquirc.a")
add_library(libtbb STATIC IMPORTED)
set_target_properties(libtbb PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libtbb.a")
add_library(libtegra_hal STATIC IMPORTED)
set_target_properties(libtegra_hal PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libtegra_hal.a")

set(3dparty_libs libade libcpufeatures libIlmImf libittnotify liblibjasper liblibjpeg-turbo
    liblibpng liblibprotobuf liblibtiff liblibwebp libquirc libtbb libtegra_hal)

##### opencv
add_library(libopencv_core STATIC IMPORTED)
set_target_properties(libopencv_core PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libopencv_core.a")

add_library(libopencv_calib3d STATIC IMPORTED)
set_target_properties(libopencv_calib3d PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libopencv_calib3d.a")

add_library(libopencv_dnn STATIC IMPORTED)
set_target_properties(libopencv_dnn PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libopencv_dnn.a")

add_library(libopencv_features2d STATIC IMPORTED)
set_target_properties(libopencv_features2d PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libopencv_features2d.a")

add_library(libopencv_flann STATIC IMPORTED)
set_target_properties(libopencv_flann PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libopencv_flann.a")

add_library(libopencv_gapi STATIC IMPORTED)
set_target_properties(libopencv_gapi PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libopencv_gapi.a")

add_library(libopencv_highgui STATIC IMPORTED)
set_target_properties(libopencv_highgui PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libopencv_highgui.a")

add_library(libopencv_imgcodecs STATIC IMPORTED)
set_target_properties(libopencv_imgcodecs PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libopencv_imgcodecs.a")

add_library(libopencv_imgproc STATIC IMPORTED)
set_target_properties(libopencv_imgproc PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libopencv_imgproc.a")

add_library(libopencv_ml STATIC IMPORTED)
set_target_properties(libopencv_ml PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libopencv_ml.a")

add_library(libopencv_objdetect STATIC IMPORTED)
set_target_properties(libopencv_objdetect PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libopencv_objdetect.a")

add_library(libopencv_photo STATIC IMPORTED)
set_target_properties(libopencv_photo PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libopencv_photo.a")

add_library(libopencv_stitching STATIC IMPORTED)
set_target_properties(libopencv_stitching PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libopencv_stitching.a")

add_library(libopencv_video STATIC IMPORTED)
set_target_properties(libopencv_video PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libopencv_video.a")

add_library(libopencv_videoio STATIC IMPORTED)
set_target_properties(libopencv_videoio PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libopencv_videoio.a")

set(opencv_libs libopencv_calib3d libopencv_core libopencv_features2d libopencv_dnn libopencv_flann
    libopencv_gapi libopencv_highgui libopencv_imgcodecs libopencv_imgproc libopencv_ml
    libopencv_objdetect libopencv_photo libopencv_stitching libopencv_video libopencv_videoio)

##### ffmpeg
add_library(libavcodec STATIC IMPORTED)
set_target_properties(libavcodec PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libavcodec.a")

add_library(libavdevice STATIC IMPORTED)
set_target_properties(libavdevice PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libavdevice.a")

add_library(libavfilter STATIC IMPORTED)
set_target_properties(libavfilter PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libavfilter.a")

add_library(libavformat STATIC IMPORTED)
set_target_properties(libavformat PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libavformat.a")

add_library(libavutil STATIC IMPORTED)
set_target_properties(libavutil PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libavutil.a")

add_library(libswresample STATIC IMPORTED)
set_target_properties(libswresample PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libswresample.a")

add_library(libswscale STATIC IMPORTED)
set_target_properties(libswscale PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libswscale.a")

set(ffmpeg_libs libavcodec libavdevice libavfilter libavformat libavutil libswresample libswscale)

##### freetype2
add_library(libfreetype STATIC IMPORTED)
set_target_properties(libfreetype PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libfreetype.a")

set(freetype_lib libfreetype)


##### yaml
add_library(yaml-cpp STATIC IMPORTED)
set_target_properties(yaml-cpp PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libyaml-cpp.a")

set(MODULE_SRC
    ${root}/core/jni/jniBridge.cpp
    ${root}/core/jni/PreviewController.cpp
    ${root}/core/render/egl/EGLCore.cpp
    ${root}/core/render/texture/OESTexture.cpp
    ${root}/core/render/texture/NormalTexture.cpp
    ${root}/core/render/texture/GrayscaleTexture.cpp
    ${root}/core/render/texture/LuminanceTexture.cpp
    ${root}/core/render/copier/OESCopier.cpp
    ${root}/core/render/copier/Copier.cpp
    ${root}/core/render/copier/Drawer.cpp
    ${root}/core/render/copier/Recorder.cpp
    ${root}/core/render/copier/Capturer.cpp
    ${root}/core/render/copier/Surface.cpp
    ${root}/core/render/GLRender.cpp
    ${root}/core/render/Flow.cpp
    ${root}/core/render/filter/FilterFactory.cpp
    ${root}/core/render/filter/FilterPipe.cpp
    ${root}/core/render/filter/FilterCom.cpp
    ${root}/core/render/filter/filters/BaseFilter.cpp
    ${root}/core/render/filter/filters/ExposureFilter.cpp
    ${root}/core/render/filter/filters/OpacityFilter.cpp
    ${root}/core/render/filter/filters/ColorInvertFilter.cpp
    ${root}/core/render/filter/filters/BlurFilter.cpp
    ${root}/core/render/filter/filters/WhiteBalanceFilter.cpp
    ${root}/core/render/filter/filters/GrayscaleFilter.cpp
    ${root}/core/render/filter/filters/GammaFilter.cpp
    ${root}/core/render/filter/filters/RGBFilter.cpp
    ${root}/core/render/filter/filters/HueFilter.cpp
    ${root}/core/render/filter/filters/SharpenFilter.cpp
    ${root}/core/render/filter/filters/MosaicFilter.cpp
    ${root}/core/render/filter/filters/TimeColorFilter.cpp
    ${root}/core/render/filter/filters/WhirlpoolFilter.cpp
    ${root}/core/render/filter/filters/MonochromeFilter.cpp
    ${root}/core/render/filter/filters/FireFilter.cpp
    ${root}/core/render/polygon/PolygonDrawer.cpp
    ${root}/core/render/polygon/PolygonModel.cpp
    ${root}/core/render/polygon/Polygon.cpp
    ${root}/core/render/model/BaseModel.cpp
    ${root}/core/process/facedetector/FaceDetector.cpp
    ${root}/core/process/base/RGBAToGray.cpp
    ${root}/core/process/facedetector/CascadeClassifierWrap.cpp
    ${root}/core/process/base/PixelReader.cpp
    ${root}/core/process/base/ProcessPipe.cpp
    ${root}/core/process/base/ProcessTextures.cpp
    ${root}/core/process/base/ProcessBuffers.cpp
    ${root}/core/process/base/WorkerFlow.cpp
    ${root}/core/process/base/BufferProcessTask.cpp
    ${root}/core/process/base/NormalProcessTask.cpp
    ${root}/core/process/base/ProcessFactory.cpp
    ${root}/core/process/ar/Marker.cpp
    ${root}/core/process/ar/MarkerAR.cpp
    ${root}/core/process/ar/CalibrateCamera.cpp
    ${root}/core/utils/Log.cpp
    ${root}/core/utils/FpsStatics.cpp
    ${root}/core/softarch/GLThread.cpp
    ${root}/core/softarch/ThreadPool.cpp
    ${root}/core/softarch/SingleThreadPool.cpp
    ${root}/core/res/ResManager.cpp
    ${root}/core/res/Printer.cpp
    ${root}/core/res/Shader.cpp
    ${root}/core/res/font/Font.cpp
    ${root}/core/res/font/FontGlyph.cpp
    ${root}/core/res/font/FontRender.cpp
    ${root}/core/res/font/FontSquareModel.cpp
    ${root}/core/res/SquareModel.cpp)

add_library(${MODULE_CORE} SHARED ${MODULE_SRC})

target_link_libraries(
    ${MODULE_CORE}
    log android EGL GLESv3 z OpenSLES
    yaml-cpp

    -Wl,--start-group
    ${3dparty_libs} ${opencv_libs} ${freetype_lib} ${ffmpeg_libs}
    -Wl,--end-group
)