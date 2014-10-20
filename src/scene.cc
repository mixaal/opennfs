#include <opennfs/scene.h>
#include <opennfs/model.h>
#include <opennfs/skydome.h>
#include <assert.h>
#include <algorithm>
#include <opennfs/billboard.h>

//FIXME: not needed for windows
#include <sys/time.h>

namespace visualizer {

	void Scene::time_delta() {
#ifdef WINDOWS
		static clock_t previous_time = std::clock();
		now = std::clock();
		dt = now - previous_time;
#else
		static bool first=true;
		static timeval previous_time;

		if(first) {
			first = false;
			gettimeofday(&previous_time, NULL);
		}
		timeval now, delta;
		gettimeofday(&now, NULL);
		timersub(&now, &previous_time, &delta);
		dt = delta.tv_sec * 1e6 + delta.tv_usec;
		previous_time = now;
#endif

	}

	Scene::Scene(GLsizei left, GLsizei top, GLsizei w, GLsizei h,
			float fov, float z_near, float z_far, int fps) :
			left(left), top(top), w(w), h(h),
			field_of_view(fov), z_near(z_near), z_far(z_far), fps(fps) {
		_position[0] = _position[1] = _position[2] = 0.0f;
		_forward[0] = 0.0f;
		_forward[1] = 0.0f;
		_forward[2] = -1.0f;
		_up[0] = 0.0f;
		_up[1] = 1.0f;
		_up[2] = 0.0f;

		_fog.enabled=false;
	}

	bool Scene::shouldRender(){
		return render;
	}

	void Scene::backgroundColor(float r, float g, float b, float a) {
		clearColor[0]=r;
		clearColor[1]=g;
		clearColor[2]=b;
		clearColor[3]=a;
	}

	void Scene::maintain_fps() {
			time_delta();
			//printf("dt=%f\n", dt);
#if 0
			if (dt < 1.0 / get_fps()) {
				render=false;
			}
			else{
				render=true;
			    previous_time = std::clock();
			}
#endif
		}

	void Scene::onRender(std::function<void(Scene*)> _render_cb) {
		_onRender = _render_cb;
	}

	std::function<void(Scene*)> Scene::getRenderer() {
		return _onRender;
	}

	void Scene::resize(int new_w, int new_h) {
		w = new_w;
		h = new_h;
	}

	GLsizei Scene::getViewportWidth() {
		return w;
	}
	GLsizei Scene::getViewportHeight() {
		return h;
	}

	float Scene::getFoV() {
		return field_of_view;
	}
	float Scene::getZNear() {
		return z_near;
	}
	float Scene::getZFar() {
		return z_far;
	}
	void Scene::fov(float _fov) {
		field_of_view = _fov;
	}

	float Scene::get_eyex() {
		return _position[0];
	}
	float Scene::get_eyey() {
		return _position[1];
	}
	float Scene::get_eyez() {
		return _position[2];
	}

	float Scene::get_upx() {
		return _up[0];
	}
	float Scene::get_upy() {
		return _up[1];
	}
	float Scene::get_upz() {
		return _up[2];
	}

	float Scene::get_forwardx() {
		return _forward[0];
	}
	float Scene::get_forwardy() {
		return _forward[1];
	}
	float Scene::get_forwardz() {
		return _forward[2];
	}

	void Scene::eye(float x, float y, float z) {
		_position[0] = x;
		_position[1] = y;
		_position[2] = z;
	}
	
	void Scene::eye(types::XYZ position) {
		_position[0] = position.x;
		_position[1] = position.y;
		_position[2] = position.z;
	}

	void Scene::up(float x, float y, float z) {
		_up[0] = x;
		_up[1] = y;
		_up[2] = z;
	}

	void Scene::up(types::XYZ direction) {
		_up[0] = direction.x;
		_up[1] = direction.y;
		_up[2] = direction.z;
	}

	void Scene::forward(float x, float y, float z) {
		_forward[0] = x;
		_forward[1] = y;
		_forward[2] = z;
	}

	void Scene::forward(types::XYZ direction) {
		_forward[0] = direction.x;
		_forward[1] = direction.y;
		_forward[2] = direction.z;
	}

	void Scene::fpsrate(int hz) {
		fps=hz;
	}

	void Scene::draw_fog() {
 		if(_fog.enabled) {
 				glEnable(GL_FOG);
 				glFogfv(GL_FOG_COLOR, _fog.color);
 				glFogi(GL_FOG_START, _fog.start);
 				glFogi(GL_FOG_END, _fog.end);
 				glFogi(GL_FOG_MODE, GL_LINEAR);
 			}
 		else {
 			glDisable(GL_FOG);
 		}

	}

    void Scene::enable_multisample() {
        _msaa.enabled = true;
    }

	void Scene::initialize() {
        if(_msaa.enabled) {
            glEnable(GL_MULTISAMPLE);
        }
		glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
		glViewport(left, top, w, h);
		glScissor(left, top, w, h);
		//glClear(GL_COLOR_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
 		if(_lighting.enabled) {
 			glEnable(GL_LIGHTING);
 		}
 		else {
 			glDisable(GL_LIGHTING);
 		}
 		for(int idx=0; idx<_lighting.lights.size(); idx++) {
 			_lighting.lights.at(idx) -> draw();
 		}
 		//glEnable(GL_COLOR_MATERIAL);
		glBindTexture(GL_TEXTURE_2D, 0);
 				

		float aspect = (float) w / h;
		gluPerspective(field_of_view, aspect, z_near, z_far);

		glMatrixMode(GL_MODELVIEW);

	}


 	int Scene::get_fps() {
 		return fps;
 	}

 	void Scene::fog(float r, float g, float b, float start, float end) {
 		_fog.enabled=true;
 		_fog.color[0]=r;
 		_fog.color[1]=g;
 		_fog.color[2]=b;
 		_fog.start=start;
 		_fog.end=end;
 	}

 	Model* Scene::getModelById(int id) {
 		for (std::vector<Model*>::iterator it=_world.begin(); it!=_world.end(); ++it) {
 			Model *m = *it;
 			if (m->getId()==id) {
 				return m;
 			}
 		}
 		return NULL;
 	}

 	void Scene::addModel(Model *model) {
 		_world.push_back(model);
 	}

	void Scene::add_clouds(CloudLayer *cloud_layer) {
		_clouds.push_back(cloud_layer);
	}

	void Scene::angle_between_eye_and_sun() {
		if(!_atmosphere.enabled) 
			return;
		float ox = _position[0] - _forward[0];
		float oy = _position[1] - _forward[1];
		float oz = _position[2] - _forward[2];

		float sx = _atmosphere.sky->solX();
		float sy = _atmosphere.sky->solY();
		float sz = _atmosphere.sky->solZ();

		float o = sqrt(ox*ox + oy*oy + oz*oz);
		if (o==0.0f) return;
		float s = sqrt(sx*sx + sy*sy + sz*sz);

		ox /= o;
		oy /= o;
		oz /= o;

		sx /= s;
		sy /= s;
		sz /= s;

		_atmosphere.observer_angle = ox * sx + oy * sy + oz * sz;
	}

	void Scene::setup_up_fog_based_on_observer_to_sun_angle() {
		float r = fabs(_atmosphere.observer_angle - 1.0f) / 2.0f;
		this->fog(
				_atmosphere.sky->hazeRed(),
				_atmosphere.sky->hazeGreen(),
				_atmosphere.sky->hazeBlue(),
				700.0f - 200.0f * r, 
				800.0f
			);
	//	_fog.enabled = (_atmosphere.observer_angle < 0.0f );

	}

	void Scene::draw_atmosphere() {
		if(_atmosphere.enabled) {
			_atmosphere.sunlight->position(
				_atmosphere.sky->solX(),
				_atmosphere.sky->solY(),
				_atmosphere.sky->solZ()
			);
			angle_between_eye_and_sun();
			//printf("Sky::draw: [angle=%f %f %f %f]\n", _atmosphere.observer_angle, _atmosphere.sky->solX(), _atmosphere.sky->solY(), _atmosphere.sky->solZ());
			_atmosphere.sunlight -> draw();

    			glDisable(GL_LIGHTING);
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_FOG);
			_atmosphere.sky->draw();
			setup_up_fog_based_on_observer_to_sun_angle();
			if(_fog.enabled) {
				glEnable(GL_FOG);
			}
			if(_lighting.enabled) {
				glEnable(GL_LIGHTING);
			}
			glEnable(GL_TEXTURE_2D);
			_atmosphere.sunlight->enable();
		}

	}
	
	void Scene::draw_world() {
 		for(int idx=0; idx<_world.size(); idx++) {
 			_world.at(idx)->draw();
 		}
	}
	std::vector<CloudLayer*> Scene::get_cloud_layer() {
		return _clouds;
	}

	void Scene::draw_clouds() {

  		glDisable(GL_LIGHTING);
		glDisable(GL_FOG);
		glDisable(GL_ALPHA_TEST);
		glDepthMask(false);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);


		//glEnable(GL_POINT_SPRITE);
		//glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);

		// glAlphaFunc ( GL_GREATER, 0.0001 ) ;
	    	// glEnable ( GL_ALPHA_TEST ) ;

		//glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		//glBlendFunc(GL_ONE, GL_ONE); 
		//glBlendFunc(GL_ONE, GL_SRC_ALPHA);

		if(_atmosphere.enabled){
			// we have the sun position
			float light_direction[] = {
				_atmosphere.sky->solX(),
				_atmosphere.sky->solY(),
				_atmosphere.sky->solZ()
			};

			float sun_color[] = {
				_atmosphere.sky->hazeRed(),
				_atmosphere.sky->hazeGreen(),
				_atmosphere.sky->hazeBlue(),
			};

			float shadow_color[] = {
				sun_color[0]*0.5f,
				sun_color[1]*0.5f,
				sun_color[2]*0.5f
			};
		
			
			for(int idx=0; idx<_clouds.size(); idx++) {
				CloudLayer *clouds = _clouds.at(idx);
				clouds->set_observer_position(
					_position[0],
					_position[1],
					_position[2]
				);
				clouds->set_sun_position(
					_atmosphere.sky->solX(), 
					_atmosphere.sky->solY(),
					_atmosphere.sky->solZ()
				);
				clouds->set_sun_position(
					_atmosphere.sky->sol_azimuth(),
					_atmosphere.sky->sol_elevation()
				);
				clouds->set_sun_color(sun_color);
				clouds->draw(_position);
			}
		}
		if(_lighting.enabled) {
			glEnable(GL_LIGHTING);
		}
		if(_fog.enabled) {
			glEnable(GL_FOG);
		}
		glDisable(GL_BLEND);
		glEnable(GL_ALPHA_TEST);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(true);

	
		//glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_FALSE);
		//glDisable(GL_POINT_SPRITE);
	}

 	void Scene::draw() {
		draw_atmosphere();
		draw_world();
		draw_clouds();
		draw_fog();
 	}


 	Light *Scene::addLight()
 	{
 		int curr_light_no=_lighting.lights.size();

 		assert(curr_light_no < MAX_LIGHTS);
 		int gl_lightno = _lighting.lightno[curr_light_no];

 		Light *light = new Light(gl_lightno);
 		_lighting.lights.push_back(light);
 		return light;
 	}

 	void Scene::lighting_enable()
 	{
 		_lighting.enabled=true;
 	}


 	void Scene::lighting_disable()
 	{
 		_lighting.enabled=false;
 	}

 	void Scene::global_ambient(float r, float g, float b, float a)
 	{
 		_lighting.global_ambient[0]=r;
 		_lighting.global_ambient[1]=g;
 		_lighting.global_ambient[2]=b;
 		_lighting.global_ambient[3]=a;
 	}

	void Scene::add_atmosphere(SkyDome *sky) {
		if (_atmosphere.enabled) {
			return;
		}
		_atmosphere.enabled = true;
		_atmosphere.sky = sky;
		_atmosphere.sunlight = this->addLight();
		_atmosphere.sunlight->ambient(1.0f, 1.0f, 1.0f);
		_atmosphere.sunlight->diffuse(1.0f, 1.0f, 1.0f);
		_atmosphere.sunlight->specular(0.0f, 0.0f, 0.0f);
		_atmosphere.sunlight->enable();
	}

};
