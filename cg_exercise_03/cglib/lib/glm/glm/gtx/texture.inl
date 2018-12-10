/// @ref gtx_texture
/// @file glm/gtx/texture.inl

namespace glm
{
	template <length_t L, typename T, qualifier Q>
	inline T levels(vec<L, T, Q> const& Extent)
	{
		return glm::log2(compMax(Extent)) + static_cast<T>(1);
	}

	template <typename T>
	inline T levels(T Extent)
	{
		return vec<1, T, defaultp>(Extent).x;
	}
}//namespace glm

// CG_REVISION 2c745227b702779fc1999dee8c34f43b977f0871
