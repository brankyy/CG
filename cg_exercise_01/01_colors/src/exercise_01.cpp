#include <cglib/colors/exercise.h>
#include <cglib/colors/convert.h>
#include <cglib/colors/cmf.h>

#include <cglib/core/glheaders.h>
#include <cglib/core/glmstream.h>

#include <cglib/core/assert.h>
#include <iostream>

/*
 * Draw the given vertices directly as GL_TRIANGLES.
 * For each vertex, also set the corresponding color.
 */
void draw_triangles(
	std::vector<glm::vec3> const& vertices,
	std::vector<glm::vec3> const& colors)
{
	cg_assert(vertices.size() == colors.size());
	cg_assert(vertices.size() % 3 == 0);

	glBegin(GL_TRIANGLES);

	for (uint i = 0; i < vertices.size(); i++)
	{
		glColor3f(colors[i].r, colors[i].g, colors[i].b);
		glVertex3f(vertices[i].x, vertices[i].y, vertices[i].z);
	}

	glEnd();

}

/*
 * Generate a regular grid of resolution N*N (2*N*N triangles) in the xy-plane (z=0).
 * Store the grid in vertex-index form.
 *
 * The vertices of the triangles should be in counter clock-wise order.
 *
 * The grid must fill exactly the square [0, 1]x[0, 1], and must
 * be generated as an Indexed Face Set (Shared Vertex representation).
 *
 * The first vertex should be at position (0,0,0) and the last
 * vertex at position (1,1,0)
 *
 * An example for N = 3:
 *
 * 4x4=16 vertices
 *
 *   ^
 *   |  ----------
 *   |  |\ |\ |\ |
 *   |  | \| \| \|
 *   |  ----------
 *   |  |\ |\ |\ |
 * y |  | \| \| \|
 *   |  ----------
 *   |  |\ |\ |\ |
 *   |  | \| \| \|
 *   |  ----------
 *   |
 *   |-------------->
 *          x
 *
 */
void generate_grid(
	std::uint32_t N,
	std::vector<glm::vec3>* vertices,
	std::vector<glm::uvec3>* indices)
{
	cg_assert(N >= 1);
	cg_assert(vertices);
	cg_assert(indices);

	vertices->clear();
	//vertices[0]={0,0,0} --{0,0,0}
	//vertices[1]={1,0,0} --{0.3,0,0}
	//vertices[2]={2,0,0} --{0.6,0,0}
	//vertices[3]={3,0,0} --{1,0,0}
	//vertices[4]={0,1,0} --{0,0.3,0}
	//vertices[5]={1,1,0} --{0.3,0.3,0}
	//vertices[6]={2,1,0} --{0.6,0.3,0}

	for (uint j = 0; j < N + 1; j++) //row
	{
		for (uint i = 0; i < N + 1; i++) //column
		{
			vertices[0].push_back(glm::vec3((float) i / N, (float) j / N, 0.0f));
		}
	}


	indices->clear();
	//N=3
	//row=0, column=0
	//indices[0]={0,1,4} --0
	//indices[1]={4,1,5} --{z0,y0,z0+1}
	//row=0, column=1
	//indices[2]={1,2,5} --1
	//indices[3]={5,2,6}
	//row=0, column=2
	//indices[4]={2,3,6} --2
	//indices[5]={6,3,7}
	//row=1, column=0
	//indices[6]={4,5,8} --4
	//indices[7]={8,5,9}
	//row=1, column=1
	//indices[8]={5,6,9} --5
	//indices[9]={9,6,10}
	//row=1, column=2
	//indices[10]={6,7,10} --6
	//indices[11]={10,7,11}
	// --(row * (N + 1) + column)
	for (uint row = 0; row < N; row++)
	{
		for (uint column = 0; column < N; column++)
		{
			int temp = row * (N + 1) + column;
			indices[0].push_back(glm::uvec3(temp, temp + 1, temp + 4));
			indices[0].push_back(glm::uvec3(temp + 4, temp + 1, temp + 5));
		}
	}
}

/*
 * Draw the given vertices as indexed GL_TRIANGLES using glDrawElements.
 * For each vertex, also set the corresponding color.
 *
 * Don't forget to enable the correct client states. After drawing
 * the triangles, you need to disable the client states again.
 */
void draw_indexed_triangles(
	std::vector<glm::vec3>  const& vertices,
	std::vector<glm::vec3>  const& colors,
	std::vector<glm::uvec3> const& indices)
{
	cg_assert(vertices.size() == colors.size());

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, vertices.data());
	glColorPointer(3, GL_FLOAT, 0, colors.data());
	glDrawElements(GL_TRIANGLES, indices.size() * 3, GL_UNSIGNED_INT, indices.data());

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}

/*
 * Generate a triangle strip with N segments (2*N triangles)
 * in the xy plane (z=0).
 *
 * The vertices of the triangles should be in counter clock-wise order.
 *
 * The triangle strip must fill exactly the square [0, 1]x[0, 1].
 *
 * The first vertex should be at position (0,1,0) and the last
 * vertex at position (1,0,0)
 *
 * An example for N = 3:
 *
 *   ^
 *   |  ----------
 *   |  | /| /| /|
 * y |  |/ |/ |/ |
 *   |  ----------
 *   |
 *   |-------------->
 *           x
 *
 */
void generate_strip(
	std::uint32_t N,
	std::vector<glm::vec3>* vertices)
{
	cg_assert(N >= 1);
	cg_assert(vertices);

	vertices->clear();

	for (uint i = 0; i < N + 1; i++) //column
	{
		vertices[0].push_back(glm::vec3((float) i / N, 1.0f , 0.0f));
		vertices[0].push_back(glm::vec3((float) i / N, 0.0f , 0.0f));
	}

}

/*
 * Draw the given vertices as a triangle strip.
 * For each vertex, also set the corresponding color.
 */
void draw_triangle_strip(
	std::vector<glm::vec3> const& vertices,
	std::vector<glm::vec3> const& colors)
{
	cg_assert(vertices.size() == colors.size());

	glBegin(GL_TRIANGLE_STRIP);

	for (uint i = 0; i < vertices.size(); i++)
	{
		glColor3f(colors[i].r, colors[i].g, colors[i].b);
		glVertex3f(vertices[i].x, vertices[i].y, vertices[i].z);
	}

	glEnd();

}

/*
 * Integrate the given piecewise linear function
 * using trapezoidal integration.
 *
 * The function is given at points
 *     x[0], ..., x[N]
 * and its corresponding values are
 *     y[0], ..., y[N]
 */
float integrate_trapezoidal(
	std::vector<float> const& x,
	std::vector<float> const& y)
{
	cg_assert(x.size() == y.size());
	cg_assert(x.size() > 1);

	float r = 0.0f;
	for (uint i = 1; i < x.size() + 1; i++)
	{
		r += ((x[i] - x[i - 1])) * (y[i - 1] + y[i]) / 2;
	}

	return r;
}

/*
 * Convert the given spectrum to RGB using your
 * implementation of integrate_trapezoidal(...)
 *
 * The color matching functions and the wavelengths
 * for which they are given can be found in
 *     cglib/colors/cmf.h
 * and
 *     cglib/src/colors/cmf.cpp
 *
 * The wavelengths corresponding to the spectral values
 * given in spectrum are defined in cmf::wavelengths
 */
glm::vec3 spectrum_to_rgb(std::vector<float> const& spectrum)
{
	cg_assert(spectrum.size() == cmf::wavelengths.size());

	std::vector<float> sx;
	std::vector<float> sy;
	std::vector<float> sz;

	for(uint i = 0; i < spectrum.size(); i++)
	{
		sx.push_back(spectrum[i] * cmf::x[i]);
		sy.push_back(spectrum[i] * cmf::y[i]);
		sz.push_back(spectrum[i] * cmf::z[i]);
	}

	float x = integrate_trapezoidal(cmf::wavelengths,sx);
	float y = integrate_trapezoidal(cmf::wavelengths,sy);
	float z = integrate_trapezoidal(cmf::wavelengths,sz);

	return convert::xyz_to_rgb(glm::vec3(x,y,z));
}
// CG_REVISION 719c9337e18b5a8d331f5b5580d53f2438ab503f
