#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/vector_relational.hpp>
#include <glm/gtc/vec1.hpp>

static int test_not()
{
	int Error(0);

	{
		glm::bvec1 v(false);
		Error += glm::all(glm::not_(v)) ? 0 : 1;
	}

	{
		glm::bvec2 v(false);
		Error += glm::all(glm::not_(v)) ? 0 : 1;
	}

	{
		glm::bvec3 v(false);
		Error += glm::all(glm::not_(v)) ? 0 : 1;
	}
	
	{
		glm::bvec4 v(false);
		Error += glm::all(glm::not_(v)) ? 0 : 1;
	}

	return Error;
}

int main()
{
	int Error(0);

	Error += test_not();

	return Error;
}

// CG_REVISION 4ad3af6a3d282d95a68e54f217347edc0b637413
