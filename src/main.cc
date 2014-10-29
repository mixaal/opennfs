#include <GL/glew.h>

#include <opennfs/scene.h>
#include <opennfs/scene_handler.h>
#include <opennfs/sdl.h>
#include <opennfs/opengl.h>
#include <opennfs/window.h>
#include <opennfs/skydome.h>
#include <opennfs/cloud.h>
#include <opennfs/camera.h>
#include <opennfs/world_time.h>
#include <opennfs/config_util.h>
#include <opennfs/level.h>

#define KIA_RIO_YELLOW 0
#define KIA_RIO_BLUE   1
#define KIA_RIO_RED    2
#define CLOUD_LAYER    3


static game::WorldTime world_time(2014, 8, 1, 14, 0, 0);
static visualizer::Camera camera(0.0f, 20.0f, -700.0f, 90.0f, 0.0f, 1.5f);

class SceneHandlerImpl: public visualizer::SceneHandler {
public:
	SceneHandlerImpl(visualizer::Window *window) :
			SceneHandler(window) {

	}

	void onMousePress(Uint8 button)
	{
		switch(button) {
			case SDL_BUTTON_LEFT:
				world_time.forward();
				break;
			case SDL_BUTTON_MIDDLE:
				world_time.freeze();
				camera.set_velocity(0);
				break;
			case SDL_BUTTON_RIGHT:
				world_time.backward();
				break;
		}
	}

	void onMouseWheel(int horiz, int vert) {
		float v = camera.get_velocity();
		v += vert / 10.0f;
		camera.set_velocity(v);
	}


	void onMouseMotion(int dx, int dy) {
		static bool firstMotion = true;

		if(firstMotion) {
			// screen initialization
			firstMotion = false;
			return;
		}

		float elevation = camera.get_elevation();
		float azimuth = camera.get_azimuth();
		
		elevation -= dy/8.0f;
		azimuth += dx/8.0f;

		elevation = fmax( -90.0f, elevation );
		elevation = fmin( 90.0f, elevation );

		azimuth = fmod( azimuth, 360.0f );
		
		camera.look_at(azimuth, elevation);
	
		//printf("MOUSE MOTION: [%d %d], Camera [%f %f]\n", dx, dy, Camera.azimuth, Camera.elevation);
		
	}

	void onKeyDown(SDL_Keysym* keysym) {
                static bool wireframe=false;
		switch (keysym->sym) {

		case SDLK_HOME:
			camera.look_straight();
			break;

		case SDLK_PAGEDOWN:
			camera.update_altitude(-5.0f);
			break;

		case SDLK_PAGEUP:
			camera.update_altitude(5.0f);
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
		
		case SDL_MOUSEBUTTONDOWN:
			onMousePress(event->button.button);
			break;

		case SDL_MOUSEMOTION:
			onMouseMotion(event->motion.xrel, event->motion.yrel);
			break;

		case SDL_MOUSEWHEEL:
			onMouseWheel(event->wheel.x, event->wheel.y);
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
static int _day=1, _hour=8, _minute=0, _sec=0;

int main(int argc, char *argv[]) {
	SDL_Event event;
	glewExperimental = GL_TRUE;
	glewInit();

	game::configuration::ScreenProperties screen_properties;
	game::configuration::types::screen screen = screen_properties.get();
	int flags = SDL_WINDOW_OPENGL;
	if(screen.fullscreen) {
		flags |= SDL_WINDOW_FULLSCREEN;
	}

	// Configure the main screen
	visualizer::Window *window = visualizer::Window::createWindow(
		screen.title,
		screen.width,
		screen.height,
		flags,
		screen.depth,
		screen.antialiasing
	);

	// Create and configure the scene paramaters
	visualizer::Scene *scene1 = new visualizer::Scene(0, 0, screen.width, screen.height);
	scene1->lighting_enable();
	scene1->fov(screen.field_of_view);
	scene1->fpsrate(screen.target_fps);
	//visualizer::Model *kiaRio =new visualizer::Model(KIA_RIO_YELLOW, "data/cars/kiario/kia_rio.obj", 2);

	game::level::Initializer alps(scene1, "levels/alps", &world_time);

	// Add models
	//visualizer::Model *tyrol =new visualizer::Model(CLOUD_LAYER, "data/terrain/alpy2.obj", 1, 700.0f);
	//scene1->addModel(tyrol);

	visualizer::Model *alpsModel =new visualizer::Model(50, "data/alps1/al1.tri.obj", 1, 0.05f);
	alpsModel->positionInstance(0, 0.0f, 0.0f, -2700.0f);
	scene1->addModel(alpsModel);
	

	// Sky dome
	scene1->onRender([](visualizer::Scene *s) {
		static float x=-700.0f;
		static float y=-2.5f;
		static int hour=11;
		static int minute=0;
		static float dissipation = 0.001f;
		static int fps=0;
		static time_t start = time(NULL);
		fps++;

		SDL_SetRelativeMouseMode(SDL_TRUE);

		time_t now = time(NULL);
		if(now-start>=5) {
			float d = now-start;
			start=now;
			std::cout << "FPS: " << fps/d << std::endl;
			fps = 0;
		}
		dissipation += 0.0001;

		camera.update();
		world_time.update();
		s->eye(camera.get_position());
		s->forward(camera.get_forward());
		visualizer::SkyDome *sky = s->get_atmosphere();
		sky->update(world_time);
		visualizer::CloudLayer *clouds = s->get_cloud_layer().at(0);
//		visualizer::CloudLayer *clouds2 = s->get_cloud_layer().at(1);
//		visualizer::CloudLayer *clouds3 = s->get_cloud_layer().at(2);
		clouds->evolve(world_time.get_hour(), world_time.get_minute());
//		clouds2->evolve(hour, minute);
//		clouds3->evolve(hour, minute);


#if 0 // fog, needs scale of 1000.0f
		clouds->position(-1700.0f, 100.0f, -2350.0f);
#else // mid-alt clouds, scale==500.0f
	//	clouds->position(-680.0f, 500.0f, 0.0f);
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
