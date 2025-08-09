#pragma once
#ifndef MESMER_APPLICATION_HPP
#define MESMER_APPLICATION_HPP

#include <iostream>
#include <stdexcept>
#include <glad/glad.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

class Application {
public:
	Application(int width = 1920, int height = 1080, const char* title = "program", int argc = 0, char* argv[] = 0);
	~Application() = default;
	void run();
private:
	int screenWidth;
	int screenHeight;
	const char* windowTitle;
	int argc;
	char** argv;
};

#endif