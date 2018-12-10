/// @ref gtx_optimum_pow
/// @file glm/gtx/optimum_pow.inl

namespace glm
{
	template<typename genType>
	GLM_FUNC_QUALIFIER genType pow2(genType const& x)
	{
		return x * x;
	}

	template<typename genType>
	GLM_FUNC_QUALIFIER genType pow3(genType const& x)
	{
		return x * x * x;
	}

	template<typename genType>
	GLM_FUNC_QUALIFIER genType pow4(genType const& x)
	{
		return (x * x) * (x * x);
	}
}//namespace glm
// CG_REVISION 2c745227b702779fc1999dee8c34f43b977f0871
