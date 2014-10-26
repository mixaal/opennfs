#ifndef __SCENE_H__
#define __SCENE_H__ 1

#include <memory>
#include <iostream>

#include <opennfs/opengl.h>
#include <opennfs/sdl.h>

#include <ctime>
#include <functional>

#include <opennfs/model.h>
#include <opennfs/light.h>
#include <opennfs/skydome.h>
#include <opennfs/cloud.h>
#include <opennfs/types.h>

namespace visualizer {

typedef float color_t[4];


class Scene {
private:

	GLsizei top, left, w, h;

	float field_of_view;
	float z_near;
	float z_far;
	int fps;
	float _position[3];
	float _forward[3];
	float _up[3];

	color_t clearColor;

	std::function<void(Scene*)> _onRender;

	bool render;
	std::vector<Model *> _world;
	std::vector<CloudLayer *> _clouds;

	struct {
		bool enabled=false;
		float color[3], start, end;
	} _fog;

	struct {
		bool enabled = false;
		SkyDome *sky;
		Light *sunlight;
		float observer_angle;
	} _atmosphere;

    struct {
        bool enabled = false;
        size_t samples = 4;
    } _msaa;


#define MAX_LIGHTS 8
	struct {
		bool enabled=false;

		int lightno[MAX_LIGHTS]= {
				GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3,
				GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7
		};
		float global_ambient[4];
		std::vector<Light *> lights;
	} _lighting;

	void time_delta();
	void draw_fog();
	void draw_atmosphere();
	void draw_lights();
	void draw_world();
	void draw_clouds();
	void angle_between_eye_and_sun();
	void setup_up_fog_based_on_observer_to_sun_angle();

public:
	float dt=0.0f;
	Scene(GLsizei left=0, GLsizei top=0, GLsizei w = 800, GLsizei h = 600,
		  float fov = 60.0f, float z_near = 1.0f, float z_far = 10000.0f, int fps=60);

	void initialize();
	void onRender(std::function<void(Scene*)> _render_cb);
	std::function<void(Scene*)> getRenderer() ;

	std::vector<CloudLayer*> get_cloud_layer();

	// Terminate the scene

	void resize(int new_w, int new_h);

	// Window parameters
	GLsizei getViewportWidth();
	GLsizei getViewportHeight();


	// OpenGL perspective, near and far plane
	float getFoV();
	float getZNear();
	float getZFar();

	// Set field of view
	void fov(float _fov);

	// Get camera position
	float get_eyex();
	float get_eyey();
	float get_eyez();

	// Get camera up view
	float get_upx();
	float get_upy();
	float get_upz();

	// Get camera target view
	float get_forwardx();
	float get_forwardy();
	float get_forwardz();

	// Set camera position, up and target vectors
	void eye(float x, float y, float z);
	void eye(types::XYZ position);
	void up(float x, float y, float z);
	void up(types::XYZ direction);
	void forward(float x, float y, float z);
	void forward(types::XYZ direction);

	void fpsrate(int hz);
	int get_fps();

	void maintain_fps();
	bool shouldRender();

	void backgroundColor(float r, float g, float b, float a);
	void fog(float r, float g, float b, float start, float end);
	void addModel(Model *model);
	Model* getModelById(int id);
	void draw();
	Light *addLight();
	void lighting_enable();
	void lighting_disable();
	void global_ambient(float r, float g, float b, float a=1.0f);
	void add_atmosphere(SkyDome *sky);
	SkyDome *get_atmosphere();
	void enable_multisample();
        void add_clouds(CloudLayer *clouds);
};

};

#endif /* __SCENE_H__ */
