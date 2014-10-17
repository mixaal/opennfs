/*
 * scene_handler.h
 *
 *  Created on: 8. 10. 2013
 *      Author: mikc
 */

#ifndef SCENE_HANDLER_H_
#define SCENE_HANDLER_H_

#include <opennfs/scene.h>
#include <opennfs/window.h>
#include <vector>

namespace visualizer {


class SceneHandler {
private:
	std::vector<Scene *> _scenes;
	bool running=true;
	Window *window;
	bool anySceneForRender();
public:
	float dt=0.0f;
	SceneHandler(Window *window);
	void addScene(Scene *scene);


	virtual void onEvent(SDL_Event *evt)=0;

	void terminate();
	void mainloop();
};


};



#endif /* SCENE_HANDLER_H_ */
