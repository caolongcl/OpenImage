/// @ref core
/// @file glm/mat3x4.hpp

#pragma once

#include "detail/type_mat3x4.hpp"

namespace glm {
/// 3 columns of 4 components matrix of low precision floating-point numbers.
/// There is no guarantee on the actual precision.
///
/// @see <a href="http://www.opengl.org/registry/doc/GLSLangSpec.4.20.8.pdf">GLSL 4.20.8 specification, section 4.1.6 Matrices</a>
/// @see <a href="http://www.opengl.org/registry/doc/GLSLangSpec.4.20.8.pdf">GLSL 4.20.8 specification, section 4.7.2 Precision Qualifier</a>
typedef tmat3x4<float, lowp> lowp_mat3x4;

/// 3 columns of 4 components matrix of medium precision floating-point numbers.
/// There is no guarantee on the actual precision.
///
/// @see <a href="http://www.opengl.org/registry/doc/GLSLangSpec.4.20.8.pdf">GLSL 4.20.8 specification, section 4.1.6 Matrices</a>
/// @see <a href="http://www.opengl.org/registry/doc/GLSLangSpec.4.20.8.pdf">GLSL 4.20.8 specification, section 4.7.2 Precision Qualifier</a>
typedef tmat3x4<float, mediump> mediump_mat3x4;

/// 3 columns of 4 components matrix of high precision floating-point numbers.
/// There is no guarantee on the actual precision.
///
/// @see <a href="http://www.opengl.org/registry/doc/GLSLangSpec.4.20.8.pdf">GLSL 4.20.8 specification, section 4.1.6 Matrices</a>
/// @see <a href="http://www.opengl.org/registry/doc/GLSLangSpec.4.20.8.pdf">GLSL 4.20.8 specification, section 4.7.2 Precision Qualifier</a>
typedef tmat3x4<float, highp> highp_mat3x4;

}//namespace glm
