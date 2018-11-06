/// @ref gtx_mixed_product
/// @file glm/gtx/mixed_product.inl

namespace glm
{
	template<typename T, qualifier Q>
	GLM_FUNC_QUALIFIER T mixedProduct
	(
		vec<3, T, Q> const& v1,
		vec<3, T, Q> const& v2,
		vec<3, T, Q> const& v3
	)
	{
		return dot(cross(v1, v2), v3);
	}
}//namespace glm
// CG_REVISION 719c9337e18b5a8d331f5b5580d53f2438ab503f
