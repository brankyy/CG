/// @ref core
/// @file glm/mat2x4.hpp

#include "detail/setup.hpp"

#pragma once

#include "detail/type_mat2x4.hpp"

namespace glm
{
	/// 2 columns of 4 components matrix of low qualifier floating-point numbers.
	/// There is no guarantee on the actual qualifier.
	///
	/// @see <a href="http://www.opengl.org/registry/doc/GLSLangSpec.4.20.8.pdf">GLSL 4.20.8 specification, section 4.1.6 Matrices</a>
	/// @see <a href="http://www.opengl.org/registry/doc/GLSLangSpec.4.20.8.pdf">GLSL 4.20.8 specification, section 4.7.2 Precision Qualifier</a>
	typedef mat<2, 4, float, lowp>		lowp_mat2x4;

	/// 2 columns of 4 components matrix of medium qualifier floating-point numbers.
	/// There is no guarantee on the actual qualifier.
	///
	/// @see <a href="http://www.opengl.org/registry/doc/GLSLangSpec.4.20.8.pdf">GLSL 4.20.8 specification, section 4.1.6 Matrices</a>
	/// @see <a href="http://www.opengl.org/registry/doc/GLSLangSpec.4.20.8.pdf">GLSL 4.20.8 specification, section 4.7.2 Precision Qualifier</a>
	typedef mat<2, 4, float, mediump>		mediump_mat2x4;

	/// 2 columns of 4 components matrix of high qualifier floating-point numbers.
	/// There is no guarantee on the actual qualifier.
	///
	/// @see <a href="http://www.opengl.org/registry/doc/GLSLangSpec.4.20.8.pdf">GLSL 4.20.8 specification, section 4.1.6 Matrices</a>
	/// @see <a href="http://www.opengl.org/registry/doc/GLSLangSpec.4.20.8.pdf">GLSL 4.20.8 specification, section 4.7.2 Precision Qualifier</a>
	typedef mat<2, 4, float, highp>		highp_mat2x4;

}//namespace glm
// CG_REVISION 4ad3af6a3d282d95a68e54f217347edc0b637413