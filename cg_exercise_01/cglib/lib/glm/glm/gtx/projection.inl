/// @ref gtx_projection
/// @file glm/gtx/projection.inl

namespace glm
{
	template<typename genType>
	GLM_FUNC_QUALIFIER genType proj(genType const& x, genType const& Normal)
	{
		return glm::dot(x, Normal) / glm::dot(Normal, Normal) * Normal;
	}
}//namespace glm
// CG_REVISION 719c9337e18b5a8d331f5b5580d53f2438ab503f
