/// @ref core
/// @file glm/detail/type_half.hpp

#pragma once

#include "setup.hpp"

namespace glm{
namespace detail
{
	typedef short hdata;

	GLM_FUNC_DECL float toFloat32(hdata value);
	GLM_FUNC_DECL hdata toFloat16(float const& value);

}//namespace detail
}//namespace glm

#include "type_half.inl"
// CG_REVISION 2c745227b702779fc1999dee8c34f43b977f0871
