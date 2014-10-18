#include <GL/glew.h>

#include <opennfs/scene.h>
#include <opennfs/scene_handler.h>
#include <opennfs/sdl.h>
#include <opennfs/opengl.h>
#include <opennfs/window.h>
#include <opennfs/skydome.h>
#include <opennfs/cloud.h>

#define KIA_RIO_YELLOW 0
#define KIA_RIO_BLUE   1
#define KIA_RIO_RED    2
#define CLOUD_LAYER    3

static float eye_x=0.0f, eye_z=200.f, eye_y = 20.0f;
static float camera_y = 0.0f;

class SceneHandlerImpl: public visualizer::SceneHandler {
public:
	SceneHandlerImpl(visualizer::Window *window) :
			SceneHandler(window) {

	}

	void onKeyDown(SDL_Keysym* keysym) {
                static bool wireframe=false;
		switch (keysym->sym) {
		case SDLK_UP:
			eye_z += 5.5f;
			break;
		case SDLK_DOWN:
			eye_z -=0.5f;
			break;
		case SDLK_RIGHT:
			eye_x -= 0.5f;
			break;
		case SDLK_LEFT:
			eye_x += 0.5f;
			break;
		case SDLK_PAGEUP:
			camera_y += 0.1f;
			break;
		case SDLK_PAGEDOWN:
			camera_y -= 0.1f;
			break;
		case SDLK_a:
			eye_y += 5.5f;
			break;
		case SDLK_z:
			eye_y -= 5.5f;
			break;

		case SDLK_ESCAPE:
			terminate();
			break;
                case SDLK_F10:
			if( wireframe ) {
				wireframe = false;
			}
			else {
				wireframe = true;
			}
                        if(wireframe) {
                            glDisable(GL_LIGHTING);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
  			else {
                glEnable(GL_LIGHTING);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}

		default:
			break;
		}

	}
	void onEvent(SDL_Event* event) {
		switch (event->type) {
		case SDL_KEYDOWN:
			onKeyDown(&event->key.keysym);
			break;
		case SDL_QUIT:
			terminate();
			break;

		default:
			break;

		}
	}

};
static GLint triangle;
static visualizer::SkyDome *sky = NULL;
static int _day=1, _hour=8, _minute=0, _sec=0;

//#define SCREEN_W 1920
//#define SCREEN_H 1080

#ifndef _SCREEN_W
#  warning "Undefined Screen width."
#  define _SCREEN_W 1280
#endif

#ifndef _SCREEN_H
#  warning "Undefined Screen height."
#  define _SCREEN_H 1024
#endif
int main(int argc, char *argv[]) {
	SDL_Event event;
	glewExperimental = GL_TRUE;
	glewInit();

	visualizer::Window *window = visualizer::Window::createWindow("ahojda", _SCREEN_W,
			_SCREEN_H, SDL_WINDOW_OPENGL|SDL_WINDOW_FULLSCREEN, 16);
	visualizer::Scene *scene1 = new visualizer::Scene(0, 0, _SCREEN_W, _SCREEN_H);
	scene1->lighting_enable();
	scene1->fov(60.0f);
	scene1->fpsrate(60);
	//visualizer::Model *kiaRio =new visualizer::Model(KIA_RIO_YELLOW, "data/cars/kiario/kia_rio.obj", 2);
	visualizer::CloudLayer *clouds =new visualizer::CloudLayer(
		"data/clouds/cloud-map-256",
		//"overcast.map",
		//"cumulus_layer.map",
		//"cloud-layer.map",
		100.0f,
		500.0f
	);

	visualizer::CloudLayer *clouds2 =new visualizer::CloudLayer(
		"data/clouds/cloud-layer.map",
		100.0f,
		500.0f
	);

	visualizer::CloudLayer *clouds3 =new visualizer::CloudLayer(
		"data/clouds/cloud-layer.map",
		100.0f,
		500.0f
	);


	
	visualizer::Model *tyrol =new visualizer::Model(CLOUD_LAYER, "data/terrain/alpy2.obj", 1, 700.0f);
	scene1->addModel(tyrol);
	scene1->add_clouds(clouds);
//	scene1->add_clouds(clouds2);
//	scene1->add_clouds(clouds3);
        sky = new visualizer::SkyDome(
		50.0f, 15.0f,   /* latitude, longitude */
		2015, 8, 1,     /* year, month, day */
		8, 0, 0,      /* time */
		1.0f,            /* exposure */
		4.0f,            /* turbidity */
		15.0f,
		4000.0f

	);
	printf("sky=%p\n", sky);
	//sunLight->enable();
	//sunLight->position(sky->solX(), -sky->solZ(), sky->solY());
	scene1->add_atmosphere(sky);

	scene1->onRender([](visualizer::Scene *s) {
		static float x=-700.0f;
		static float y=-2.5f;
		static int hour=11;
		static int minute=0;
		static float dissipation = 0.001f;
		static int fps=0;
		static time_t start = time(NULL);
		fps++;

		time_t now = time(NULL);
		if(now-start>=5) {
			float d = now-start;
			start=now;
			std::cout << "FPS: " << fps/d << std::endl;
			fps = 0;
		}
		dissipation += 0.0001;
		//eye_z+=0.9f;
		//eye_y+=0.3f;
		s->eye(eye_x, eye_y, eye_z);
		s->forward(eye_x, eye_y+camera_y, eye_z+1.0f);
           	//s->eye(-sky->solX(), -sky->solY(), -sky->solZ());
		//s->forward(0.0f, 0.0f, 0.0f);
		sky->update(2015, 8, 1, hour, minute, 0);
		visualizer::CloudLayer *clouds = s->get_cloud_layer().at(0);
//		visualizer::CloudLayer *clouds2 = s->get_cloud_layer().at(1);
//		visualizer::CloudLayer *clouds3 = s->get_cloud_layer().at(2);
		clouds->evolve(hour, minute);
//		clouds2->evolve(hour, minute);
//		clouds3->evolve(hour, minute);


#if 0 // fog, needs scale of 1000.0f
		clouds->position(-1700.0f, 100.0f, -2350.0f);
#else // mid-alt clouds, scale==500.0f
		clouds->position(-680.0f, 500.0f, 0.0f);
#endif
//		clouds2->position(300.0f, 500.0f, 0.0f);
//		clouds3->position(-1680.0f, 500.0f, 0.0f);
		minute++;
		if(minute==60) hour++;
		minute%=60;
		hour%=24;
		if(hour==20) hour=8;
		//sky->draw();
		//sky->increase_turbidity();

	
	});
	SceneHandlerImpl *sh = new SceneHandlerImpl(window);
	sh->addScene(scene1);
	sh->mainloop();

	return 0;
}
