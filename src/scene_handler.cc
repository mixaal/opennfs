/*
 * scene_handler.cc
 *
 *  Created on: 8. 10. 2013
 *      Author: mikc
 */

#include <opennfs/scene_handler.h>
#include <opennfs/opengl.h>
#include <ctime>

namespace visualizer {
SceneHandler::SceneHandler(Window *window) :
		window(window) {

}

void SceneHandler::addScene(Scene *scene) {
	_scenes.push_back(scene);
}

bool SceneHandler::anySceneForRender() {
	for (int idx=0; idx<_scenes.size(); idx++) {
		Scene *scene = _scenes.at(idx);
		scene->maintain_fps();
		if(scene->shouldRender()) {
			std::cout << "Scene:" << idx << " for render." << std::endl;
			return true;
		}
	}

	return false;
}

void SceneHandler::mainloop() {
	SDL_Event event;


	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable( GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glEnable(GL_ALPHA_TEST);
	glEnable( GL_TEXTURE_2D );
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glAlphaFunc(GL_GREATER, 0.5f);
	glEnable(GL_FOG);
	glDepthFunc( GL_LEQUAL);
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glHint( GL_POINT_SMOOTH, GL_NICEST);
	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	bool skip_scene_init=false;
	if(1==_scenes.size()) {
		Scene *s = _scenes.at(0);
		s->initialize();
		skip_scene_init=true;
	}
	for (; running;) {

		while (SDL_PollEvent(&event)) {
			onEvent(&event);
		}

		

				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();
		for (int idx = 0; idx < _scenes.size(); idx++) {
			glPushMatrix();
			Scene *scene = _scenes.at(idx);


			scene->maintain_fps();
//			if (scene->shouldRender()) {
				//std::cout << "scene:" << idx << std::endl;
			 	//glPushAttrib(GL_VIEWPORT_BIT);
				if(!skip_scene_init) {
					scene->initialize();
				}	
				glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
				//glClear(GL_DEPTH_BUFFER_BIT);

				gluLookAt(scene->get_eyex(), scene->get_eyey(),
						scene->get_eyez(), scene->get_forwardx(),
						scene->get_forwardy(), scene->get_forwardz(),
						scene->get_upx(), scene->get_upy(), scene->get_upz());


				scene->getRenderer()(scene);
				//glPopAttrib();


	//		}
	//		glColor3f(1.0f, 1.0f, 1.0f);
			scene->draw();

			glPopMatrix();

		}
		//glFlush();
		SDL_GL_SwapWindow (window->getWindow());


	}
}

void SceneHandler::terminate() {
	running = false;
}

}
;
