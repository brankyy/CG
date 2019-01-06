#include <cglib/rt/scene.h>

#include <cglib/rt/epsilon.h>
#include <cglib/rt/light.h>
#include <cglib/rt/object.h>
#include <cglib/rt/raytracing_parameters.h>
#include <cglib/rt/texture.h>

#include <cglib/rt/transform.h>

#include <cglib/rt/bvh.h>
#include <cglib/rt/triangle_soup.h>

#include <cglib/core/camera.h>
#include <cglib/core/image.h>

#include <sstream>
#include <random>

Scene::~Scene()
{
}

void Scene::
set_active_camera()
{
	if(camera)
		camera->set_active();
}

GaussScene::GaussScene(RaytracingParameters& params)
{
    init_scene(params);
}

void GaussScene::init_scene(RaytracingParameters const& params)
{
  textures.clear();

  std::shared_ptr<Image> lion = std::make_shared<Image>();
  lion->load("assets/lion.png", 2.2f);
  std::shared_ptr<Image> filtered = std::make_shared<Image>(
      lion->getWidth(), lion->getHeight());

  textures.insert({"input", std::make_shared<ImageTexture>(
      *lion.get(), NEAREST, ZERO)}); 
  textures.insert({"filtered_naive", std::make_shared<ImageTexture>(
      *filtered.get(), NEAREST, ZERO)}); 
  textures.insert({"filtered_separable", std::make_shared<ImageTexture>(
      *filtered.get(), NEAREST, ZERO)}); 
}

void GaussScene::refresh_scene(RaytracingParameters const& params)
{
	auto &img_src = textures["input"]->get_mip_levels()[0];
	auto &img_tgt_naive = textures["filtered_naive"]->get_mip_levels()[0];
	auto &img_tgt_separable = textures["filtered_separable"]->get_mip_levels()[0];

	img_src->filter_gaussian(img_tgt_naive.get(), params.sigma, params.kernel_radius * 2 + 1);
	img_src->filter_gaussian_separable(img_tgt_separable.get(), params.sigma, params.kernel_radius * 2 + 1);
}

FourierScene::FourierScene(RaytracingParameters& params)
{
    init_scene(params);
}

void FourierScene::init_scene(RaytracingParameters const& params)
{
}

void FourierScene::refresh_scene(RaytracingParameters const& params)
{
	if(textures.size() > 0)
		return;

	std::cout << "Computing reconstructed image (this might take a while)... " << std::flush;

	std::shared_ptr<Image> img_spectrum = std::make_shared<Image>();
	img_spectrum->load_pfm("assets/mystery.pfm");
	textures.insert({"img_spectrum", std::make_shared<ImageTexture>(
				*img_spectrum.get(), NEAREST, ZERO)});

	glm::vec4 *spec_data = img_spectrum->getPixels();

	// calculate amplitude
	std::shared_ptr<Image> amplitude = std::make_shared<Image>(
			img_spectrum->getWidth(), img_spectrum->getHeight());
	glm::vec4 *ampl_data = amplitude->getPixels();
	for(int i = 0; i < img_spectrum->getWidth() * img_spectrum->getHeight(); i++)
	{
		float re = spec_data[i].r;
		float im = spec_data[i].g;
		float a  = sqrtf(re*re+im*im);

		ampl_data[i] = glm::vec4(a, a, a, 1.0f);
	}

	textures.insert({"amplitude", std::make_shared<ImageTexture>(
				*amplitude.get(), NEAREST, ZERO)});


	// calculate phase
	std::shared_ptr<Image> phase = std::make_shared<Image>(
			img_spectrum->getWidth(), img_spectrum->getHeight());
	glm::vec4 *phase_data = phase->getPixels();
	for(int i = 0; i < img_spectrum->getWidth() * img_spectrum->getHeight(); i++)
	{
		float re = spec_data[i].r;
		float im = spec_data[i].g;
		float ph = (std::atan2(im, re)+M_PI)/(2.0f*M_PI);

		phase_data[i] = glm::vec4(ph, ph, ph, 1.0f);
	}

	textures.insert({"phase", std::make_shared<ImageTexture>(
				*phase.get(), NEAREST, ZERO)});

	int sx = img_spectrum->getWidth(), sy = img_spectrum->getHeight();
	std::vector<std::complex<float>> spectrum (sx*sy, std::complex<float>(0.0f, 0.0f));
	Image::image_to_complex(*img_spectrum, &spectrum, false);

	// reconstruct the loaded spectrum and store it as png
	std::vector<std::complex<float>> reconstruction(sx*sy, std::complex<float>(0.0f, 0.0f));
	DiscreteFourier2D::reconstruct(sx, sy, spectrum.data(), reconstruction.data());
	Image img_rec(sx, sy);
	Image::complex_to_image(reconstruction, &img_rec, true);

	textures.insert({"reconstructed", std::make_shared<ImageTexture>(img_rec, NEAREST, ZERO)});

	std::cout << "done" << std::endl;
}

TriangleScene::TriangleScene(RaytracingParameters& params)
{
    init_camera(params);
    init_scene(params);
}

std::shared_ptr<TriangleSoup> createTriangleSoup(int num_triangles)
{
	std::vector<glm::vec3> positions;
	for (int i = 0; i < num_triangles; ++i)
	{
		positions.push_back(glm::vec3(
			(i+1)*0.1*std::sin(float(2*i)/num_triangles*2*M_PI)-0.5, 
			(i+1)*0.1*std::sin(float(4*i)/num_triangles*2*M_PI)-0.5, 
			1.0-i*0.2));
		positions.push_back(glm::vec3(
			(i+1)*0.1*std::sin(float(2*i)/num_triangles*2*M_PI)+0.5,
			(i+1)*0.1*std::sin(float(4*i)/num_triangles*2*M_PI)-0.5,
			1.0-i*0.2));
		positions.push_back(glm::vec3(
			(i+1)*0.1*std::sin(float(2*i)/num_triangles*2*M_PI),
			(i+1)*0.1*std::sin(float(4*i)/num_triangles*2*M_PI)+0.5,
			1.0-i*0.2));
	}
	std::vector<glm::vec2> tex_coords;
	for (int i = 0; i < num_triangles; ++i) {
		tex_coords.push_back(glm::vec2(0, 0)); 
		tex_coords.push_back(glm::vec2(1, 1));
		tex_coords.push_back(glm::vec2(1, 0));
	}
	std::vector<glm::vec3> normals(num_triangles*3, glm::vec3(0, 0, 1));

	std::vector<Material> materials(7);
	materials[0].k_d = std::make_shared<ConstTexture>(glm::vec3(0.9f, 0.9f, 0.9f));
	materials[1].k_d = std::make_shared<ConstTexture>(glm::vec3(0.1f, 0.1f, 0.9f));
	materials[2].k_d = std::make_shared<ConstTexture>(glm::vec3(0.1f, 0.9f, 0.1f));
	materials[3].k_d = std::make_shared<ConstTexture>(glm::vec3(0.1f, 0.9f, 0.9f));
	materials[4].k_d = std::make_shared<ConstTexture>(glm::vec3(0.9f, 0.1f, 0.1f));
	materials[5].k_d = std::make_shared<ConstTexture>(glm::vec3(0.9f, 0.1f, 0.9f));
	materials[6].k_d = std::make_shared<ConstTexture>(glm::vec3(0.9f, 0.9f, 0.1f));
	std::vector<int> material_ids;
	for (int i = 0; i < num_triangles; ++i) {
		material_ids.push_back(i%(materials.size()-1));
	}

	return std::make_shared<TriangleSoup>(
		std::move(positions),
		std::move(normals),
		std::move(tex_coords),
		std::move(material_ids),
		std::move(materials));
}

void TriangleScene::init_scene(RaytracingParameters const& params)
{
    objects.clear();
    lights.clear();
    textures.clear();
    soups.clear();

	soups.emplace_back(createTriangleSoup(params.num_triangles));
    objects.emplace_back(new BVH(*soups.back()));
    lights.emplace_back(new Light(glm::vec3(0.f, 200.f, 400.f), glm::vec3(15000.f)));
}

void TriangleScene::refresh_scene(RaytracingParameters const& params)
{
    soups.clear();
    objects.clear();
    
	soups.emplace_back(createTriangleSoup(params.num_triangles));
	objects.emplace_back(new BVH(*soups.back()));
}

void TriangleScene::init_camera(RaytracingParameters& params)
{
    camera = std::make_shared<LookAroundCamera>(
        glm::vec3(0.f, 0.f, 3.f),
        glm::vec3(0.f, 0.f, 0.5f),
        params.eye_separation,
		params.focal_distance);
}

MonkeyScene::MonkeyScene(RaytracingParameters& params)
{
    init_camera(params);
    init_scene(params);
}

void MonkeyScene::init_scene(RaytracingParameters const& params)
{
    objects.clear();
    lights.clear();
    textures.clear();
    soups.clear();


    textures.insert({"floor", std::make_shared<ImageTexture>(
		"assets/checker.tga", params.get_tex_filter_mode(), 
		params.get_tex_wrap_mode(), 2.2f)});
	textures["floor"]->create_mipmap();

	std::shared_ptr<Image> appartment = std::make_shared<Image>();
	appartment->load("assets/appartment.jpg", 1.f);
    textures.insert({"appartment_env",          
		std::make_shared<ImageTexture>(*appartment,
		BILINEAR, REPEAT)});
	textures["appartment_env"]->create_mipmap();
	env_map = textures["appartment_env"].get();
	
    soups.push_back(std::make_shared<TriangleSoup>(
		"assets/suzanne.obj", &this->textures));
    objects.emplace_back(new BVH(*soups.back()));
	objects.back()->set_transform_object_to_world(
		glm::translate(glm::mat4(1.0), glm::vec3(0.f, 2.f, 0.f)) * 
		glm::scale(glm::mat4(1.0), glm::vec3(3.f, 3.f, 3.f)));
    objects.emplace_back((create_plane(
        glm::vec3(0.f, -2.f, 0.f),
        glm::vec3(0.f, 1.f, 0.f),
        glm::vec3(1.f, 0.f, 0.f),
        glm::vec3(0.f, 0.f, -1.f),
        glm::vec2(4.f))));
    objects.back()->material->k_d = textures["floor"];


    lights.emplace_back(new Light(
		glm::vec3(0.f, 6.f, 12.f), glm::vec3(3.f)));
    lights.emplace_back(new Light(
		glm::vec3(0.f, 12.f, 6.f), glm::vec3(3.f)));

	env_map = textures["appartment_env"].get();
}

void MonkeyScene::refresh_scene(RaytracingParameters const& params)
{
}

void MonkeyScene::init_camera(RaytracingParameters& params)
{
    camera = std::make_shared<LookAroundCamera>(
        glm::vec3(0.f, 2.f, 10.f),
        glm::vec3(0.f, 2.f, -1.f),
        params.eye_separation,
		params.focal_distance);
}

SponzaScene::SponzaScene(RaytracingParameters& params)
{
	init_camera(params);
}

void SponzaScene::init_scene(RaytracingParameters const& params)
{
	objects.clear();
	lights.clear();
	textures.clear();
	soups.clear();

	for (int i = 0; i < 4; ++i) {
		objects.emplace_back(create_sphere(glm::vec3(-3.f*i, 1.f, -0.3f), 0.5f));
		objects.back()->material->k_d = std::shared_ptr<ConstTexture>(new ConstTexture(glm::vec3(0.0f)));
		objects.back()->material->k_r = std::shared_ptr<ConstTexture>(new ConstTexture(glm::vec3(0.4f)));
	}

	auto objTriangles = std::make_shared<TriangleSoup>("assets/crytek-sponza/sponza_subdiv3.obj", &this->textures);
	soups.push_back(objTriangles);
	objects.emplace_back(new BVH(*objTriangles));
	objects.back()->set_transform_object_to_world(
		glm::scale(glm::mat4(1.0), glm::vec3(0.01f)));
	
	for (int i = 0; i < 2; ++i) {
		lights.emplace_back(new Light(glm::vec3(-2.5f+5.f*i, 6.f, 0.f), glm::vec3(10.f)));
	}
}

void SponzaScene::refresh_scene(RaytracingParameters const& params)
{
	if(!scene_loaded) {
		init_scene(params);
		scene_loaded = true;
	}
	for (auto &tex : textures) {
		tex.second->filter_mode = params.get_tex_filter_mode();
		tex.second->wrap_mode = params.get_tex_wrap_mode();
	}
}

void SponzaScene::init_camera(RaytracingParameters& params)
{
	camera = std::make_shared<FreeFlightCamera>(
		glm::vec3(4.f, 5.f, -0.3f),
		glm::normalize(glm::vec3(-1.f, -0.5f, 0.f)),
		params.eye_separation,
		params.focal_distance);
}

