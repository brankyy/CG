#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec2.hpp>
#include <glm/gtx/texture.hpp>

int test_levels()
{
	int Error = 0;

	int const Levels = glm::levels(glm::ivec2(3, 2));
	Error += Levels == 2 ? 0 : 1;

	return Error;
}

int main()
{
	int Error = 0;

	Error += test_levels();

	return Error;
}
// CG_REVISION 719c9337e18b5a8d331f5b5580d53f2438ab503f
