#pragma once
#ifndef MESMER_APPLICATION_HPP
#define MESMER_APPLICATION_HPP

#include <shader.hpp>
#include <settings.hpp>

#include <iostream>
#include <stdexcept>
#include <string>
#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_ttf.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>

class Application {
public:
    Application(const char* title, const char* settings_file, int argc, char* argv[]);
    void run();

private:
    int screenWidth;
    int screenHeight;
    const char* windowTitle;
    int argc;
    char** argv;
	Shader* ourShader; // Pointer to the shader object

    SDL_Window* window;
    SDL_GLContext gl_context;

    unsigned int VBO_vertices, VBO_colors, VAO, shaderProgram;
    void initSDL();
    void initOpenGL();
    void initImGui();
    void initTriangle();
    void cleanup();
};

#endif