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
#include <map>
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
    Application(const char* title, const char* settings_file, std::map<std::string, std::string> metadata, int argc, char* argv[]);
    void run();

private:
    int screenWidth;
    int screenHeight;
    const char* windowTitle;
    const char* settings_file;
	std::map<std::string, std::string> app_metadata;
    int argc;
    char** argv;
	Shader* ourShader; // Pointer to the shader object

    SDL_Window* window;
    SDL_GLContext gl_context;

    unsigned int VBO_vertices, VAO, EBO, shaderProgram;
    void initSDL();
    void initOpenGL();
    void initImGui();
    void initBG();
    void cleanup();

    void addTextWithStroke(ImDrawList* draw_list, ImFont* font, float size, ImVec2 pos, ImU32 fill_col, ImU32 outline_col, float thickness, const char* text);

    ImFont* m_font_regular;
    ImFont* m_font_large;
    enum class FractalType { NONE, MANDELBROT, JULIA, BURNING_SHIP, TRICORN, PHOENIX, LYAPUNOV, NEWTON, MULTIBROT, NOVA, SPIDER};
    FractalType m_currentFractal = FractalType::NONE;
    int max_iterations_default = 10000;
    bool m_adaptive_iterations = true;
    int m_base_iterations = 200;
    double m_mandel_zoom = 1.0;
    double m_mandel_center_x = -0.75;
    double m_mandel_center_y = 0.0;
    int m_mandel_max_iterations = 200;
    bool m_is_dragging = false;
    ImVec2 m_drag_start_pos;
    double m_julia_c_x = -0.8;
    double m_julia_c_y = 0.156;
    bool m_julia_interactive_mode = true;
    bool m_julia_c_map_to_mouse = false;
    ImVec4 m_palette_a = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    ImVec4 m_palette_b = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    ImVec4 m_palette_c = ImVec4(1.0f, 1.0f, 0.5f, 1.0f);
    ImVec4 m_palette_d = ImVec4(0.8f, 0.9f, 0.3f, 1.0f);
    bool m_apply_common_color_palette = false;
    ImVec4 m_palette_mandelbrot_a = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    ImVec4 m_palette_mandelbrot_b = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    ImVec4 m_palette_mandelbrot_c = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    ImVec4 m_palette_mandelbrot_d = ImVec4(0.00f, 0.10f, 0.20f, 1.0f);
    ImVec4 m_palette_julia_a = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    ImVec4 m_palette_julia_b = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    ImVec4 m_palette_julia_c = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    ImVec4 m_palette_julia_d = ImVec4(0.80f, 0.90f, 1.00f, 1.0f);
    ImVec4 m_palette_burning_ship_a = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    ImVec4 m_palette_burning_ship_b = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    ImVec4 m_palette_burning_ship_c = ImVec4(1.0f, 1.0f, 0.925f, 1.0f);
    ImVec4 m_palette_burning_ship_d = ImVec4(0.00f, 1.0f, 0.220f, 1.0f);
    ImVec4 m_palette_tricorn_a = ImVec4(0.02f, 0.00f, 0.20f, 1.0f);
    ImVec4 m_palette_tricorn_b = ImVec4(0.20f, 0.80f, 1.00f, 1.0f);
    ImVec4 m_palette_tricorn_c = ImVec4(1.00f, 0.20f, 0.80f, 1.0f);
    ImVec4 m_palette_tricorn_d = ImVec4(1.00f, 0.90f, 0.00f, 1.0f);
    ImVec4 m_palette_phoenix_a = ImVec4(0.1f, 0.5f, 0.2f, 1.0f);
    ImVec4 m_palette_phoenix_b = ImVec4(0.4f, 0.1f, 0.6f, 1.0f);
    ImVec4 m_palette_phoenix_c = ImVec4(0.9f, 0.4f, 0.9f, 1.0f);
    ImVec4 m_palette_phoenix_d = ImVec4(1.0f, 0.8f, 0.5f, 1.0f);
    ImVec4 m_palette_lyapunov_a = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    ImVec4 m_palette_lyapunov_b = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    ImVec4 m_palette_lyapunov_c = ImVec4(1.0f, 0.5f, 0.2f, 1.0f);
    ImVec4 m_palette_lyapunov_d = ImVec4(0.0f, 0.15f, 0.20f, 1.0f);
    ImVec4 m_palette_newton_a = ImVec4(0.1f, 0.1f, 0.2f, 1.0f);
    ImVec4 m_palette_newton_b = ImVec4(0.8f, 0.1f, 0.1f, 1.0f);
    ImVec4 m_palette_newton_c = ImVec4(0.2f, 0.7f, 0.3f, 1.0f);
    ImVec4 m_palette_newton_d = ImVec4(0.9f, 0.9f, 0.2f, 1.0f);
    ImVec4 m_palette_multibrot_a = ImVec4(0.0f, 0.0f, 0.3f, 1.0f);
    ImVec4 m_palette_multibrot_b = ImVec4(0.2f, 0.4f, 0.9f, 1.0f);
    ImVec4 m_palette_multibrot_c = ImVec4(0.9f, 0.3f, 0.6f, 1.0f);
    ImVec4 m_palette_multibrot_d = ImVec4(1.0f, 0.9f, 0.4f, 1.0f);
	ImVec4 m_palette_nova_a = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	ImVec4 m_palette_nova_b = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
	ImVec4 m_palette_nova_c = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
	ImVec4 m_palette_nova_d = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 m_palette_spider_a = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	ImVec4 m_palette_spider_b = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
	ImVec4 m_palette_spider_c = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
	ImVec4 m_palette_spider_d = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    double m_phoenix_c_x = 0.5067;
    double m_phoenix_c_y = 0.0200;
    double m_phoenix_p = -0.9900;
    double m_lyapunov_center_a = 3.4;
    double m_lyapunov_center_b = 3.4;
    double m_lyapunov_zoom = 0.25;
    double m_multibrot_power = 2.0;
    double m_nova_power = 3.0;
    double m_nova_relaxation = 1.0;

    // color density
    float m_color_density = 0.05f;
    
    // menu bg color params
    ImVec4 m_menu_bg_color_one = ImVec4(0.05f, 0.08f, 0.20f, 1.0f);
    ImVec4 m_menu_bg_color_two = ImVec4(0.15f, 0.05f, 0.25f, 1.0f);
    ImVec4 m_menu_bg_accent_color = ImVec4(0.95f, 0.35f, 0.65f, 1.0f);
};

#endif