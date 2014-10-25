/*
 * window.h
 *
 *  Created on: 8. 10. 2013
 *      Author: mikc
 */

#ifndef WINDOW_H_
#define WINDOW_H_

#include <string>
#include <opennfs/sdl.h>

namespace visualizer {
class Window {
public:
	static Window * createWindow(std::string title, int width, int height, int flags, int depth, int antialiasing);
	int getWidth();
	int getHeight();
	int getDepth();
	int getFlags();
	std::string getTitle();
	SDL_Window *getWindow();
private:
	std::string title;
	int width;
	int height;
	int flags;
	int depth;
	int antialiasing;

	static SDL_Window *window;
	static SDL_GLContext glCtx;

	static void cleanUp();
	Window(std::string title, int width, int height, int flags, int depth, int antialiasing);
	~Window() { }
	Window(Window const&);
	Window& operator=(Window const);
	static Window *instanceHolder;
};

};

#endif /* WINDOW_H_ */
