/*
 * window.cc
 *
 *  Created on: 8. 10. 2013
 *      Author: mikc
 */

#include <opennfs/window.h>
#include <opennfs/sdl.h>
#include <stdlib.h>

namespace visualizer {

Window *Window::instanceHolder=0;
SDL_Window *Window::window=0;
SDL_GLContext Window::glCtx=0;

Window* Window::createWindow(std::string title, int width, int height, int flags, int depth, int antialiasing)
{
	if (instanceHolder == 0)
		instanceHolder = new Window(title, width, height, flags, depth, antialiasing);
	return instanceHolder;
}

Window::Window(std::string title, int width, int height, int flags, int depth, int antialiasing) :
		title(title),
		width(width),
		height(height),
		flags(flags),
		depth(depth),
		antialiasing(antialiasing)
{
	SDL_Init (SDL_INIT_EVERYTHING);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	if(antialiasing > 1) {
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, antialiasing);
	}
	//SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, depth);
	window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);
	if (!window) {
		exit (EXIT_FAILURE);
	}
	glCtx = SDL_GL_CreateContext(window);

	atexit(&cleanUp);
}

void Window::cleanUp(){
	SDL_GL_DeleteContext(glCtx);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int Window::getWidth() {
	return width;
}

int Window::getHeight() {
	return height;
}

int Window::getFlags() {
	return flags;
}

int Window::getDepth() {
	return depth;
}

SDL_Window *Window::getWindow() {
	return window;
}
};




