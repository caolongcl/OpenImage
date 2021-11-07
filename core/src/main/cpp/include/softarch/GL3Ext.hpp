//
// Created by caolong on 2020/8/9.
//

#pragma once

#include <GLES3/gl3ext.h>

/* $Revision: 17809 $ on $Date:: 2012-05-14 08:03:36 -0700 #$ */

/*
 * This document is licensed under the SGI Free Software B License Version
 * 2.0. For details, see http://oss.sgi.com/projects/FreeB/ .
 */

/* OpenGL ES 3 Extensions
 *
 * After an OES extension's interactions with OpenGl ES 3.0 have been documented,
 * its tokens and function definitions should be added to this file in a manner
 * that does not conflict with gl2ext.h or gl3.h.
 *
 * Tokens and function definitions for extensions that have become standard
 * features in OpenGL ES 3.0 will not be added to this file.
 *
 * Applications using OpenGL-ES-2-only extensions should include gl2ext.h
 */

#ifndef GL_OES_EGL_image
#define GL_OES_EGL_image 1
typedef void *GLeglImageOES;

typedef void (GL_APIENTRYP PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)(GLenum target, GLeglImageOES image);

typedef void (GL_APIENTRYP PFNGLEGLIMAGETARGETRENDERBUFFERSTORAGEOESPROC)(GLenum target,
                                                                          GLeglImageOES image);

#ifdef GL_GLEXT_PROTOTYPES

GL_APICALL void GL_APIENTRY glEGLImageTargetTexture2DOES(GLenum target, GLeglImageOES image);

GL_APICALL void GL_APIENTRY
glEGLImageTargetRenderbufferStorageOES(GLenum target, GLeglImageOES image);

#endif
#endif /* GL_OES_EGL_image */

#ifndef GL_OES_EGL_image_external
#define GL_OES_EGL_image_external 1
#define GL_TEXTURE_EXTERNAL_OES           0x8D65
#define GL_TEXTURE_BINDING_EXTERNAL_OES   0x8D67
#define GL_REQUIRED_TEXTURE_IMAGE_UNITS_OES 0x8D68
#define GL_SAMPLER_EXTERNAL_OES           0x8D66
#endif /* GL_OES_EGL_image_external */

#ifndef GL_OES_EGL_image_external_essl3
#define GL_OES_EGL_image_external_essl3 1
#endif /* GL_OES_EGL_image_external_essl3 */

#ifndef GL_OES_compressed_ETC1_RGB8_sub_texture
#define GL_OES_compressed_ETC1_RGB8_sub_texture 1
#endif /* GL_OES_compressed_ETC1_RGB8_sub_texture */

#ifndef GL_OES_compressed_ETC1_RGB8_texture
#define GL_OES_compressed_ETC1_RGB8_texture 1
#define GL_ETC1_RGB8_OES                  0x8D64
#endif /* GL_OES_compressed_ETC1_RGB8_texture */

#ifndef GL_OES_compressed_paletted_texture
#define GL_OES_compressed_paletted_texture 1
#define GL_PALETTE4_RGB8_OES              0x8B90
#define GL_PALETTE4_RGBA8_OES             0x8B91
#define GL_PALETTE4_R5_G6_B5_OES          0x8B92
#define GL_PALETTE4_RGBA4_OES             0x8B93
#define GL_PALETTE4_RGB5_A1_OES           0x8B94
#define GL_PALETTE8_RGB8_OES              0x8B95
#define GL_PALETTE8_RGBA8_OES             0x8B96
#define GL_PALETTE8_R5_G6_B5_OES          0x8B97
#define GL_PALETTE8_RGBA4_OES             0x8B98
#define GL_PALETTE8_RGB5_A1_OES           0x8B99
#endif /* GL_OES_compressed_paletted_texture */

#ifndef GL_OES_copy_image
#define GL_OES_copy_image 1

typedef void (GL_APIENTRYP PFNGLCOPYIMAGESUBDATAOESPROC)(GLuint srcName, GLenum srcTarget,
                                                         GLint srcLevel, GLint srcX, GLint srcY,
                                                         GLint srcZ, GLuint dstName,
                                                         GLenum dstTarget, GLint dstLevel,
                                                         GLint dstX, GLint dstY, GLint dstZ,
                                                         GLsizei srcWidth, GLsizei srcHeight,
                                                         GLsizei srcDepth);

#ifdef GL_GLEXT_PROTOTYPES

GL_APICALL void GL_APIENTRY
glCopyImageSubDataOES(GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY,
                      GLint srcZ, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX,
                      GLint dstY, GLint dstZ, GLsizei srcWidth, GLsizei srcHeight,
                      GLsizei srcDepth);

#endif
#endif /* GL_OES_copy_image */

#ifndef GL_OES_depth24
#define GL_OES_depth24 1
#define GL_DEPTH_COMPONENT24_OES          0x81A6
#endif /* GL_OES_depth24 */

#ifndef GL_OES_depth32
#define GL_OES_depth32 1
#define GL_DEPTH_COMPONENT32_OES          0x81A7
#endif /* GL_OES_depth32 */

#ifndef GL_OES_depth_texture
#define GL_OES_depth_texture 1
#endif /* GL_OES_depth_texture */

#ifndef GL_OES_draw_buffers_indexed
#define GL_OES_draw_buffers_indexed 1
#define GL_MIN                            0x8007
#define GL_MAX                            0x8008

typedef void (GL_APIENTRYP PFNGLENABLEIOESPROC)(GLenum target, GLuint index);

typedef void (GL_APIENTRYP PFNGLDISABLEIOESPROC)(GLenum target, GLuint index);

typedef void (GL_APIENTRYP PFNGLBLENDEQUATIONIOESPROC)(GLuint buf, GLenum mode);

typedef void (GL_APIENTRYP PFNGLBLENDEQUATIONSEPARATEIOESPROC)(GLuint buf, GLenum modeRGB,
                                                               GLenum modeAlpha);

typedef void (GL_APIENTRYP PFNGLBLENDFUNCIOESPROC)(GLuint buf, GLenum src, GLenum dst);

typedef void (GL_APIENTRYP PFNGLBLENDFUNCSEPARATEIOESPROC)(GLuint buf, GLenum srcRGB, GLenum dstRGB,
                                                           GLenum srcAlpha, GLenum dstAlpha);

typedef void (GL_APIENTRYP PFNGLCOLORMASKIOESPROC)(GLuint index, GLboolean r, GLboolean g,
                                                   GLboolean b, GLboolean a);

typedef GLboolean (GL_APIENTRYP PFNGLISENABLEDIOESPROC)(GLenum target, GLuint index);

#ifdef GL_GLEXT_PROTOTYPES

GL_APICALL void GL_APIENTRY glEnableiOES(GLenum target, GLuint index);

GL_APICALL void GL_APIENTRY glDisableiOES(GLenum target, GLuint index);

GL_APICALL void GL_APIENTRY glBlendEquationiOES(GLuint buf, GLenum mode);

GL_APICALL void GL_APIENTRY
glBlendEquationSeparateiOES(GLuint buf, GLenum modeRGB, GLenum modeAlpha);

GL_APICALL void GL_APIENTRY glBlendFunciOES(GLuint buf, GLenum src, GLenum dst);

GL_APICALL void GL_APIENTRY
glBlendFuncSeparateiOES(GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);

GL_APICALL void GL_APIENTRY
glColorMaskiOES(GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a);

GL_APICALL GLboolean GL_APIENTRY glIsEnablediOES(GLenum target, GLuint index);

#endif
#endif /* GL_OES_draw_buffers_indexed */

#ifndef GL_OES_draw_elements_base_vertex
#define GL_OES_draw_elements_base_vertex 1

typedef void (GL_APIENTRYP PFNGLDRAWELEMENTSBASEVERTEXOESPROC)(GLenum mode, GLsizei count,
                                                               GLenum type, const void *indices,
                                                               GLint basevertex);

typedef void (GL_APIENTRYP PFNGLDRAWRANGEELEMENTSBASEVERTEXOESPROC)(GLenum mode, GLuint start,
                                                                    GLuint end, GLsizei count,
                                                                    GLenum type,
                                                                    const void *indices,
                                                                    GLint basevertex);

typedef void (GL_APIENTRYP PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXOESPROC)(GLenum mode, GLsizei count,
                                                                        GLenum type,
                                                                        const void *indices,
                                                                        GLsizei instancecount,
                                                                        GLint basevertex);

typedef void (GL_APIENTRYP PFNGLMULTIDRAWELEMENTSBASEVERTEXEXTPROC)(GLenum mode,
                                                                    const GLsizei *count,
                                                                    GLenum type,
                                                                    const void *const *indices,
                                                                    GLsizei primcount,
                                                                    const GLint *basevertex);

#ifdef GL_GLEXT_PROTOTYPES

GL_APICALL void GL_APIENTRY
glDrawElementsBaseVertexOES(GLenum mode, GLsizei count, GLenum type, const void *indices,
                            GLint basevertex);

GL_APICALL void GL_APIENTRY
glDrawRangeElementsBaseVertexOES(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type,
                                 const void *indices, GLint basevertex);

GL_APICALL void GL_APIENTRY
glDrawElementsInstancedBaseVertexOES(GLenum mode, GLsizei count, GLenum type, const void *indices,
                                     GLsizei instancecount, GLint basevertex);

GL_APICALL void GL_APIENTRY
glMultiDrawElementsBaseVertexEXT(GLenum mode, const GLsizei *count, GLenum type,
                                 const void *const *indices, GLsizei primcount,
                                 const GLint *basevertex);

#endif
#endif /* GL_OES_draw_elements_base_vertex */

#ifndef GL_OES_element_index_uint
#define GL_OES_element_index_uint 1
#endif /* GL_OES_element_index_uint */

#ifndef GL_OES_fbo_render_mipmap
#define GL_OES_fbo_render_mipmap 1
#endif /* GL_OES_fbo_render_mipmap */

#ifndef GL_OES_fragment_precision_high
#define GL_OES_fragment_precision_high 1
#endif /* GL_OES_fragment_precision_high */

#ifndef GL_OES_geometry_point_size
#define GL_OES_geometry_point_size 1
#endif /* GL_OES_geometry_point_size */

#ifndef GL_OES_geometry_shader
#define GL_OES_geometry_shader 1
#define GL_GEOMETRY_SHADER_OES            0x8DD9
#define GL_GEOMETRY_SHADER_BIT_OES        0x00000004
#define GL_GEOMETRY_LINKED_VERTICES_OUT_OES 0x8916
#define GL_GEOMETRY_LINKED_INPUT_TYPE_OES 0x8917
#define GL_GEOMETRY_LINKED_OUTPUT_TYPE_OES 0x8918
#define GL_GEOMETRY_SHADER_INVOCATIONS_OES 0x887F
#define GL_LAYER_PROVOKING_VERTEX_OES     0x825E
#define GL_LINES_ADJACENCY_OES            0x000A
#define GL_LINE_STRIP_ADJACENCY_OES       0x000B
#define GL_TRIANGLES_ADJACENCY_OES        0x000C
#define GL_TRIANGLE_STRIP_ADJACENCY_OES   0x000D
#define GL_MAX_GEOMETRY_UNIFORM_COMPONENTS_OES 0x8DDF
#define GL_MAX_GEOMETRY_UNIFORM_BLOCKS_OES 0x8A2C
#define GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS_OES 0x8A32
#define GL_MAX_GEOMETRY_INPUT_COMPONENTS_OES 0x9123
#define GL_MAX_GEOMETRY_OUTPUT_COMPONENTS_OES 0x9124
#define GL_MAX_GEOMETRY_OUTPUT_VERTICES_OES 0x8DE0
#define GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS_OES 0x8DE1
#define GL_MAX_GEOMETRY_SHADER_INVOCATIONS_OES 0x8E5A
#define GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS_OES 0x8C29
#define GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS_OES 0x92CF
#define GL_MAX_GEOMETRY_ATOMIC_COUNTERS_OES 0x92D5
#define GL_MAX_GEOMETRY_IMAGE_UNIFORMS_OES 0x90CD
#define GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS_OES 0x90D7
#define GL_FIRST_VERTEX_CONVENTION_OES    0x8E4D
#define GL_LAST_VERTEX_CONVENTION_OES     0x8E4E
#define GL_UNDEFINED_VERTEX_OES           0x8260
#define GL_PRIMITIVES_GENERATED_OES       0x8C87
#define GL_FRAMEBUFFER_DEFAULT_LAYERS_OES 0x9312
#define GL_MAX_FRAMEBUFFER_LAYERS_OES     0x9317
#define GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_OES 0x8DA8
#define GL_FRAMEBUFFER_ATTACHMENT_LAYERED_OES 0x8DA7
#define GL_REFERENCED_BY_GEOMETRY_SHADER_OES 0x9309

typedef void (GL_APIENTRYP PFNGLFRAMEBUFFERTEXTUREOESPROC)(GLenum target, GLenum attachment,
                                                           GLuint texture, GLint level);

#ifdef GL_GLEXT_PROTOTYPES

GL_APICALL void GL_APIENTRY
glFramebufferTextureOES(GLenum target, GLenum attachment, GLuint texture, GLint level);

#endif
#endif /* GL_OES_geometry_shader */

#ifndef GL_OES_get_program_binary
#define GL_OES_get_program_binary 1
#define GL_PROGRAM_BINARY_LENGTH_OES      0x8741
#define GL_NUM_PROGRAM_BINARY_FORMATS_OES 0x87FE
#define GL_PROGRAM_BINARY_FORMATS_OES     0x87FF

typedef void (GL_APIENTRYP PFNGLGETPROGRAMBINARYOESPROC)(GLuint program, GLsizei bufSize,
                                                         GLsizei *length, GLenum *binaryFormat,
                                                         void *binary);

typedef void (GL_APIENTRYP PFNGLPROGRAMBINARYOESPROC)(GLuint program, GLenum binaryFormat,
                                                      const void *binary, GLint length);

#ifdef GL_GLEXT_PROTOTYPES

GL_APICALL void GL_APIENTRY
glGetProgramBinaryOES(GLuint program, GLsizei bufSize, GLsizei *length, GLenum *binaryFormat,
                      void *binary);

GL_APICALL void GL_APIENTRY
glProgramBinaryOES(GLuint program, GLenum binaryFormat, const void *binary, GLint length);

#endif
#endif /* GL_OES_get_program_binary */

#ifndef GL_OES_gpu_shader5
#define GL_OES_gpu_shader5 1
#endif /* GL_OES_gpu_shader5 */

#ifndef GL_OES_mapbuffer
#define GL_OES_mapbuffer 1
#define GL_WRITE_ONLY_OES                 0x88B9
#define GL_BUFFER_ACCESS_OES              0x88BB
#define GL_BUFFER_MAPPED_OES              0x88BC
#define GL_BUFFER_MAP_POINTER_OES         0x88BD

typedef void *(GL_APIENTRYP PFNGLMAPBUFFEROESPROC)(GLenum target, GLenum access);

typedef GLboolean (GL_APIENTRYP PFNGLUNMAPBUFFEROESPROC)(GLenum target);

typedef void (GL_APIENTRYP PFNGLGETBUFFERPOINTERVOESPROC)(GLenum target, GLenum pname,
                                                          void **params);

#ifdef GL_GLEXT_PROTOTYPES

GL_APICALL void *GL_APIENTRY glMapBufferOES(GLenum target, GLenum access);

GL_APICALL GLboolean GL_APIENTRY glUnmapBufferOES(GLenum target);

GL_APICALL void GL_APIENTRY glGetBufferPointervOES(GLenum target, GLenum pname, void **params);

#endif
#endif /* GL_OES_mapbuffer */

#ifndef GL_OES_packed_depth_stencil
#define GL_OES_packed_depth_stencil 1
#define GL_DEPTH_STENCIL_OES              0x84F9
#define GL_UNSIGNED_INT_24_8_OES          0x84FA
#define GL_DEPTH24_STENCIL8_OES           0x88F0
#endif /* GL_OES_packed_depth_stencil */

#ifndef GL_OES_primitive_bounding_box
#define GL_OES_primitive_bounding_box 1
#define GL_PRIMITIVE_BOUNDING_BOX_OES     0x92BE

typedef void (GL_APIENTRYP PFNGLPRIMITIVEBOUNDINGBOXOESPROC)(GLfloat minX, GLfloat minY,
                                                             GLfloat minZ, GLfloat minW,
                                                             GLfloat maxX, GLfloat maxY,
                                                             GLfloat maxZ, GLfloat maxW);

#ifdef GL_GLEXT_PROTOTYPES

GL_APICALL void GL_APIENTRY
glPrimitiveBoundingBoxOES(GLfloat minX, GLfloat minY, GLfloat minZ, GLfloat minW, GLfloat maxX,
                          GLfloat maxY, GLfloat maxZ, GLfloat maxW);

#endif
#endif /* GL_OES_primitive_bounding_box */

#ifndef GL_OES_required_internalformat
#define GL_OES_required_internalformat 1
#define GL_ALPHA8_OES                     0x803C
#define GL_DEPTH_COMPONENT16_OES          0x81A5
#define GL_LUMINANCE4_ALPHA4_OES          0x8043
#define GL_LUMINANCE8_ALPHA8_OES          0x8045
#define GL_LUMINANCE8_OES                 0x8040
#define GL_RGBA4_OES                      0x8056
#define GL_RGB5_A1_OES                    0x8057
#define GL_RGB565_OES                     0x8D62
#define GL_RGB8_OES                       0x8051
#define GL_RGBA8_OES                      0x8058
#define GL_RGB10_EXT                      0x8052
#define GL_RGB10_A2_EXT                   0x8059
#endif /* GL_OES_required_internalformat */

#ifndef GL_OES_rgb8_rgba8
#define GL_OES_rgb8_rgba8 1
#endif /* GL_OES_rgb8_rgba8 */

#ifndef GL_OES_sample_shading
#define GL_OES_sample_shading 1
#define GL_SAMPLE_SHADING_OES             0x8C36
#define GL_MIN_SAMPLE_SHADING_VALUE_OES   0x8C37

typedef void (GL_APIENTRYP PFNGLMINSAMPLESHADINGOESPROC)(GLfloat value);

#ifdef GL_GLEXT_PROTOTYPES

GL_APICALL void GL_APIENTRY glMinSampleShadingOES(GLfloat value);

#endif
#endif /* GL_OES_sample_shading */

#ifndef GL_OES_sample_variables
#define GL_OES_sample_variables 1
#endif /* GL_OES_sample_variables */

#ifndef GL_OES_shader_image_atomic
#define GL_OES_shader_image_atomic 1
#endif /* GL_OES_shader_image_atomic */

#ifndef GL_OES_shader_io_blocks
#define GL_OES_shader_io_blocks 1
#endif /* GL_OES_shader_io_blocks */

#ifndef GL_OES_shader_multisample_interpolation
#define GL_OES_shader_multisample_interpolation 1
#define GL_MIN_FRAGMENT_INTERPOLATION_OFFSET_OES 0x8E5B
#define GL_MAX_FRAGMENT_INTERPOLATION_OFFSET_OES 0x8E5C
#define GL_FRAGMENT_INTERPOLATION_OFFSET_BITS_OES 0x8E5D
#endif /* GL_OES_shader_multisample_interpolation */

#ifndef GL_OES_standard_derivatives
#define GL_OES_standard_derivatives 1
#define GL_FRAGMENT_SHADER_DERIVATIVE_HINT_OES 0x8B8B
#endif /* GL_OES_standard_derivatives */

#ifndef GL_OES_stencil1
#define GL_OES_stencil1 1
#define GL_STENCIL_INDEX1_OES             0x8D46
#endif /* GL_OES_stencil1 */

#ifndef GL_OES_stencil4
#define GL_OES_stencil4 1
#define GL_STENCIL_INDEX4_OES             0x8D47
#endif /* GL_OES_stencil4 */

#ifndef GL_OES_surfaceless_context
#define GL_OES_surfaceless_context 1
#define GL_FRAMEBUFFER_UNDEFINED_OES      0x8219
#endif /* GL_OES_surfaceless_context */

#ifndef GL_OES_tessellation_point_size
#define GL_OES_tessellation_point_size 1
#endif /* GL_OES_tessellation_point_size */

#ifndef GL_OES_tessellation_shader
#define GL_OES_tessellation_shader 1
#define GL_PATCHES_OES                    0x000E
#define GL_PATCH_VERTICES_OES             0x8E72
#define GL_TESS_CONTROL_OUTPUT_VERTICES_OES 0x8E75
#define GL_TESS_GEN_MODE_OES              0x8E76
#define GL_TESS_GEN_SPACING_OES           0x8E77
#define GL_TESS_GEN_VERTEX_ORDER_OES      0x8E78
#define GL_TESS_GEN_POINT_MODE_OES        0x8E79
#define GL_ISOLINES_OES                   0x8E7A
#define GL_QUADS_OES                      0x0007
#define GL_FRACTIONAL_ODD_OES             0x8E7B
#define GL_FRACTIONAL_EVEN_OES            0x8E7C
#define GL_MAX_PATCH_VERTICES_OES         0x8E7D
#define GL_MAX_TESS_GEN_LEVEL_OES         0x8E7E
#define GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS_OES 0x8E7F
#define GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS_OES 0x8E80
#define GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS_OES 0x8E81
#define GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS_OES 0x8E82
#define GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS_OES 0x8E83
#define GL_MAX_TESS_PATCH_COMPONENTS_OES  0x8E84
#define GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS_OES 0x8E85
#define GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS_OES 0x8E86
#define GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS_OES 0x8E89
#define GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS_OES 0x8E8A
#define GL_MAX_TESS_CONTROL_INPUT_COMPONENTS_OES 0x886C
#define GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS_OES 0x886D
#define GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS_OES 0x8E1E
#define GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS_OES 0x8E1F
#define GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS_OES 0x92CD
#define GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS_OES 0x92CE
#define GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS_OES 0x92D3
#define GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS_OES 0x92D4
#define GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS_OES 0x90CB
#define GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS_OES 0x90CC
#define GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS_OES 0x90D8
#define GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS_OES 0x90D9
#define GL_PRIMITIVE_RESTART_FOR_PATCHES_SUPPORTED_OES 0x8221
#define GL_IS_PER_PATCH_OES               0x92E7
#define GL_REFERENCED_BY_TESS_CONTROL_SHADER_OES 0x9307
#define GL_REFERENCED_BY_TESS_EVALUATION_SHADER_OES 0x9308
#define GL_TESS_CONTROL_SHADER_OES        0x8E88
#define GL_TESS_EVALUATION_SHADER_OES     0x8E87
#define GL_TESS_CONTROL_SHADER_BIT_OES    0x00000008
#define GL_TESS_EVALUATION_SHADER_BIT_OES 0x00000010

typedef void (GL_APIENTRYP PFNGLPATCHPARAMETERIOESPROC)(GLenum pname, GLint value);

#ifdef GL_GLEXT_PROTOTYPES

GL_APICALL void GL_APIENTRY glPatchParameteriOES(GLenum pname, GLint value);

#endif
#endif /* GL_OES_tessellation_shader */

#ifndef GL_OES_texture_3D
#define GL_OES_texture_3D 1
#define GL_TEXTURE_WRAP_R_OES             0x8072
#define GL_TEXTURE_3D_OES                 0x806F
#define GL_TEXTURE_BINDING_3D_OES         0x806A
#define GL_MAX_3D_TEXTURE_SIZE_OES        0x8073
#define GL_SAMPLER_3D_OES                 0x8B5F
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_OES 0x8CD4

typedef void (GL_APIENTRYP PFNGLTEXIMAGE3DOESPROC)(GLenum target, GLint level,
                                                   GLenum internalformat, GLsizei width,
                                                   GLsizei height, GLsizei depth, GLint border,
                                                   GLenum format, GLenum type, const void *pixels);

typedef void (GL_APIENTRYP PFNGLTEXSUBIMAGE3DOESPROC)(GLenum target, GLint level, GLint xoffset,
                                                      GLint yoffset, GLint zoffset, GLsizei width,
                                                      GLsizei height, GLsizei depth, GLenum format,
                                                      GLenum type, const void *pixels);

typedef void (GL_APIENTRYP PFNGLCOPYTEXSUBIMAGE3DOESPROC)(GLenum target, GLint level, GLint xoffset,
                                                          GLint yoffset, GLint zoffset, GLint x,
                                                          GLint y, GLsizei width, GLsizei height);

typedef void (GL_APIENTRYP PFNGLCOMPRESSEDTEXIMAGE3DOESPROC)(GLenum target, GLint level,
                                                             GLenum internalformat, GLsizei width,
                                                             GLsizei height, GLsizei depth,
                                                             GLint border, GLsizei imageSize,
                                                             const void *data);

typedef void (GL_APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE3DOESPROC)(GLenum target, GLint level,
                                                                GLint xoffset, GLint yoffset,
                                                                GLint zoffset, GLsizei width,
                                                                GLsizei height, GLsizei depth,
                                                                GLenum format, GLsizei imageSize,
                                                                const void *data);

typedef void (GL_APIENTRYP PFNGLFRAMEBUFFERTEXTURE3DOESPROC)(GLenum target, GLenum attachment,
                                                             GLenum textarget, GLuint texture,
                                                             GLint level, GLint zoffset);

#ifdef GL_GLEXT_PROTOTYPES

GL_APICALL void GL_APIENTRY
glTexImage3DOES(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height,
                GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels);

GL_APICALL void GL_APIENTRY
glTexSubImage3DOES(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset,
                   GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type,
                   const void *pixels);

GL_APICALL void GL_APIENTRY
glCopyTexSubImage3DOES(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset,
                       GLint x, GLint y, GLsizei width, GLsizei height);

GL_APICALL void GL_APIENTRY
glCompressedTexImage3DOES(GLenum target, GLint level, GLenum internalformat, GLsizei width,
                          GLsizei height, GLsizei depth, GLint border, GLsizei imageSize,
                          const void *data);

GL_APICALL void GL_APIENTRY
glCompressedTexSubImage3DOES(GLenum target, GLint level, GLint xoffset, GLint yoffset,
                             GLint zoffset, GLsizei width, GLsizei height, GLsizei depth,
                             GLenum format, GLsizei imageSize, const void *data);

GL_APICALL void GL_APIENTRY
glFramebufferTexture3DOES(GLenum target, GLenum attachment, GLenum textarget, GLuint texture,
                          GLint level, GLint zoffset);

#endif
#endif /* GL_OES_texture_3D */

#ifndef GL_OES_texture_border_clamp
#define GL_OES_texture_border_clamp 1
#define GL_TEXTURE_BORDER_COLOR_OES       0x1004
#define GL_CLAMP_TO_BORDER_OES            0x812D

typedef void (GL_APIENTRYP PFNGLTEXPARAMETERIIVOESPROC)(GLenum target, GLenum pname,
                                                        const GLint *params);

typedef void (GL_APIENTRYP PFNGLTEXPARAMETERIUIVOESPROC)(GLenum target, GLenum pname,
                                                         const GLuint *params);

typedef void (GL_APIENTRYP PFNGLGETTEXPARAMETERIIVOESPROC)(GLenum target, GLenum pname,
                                                           GLint *params);

typedef void (GL_APIENTRYP PFNGLGETTEXPARAMETERIUIVOESPROC)(GLenum target, GLenum pname,
                                                            GLuint *params);

typedef void (GL_APIENTRYP PFNGLSAMPLERPARAMETERIIVOESPROC)(GLuint sampler, GLenum pname,
                                                            const GLint *param);

typedef void (GL_APIENTRYP PFNGLSAMPLERPARAMETERIUIVOESPROC)(GLuint sampler, GLenum pname,
                                                             const GLuint *param);

typedef void (GL_APIENTRYP PFNGLGETSAMPLERPARAMETERIIVOESPROC)(GLuint sampler, GLenum pname,
                                                               GLint *params);

typedef void (GL_APIENTRYP PFNGLGETSAMPLERPARAMETERIUIVOESPROC)(GLuint sampler, GLenum pname,
                                                                GLuint *params);

#ifdef GL_GLEXT_PROTOTYPES

GL_APICALL void GL_APIENTRY glTexParameterIivOES(GLenum target, GLenum pname, const GLint *params);

GL_APICALL void GL_APIENTRY
glTexParameterIuivOES(GLenum target, GLenum pname, const GLuint *params);

GL_APICALL void GL_APIENTRY glGetTexParameterIivOES(GLenum target, GLenum pname, GLint *params);

GL_APICALL void GL_APIENTRY glGetTexParameterIuivOES(GLenum target, GLenum pname, GLuint *params);

GL_APICALL void GL_APIENTRY
glSamplerParameterIivOES(GLuint sampler, GLenum pname, const GLint *param);

GL_APICALL void GL_APIENTRY
glSamplerParameterIuivOES(GLuint sampler, GLenum pname, const GLuint *param);

GL_APICALL void GL_APIENTRY
glGetSamplerParameterIivOES(GLuint sampler, GLenum pname, GLint *params);

GL_APICALL void GL_APIENTRY
glGetSamplerParameterIuivOES(GLuint sampler, GLenum pname, GLuint *params);

#endif
#endif /* GL_OES_texture_border_clamp */

#ifndef GL_OES_texture_buffer
#define GL_OES_texture_buffer 1
#define GL_TEXTURE_BUFFER_OES             0x8C2A
#define GL_TEXTURE_BUFFER_BINDING_OES     0x8C2A
#define GL_MAX_TEXTURE_BUFFER_SIZE_OES    0x8C2B
#define GL_TEXTURE_BINDING_BUFFER_OES     0x8C2C
#define GL_TEXTURE_BUFFER_DATA_STORE_BINDING_OES 0x8C2D
#define GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT_OES 0x919F
#define GL_SAMPLER_BUFFER_OES             0x8DC2
#define GL_INT_SAMPLER_BUFFER_OES         0x8DD0
#define GL_UNSIGNED_INT_SAMPLER_BUFFER_OES 0x8DD8
#define GL_IMAGE_BUFFER_OES               0x9051
#define GL_INT_IMAGE_BUFFER_OES           0x905C
#define GL_UNSIGNED_INT_IMAGE_BUFFER_OES  0x9067
#define GL_TEXTURE_BUFFER_OFFSET_OES      0x919D
#define GL_TEXTURE_BUFFER_SIZE_OES        0x919E

typedef void (GL_APIENTRYP PFNGLTEXBUFFEROESPROC)(GLenum target, GLenum internalformat,
                                                  GLuint buffer);

typedef void (GL_APIENTRYP PFNGLTEXBUFFERRANGEOESPROC)(GLenum target, GLenum internalformat,
                                                       GLuint buffer, GLintptr offset,
                                                       GLsizeiptr size);

#ifdef GL_GLEXT_PROTOTYPES

GL_APICALL void GL_APIENTRY glTexBufferOES(GLenum target, GLenum internalformat, GLuint buffer);

GL_APICALL void GL_APIENTRY
glTexBufferRangeOES(GLenum target, GLenum internalformat, GLuint buffer, GLintptr offset,
                    GLsizeiptr size);

#endif
#endif /* GL_OES_texture_buffer */

#ifndef GL_OES_texture_compression_astc
#define GL_OES_texture_compression_astc 1
#define GL_COMPRESSED_RGBA_ASTC_3x3x3_OES 0x93C0
#define GL_COMPRESSED_RGBA_ASTC_4x3x3_OES 0x93C1
#define GL_COMPRESSED_RGBA_ASTC_4x4x3_OES 0x93C2
#define GL_COMPRESSED_RGBA_ASTC_4x4x4_OES 0x93C3
#define GL_COMPRESSED_RGBA_ASTC_5x4x4_OES 0x93C4
#define GL_COMPRESSED_RGBA_ASTC_5x5x4_OES 0x93C5
#define GL_COMPRESSED_RGBA_ASTC_5x5x5_OES 0x93C6
#define GL_COMPRESSED_RGBA_ASTC_6x5x5_OES 0x93C7
#define GL_COMPRESSED_RGBA_ASTC_6x6x5_OES 0x93C8
#define GL_COMPRESSED_RGBA_ASTC_6x6x6_OES 0x93C9
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_3x3x3_OES 0x93E0
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x3x3_OES 0x93E1
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x3_OES 0x93E2
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x4_OES 0x93E3
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4x4_OES 0x93E4
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x4_OES 0x93E5
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x5_OES 0x93E6
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5x5_OES 0x93E7
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x5_OES 0x93E8
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x6_OES 0x93E9
#endif /* GL_OES_texture_compression_astc */

#ifndef GL_OES_texture_cube_map_array
#define GL_OES_texture_cube_map_array 1
#define GL_TEXTURE_CUBE_MAP_ARRAY_OES     0x9009
#define GL_TEXTURE_BINDING_CUBE_MAP_ARRAY_OES 0x900A
#define GL_SAMPLER_CUBE_MAP_ARRAY_OES     0x900C
#define GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW_OES 0x900D
#define GL_INT_SAMPLER_CUBE_MAP_ARRAY_OES 0x900E
#define GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY_OES 0x900F
#define GL_IMAGE_CUBE_MAP_ARRAY_OES       0x9054
#define GL_INT_IMAGE_CUBE_MAP_ARRAY_OES   0x905F
#define GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY_OES 0x906A
#endif /* GL_OES_texture_cube_map_array */

#ifndef GL_OES_texture_float
#define GL_OES_texture_float 1
#endif /* GL_OES_texture_float */

#ifndef GL_OES_texture_float_linear
#define GL_OES_texture_float_linear 1
#endif /* GL_OES_texture_float_linear */

#ifndef GL_OES_texture_half_float
#define GL_OES_texture_half_float 1
#define GL_HALF_FLOAT_OES                 0x8D61
#endif /* GL_OES_texture_half_float */

#ifndef GL_OES_texture_half_float_linear
#define GL_OES_texture_half_float_linear 1
#endif /* GL_OES_texture_half_float_linear */

#ifndef GL_OES_texture_npot
#define GL_OES_texture_npot 1
#endif /* GL_OES_texture_npot */

#ifndef GL_OES_texture_stencil8
#define GL_OES_texture_stencil8 1
#define GL_STENCIL_INDEX_OES              0x1901
#define GL_STENCIL_INDEX8_OES             0x8D48
#endif /* GL_OES_texture_stencil8 */

#ifndef GL_OES_texture_storage_multisample_2d_array
#define GL_OES_texture_storage_multisample_2d_array 1
#define GL_TEXTURE_2D_MULTISAMPLE_ARRAY_OES 0x9102
#define GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY_OES 0x9105
#define GL_SAMPLER_2D_MULTISAMPLE_ARRAY_OES 0x910B
#define GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY_OES 0x910C
#define GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY_OES 0x910D

typedef void (GL_APIENTRYP PFNGLTEXSTORAGE3DMULTISAMPLEOESPROC)(GLenum target, GLsizei samples,
                                                                GLenum internalformat,
                                                                GLsizei width, GLsizei height,
                                                                GLsizei depth,
                                                                GLboolean fixedsamplelocations);

#ifdef GL_GLEXT_PROTOTYPES

GL_APICALL void GL_APIENTRY
glTexStorage3DMultisampleOES(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width,
                             GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);

#endif
#endif /* GL_OES_texture_storage_multisample_2d_array */

#ifndef GL_OES_texture_view
#define GL_OES_texture_view 1
#define GL_TEXTURE_VIEW_MIN_LEVEL_OES     0x82DB
#define GL_TEXTURE_VIEW_NUM_LEVELS_OES    0x82DC
#define GL_TEXTURE_VIEW_MIN_LAYER_OES     0x82DD
#define GL_TEXTURE_VIEW_NUM_LAYERS_OES    0x82DE
#define GL_TEXTURE_IMMUTABLE_LEVELS       0x82DF

typedef void (GL_APIENTRYP PFNGLTEXTUREVIEWOESPROC)(GLuint texture, GLenum target,
                                                    GLuint origtexture, GLenum internalformat,
                                                    GLuint minlevel, GLuint numlevels,
                                                    GLuint minlayer, GLuint numlayers);

#ifdef GL_GLEXT_PROTOTYPES

GL_APICALL void GL_APIENTRY
glTextureViewOES(GLuint texture, GLenum target, GLuint origtexture, GLenum internalformat,
                 GLuint minlevel, GLuint numlevels, GLuint minlayer, GLuint numlayers);

#endif
#endif /* GL_OES_texture_view */

#ifndef GL_OES_vertex_array_object
#define GL_OES_vertex_array_object 1
#define GL_VERTEX_ARRAY_BINDING_OES       0x85B5

typedef void (GL_APIENTRYP PFNGLBINDVERTEXARRAYOESPROC)(GLuint array);

typedef void (GL_APIENTRYP PFNGLDELETEVERTEXARRAYSOESPROC)(GLsizei n, const GLuint *arrays);

typedef void (GL_APIENTRYP PFNGLGENVERTEXARRAYSOESPROC)(GLsizei n, GLuint *arrays);

typedef GLboolean (GL_APIENTRYP PFNGLISVERTEXARRAYOESPROC)(GLuint array);

#ifdef GL_GLEXT_PROTOTYPES

GL_APICALL void GL_APIENTRY glBindVertexArrayOES(GLuint array);

GL_APICALL void GL_APIENTRY glDeleteVertexArraysOES(GLsizei n, const GLuint *arrays);

GL_APICALL void GL_APIENTRY glGenVertexArraysOES(GLsizei n, GLuint *arrays);

GL_APICALL GLboolean GL_APIENTRY glIsVertexArrayOES(GLuint array);

#endif
#endif /* GL_OES_vertex_array_object */

#ifndef GL_OES_vertex_half_float
#define GL_OES_vertex_half_float 1
#endif /* GL_OES_vertex_half_float */

#ifndef GL_OES_vertex_type_10_10_10_2
#define GL_OES_vertex_type_10_10_10_2 1
#define GL_UNSIGNED_INT_10_10_10_2_OES    0x8DF6
#define GL_INT_10_10_10_2_OES             0x8DF7
#endif /* GL_OES_vertex_type_10_10_10_2 */

#ifndef GL_OES_viewport_array
#define GL_OES_viewport_array 1
#define GL_MAX_VIEWPORTS_OES              0x825B
#define GL_VIEWPORT_SUBPIXEL_BITS_OES     0x825C
#define GL_VIEWPORT_BOUNDS_RANGE_OES      0x825D
#define GL_VIEWPORT_INDEX_PROVOKING_VERTEX_OES 0x825F

typedef void (GL_APIENTRYP PFNGLVIEWPORTARRAYVOESPROC)(GLuint first, GLsizei count,
                                                       const GLfloat *v);

typedef void (GL_APIENTRYP PFNGLVIEWPORTINDEXEDFOESPROC)(GLuint index, GLfloat x, GLfloat y,
                                                         GLfloat w, GLfloat h);

typedef void (GL_APIENTRYP PFNGLVIEWPORTINDEXEDFVOESPROC)(GLuint index, const GLfloat *v);

typedef void (GL_APIENTRYP PFNGLSCISSORARRAYVOESPROC)(GLuint first, GLsizei count, const GLint *v);

typedef void (GL_APIENTRYP PFNGLSCISSORINDEXEDOESPROC)(GLuint index, GLint left, GLint bottom,
                                                       GLsizei width, GLsizei height);

typedef void (GL_APIENTRYP PFNGLSCISSORINDEXEDVOESPROC)(GLuint index, const GLint *v);

typedef void (GL_APIENTRYP PFNGLDEPTHRANGEARRAYFVOESPROC)(GLuint first, GLsizei count,
                                                          const GLfloat *v);

typedef void (GL_APIENTRYP PFNGLDEPTHRANGEINDEXEDFOESPROC)(GLuint index, GLfloat n, GLfloat f);

typedef void (GL_APIENTRYP PFNGLGETFLOATI_VOESPROC)(GLenum target, GLuint index, GLfloat *data);

#ifdef GL_GLEXT_PROTOTYPES

GL_APICALL void GL_APIENTRY glViewportArrayvOES(GLuint first, GLsizei count, const GLfloat *v);

GL_APICALL void GL_APIENTRY
glViewportIndexedfOES(GLuint index, GLfloat x, GLfloat y, GLfloat w, GLfloat h);

GL_APICALL void GL_APIENTRY glViewportIndexedfvOES(GLuint index, const GLfloat *v);

GL_APICALL void GL_APIENTRY glScissorArrayvOES(GLuint first, GLsizei count, const GLint *v);

GL_APICALL void GL_APIENTRY
glScissorIndexedOES(GLuint index, GLint left, GLint bottom, GLsizei width, GLsizei height);

GL_APICALL void GL_APIENTRY glScissorIndexedvOES(GLuint index, const GLint *v);

GL_APICALL void GL_APIENTRY glDepthRangeArrayfvOES(GLuint first, GLsizei count, const GLfloat *v);

GL_APICALL void GL_APIENTRY glDepthRangeIndexedfOES(GLuint index, GLfloat n, GLfloat f);

GL_APICALL void GL_APIENTRY glGetFloati_vOES(GLenum target, GLuint index, GLfloat *data);

#endif
#endif /* GL_OES_viewport_array */
