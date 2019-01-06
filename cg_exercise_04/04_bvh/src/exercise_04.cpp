#include <cglib/rt/bvh.h>

#include <cglib/rt/triangle_soup.h>

#include <cglib/core/image.h>
#include <complex>

/*
 * Create a 1 dimensional normalized gauss kernel
 *
 * Parameters:
 *  - sigma:       the parameter of the gaussian
 *  - kernel_size: the size of the kernel (has to be odd)
 *  - kernel:      an array with size kernel_size elements that
 *                 has to be filled with the kernel values
 *
 */
void Image::create_gaussian_kernel_1d(
		float sigma,
		int kernel_size,
		float* kernel) 
{
	cg_assert(kernel_size%2==1);

	// TODO: calculate filter values as described on the exercise sheet. 
	// Make sure your kernel is normalized
	int kernel_center = (kernel_size - 1) / 2;
	float sum_kernel  = 0.f;
	float sigma2      = 2 *(sigma * sigma);
	
	for (int i = 0; i < kernel_size; ++i) {
		int distance = i - kernel_center;
		kernel[i]    = 1.f / (sqrt(2.f * glm::pi<float>()) * sigma) * exp(-pow(distance,2) / sigma2);
		sum_kernel   += kernel[i];
	}

	for (int i = 0; i < kernel_size; ++i){
		kernel[i] /= sum_kernel;
	}
}

/*
 * Create a 2 dimensional quadratic and normalized gauss kernel
 *
 * Parameters:
 *  - sigma:       the parameter of the gaussian
 *  - kernel_size: the size of the kernel (has to be odd)
 *  - kernel:      an array with kernel_size*kernel_size elements that
 *                 has to be filled with the kernel values
 */
void Image::create_gaussian_kernel_2d(
		float sigma,
		int kernel_size,
		float* kernel) 
{
	cg_assert(kernel_size%2==1);

	int kernel_center = (kernel_size - 1) / 2;
	float sum_kernel  = 0.f;
	float sigma2      = 2 *(sigma * sigma);

	// TODO: calculate filter values as described on the exercise sheet. 
	// Make sure your kernel is normalized
	for (int j = 0; j < kernel_size; ++j) 
	{
		for (int i = 0; i < kernel_size; ++i) 
		{
			int distance_x              = i - kernel_center;
			int distance_y              = j - kernel_center;
			kernel[i + j * kernel_size] = 1.f / (glm::pi<float>() * sigma2) * exp(-(pow(distance_x,2) + pow(distance_y,2)) / sigma2);
			sum_kernel                  += kernel[i + j * kernel_size];
		}
	}

	for (int k = 0; k < kernel_size * kernel_size; ++k)
	{
		kernel[k] /=sum_kernel;
	}
}

/*
 * Convolve an image with a 2d filter kernel
 *
 * Parameters:
 *  - kernel_size: the size of the 2d-kernel
 *  - kernel:      the 2d-kernel with kernel_size*kernel_size elements
 *  - wrap_mode:   needs to be known to handle repeating 
 *                 textures correctly
 */
void Image::filter(Image *target, int kernel_size, float* kernel, WrapMode wrap_mode) const
{
	cg_assert (kernel_size%2==1 && "kernel size should be odd.");
	cg_assert (kernel_size > 0 && "kernel size should be greater than 0.");
	cg_assert (target);
	cg_assert (target->getWidth() == m_width && target->getHeight() == m_height);

	Image *temptarget = new Image(m_width,m_height);
	int r = (kernel_size - 1) / 2;

	for (int x = 0; x < getWidth(); ++x){
		for ( int y = 0; y < getHeight(); ++y){
			glm::vec4 newpixel(0.f);
			for (int j = 0; j < kernel_size; ++j){
				for (int i = 0; i < kernel_size; ++i){
					int m = x - r + i;
					int n = y - r + j;
					if (wrap_mode == REPEAT)
					{
						newpixel += getPixel(m, n,REPEAT) * kernel[i + j * kernel_size];
						// m = (m % getWidth() + getWidth()) % getWidth();
						// n = (n % getHeight() + getHeight()) % getHeight();
					}
					if (wrap_mode == CLAMP)
					{
						newpixel += getPixel(m, n,CLAMP) * kernel[i + j * kernel_size];
						// m = glm::clamp(m,0,getWidth() - 1);
						// n = glm::clamp(n,0,getHeight() - 1);						
					}
					// newpixel += getPixel(m, n) * kernel[i + j * kernel_size];
				}
			}
			temptarget->setPixel(x,y,newpixel);		// temptarget->setPixel(x,y,newpixel);
		}
	}
	
	for (int x = 0; x < getWidth(); ++x){
		for (int y = 0; y < getHeight(); ++y){
			target->setPixel(x,y,temptarget->getPixel(x,y));
		}
	} 

}

/*
 * Convolve an image with a separable 1d filter kernel
 *
 * Parameters:
 *  - kernel_size: the size of the 1d kernel
 *  - kernel:      the 1d-kernel with kernel_size elements
 *  - wrap_mode:   needs to be known to handle repeating 
 *                 textures correctly
 */
void Image::filter_separable(Image *target, int kernel_size, float* kernel, WrapMode wrap_mode) const
{
	cg_assert (kernel_size%2==1 && "kernel size should be odd.");
	cg_assert (kernel_size > 0 && "kernel size should be greater than 0.");
	cg_assert (target);
	cg_assert (target->getWidth() == m_width && target->getHeight() == m_height);

	// TODO: realize the 2d convolution with two
	// convolutions of the image with a 1d-kernel.
	// convolve the image horizontally and then convolve
	// the result vertically (or vise-versa).
	//
	// use the methods getPixel(x, y, wrap_mode) and
	// setPixel(x, y, value) to get and set pixels of an image
	Image *temptarget = new Image(getWidth(),getHeight());
	int kernel_center = (kernel_size - 1) / 2;

	for (int x = 0; x < getWidth(); ++x)
	{
		for (int y = 0; y < getHeight(); ++y){
			glm::vec4 newpixel(0.f);
			for ( int i = 0; i < kernel_size; ++i){
				int m = x - kernel_center + i;
				if (wrap_mode == REPEAT){
					newpixel += getPixel(m,y,REPEAT) * kernel[i];
				}
				if (wrap_mode == CLAMP){
					newpixel += getPixel(m,y,CLAMP) * kernel[i];
				}
			}
			temptarget->setPixel(x,y,newpixel);
		}
	}
	
	for (int x = 0; x < getWidth(); ++x)
	{
		for (int y = 0; y < getHeight(); ++y){
			glm::vec4 newpixel(0.f);
			for ( int i = 0; i < kernel_size; ++i){
				int n = y - kernel_center + i;
				if (wrap_mode == REPEAT){
					newpixel += temptarget->getPixel(x,n,REPEAT) * kernel[i];
				}
				if (wrap_mode == CLAMP){
					newpixel += temptarget->getPixel(x,n,CLAMP) * kernel[i];
				}
			}
			temptarget->setPixel(x,y,newpixel);
		}
	}
	for (int x = 0; x < getWidth(); ++x){
		for (int y = 0; y < getHeight(); ++y){
			target->setPixel(x,y,temptarget->getPixel(x,y));
		}
	}
}

/**
 * Reorder triangle indices in the vector triangle_indices 
 * in the range [first_triangle_idx, first_triangle_idx+num_triangles-1] 
 * so that the range is split in two sets where all triangles in the first set
 * are "less than equal" than the median, and all triangles in the second set
 * are "greater than equal" the median.
 *
 * Ordering ("less than") is defined by the ordering of triangle
 * bounding box centers along the given axis.
 *
 * Triangle indices within a set need not be sorted.
 *
 * The resulting sets must have an equal number of elements if num_triangles
 * is even. Otherwise, one of the sets must have one more element.
 *
 * For example, 8 triangles must be split 4-4. 7 Triangles must be split
 * 4-3 or 3-4.
 *
 * Parameters:
 *  - first_triangle_idx: The index of the first triangle in the given range.
 *  - num_triangles:      The number of triangles in the range.
 *  - axis:               The sort axis. 0 is X, 1 is Y, 2 is Z.
 *
 * Return value:
 *  - The number of triangles in the first set.
 */


struct triangleSort {
	const TriangleSoup& soup;
	const int axis;
	triangleSort (const TriangleSoup& soup, const int axis) :soup(soup), axis(axis){}

	glm::vec3 getBBCenter(int index) {
		glm::vec3 min(soup.vertices[3 * index]);
		glm::vec3 max(min);
		for (int restIndex = 1; restIndex < 3; restIndex++) {
			for (int i = 0; i < 3; i++) {
				float point3DValue = soup.vertices[3 * index + restIndex][i];
				if (min[i] > point3DValue) min[i] = point3DValue;
				if (max[i] < point3DValue) max[i] = point3DValue; 
			}
		}
		return (min + (max - min) / 2.f);
	}

	bool operator()(int i, int j) {
		glm::vec3 centerI(getBBCenter(i));
		glm::vec3 centerJ(getBBCenter(j));
		return (centerI[axis] < centerJ[axis]);
	}
};

int BVH::reorder_triangles_median(
	int first_triangle_idx, 
	int num_triangles, 
	int axis)
{
	cg_assert(first_triangle_idx < static_cast<int>(triangle_indices.size()));
	cg_assert(first_triangle_idx >= 0);
	cg_assert(num_triangles <= static_cast<int>(triangle_indices.size() - first_triangle_idx));
	cg_assert(num_triangles > 1);
	cg_assert(axis >= 0);
	cg_assert(axis < 3);

	// TODO: Implement reordering.
	triangleSort sorter(triangle_soup, axis);

	std::sort(triangle_indices.begin() + first_triangle_idx, triangle_indices.begin() + first_triangle_idx + num_triangles, sorter);
	return num_triangles / 2;
}

/*
 * Build a BVH recursively using the object median split heuristic.
 *
 * This method must first fully initialize the current node, and then
 * potentially split it. 
 *
 * A node must not be split if it contains MAX_TRIANGLES_IN_LEAF triangles or
 * less. No leaf node may be empty. All nodes must have either two or no
 * children.
 *
 * Use reorder_triangles_median to perform the split in triangle_indices.
 * Split along X for depth 0. Then, proceed in the order Y, Z, X, Y, Z, X, Y, ..
 *
 * Parameters:
 *  - node_idx:           The index of the node to be split.
 *  - first_triangle_idx: An index into the array triangle_indices. It points 
 *                        to the first triangle contained in the current node.
 *  - num_triangles:      The number of triangles contained in the current node.
 */
void BVH::
build_bvh(int node_idx, int first_triangle_idx, int num_triangles, int depth)
{
	cg_assert(num_triangles > 0);
	cg_assert(node_idx >= 0);
	cg_assert(node_idx < static_cast<int>(nodes.size()));
	cg_assert(depth >= 0);

	// TODO: Implement recursive build.
	nodes[node_idx].triangle_idx  = first_triangle_idx;
	nodes[node_idx].num_triangles = num_triangles;
	nodes[node_idx].aabb.min      = glm::vec3(-FLT_MAX);
	nodes[node_idx].aabb.max      = glm::vec3(FLT_MAX);
	nodes[node_idx].left          = -1;
	nodes[node_idx].right         = -1;

	int first_triangle = triangle_indices[first_triangle_idx];
	glm::vec3 min(triangle_soup.vertices[3 * first_triangle]);
	glm::vec3 max(triangle_soup.vertices[3 * first_triangle]);

	for (int triangleOffset = 0; triangleOffset < num_triangles; triangleOffset++) {
		for (int vertexIndex = 0; vertexIndex < 3; vertexIndex++) {
			for (int dimensionIndex = 0; dimensionIndex < 3; dimensionIndex++) {
				int triangle = triangle_indices[first_triangle_idx + triangleOffset];
				float dimValue = triangle_soup.vertices[3 * triangle + vertexIndex][dimensionIndex];
				if (min[dimensionIndex] > dimValue) min[dimensionIndex] = dimValue;
				if (max[dimensionIndex] < dimValue) max[dimensionIndex] = dimValue; 
			}
		}
	}

	nodes[node_idx].aabb.min = min;
	nodes[node_idx].aabb.max = max;

	if (nodes[node_idx].num_triangles > BVH::MAX_TRIANGLES_IN_LEAF) {
		int newnumtriangles = reorder_triangles_median(first_triangle_idx, num_triangles, (depth % 3));
		Node nodeLeft;
		Node nodeRight;
		nodes.push_back(nodeLeft);
		nodes.push_back(nodeRight);
		nodes[node_idx].left = nodes.size() - 2;
		nodes[node_idx].right = nodes.size() - 1;

		build_bvh(nodes[node_idx].left, first_triangle_idx, newnumtriangles, depth + 1);
		build_bvh(nodes[node_idx].right, first_triangle_idx + newnumtriangles, num_triangles - newnumtriangles, depth + 1);
	}

}

/*
 * Intersect the BVH recursively, returning the nearest intersection if
 * there is one.
 *
 * Caution: BVH nodes may overlap.
 *
 * Parameters:
 *  - ray:                  The ray to intersect the BVH with.
 *  - idx:                  The node to be intersected.
 *  - nearest_intersection: The distance to the intersection point, if an 
 *                          intersection was found. Must not be changed 
 *                          otherwise.
 *  - isect:                The intersection, if one was found. Must not be 
 *                          changed otherwise.
 *
 * Return value:
 *  true if an intersection was found, false otherwise.
 */
bool BVH::
intersect_recursive(const Ray &ray, int idx, float *nearest_intersection, Intersection* isect) const
{
	cg_assert(nearest_intersection);
	cg_assert(isect);
	cg_assert(idx >= 0);
	cg_assert(idx < static_cast<int>(nodes.size()));

	const Node &n = nodes[idx];

	// This is a leaf node. Intersect all triangles.
	if(n.left < 0) { 
		glm::vec3 bary(0.f);
		bool hit = false;
		for(int i = 0; i < n.num_triangles; i++) {
			int x = triangle_indices[n.triangle_idx + i];
			float dist;
			glm::vec3 b;
			if(intersect_triangle(ray.origin, ray.direction,
						triangle_soup.vertices[x * 3 + 0],
						triangle_soup.vertices[x * 3 + 1],
						triangle_soup.vertices[x * 3 + 2], 
						b, dist)) {
				hit = true;
				if(dist <= *nearest_intersection) {
					*nearest_intersection = dist;
					bary = b;
					cg_assert(x >= 0);
					if(isect)
						triangle_soup.fill_intersection(isect, x, *nearest_intersection, bary);
				}
			}
		}
		return hit;
	}

	// This is an inner node. Recurse into child nodes.
	else { 
		// TODO: Implement recursive traversal here.
		float t1_min = 0.0f;
		float t1_max = FLT_MAX;
		float t1 = FLT_MAX;
		float t2_min = 0.0f;
		float t2_max = FLT_MAX;
		float t2 = FLT_MAX;
		bool t1hit = false;
		bool t2hit = false;

		if (nodes[n.left].aabb.intersect(ray, t1_min, t1_max)) {
			t1 = t1_min > 0 ? t1_min : t1_max;
			t1hit = true;
		}

		if (nodes[n.right].aabb.intersect(ray, t2_min, t2_max)) {
			t2 = t2_min > 0 ? t2_min : t2_max;
			t2hit = true;
		}

		if (t1hit && t2hit) {
			Intersection isect_local_1;
			Intersection isect_local_2;
			if (t1 <= t2) {
				if (intersect_recursive(ray, n.left, nearest_intersection, &isect_local_1)) {
					*isect = isect_local_1;
					if (intersect_recursive(ray, n.right, nearest_intersection, &isect_local_2)) {
						if (isect_local_2.t < isect_local_1.t) {
							*isect = isect_local_2;
						}
					}
					return true;
				}
				if (intersect_recursive(ray, n.right, nearest_intersection, isect)) return true;
			}
			else {
				if (intersect_recursive(ray, n.right, nearest_intersection, &isect_local_2)) {
					*isect = isect_local_2;
					if (intersect_recursive(ray, n.left, nearest_intersection, &isect_local_1)) {
						if (isect_local_1.t < isect_local_2.t) {
							*isect = isect_local_1;
						}
					}
					return true;
				}
				if (intersect_recursive(ray, n.left, nearest_intersection, isect)) return true;
			}
		}
		else if (t1hit) {
			if (intersect_recursive(ray, n.left, nearest_intersection, isect)) return true;
		}
		else if (t2hit){
			if (intersect_recursive(ray, n.right, nearest_intersection, isect)) return true;
		}
	}

	return false;
}

// Spectrum and reconstruction are 2D memory blocks of the same size [sx, sy]
// in row major layout, i.e one row of size sx after the other. 
//
// Spectrum contains the complex fourier-coefficients \hat{x}_{kl}.
//
// Reconstruct the original grayscale image using fourier transformation!
void DiscreteFourier2D::reconstruct(
	int M, int N,
	std::complex<float> const* spectrum,
	std::complex<float>	* reconstruction)
{
	float pi_2 = 2 * glm::pi<float>();
	float mn = 1 / sqrtf(M * N);
	


	for (int l = 0; l < N; ++l){
		for (int k = 0; k < M; ++k){
				std::complex<float> temp (0.f,0.f);
				std::complex<float> sum (0.f,0.f);

			for (int n = 0; n < N; ++n){
				for (int m =0; m < M; ++m){
					float r = spectrum[n * M + m].real();
					float i = spectrum[n * M + m].imag();
					float coefficient = pi_2 * (k * ((float)m / (float)M - 0.5f) + l * ((float)n / (float)N - 0.5f));
					temp = std::complex<float>(r * glm::cos(coefficient)-(i * glm::sin(coefficient)),
											   r * glm::sin(coefficient) + i * glm::cos(coefficient));
					sum += temp;

				}
			}
			reconstruction[k + l * M] = mn * sum;
		}
	}

}
// CG_REVISION 4ad3af6a3d282d95a68e54f217347edc0b637413
