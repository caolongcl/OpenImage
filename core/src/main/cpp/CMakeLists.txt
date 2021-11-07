cmake_minimum_required(VERSION 3.4.1)
cmake_policy(SET CMP0012 NEW)
project(avcore)

set(CMAKE_VERBOSE_MAKEFILE on)
set(MODULE_CORE "core")

add_definitions("
-DGL_GLEXT_PROTOTYPES
-DEGL_EGLEXT_PROTOTYPES
")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")
if (DEFINED ANDROID_NDK_MAJOR AND ${ANDROID_NDK_MAJOR} GREATER 20)
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-openmp")
endif ()

set(CMAKE_CXX_STANDARD 17)
# 开启native LOG
option(WITH_LOG "Build with output logs" ON)
if (WITH_LOG)
  add_definitions(-D__LOG__)
endif (WITH_LOG)

option(WITH_DEBUG "Build with debug logic" ON)
if (WITH_DEBUG)
  add_definitions(-D__CLT_DEBUG__)
endif (WITH_DEBUG)

###
set(root "${CMAKE_SOURCE_DIR}")
set(libs "${root}/../jniLibs")

message(STATUS "root: ${root}")
message(STATUS "libs: ${libs}")

#### opencv
set(OpenCV_DIR ${root}/../../../../opencv/native/jni)
message(STATUS "OpenCV libraries: ${OpenCV_DIR}")
find_package(OpenCV REQUIRED)
message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")

#### ncnn
set(ncnn_DIR ${root}/../../../ncnn/${ANDROID_ABI}/lib/cmake/ncnn)
message(STATUS "ncnn libraries: ${ncnn_DIR}")
find_package(ncnn REQUIRED)

#set(ncnn_ODIR ${root}/../../../ncnn/${ANDROID_ABI}/lib)
#add_library(glslang STATIC IMPORTED)
#add_library(OGLCompiler STATIC IMPORTED)
#add_library(OSDependent STATIC IMPORTED)
#add_library(SPIRV STATIC IMPORTED)
#set_target_properties(glslang PROPERTIES IMPORTED_LOCATION ${ncnn_ODIR}/libglslang.a)
#set_target_properties(OGLCompiler PROPERTIES IMPORTED_LOCATION ${ncnn_ODIR}/libOGLCompiler.a)
#set_target_properties(OSDependent PROPERTIES IMPORTED_LOCATION ${ncnn_ODIR}/libOSDependent.a)
#set_target_properties(SPIRV PROPERTIES IMPORTED_LOCATION ${ncnn_ODIR}/libSPIRV.a)


#### freetype2
add_library(freetype STATIC IMPORTED)
set_target_properties(freetype PROPERTIES IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libfreetype.a")

#### yaml
add_library(yaml-cpp STATIC IMPORTED)
set_target_properties(yaml-cpp PROPERTIES IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libyaml-cpp.a")

include_directories(${root}/include)
include_directories(${root}/3rdparty)
include_directories(${root}/3rdparty/freetype2)
include_directories(${root}/../../../ncnn/${ANDROID_ABI}/include)

aux_source_directory(${root}/jni jni_src)
aux_source_directory(${root}/process/ar process_ar_src)
aux_source_directory(${root}/process/base process_base_src)
aux_source_directory(${root}/process/facedetector process_facedetector_src)
aux_source_directory(${root}/process/yolov5 process_yolov5_src)
aux_source_directory(${root}/render render_src)
aux_source_directory(${root}/render/copier render_copier_src)
aux_source_directory(${root}/render/egl render_egl_src)
aux_source_directory(${root}/render/filter render_filter_src)
aux_source_directory(${root}/render/filter/filters render_filter_filters_src)
aux_source_directory(${root}/render/model render_model_src)
aux_source_directory(${root}/render/polygon render_polygon_src)
aux_source_directory(${root}/render/texture render_texture_src)
aux_source_directory(${root}/res res_src)
aux_source_directory(${root}/res/font res_font_src)
aux_source_directory(${root}/softarch softarch_src)
aux_source_directory(${root}/utils utils_src)

list(APPEND MODULE_SRC
    ${jni_src}
    ${process_ar_src} ${process_base_src} ${process_facedetector_src} ${process_yolov5_src}
    ${render_src} ${render_copier_src} ${render_egl_src} ${render_filter_src} ${render_filter_filters_src}
    ${render_model_src} ${render_polygon_src} ${render_texture_src}
    ${res_src} ${res_font_src} ${softarch_src} ${utils_src})

message(STATUS "module src files: ${MODULE_SRC}")

add_library(${MODULE_CORE} SHARED ${MODULE_SRC})

target_link_libraries(
    ${MODULE_CORE}
    log android EGL GLESv3 z OpenSLES

    -Wl,--start-group
    yaml-cpp freetype ${OpenCV_LIBS}
    ncnn
    vulkan
    -Wl,--end-group
)

