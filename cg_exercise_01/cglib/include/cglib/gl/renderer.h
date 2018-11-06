#pragma once

#include <cglib/core/assert.h>
#include <cglib/core/camera.h>
#include <cglib/gl/shader.h>

#include <glm/glm.hpp>

// DO NOT PUT ANY OPENGL CALLS INTO CONSTRUCTOR!!!
// all intialization must be done in initialize!

class Parameters;
class GLObjModel;
struct FBO;
struct AABB;

class Renderer
{
public:
	Renderer(const MapNameProgramDefinition &program_definitions)
		: shader_manager(program_definitions)
	{ }
	Renderer() {}
	virtual ~Renderer() {}
	void destroy();
	int  initialize();
	virtual const char * get_name() { return "unknown"; }
	void draw();
	void activate();
	void deactivate();
	void update_objects(double time_step);

	void resize(int width, int height);

	ShaderManager shader_manager;
	std::shared_ptr<Camera> camera;
	glm::mat4 projection;
	float fov = 65.0f;
	float near_clip = 0.1f, far_clip = 100.0f;
	double current_time = 0.0;
	bool enable_animation = true;

	// these are the interfaces to be overridden by the derived classes
protected:
	virtual void _draw()       {}
	virtual void _destroy()    {}
	virtual void _activate()   {}
	virtual void _deactivate() {}
	virtual int  _initialize() { return 0; }
	virtual void _update_objects(double time_step) {}
	virtual void _resize(int width, int height) {}

	bool resize_was_called = false;
};
