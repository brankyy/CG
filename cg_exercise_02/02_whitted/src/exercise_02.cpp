#include <cglib/rt/renderer.h>
#include <cglib/rt/intersection_tests.h>
#include <cglib/rt/raytracing_context.h>
#include <cglib/rt/intersection.h>
#include <cglib/rt/ray.h>
#include <cglib/rt/scene.h>
#include <cglib/rt/light.h>
#include <cglib/rt/material.h>
#include <cglib/rt/render_data.h>

/*
 * TODO: implement a ray-sphere intersection test here.
 * The sphere is defined by its center and radius.
 *
 * Return true, if (and only if) the ray intersects the sphere.
 * In this case, also fill the parameter t with the distance such that
 *    ray_origin + t * ray_direction
 * is the intersection point.
 */
bool intersect_sphere(
    glm::vec3 const& ray_origin,    // starting point of the ray
    glm::vec3 const& ray_direction, // direction of the ray
    glm::vec3 const& center,        // position of the sphere
    float radius,                   // radius of the sphere
    float* t)                       // output parameter which contains distance to the hit point
{
    cg_assert(t);
	cg_assert(std::fabs(glm::length(ray_direction) - 1.f) < EPSILON);

  const float a = glm::dot(ray_direction, ray_direction);
  const float b = 2 * glm::dot(ray_direction, ray_origin - center);
  const float c = glm::dot((ray_origin - center), (ray_origin - center)) - radius * radius;
  const float delta = b * b - 4 * a * c;

  if(delta == 0.f)
  {
    float temp = ( - b / a) * 0.5;
    if(temp > 0.f)
    {
      *t = temp;
      return true;
    }
    else
      return false;
  }

  float temp3 = 0.5 * ( - b - sqrt(delta)) / a;
  if(temp3 > 0.f)
  {
    *t = temp3;
    return true;
  }

  return false;
}

/*
 * emission characteristic of a spotlight
 */
glm::vec3 SpotLight::getEmission(
		glm::vec3 const& omega // world space direction
		) const
{
	cg_assert(std::fabs(glm::length(omega) - 1.f) < EPSILON);

	// TODO: implement a spotlight emitter as specified on the exercise sheet
  float costheta = glm::cos(dot(direction, omega));
  glm::vec3 Emission = getPower() * (falloff + 2.f) * powf(std::max(0.f, costheta), falloff);
	return Emission;
}

glm::vec3 evaluate_phong(
	RenderData &data,			// class containing raytracing information
	MaterialSample const& mat,	// the material at position
	glm::vec3 const& P,			// world space position
	glm::vec3 const& N,			// normal at the position (already normalized)
	glm::vec3 const& V)			// view vector (already normalized)
{
	cg_assert(std::fabs(glm::length(N) - 1.f) < EPSILON);
	cg_assert(std::fabs(glm::length(V) - 1.f) < EPSILON);

	glm::vec3 contribution(0.f);

	// iterate over lights and sum up their contribution
	for (auto& light_uptr : data.context.get_active_scene()->lights)
	{
		// TODO: calculate the (normalized) direction to the light
		const Light *light = light_uptr.get();
    glm::vec3 l = light -> Light::getPosition();
		glm::vec3 L = glm::normalize(l - P);

		float visibility = 1.f;
		if (data.context.params.shadows) {
			// TODO: check if light source is visible
      visibility = visible(data, P, l);
		}

    float NdotL = glm::dot(N, L);
    float distance2 = glm::length(l - P) * glm::length(l - P);
    float O = NdotL > 0.f ? 1.f : 0.f ;

    glm::vec3 coefficient = O * visibility * light -> Light::getEmission(-L) / distance2;
    glm::vec3 RL = N * NdotL * 2.f - L;
    float RLdotV = glm::dot(RL, V);

		glm::vec3 diffuse(0.f);
		if (data.context.params.diffuse) {
			// TODO: compute diffuse component of phong model
      diffuse = mat.k_d * (NdotL > 0.f ? NdotL : 0.f);
		}

		glm::vec3 specular(0.f);
		if (data.context.params.specular) {
			// TODO: compute specular component of phong model
      specular = mat.k_s * powf(RLdotV > 0.f ? RLdotV : 0.f, mat.n);
		}

		glm::vec3 ambient = data.context.params.ambient ? mat.k_a : glm::vec3(0.0f);

		// TODO: modify this and implement the phong model as specified on the exercise sheet
		contribution += ambient * light -> Light::getPower() / distance2 + (specular + diffuse) * coefficient;

	}

	return contribution;
}

glm::vec3 evaluate_reflection(
	RenderData &data,			// class containing raytracing information
	int depth,					// the current recursion depth
	glm::vec3 const& P,			// world space position
	glm::vec3 const& N,			// normal at the position (already normalized)
	glm::vec3 const& V)			// view vector (already normalized)
{
	// TODO: calculate reflective contribution by constructing and shooting a reflection ray.
  glm::vec3 R = reflect(V, N);
  glm::vec3 startpunkt = P + data.context.params.ray_epsilon * R;
  Ray ray(startpunkt, R);
  return trace_recursive(data, ray, depth + 1);
}

glm::vec3 evaluate_transmission(
	RenderData &data,			// class containing raytracing information
	int depth,					// the current recursion depth
	glm::vec3 const& P,			// world space position
	glm::vec3 const& N,			// normal at the position (already normalized)
	glm::vec3 const& V,			// view vector (already normalized)
	float eta)					// the relative refraction index
{
	// TODO: calculate transmissive contribution by constructing and shooting a transmission ray.
  glm::vec3 T;
  if(refract(V, N, eta, &T))
  {
    glm::vec3 startpunkt = P + data.context.params.ray_epsilon * T;
    Ray ray(startpunkt, T);
    return trace_recursive(data, ray, depth + 1);
  }
  else
    return glm::vec3(0.f);
}

glm::vec3 handle_transmissive_material_single_ior(
	RenderData &data,			// class containing raytracing information
	int depth,					// the current recursion depth
	glm::vec3 const& P,			// world space position
	glm::vec3 const& N,			// normal at the position (already normalized)
	glm::vec3 const& V,			// view vector (already normalized)
	float eta)					// the relative refraction index
{
  float F = fresnel(V, N, eta);
  glm::vec3 T = evaluate_transmission(data, depth, P, N, V, eta);
  glm::vec3 R = evaluate_reflection(data, depth, P, N, V);
	if (data.context.params.fresnel) {
		// TODO: replace with proper fresnel handling.
		return (1.f - F) * T + F * R;
	}
	else {
		// just regular transmission
		return T;
	}
}

glm::vec3 handle_transmissive_material(
	RenderData &data,					// class containing raytracing information
	int depth,							// the current recursion depth
	glm::vec3 const& P,					// world space position
	glm::vec3 const& N,					// normal at the position (already normalized)
	glm::vec3 const& V,					// view vector (already normalized)
	glm::vec3 const& eta_of_channel)	// relative refraction index of red, green and blue color channel
{
	if (data.context.params.dispersion && !(eta_of_channel[0] == eta_of_channel[1] && eta_of_channel[0] == eta_of_channel[2])) {
		// TODO: split ray into 3 rays (one for each color channel) and implement dispersion here
    glm::vec3 x, y, z;
    x = handle_transmissive_material_single_ior(data, depth, P, N, V, eta_of_channel[0]);
    y = handle_transmissive_material_single_ior(data, depth, P, N, V, eta_of_channel[1]);
    z = handle_transmissive_material_single_ior(data, depth, P, N, V, eta_of_channel[2]);

		return glm::vec3(x.x, y.y, z.z);
	}
	else {
		// dont handle transmission, take average refraction index instead.
		const float eta = 1.f/3.f*(eta_of_channel[0]+eta_of_channel[1]+eta_of_channel[2]);
		return handle_transmissive_material_single_ior(data, depth, P, N, V, eta);
	}
	return glm::vec3(0.f);
}
// CG_REVISION 81651e9d3440cf57bc693b5ac8ad23edb0a3bee0
