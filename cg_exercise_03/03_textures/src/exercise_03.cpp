#include <cglib/rt/intersection.h>
#include <cglib/rt/object.h>
#include <cglib/rt/ray.h>
#include <cglib/rt/raytracing_context.h>
#include <cglib/rt/render_data.h>
#include <cglib/rt/renderer.h>
#include <cglib/rt/texture.h>
#include <cglib/rt/texture_mapping.h>
#include <cglib/core/thread_local_data.h>

#include <cglib/core/image.h>
#include <cglib/core/glmstream.h>
#include <cglib/core/assert.h>

#include <algorithm>

// -----------------------------------------------------------------------------

/*
 * Evaluates a texture for the given uv-coordinate without filtering.
 *
 * This method transformes the uv-coordinate into a st-coordinate and
 * rounds down to integer pixel values.
 *
 * The parameter level in [0, mip_levels.size()-1] is the miplevel of
 * the texture that is to be evaluated.
 */
glm::vec4 ImageTexture::
evaluate_nearest(int level, glm::vec2 const& uv) const
{
	cg_assert(level >= 0 && level < static_cast<int>(mip_levels.size()));
	cg_assert(mip_levels[level]);

	// TODO: compute the st-coordinates for the given uv-coordinates and mipmap level
	int s = std::round(uv.x * mip_levels[level]->getWidth());
	int t = std::round(uv.y * mip_levels[level]->getHeight());

	// get the value of pixel (s, t) of miplevel level
	return get_texel(level, s, t);
}

// -----------------------------------------------------------------------------

/*
 * Implement clamping here.
 *
 * The input "val" may be arbitrary, including negative and very large positive values.
 * The method shall always return a value in [0, size).
 * Out-of-bounds inputs must be clamped to the nearest boundary.
 */
int ImageTexture::
wrap_clamp(int val, int size)
{
	cg_assert(size > 0);
	if(val < 0)
	{
		val = 0;
	}else if(val >= size)
	{
		val = size - 1;
	}

	return val;
}

/*
 * Implement repeating here.
 *
 * The input "val" may be arbitrary, including negative and very large positive values.
 * The method shall always return a value in [0, size).
 * Out-of-bounds inputs must be mapped back into [0, size) so that
 * the texture repeats infinitely.
 */
int ImageTexture::
wrap_repeat(int val, int size)
{
	cg_assert(size > 0);
	if(val < 0)
	{
		if(val % size != 0)
			val = size + val % size;
		else
			val = 0;
	}else if(val >= size)
	{
		val = val % size;
	}

	return val;
}

// -----------------------------------------------------------------------------


/*
 * Implement bilinear filtering here.
 *
 * Use mip_levels[level] as the image to filter.
 * The input uv coordinates may be arbitrary, including values outside [0, 1).
 *
 * Callers of this method must guarantee that level is valid, and
 * that mip_levels[level] is properly initialized. Compare the assertions.
 *
 * The color of a texel is to be interpreted as the color at the texel center.
 */
glm::vec4 ImageTexture::
evaluate_bilinear(int level, glm::vec2 const& uv) const
{
	cg_assert(level >= 0 && level < static_cast<int>(mip_levels.size()));
	cg_assert(mip_levels[level]);

	float s = uv.x * (float)mip_levels[level]->getWidth();
	float t = uv.y * (float)mip_levels[level]->getHeight();

	int fs = floor(s - 0.5f);
	int ft = floor(t - 0.5f);

	if(fs == s - 0.5f && ft == t - 0.5f)
	{
		return get_texel(level,s,t);
	}

	float a = s - 0.5f - fs;
	float b = ft + 1 - (t - 0.5f);

	glm::vec4 ul(get_texel(level, fs, ft + 1) * (1 - a) * (1 - b));
	glm::vec4 ur(get_texel(level, fs + 1, ft + 1) * a * (1 - b));
	glm::vec4 bl(get_texel(level, fs, ft) * (1 - a) * b);
	glm::vec4 br(get_texel(level, fs + 1, ft) * a * b);

	return ul + ur + bl + br;
}

// -----------------------------------------------------------------------------

/*
 * This method creates a mipmap hierarchy for
 * the texture.
 *
 * This is done by iteratively reducing the
 * dimenison of a mipmap level and averaging
 * pixel values until the size of a mipmap
 * level is [1, 1].
 *
 * The original data of the texture is stored
 * in mip_levels[0].
 *
 * You can allocale memory for a new mipmap level
 * with dimensions (size_x, size_y) using
 *		mip_levels.emplace_back(new Image(size_x, size_y));
 */
void ImageTexture::
create_mipmap()
{
	/* this are the dimensions of the original texture/image */
	int size_x = mip_levels[0]->getWidth();
	int size_y = mip_levels[0]->getHeight();

	cg_assert("must be power of two" && !(size_x & (size_x - 1)));
	cg_assert("must be power of two" && !(size_y & (size_y - 1)));

	int new_size_x,new_size_y;
	for(int level = 0; new_size_x != 1 || new_size_y != 1; level++)
	{
			new_size_x = size_x > 1 ? size_x / 2 : 1;
			new_size_y = size_y > 1 ? size_y / 2 : 1;

			Image* image = new Image(new_size_x, new_size_y);
			for(int j = 0; j < new_size_x; j++)
			{
					for(int k = 0; k < new_size_y; k++)
					{
							float u = (j * 2 + 1) / (float)size_x;
							float v = (k * 2 + 1) / (float)size_y;
							image->setPixel(j, k, evaluate_bilinear(level,glm::vec2(u,v)));
					}
			}
			mip_levels.emplace_back(image);
			size_x = new_size_x;
			size_y = new_size_y;
	}

}

/*
 * Compute the dimensions of the pixel footprint's AABB in uv-space.
 *
 * First intersect the four rays through the pixel corners with
 * the tangent plane at the given intersection.
 *
 * Then the given code computes uv-coordinates for these
 * intersection points.
 *
 * Finally use the uv-coordinates and compute the AABB in
 * uv-space.
 *
 * Return width (du) and height (dv) of the AABB.
 *
 */
glm::vec2 Object::
compute_uv_aabb_size(const Ray rays[4], Intersection const& isect)
{
	// TODO: compute intersection positions
	glm::vec3 intersection_positions[4] = {
		isect.position, isect.position, isect.position, isect.position
	};

	for (int i = 0; i < 4; ++i) {
		// todo: compute intersection positions using a ray->plane
		// intersection
		float t = 0;
		if(intersect_plane(rays[i].origin,rays[i].direction,isect.position,isect.normal, &t))
		{
				intersection_positions[i] = rays[i].origin+t * rays[i].direction;
		}
	}

	// compute uv coordinates from intersection positions
	glm::vec2 intersection_uvs[4];
	get_intersection_uvs(intersection_positions, isect, intersection_uvs);

	float minu = intersection_uvs[0][0];
	float maxu = intersection_uvs[0][0];
	float minv = intersection_uvs[0][1];
	float maxv = intersection_uvs[0][1];

	for (int i = 1; i < 4; i++)
	{
			if (intersection_uvs[i][0] < minu) minu = intersection_uvs[i][0];
			if (intersection_uvs[i][0] > maxu) maxu = intersection_uvs[i][0];
			if (intersection_uvs[i][1] < minv) minv = intersection_uvs[i][1];
			if (intersection_uvs[i][1] > maxv) maxv = intersection_uvs[i][1];
	}

	// TODO: compute dudv = length of sides of AABB in uv space
	return glm::vec2(maxu - minu, maxv - minv);
}

/*
 * Implement trilinear filtering at a given uv-position.
 *
 * Transform the AABB dimensions dudv in st-space and
 * take the maximal coordinate as the 1D footprint size T.
 *
 * Determine mipmap levels i and i+1 such that
 *		texel_size(i) <= T <= texel_size(i+1)
 *
 *	Hint: use std::log2(T) for that.
 *
 *	Perform bilinear filtering in both mipmap levels and
 *	linearly interpolate the resulting values.
 *
 */
glm::vec4 ImageTexture::
evaluate_trilinear(glm::vec2 const& uv, glm::vec2 const& dudv) const
{
	int width = mip_levels[0]->getWidth();
	int height = mip_levels[0]->getHeight();
	float ds = dudv.x * width;
	float dt = dudv.y * height;
	float maxD = std::max(ds, dt);
	float logMax = std::log2(maxD);
	float lowerLevel = std::floor(logMax);
	float upperLevel = std::ceil(logMax);
	float ratioUpper = logMax - lowerLevel;

	if (lowerLevel < 0)			lowerLevel = 0;
	if (upperLevel < 0)			upperLevel = 0;
	if (lowerLevel >= mip_levels.size())			lowerLevel = mip_levels.size() - 1;
	if (upperLevel >= mip_levels.size())			upperLevel = mip_levels.size() - 1;
	if (lowerLevel == upperLevel) {
			return evaluate_bilinear(upperLevel,uv);
	}
	return evaluate_bilinear(upperLevel,uv) * ratioUpper + evaluate_bilinear(lowerLevel,uv) * (1 - ratioUpper);
}

// -----------------------------------------------------------------------------

/*
 * Transform the given direction d using the matrix transform.
 *
 * The output direction must be normalized, even if d is not.
 */
glm::vec3 transform_direction(glm::mat4 const& transform, glm::vec3 const& d)
{
	glm::vec4 dhomo(d.x, d.y, d.z, 0.f);
	glm::vec4 dnewhomo(transform * dhomo);
	return glm::normalize(glm::vec3(dnewhomo[0], dnewhomo[1], dnewhomo[2]));
}

/*
 * Transform the given position p using the matrix transform.
 */
glm::vec3 transform_position(glm::mat4 const& transform, glm::vec3 const& p)
{
	glm::vec4 phomo(p.x, p.y, p.z, 1.f);
	glm::vec4 pnewhomo(transform * phomo);
	return glm::vec3(pnewhomo[0], pnewhomo[1], pnewhomo[2]);
}

/*
 * Intersect with the ray, but do so in object space.
 *
 * First, transform ray into object space. Use the methods you have
 * implemented for this.
 * Then, intersect the object with the transformed ray.
 * Finally, make sure you transform the intersection back into world space.
 *
 * isect is guaranteed to be a valid pointer.
 * The method shall return true if an intersection was found and false otherwise.
 *
 * isect must be filled properly if an intersection was found.
 */
bool Object::
intersect(Ray const& ray, Intersection* isect) const
{
	cg_assert(isect);

	if (RaytracingContext::get_active()->params.transform_objects)
	{
		// TODO: transform ray, intersect object, transform intersection
		// information back
		Ray transformedRay = transform_ray(ray,Object::transform_world_to_object);
		if(!geo->intersect(transformedRay,isect))
				return false;
		if(!isect->isValid())
				return false;

		*isect = transform_intersection(*isect, Object::transform_object_to_world, Object::transform_object_to_world_normal);
		isect->t = glm::length(isect->position - ray.origin);
		return true;
	}
	return geo->intersect(ray, isect);
}


/*
 * Transform a direction from tangent space to object space.
 *
 * Tangent space is a right-handed coordinate system where
 * the tangent is your thumb, the normal is the index finger, and
 * the bitangent is the middle finger.
 *
 * normal, tangent, and bitangent are given in object space.
 * Build a matrix that rotates d from tangent space into object space.
 * Then, transform d with this matrix to obtain the result.
 *
 * You may assume that normal, tangent, and bitangent are normalized
 * to length 1.
 *
 * The output vector must be normalized to length 1, even if d is not.
 */
glm::vec3 transform_direction_to_object_space(
	glm::vec3 const& d,
	glm::vec3 const& normal,
	glm::vec3 const& tangent,
	glm::vec3 const& bitangent)
{
	cg_assert(std::fabs(glm::length(normal)    - 1.0f) < 1e-4f);
	cg_assert(std::fabs(glm::length(tangent)   - 1.0f) < 1e-4f);
	cg_assert(std::fabs(glm::length(bitangent) - 1.0f) < 1e-4f);

	glm::mat3 rotate;
	rotate[0] = tangent;
	rotate[1] = normal;
	rotate[2] = bitangent;

	return glm::normalize(rotate * d);
}

// -----------------------------------------------------------------------------
// CG_REVISION 2c745227b702779fc1999dee8c34f43b977f0871
