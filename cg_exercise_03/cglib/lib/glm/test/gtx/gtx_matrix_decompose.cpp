#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

int main()
{
	int Error(0);

	glm::mat4 Matrix(1);

	glm::vec3 Scale;
	glm::quat Orientation;
	glm::vec3 Translation;
	glm::vec3 Skew(1);
	glm::vec4 Perspective(1);

	glm::decompose(Matrix, Scale, Orientation, Translation, Skew, Perspective);

	return Error;
}
// CG_REVISION 2c745227b702779fc1999dee8c34f43b977f0871
