/// @ref gtx_float_normalize
/// @file glm/gtx/float_normalize.inl

#include <limits>

namespace glm
{
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_QUALIFIER vec<L, float, Q> floatNormalize(vec<L, T, Q> const& v)
	{
		return vec<L, float, Q>(v) / static_cast<float>(std::numeric_limits<T>::max());
	}

}//namespace glm
// CG_REVISION 719c9337e18b5a8d331f5b5580d53f2438ab503f
