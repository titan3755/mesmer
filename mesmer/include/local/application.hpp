/*
    Mesmer - An interactive and high-performance fractal generator and explorer.
    Made by -> Titan // GH: https://github.com/titan3755/mesmer
    [application.hpp]
*/

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

    unsigned int VBO_vertices, VAO, EBO, shaderProgram;
    void initSDL();
    void initOpenGL();
    void initImGui();
    void initTriangle();
    void cleanup();

    ImFont* m_font_regular;
    ImFont* m_font_large;
    enum class FractalType { NONE, MANDELBROT, JULIA };
    FractalType m_currentFractal = FractalType::NONE;
    double m_mandel_zoom = 1.0;
    double m_mandel_center_x = -0.75;
    double m_mandel_center_y = 0.0;
    int m_mandel_max_iterations = 200;
    bool m_is_dragging = false;
    ImVec2 m_drag_start_pos;
    double m_julia_c_x = -0.8;
    double m_julia_c_y = 0.156;
    bool m_julia_interactive_mode = false;
    bool m_julia_c_map_to_mouse = false;
    ImVec4 m_palette_a = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    ImVec4 m_palette_b = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    ImVec4 m_palette_c = ImVec4(1.0f, 1.0f, 0.5f, 1.0f);
    ImVec4 m_palette_d = ImVec4(0.8f, 0.9f, 0.3f, 1.0f);
};

#endif