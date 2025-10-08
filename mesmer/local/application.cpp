/*
	Mesmer - An interactive and high-performance fractal generator and explorer.
	Made by -> Titan // GH: https://github.com/titan3755/mesmer
	[application.cpp]
*/

#include <application.hpp>

Application::Application(const char* title, const char* settings_file, std::map<std::string, std::string> metadata, int argc, char* argv[]) {
	this->screenWidth = 0;
	this->screenHeight = 0;
	this->windowTitle = title;
	this->settings_file = settings_file;
	this->app_metadata = metadata;
	this->argc = argc;
	this->argv = argv;
	this->window = nullptr;
	this->gl_context = nullptr;
	this->VBO_vertices = 0;
	this->VAO = 0;
	this->EBO = 0;
	this->ourShader = nullptr;
	this->shaderProgram = 0;
	this->m_font_regular = nullptr;
	this->m_font_large = nullptr;
	if (argc > 1) {
		spdlog::info("Command line arguments detected.");
	}
}


void Application::run() {
	try {
		std::cout << "---------------------------------------------" << std::endl;
		std::cout << "| Mesmer - A fractal generator and explorer |" << std::endl;
		std::cout << "---------------------------------------------" << std::endl;
		std::cout << "\n";
		spdlog::info("Starting application...");
		static bool show_fractal_selection = false;
		static bool show_main_buttons = true;
		initSDL();
		initOpenGL();
		initImGui();
		initBG();
		std::cout << "\n\033[30;42m Mesmer application core has been initialized \033[0m\n" << std::endl;

		// settings init
		Settings app_settings(settings_file);

		// -->
		bool done = false;
		static ImVec4 clear_color = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);
		static bool show_demo_window = false;
		const char* status = "Status: All Systems OK";
		const char* sub = "Mesmer - Main Menu";
		static bool title_text_toggle = true;
		static bool hud_toggle = true;
		static bool info_gui_window_toggle = false;
		int final_adaptive_iterations = m_base_iterations;

		// settings load
		// bg menu settings load attempt
		if (app_settings.getSetting("menu_bg_color_one") != "") {
			std::string color_str = app_settings.getSetting("menu_bg_color_one");
			float r, g, b, a;
			if (sscanf_s(color_str.c_str(), "%f,%f,%f,%f", &r, &g, &b, &a) == 4) {
				m_menu_bg_color_one = ImVec4(r, g, b, a);
				spdlog::info("Loaded menu_bg_color_one from settings: {}", color_str);
			}
		}

		if (app_settings.getSetting("menu_bg_color_two") != "") {
			std::string color_str = app_settings.getSetting("menu_bg_color_two");
			float r, g, b, a;
			if (sscanf_s(color_str.c_str(), "%f,%f,%f,%f", &r, &g, &b, &a) == 4) {
				m_menu_bg_color_two = ImVec4(r, g, b, a);
				spdlog::info("Loaded menu_bg_color_two from settings: {}", color_str);
			}
		}

		if (app_settings.getSetting("menu_bg_accent_color") != "") {
			std::string color_str = app_settings.getSetting("menu_bg_accent_color");
			float r, g, b, a;
			if (sscanf_s(color_str.c_str(), "%f,%f,%f,%f", &r, &g, &b, &a) == 4) {
				m_menu_bg_accent_color = ImVec4(r, g, b, a);
				spdlog::info("Loaded menu_bg_accent_color from settings: {}", color_str);
			}
		}

		// fractal common color settings load attempt
		if (app_settings.getSetting("fractal_apply_common_palette") != "") {
			std::string bool_str = app_settings.getSetting("fractal_apply_common_palette");
			if (bool_str == "true") {
				m_apply_common_color_palette = true;
			}
			else {
				m_apply_common_color_palette = false;
			}
			spdlog::info("Loaded fractal_apply_common_palette from settings: {}", bool_str);
		}

		if (app_settings.getSetting("fractal_palette_a") != "") {
			std::string color_str = app_settings.getSetting("fractal_palette_a");
			float r, g, b, a;
			if (sscanf_s(color_str.c_str(), "%f,%f,%f,%f", &r, &g, &b, &a) == 4) {
				m_palette_a = ImVec4(r, g, b, a);
				spdlog::info("Loaded fractal_palette_a from settings: {}", color_str);
			}
		}

		if (app_settings.getSetting("fractal_palette_b") != "") {
			std::string color_str = app_settings.getSetting("fractal_palette_b");
			float r, g, b, a;
			if (sscanf_s(color_str.c_str(), "%f,%f,%f,%f", &r, &g, &b, &a) == 4) {
				m_palette_b = ImVec4(r, g, b, a);
				spdlog::info("Loaded fractal_palette_b from settings: {}", color_str);
			}
		}

		if (app_settings.getSetting("fractal_palette_c") != "") {
			std::string color_str = app_settings.getSetting("fractal_palette_c");
			float r, g, b, a;
			if (sscanf_s(color_str.c_str(), "%f,%f,%f,%f", &r, &g, &b, &a) == 4) {
				m_palette_c = ImVec4(r, g, b, a);
				spdlog::info("Loaded fractal_palette_c from settings: {}", color_str);
			}
		}

		if (app_settings.getSetting("fractal_palette_d") != "") {
			std::string color_str = app_settings.getSetting("fractal_palette_d");
			float r, g, b, a;
			if (sscanf_s(color_str.c_str(), "%f,%f,%f,%f", &r, &g, &b, &a) == 4) {
				m_palette_d = ImVec4(r, g, b, a);
				spdlog::info("Loaded fractal_palette_d from settings: {}", color_str);
			}
		}

		if (app_settings.getSetting("fractal_color_density") != "") {
			std::string float_str = app_settings.getSetting("fractal_color_density");
			try {
				float value = std::stof(float_str);
				m_color_density = value;
				spdlog::info("Loaded color_density from settings: {}", float_str);
			}
			catch (const std::exception& e) {
				spdlog::error("Error parsing color_density from settings: {}. Using default value", e.what());
			}
		}

		if (app_settings.getSetting("base_iterations") != "") {
			std::string int_str = app_settings.getSetting("base_iterations");
			try {
				int value = std::stoi(int_str);
				if (value > 0 && value <= 10000) {
					m_base_iterations = value;
					spdlog::info("Loaded base_iterations from settings: {}", int_str);
				}
				else {
					spdlog::warn("base_iterations in settings is out of range (1-10000). Using default value: 100");
				}
			}
			catch (const std::exception& e) {
				spdlog::error("Error parsing base_iterations from settings: {}. Using default value: 100", e.what());
			}
		}

		// <--


		// --------------------------------- main loop ---------------------------------

		while (!done) {
			SDL_Event event;
			while (SDL_PollEvent(&event)) {
				ImGuiIO& io = ImGui::GetIO();
				ImGui_ImplSDL2_ProcessEvent(&event);
				if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_h)
				{
					hud_toggle = !hud_toggle;
				}
				// prerender event handling
				if (m_pre_render_complete && !io.WantCaptureMouse)
				{
					if (event.type == SDL_MOUSEWHEEL)
					{
						int mouseX, mouseY;
						SDL_GetMouseState(&mouseX, &mouseY);
						double mouse_real_before = (double)(mouseX - screenWidth / 2) / (0.5 * m_view_zoom * screenWidth) + m_view_center_x;
						double mouse_imag_before = (double)(screenHeight / 2 - mouseY) / (0.5 * m_view_zoom * screenHeight) + m_view_center_y;
						if (event.wheel.y > 0)
							m_view_zoom *= 1.1;
						else if (event.wheel.y < 0)
							m_view_zoom /= 1.1;
						double mouse_real_after = (double)(mouseX - screenWidth / 2) / (0.5 * m_view_zoom * screenWidth) + m_view_center_x;
						double mouse_imag_after = (double)(screenHeight / 2 - mouseY) / (0.5 * m_view_zoom * screenHeight) + m_view_center_y;
						m_view_center_x += mouse_real_before - mouse_real_after;
						m_view_center_y += mouse_imag_before - mouse_imag_after;
					}
					if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
					{
						m_is_dragging = true;
						m_drag_start_pos = ImGui::GetMousePos();
					}
					if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
					{
						m_is_dragging = false;
					}
					if (event.type == SDL_MOUSEMOTION && m_is_dragging)
					{
						ImVec2 current_pos = ImGui::GetMousePos();
						ImVec2 delta = ImVec2(current_pos.x - m_drag_start_pos.x, current_pos.y - m_drag_start_pos.y);

						double dx = -delta.x / (0.5 * m_view_zoom * screenWidth);
						double dy = delta.y / (0.5 * m_view_zoom * screenHeight);

						m_view_center_x += dx;
						m_view_center_y += dy;

						m_drag_start_pos = current_pos;
					}
					if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
						if (m_pre_render_complete) {
							glDeleteTextures(1, &m_pre_render_texture);
							glDeleteFramebuffers(1, &m_pre_render_fbo);
							if (m_texture_view_shader != nullptr) {
								delete m_texture_view_shader;
								m_texture_view_shader = nullptr;
							}
							m_pre_render_texture = 0;
							m_pre_render_fbo = 0;
						}
						if (m_is_loading) {
							// This is a simple way to handle closing while loading, though it will still wait for the thread.
							// More advanced logic would involve signaling the thread to cancel.
							if (m_pre_render_future.valid()) m_pre_render_future.wait();
						}
						m_pre_render_enabled = false;
						m_pre_render_complete = false;
						m_is_loading = false;
						show_main_buttons = !show_main_buttons;
						show_fractal_selection = false;
						spdlog::info("'Space' key pressed - toggling main buttons.");
						Application::m_currentFractal = FractalType::NONE;
						if (ourShader != nullptr) {
							delete ourShader;
							ourShader = nullptr;
						}
						ourShader = new Shader("shaders/background.vert", "shaders/background.frag");
						spdlog::info("Reverted to background shader.");
						sub = "Mesmer - Main Menu";
						title_text_toggle = true;
					}
				}
				// fractal-specific event handling (to clean up)
				if (m_currentFractal == FractalType::MANDELBROT && !io.WantCaptureMouse)
				{
					if (event.type == SDL_MOUSEWHEEL)
					{
						int mouseX, mouseY;
						SDL_GetMouseState(&mouseX, &mouseY);
						double mouse_real_before = (double)(mouseX - screenWidth / 2) / (0.5 * m_mandel_zoom * screenWidth) + m_mandel_center_x;
						double mouse_imag_before = (double)(screenHeight / 2 - mouseY) / (0.5 * m_mandel_zoom * screenHeight) + m_mandel_center_y;
						if (event.wheel.y > 0)
							m_mandel_zoom *= 1.1;
						else if (event.wheel.y < 0)
							m_mandel_zoom /= 1.1;
						double mouse_real_after = (double)(mouseX - screenWidth / 2) / (0.5 * m_mandel_zoom * screenWidth) + m_mandel_center_x;
						double mouse_imag_after = (double)(screenHeight / 2 - mouseY) / (0.5 * m_mandel_zoom * screenHeight) + m_mandel_center_y;
						m_mandel_center_x += mouse_real_before - mouse_real_after;
						m_mandel_center_y += mouse_imag_before - mouse_imag_after;
					}
					if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
					{
						m_is_dragging = true;
						m_drag_start_pos = ImGui::GetMousePos();
					}
					if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
					{
						m_is_dragging = false;
					}
					if (event.type == SDL_MOUSEMOTION && m_is_dragging)
					{
						ImVec2 current_pos = ImGui::GetMousePos();
						ImVec2 delta = ImVec2(current_pos.x - m_drag_start_pos.x, current_pos.y - m_drag_start_pos.y);

						double dx = -delta.x / (0.5 * m_mandel_zoom * screenWidth);
						double dy = delta.y / (0.5 * m_mandel_zoom * screenHeight);

						m_mandel_center_x += dx;
						m_mandel_center_y += dy;

						m_drag_start_pos = current_pos;
					}
					if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
						show_main_buttons = !show_main_buttons;
						show_fractal_selection = false;
						spdlog::info("'Space' key pressed - toggling main buttons.");
						Application::m_currentFractal = FractalType::NONE;
						if (ourShader != nullptr) {
							delete ourShader;
						}
						ourShader = new Shader("shaders/background.vert", "shaders/background.frag");
						spdlog::info("Reverted to background shader.");
						sub = "Mesmer - Main Menu";
						title_text_toggle = true;
					}
				}
				if (m_currentFractal == FractalType::JULIA && m_julia_interactive_mode && !io.WantCaptureMouse)
				{
					if (event.type == SDL_MOUSEMOTION && m_julia_c_map_to_mouse)
					{
						int mouseX, mouseY;
						SDL_GetMouseState(&mouseX, &mouseY);
						m_julia_c_x = 4.0 * (double)mouseX / (double)screenWidth - 2.0;
						m_julia_c_y = 4.0 * (double)mouseY / (double)screenHeight - 2.0;
					}
					if (event.type == SDL_MOUSEWHEEL)
					{
						int mouseX, mouseY;
						SDL_GetMouseState(&mouseX, &mouseY);
						double mouse_real_before = (double)(mouseX - screenWidth / 2) / (0.5 * m_mandel_zoom * screenWidth) + m_mandel_center_x;
						double mouse_imag_before = (double)(screenHeight / 2 - mouseY) / (0.5 * m_mandel_zoom * screenHeight) + m_mandel_center_y;
						if (event.wheel.y > 0)
							m_mandel_zoom *= 1.1;
						else if (event.wheel.y < 0)
							m_mandel_zoom /= 1.1;
						double mouse_real_after = (double)(mouseX - screenWidth / 2) / (0.5 * m_mandel_zoom * screenWidth) + m_mandel_center_x;
						double mouse_imag_after = (double)(screenHeight / 2 - mouseY) / (0.5 * m_mandel_zoom * screenHeight) + m_mandel_center_y;
						m_mandel_center_x += mouse_real_before - mouse_real_after;
						m_mandel_center_y += mouse_imag_before - mouse_imag_after;
					}
					if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
					{
						m_is_dragging = true;
						m_drag_start_pos = ImGui::GetMousePos();
					}
					if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
					{
						m_is_dragging = false;
					}
					if (event.type == SDL_MOUSEMOTION && m_is_dragging)
					{
						ImVec2 current_pos = ImGui::GetMousePos();
						ImVec2 delta = ImVec2(current_pos.x - m_drag_start_pos.x, current_pos.y - m_drag_start_pos.y);

						double dx = -delta.x / (0.5 * m_mandel_zoom * screenWidth);
						double dy = delta.y / (0.5 * m_mandel_zoom * screenHeight);

						m_mandel_center_x += dx;
						m_mandel_center_y += dy;

						m_drag_start_pos = current_pos;
					}
					if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
						show_main_buttons = !show_main_buttons;
						show_fractal_selection = false;
						spdlog::info("'Space' key pressed - toggling main buttons.");
						Application::m_currentFractal = FractalType::NONE;
						if (ourShader != nullptr) {
							delete ourShader;
						}
						ourShader = new Shader("shaders/background.vert", "shaders/background.frag");
						spdlog::info("Reverted to background shader.");
						sub = "Mesmer - Main Menu";
						title_text_toggle = true;
					}
					if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_m) {
						m_julia_c_map_to_mouse = !m_julia_c_map_to_mouse;
						if (m_julia_c_map_to_mouse) {
							spdlog::info("'M' key pressed - Julia constant will now map to mouse position.");
						}
						else {
							spdlog::info("'M' key pressed - Julia constant will no longer map to mouse position.");
						}
					}
					if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_i) {
						m_julia_interactive_mode = !m_julia_interactive_mode;
						if (m_julia_interactive_mode) {
							spdlog::info("'I' key pressed - Julia interactive mode enabled.");
						}
						else {
							spdlog::info("'I' key pressed - Julia interactive mode disabled.");
						}
					}
				}
				if (m_currentFractal == FractalType::BURNING_SHIP && !io.WantCaptureMouse) {
					if (event.type == SDL_MOUSEWHEEL)
					{
						int mouseX, mouseY;
						SDL_GetMouseState(&mouseX, &mouseY);
						double mouse_real_before = (double)(mouseX - screenWidth / 2) / (0.5 * m_mandel_zoom * screenWidth) + m_mandel_center_x;
						double mouse_imag_before = (double)(screenHeight / 2 - mouseY) / (0.5 * m_mandel_zoom * screenHeight) + m_mandel_center_y;
						if (event.wheel.y > 0)
							m_mandel_zoom *= 1.1;
						else if (event.wheel.y < 0)
							m_mandel_zoom /= 1.1;
						double mouse_real_after = (double)(mouseX - screenWidth / 2) / (0.5 * m_mandel_zoom * screenWidth) + m_mandel_center_x;
						double mouse_imag_after = (double)(screenHeight / 2 - mouseY) / (0.5 * m_mandel_zoom * screenHeight) + m_mandel_center_y;
						m_mandel_center_x += mouse_real_before - mouse_real_after;
						m_mandel_center_y += mouse_imag_before - mouse_imag_after;
					}
					if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
					{
						m_is_dragging = true;
						m_drag_start_pos = ImGui::GetMousePos();
					}
					if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
					{
						m_is_dragging = false;
					}
					if (event.type == SDL_MOUSEMOTION && m_is_dragging)
					{
						ImVec2 current_pos = ImGui::GetMousePos();
						ImVec2 delta = ImVec2(current_pos.x - m_drag_start_pos.x, current_pos.y - m_drag_start_pos.y);

						double dx = -delta.x / (0.5 * m_mandel_zoom * screenWidth);
						double dy = delta.y / (0.5 * m_mandel_zoom * screenHeight);

						m_mandel_center_x += dx;
						m_mandel_center_y += dy;

						m_drag_start_pos = current_pos;
					}
					if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
						show_main_buttons = !show_main_buttons;
						show_fractal_selection = false;
						spdlog::info("'Space' key pressed - toggling main buttons.");
						Application::m_currentFractal = FractalType::NONE;
						if (ourShader != nullptr) {
							delete ourShader;
						}
						ourShader = new Shader("shaders/background.vert", "shaders/background.frag");
						spdlog::info("Reverted to background shader.");
						sub = "Mesmer - Main Menu";
						title_text_toggle = true;
					}
				}
				if (m_currentFractal == FractalType::TRICORN && !io.WantCaptureMouse) {
					if (event.type == SDL_MOUSEWHEEL)
					{
						int mouseX, mouseY;
						SDL_GetMouseState(&mouseX, &mouseY);
						double mouse_real_before = (double)(mouseX - screenWidth / 2) / (0.5 * m_mandel_zoom * screenWidth) + m_mandel_center_x;
						double mouse_imag_before = (double)(screenHeight / 2 - mouseY) / (0.5 * m_mandel_zoom * screenHeight) + m_mandel_center_y;
						if (event.wheel.y > 0)
							m_mandel_zoom *= 1.1;
						else if (event.wheel.y < 0)
							m_mandel_zoom /= 1.1;
						double mouse_real_after = (double)(mouseX - screenWidth / 2) / (0.5 * m_mandel_zoom * screenWidth) + m_mandel_center_x;
						double mouse_imag_after = (double)(screenHeight / 2 - mouseY) / (0.5 * m_mandel_zoom * screenHeight) + m_mandel_center_y;
						m_mandel_center_x += mouse_real_before - mouse_real_after;
						m_mandel_center_y += mouse_imag_before - mouse_imag_after;
					}
					if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
					{
						m_is_dragging = true;
						m_drag_start_pos = ImGui::GetMousePos();
					}
					if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
					{
						m_is_dragging = false;
					}
					if (event.type == SDL_MOUSEMOTION && m_is_dragging)
					{
						ImVec2 current_pos = ImGui::GetMousePos();
						ImVec2 delta = ImVec2(current_pos.x - m_drag_start_pos.x, current_pos.y - m_drag_start_pos.y);

						double dx = -delta.x / (0.5 * m_mandel_zoom * screenWidth);
						double dy = delta.y / (0.5 * m_mandel_zoom * screenHeight);

						m_mandel_center_x += dx;
						m_mandel_center_y += dy;

						m_drag_start_pos = current_pos;
					}
					if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
						show_main_buttons = !show_main_buttons;
						show_fractal_selection = false;
						spdlog::info("'Space' key pressed - toggling main buttons.");
						Application::m_currentFractal = FractalType::NONE;
						if (ourShader != nullptr) {
							delete ourShader;
						}
						ourShader = new Shader("shaders/background.vert", "shaders/background.frag");
						spdlog::info("Reverted to background shader.");
						sub = "Mesmer - Main Menu";
						title_text_toggle = true;
					}
				}
				if (m_currentFractal == FractalType::PHOENIX && !io.WantCaptureMouse)
				{
					if (event.type == SDL_MOUSEWHEEL)
					{
						int mouseX, mouseY;
						SDL_GetMouseState(&mouseX, &mouseY);
						double mouse_real_before = (double)(mouseX - screenWidth / 2) / (0.5 * m_mandel_zoom * screenWidth) + m_mandel_center_x;
						double mouse_imag_before = (double)(screenHeight / 2 - mouseY) / (0.5 * m_mandel_zoom * screenHeight) + m_mandel_center_y;
						if (event.wheel.y > 0)
							m_mandel_zoom *= 1.1;
						else if (event.wheel.y < 0)
							m_mandel_zoom /= 1.1;
						double mouse_real_after = (double)(mouseX - screenWidth / 2) / (0.5 * m_mandel_zoom * screenWidth) + m_mandel_center_x;
						double mouse_imag_after = (double)(screenHeight / 2 - mouseY) / (0.5 * m_mandel_zoom * screenHeight) + m_mandel_center_y;
						m_mandel_center_x += mouse_real_before - mouse_real_after;
						m_mandel_center_y += mouse_imag_before - mouse_imag_after;
					}
					if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
					{
						m_is_dragging = true;
						m_drag_start_pos = ImGui::GetMousePos();
					}
					if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
					{
						m_is_dragging = false;
					}
					if (event.type == SDL_MOUSEMOTION && m_is_dragging)
					{
						ImVec2 current_pos = ImGui::GetMousePos();
						ImVec2 delta = ImVec2(current_pos.x - m_drag_start_pos.x, current_pos.y - m_drag_start_pos.y);

						double dx = -delta.x / (0.5 * m_mandel_zoom * screenWidth);
						double dy = delta.y / (0.5 * m_mandel_zoom * screenHeight);

						m_mandel_center_x += dx;
						m_mandel_center_y += dy;

						m_drag_start_pos = current_pos;
					}
					if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
						show_main_buttons = !show_main_buttons;
						show_fractal_selection = false;
						spdlog::info("'Space' key pressed - toggling main buttons.");
						Application::m_currentFractal = FractalType::NONE;
						if (ourShader != nullptr) {
							delete ourShader;
						}
						ourShader = new Shader("shaders/background.vert", "shaders/background.frag");
						spdlog::info("Reverted to background shader.");
						sub = "Mesmer - Main Menu";
						title_text_toggle = true;
					}
				}
				if (m_currentFractal == FractalType::LYAPUNOV && !io.WantCaptureMouse)
				{
					if (event.type == SDL_MOUSEWHEEL)
					{
						if (event.wheel.y > 0)
							m_lyapunov_zoom *= 1.1;
						else if (event.wheel.y < 0)
							m_lyapunov_zoom /= 1.1;
					}
					if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
					{
						m_is_dragging = true;
						m_drag_start_pos = ImGui::GetMousePos();
					}
					if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
					{
						m_is_dragging = false;
					}
					if (event.type == SDL_MOUSEMOTION && m_is_dragging)
					{
						ImVec2 current_pos = ImGui::GetMousePos();
						ImVec2 delta = ImVec2(current_pos.x - m_drag_start_pos.x, current_pos.y - m_drag_start_pos.y);

						double da = -delta.x / (0.5 * m_lyapunov_zoom * screenWidth);
						double db = delta.y / (0.5 * m_lyapunov_zoom * screenHeight);

						m_lyapunov_center_a += da;
						m_lyapunov_center_b += db;

						m_drag_start_pos = current_pos;
					}
					if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
						show_main_buttons = !show_main_buttons;
						show_fractal_selection = false;
						spdlog::info("'Space' key pressed - toggling main buttons.");
						Application::m_currentFractal = FractalType::NONE;
						if (ourShader != nullptr) {
							delete ourShader;
						}
						ourShader = new Shader("shaders/background.vert", "shaders/background.frag");
						spdlog::info("Reverted to background shader.");
						sub = "Mesmer - Main Menu";
						title_text_toggle = true;
					}
				}
				if (m_currentFractal == FractalType::NEWTON && !io.WantCaptureMouse)
				{
					if (event.type == SDL_MOUSEWHEEL)
					{
						int mouseX, mouseY;
						SDL_GetMouseState(&mouseX, &mouseY);
						double mouse_real_before = (double)(mouseX - screenWidth / 2) / (0.5 * m_mandel_zoom * screenWidth) + m_mandel_center_x;
						double mouse_imag_before = (double)(screenHeight / 2 - mouseY) / (0.5 * m_mandel_zoom * screenHeight) + m_mandel_center_y;
						if (event.wheel.y > 0)
							m_mandel_zoom *= 1.1;
						else if (event.wheel.y < 0)
							m_mandel_zoom /= 1.1;
						double mouse_real_after = (double)(mouseX - screenWidth / 2) / (0.5 * m_mandel_zoom * screenWidth) + m_mandel_center_x;
						double mouse_imag_after = (double)(screenHeight / 2 - mouseY) / (0.5 * m_mandel_zoom * screenHeight) + m_mandel_center_y;
						m_mandel_center_x += mouse_real_before - mouse_real_after;
						m_mandel_center_y += mouse_imag_before - mouse_imag_after;
					}
					if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
					{
						m_is_dragging = true;
						m_drag_start_pos = ImGui::GetMousePos();
					}
					if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
					{
						m_is_dragging = false;
					}
					if (event.type == SDL_MOUSEMOTION && m_is_dragging)
					{
						ImVec2 current_pos = ImGui::GetMousePos();
						ImVec2 delta = ImVec2(current_pos.x - m_drag_start_pos.x, current_pos.y - m_drag_start_pos.y);

						double dx = -delta.x / (0.5 * m_mandel_zoom * screenWidth);
						double dy = delta.y / (0.5 * m_mandel_zoom * screenHeight);

						m_mandel_center_x += dx;
						m_mandel_center_y += dy;

						m_drag_start_pos = current_pos;
					}
					if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
						show_main_buttons = !show_main_buttons;
						show_fractal_selection = false;
						spdlog::info("'Space' key pressed - toggling main buttons.");
						Application::m_currentFractal = FractalType::NONE;
						if (ourShader != nullptr) {
							delete ourShader;
						}
						ourShader = new Shader("shaders/background.vert", "shaders/background.frag");
						spdlog::info("Reverted to background shader.");
						sub = "Mesmer - Main Menu";
						title_text_toggle = true;
					}
				}
				if (m_currentFractal == FractalType::MULTIBROT && !io.WantCaptureMouse)
				{
					if (event.type == SDL_MOUSEWHEEL)
					{
						int mouseX, mouseY;
						SDL_GetMouseState(&mouseX, &mouseY);
						double mouse_real_before = (double)(mouseX - screenWidth / 2) / (0.5 * m_mandel_zoom * screenWidth) + m_mandel_center_x;
						double mouse_imag_before = (double)(screenHeight / 2 - mouseY) / (0.5 * m_mandel_zoom * screenHeight) + m_mandel_center_y;
						if (event.wheel.y > 0)
							m_mandel_zoom *= 1.1;
						else if (event.wheel.y < 0)
							m_mandel_zoom /= 1.1;
						double mouse_real_after = (double)(mouseX - screenWidth / 2) / (0.5 * m_mandel_zoom * screenWidth) + m_mandel_center_x;
						double mouse_imag_after = (double)(screenHeight / 2 - mouseY) / (0.5 * m_mandel_zoom * screenHeight) + m_mandel_center_y;
						m_mandel_center_x += mouse_real_before - mouse_real_after;
						m_mandel_center_y += mouse_imag_before - mouse_imag_after;
					}
					if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
					{
						m_is_dragging = true;
						m_drag_start_pos = ImGui::GetMousePos();
					}
					if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
					{
						m_is_dragging = false;
					}
					if (event.type == SDL_MOUSEMOTION && m_is_dragging)
					{
						ImVec2 current_pos = ImGui::GetMousePos();
						ImVec2 delta = ImVec2(current_pos.x - m_drag_start_pos.x, current_pos.y - m_drag_start_pos.y);

						double dx = -delta.x / (0.5 * m_mandel_zoom * screenWidth);
						double dy = delta.y / (0.5 * m_mandel_zoom * screenHeight);

						m_mandel_center_x += dx;
						m_mandel_center_y += dy;

						m_drag_start_pos = current_pos;
					}
					if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
						show_main_buttons = !show_main_buttons;
						show_fractal_selection = false;
						spdlog::info("'Space' key pressed - toggling main buttons.");
						Application::m_currentFractal = FractalType::NONE;
						if (ourShader != nullptr) {
							delete ourShader;
						}
						ourShader = new Shader("shaders/background.vert", "shaders/background.frag");
						spdlog::info("Reverted to background shader.");
						sub = "Mesmer - Main Menu";
						title_text_toggle = true;
					}
				}
				if (m_currentFractal == FractalType::NOVA && !io.WantCaptureMouse)
				{
					if (event.type == SDL_MOUSEWHEEL)
					{
						int mouseX, mouseY;
						SDL_GetMouseState(&mouseX, &mouseY);
						double mouse_real_before = (double)(mouseX - screenWidth / 2) / (0.5 * m_mandel_zoom * screenWidth) + m_mandel_center_x;
						double mouse_imag_before = (double)(screenHeight / 2 - mouseY) / (0.5 * m_mandel_zoom * screenHeight) + m_mandel_center_y;
						if (event.wheel.y > 0)
							m_mandel_zoom *= 1.1;
						else if (event.wheel.y < 0)
							m_mandel_zoom /= 1.1;
						double mouse_real_after = (double)(mouseX - screenWidth / 2) / (0.5 * m_mandel_zoom * screenWidth) + m_mandel_center_x;
						double mouse_imag_after = (double)(screenHeight / 2 - mouseY) / (0.5 * m_mandel_zoom * screenHeight) + m_mandel_center_y;
						m_mandel_center_x += mouse_real_before - mouse_real_after;
						m_mandel_center_y += mouse_imag_before - mouse_imag_after;
					}
					if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
					{
						m_is_dragging = true;
						m_drag_start_pos = ImGui::GetMousePos();
					}
					if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
					{
						m_is_dragging = false;
					}
					if (event.type == SDL_MOUSEMOTION && m_is_dragging)
					{
						ImVec2 current_pos = ImGui::GetMousePos();
						ImVec2 delta = ImVec2(current_pos.x - m_drag_start_pos.x, current_pos.y - m_drag_start_pos.y);

						double dx = -delta.x / (0.5 * m_mandel_zoom * screenWidth);
						double dy = delta.y / (0.5 * m_mandel_zoom * screenHeight);

						m_mandel_center_x += dx;
						m_mandel_center_y += dy;

						m_drag_start_pos = current_pos;
					}
					if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
						show_main_buttons = !show_main_buttons;
						show_fractal_selection = false;
						spdlog::info("'Space' key pressed - toggling main buttons.");
						Application::m_currentFractal = FractalType::NONE;
						if (ourShader != nullptr) {
							delete ourShader;
						}
						ourShader = new Shader("shaders/background.vert", "shaders/background.frag");
						spdlog::info("Reverted to background shader.");
						sub = "Mesmer - Main Menu";
						title_text_toggle = true;
					}
				}
				if (m_currentFractal == FractalType::SPIDER && !io.WantCaptureMouse)
				{
					if (event.type == SDL_MOUSEWHEEL)
					{
						int mouseX, mouseY;
						SDL_GetMouseState(&mouseX, &mouseY);
						double mouse_real_before = (double)(mouseX - screenWidth / 2) / (0.5 * m_mandel_zoom * screenWidth) + m_mandel_center_x;
						double mouse_imag_before = (double)(screenHeight / 2 - mouseY) / (0.5 * m_mandel_zoom * screenHeight) + m_mandel_center_y;
						if (event.wheel.y > 0)
							m_mandel_zoom *= 1.1;
						else if (event.wheel.y < 0)
							m_mandel_zoom /= 1.1;
						double mouse_real_after = (double)(mouseX - screenWidth / 2) / (0.5 * m_mandel_zoom * screenWidth) + m_mandel_center_x;
						double mouse_imag_after = (double)(screenHeight / 2 - mouseY) / (0.5 * m_mandel_zoom * screenHeight) + m_mandel_center_y;
						m_mandel_center_x += mouse_real_before - mouse_real_after;
						m_mandel_center_y += mouse_imag_before - mouse_imag_after;
					}
					if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
					{
						m_is_dragging = true;
						m_drag_start_pos = ImGui::GetMousePos();
					}
					if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
					{
						m_is_dragging = false;
					}
					if (event.type == SDL_MOUSEMOTION && m_is_dragging)
					{
						ImVec2 current_pos = ImGui::GetMousePos();
						ImVec2 delta = ImVec2(current_pos.x - m_drag_start_pos.x, current_pos.y - m_drag_start_pos.y);

						double dx = -delta.x / (0.5 * m_mandel_zoom * screenWidth);
						double dy = delta.y / (0.5 * m_mandel_zoom * screenHeight);

						m_mandel_center_x += dx;
						m_mandel_center_y += dy;

						m_drag_start_pos = current_pos;
					}
					if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
						show_main_buttons = !show_main_buttons;
						show_fractal_selection = false;
						spdlog::info("'Space' key pressed - toggling main buttons.");
						Application::m_currentFractal = FractalType::NONE;
						if (ourShader != nullptr) {
							delete ourShader;
						}
						ourShader = new Shader("shaders/background.vert", "shaders/background.frag");
						spdlog::info("Reverted to background shader.");
						sub = "Mesmer - Main Menu";
						title_text_toggle = true;
					}
				}
				// --- fractal-specific event handling end ---
				if (event.type == SDL_QUIT) {
					done = true;
				}
				if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window)) {
					done = true;
				}
			}
			

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL2_NewFrame();
			ImGui::NewFrame();

			//---------------------------------- control panel ----------------------------------

			if (hud_toggle) 
			{
				ImGui::Begin("Control Panel");
				if (ImGui::CollapsingHeader("Menu BG Settings")) 
				{
					ImGui::SliderFloat3("Menu BG Color One", (float*)&m_menu_bg_color_one, 0.0f, 1.0f);
					ImGui::SliderFloat3("Menu BG Color Two", (float*)&m_menu_bg_color_two, 0.0f, 1.0f);
					ImGui::SliderFloat3("Menu BG Accent Color", (float*)&m_menu_bg_accent_color, 0.0f, 1.0f);
					ImGui::Separator();
					if (ImGui::Button("Save BG Preferences")) {
						app_settings.updateSetting("menu_bg_color_one", std::to_string(m_menu_bg_color_one.x) + "," + std::to_string(m_menu_bg_color_one.y) + "," + std::to_string(m_menu_bg_color_one.z) + "," + "1.000000");
						app_settings.updateSetting("menu_bg_color_two", std::to_string(m_menu_bg_color_two.x) + "," + std::to_string(m_menu_bg_color_two.y) + "," + std::to_string(m_menu_bg_color_two.z) + "," + "1.000000");
						app_settings.updateSetting("menu_bg_accent_color", std::to_string(m_menu_bg_accent_color.x) + "," + std::to_string(m_menu_bg_accent_color.y) + "," + std::to_string(m_menu_bg_accent_color.z) + "," + "1.000000");
						app_settings.saveSettings();
					}
				}
				if (ImGui::CollapsingHeader("Fractal Display Settings"))
				{
					ImGui::ColorEdit3("Background Color", (float*)&clear_color); // maybe useless

					ImGui::Separator();
					ImGui::SliderFloat("Color Density", &m_color_density, 0.01f, 0.5f, "%.3f");
					ImGui::Separator();

					ImGui::Checkbox("Apply Common Color Palette to All Fractals", &m_apply_common_color_palette);

					ImGui::Separator();
					ImGui::Text("Fractal Common Color Palette");
					ImGui::ColorEdit3("Brightness Cmn (a)", (float*)&m_palette_a);
					ImGui::ColorEdit3("Contrast Cmn (b)", (float*)&m_palette_b);
					ImGui::SliderFloat3("Frequency Cmn (c)", (float*)&m_palette_c, 0.0f, 2.0f);
					ImGui::SliderFloat3("Phase Cmn (d)", (float*)&m_palette_d, 0.0f, 1.0f);
					ImGui::Separator();
					if (ImGui::Button("Save Common Color Preferences")) {
						app_settings.updateSetting("fractal_palette_a", std::to_string(m_palette_a.x) + "," + std::to_string(m_palette_a.y) + "," + std::to_string(m_palette_a.z) + "," + "1.000000");
						app_settings.updateSetting("fractal_palette_b", std::to_string(m_palette_b.x) + "," + std::to_string(m_palette_b.y) + "," + std::to_string(m_palette_b.z) + "," + "1.000000");
						app_settings.updateSetting("fractal_palette_c", std::to_string(m_palette_c.x) + "," + std::to_string(m_palette_c.y) + "," + std::to_string(m_palette_c.z) + "," + "1.000000");
						app_settings.updateSetting("fractal_palette_d", std::to_string(m_palette_d.x) + "," + std::to_string(m_palette_d.y) + "," + std::to_string(m_palette_d.z) + "," + "1.000000");
						app_settings.updateSetting("fractal_color_density", std::to_string(m_color_density));
						app_settings.updateSetting("fractal_apply_common_palette", m_apply_common_color_palette ? "true" : "false");
						app_settings.saveSettings();
					}
				}

				if (ImGui::CollapsingHeader("Fractal Parameters"))
				{
					if (m_currentFractal == FractalType::MANDELBROT)
					{
						ImGui::Text("Mandelbrot Controls");
						ImGui::Separator();

						ImGui::Text("Mandelbrot Set Fractal Color Palette");
						ImGui::ColorEdit3("Brightness (a)", (float*)&m_palette_mandelbrot_a);
						ImGui::ColorEdit3("Contrast (b)", (float*)&m_palette_mandelbrot_b);
						ImGui::SliderFloat3("Frequency (c)", (float*)&m_palette_mandelbrot_c, 0.0f, 2.0f);
						ImGui::SliderFloat3("Phase (d)", (float*)&m_palette_mandelbrot_d, 0.0f, 1.0f);
						ImGui::Separator();

						ImGui::InputDouble("Zoom", &m_mandel_zoom, 0.1, 0.0, "%.8f");
						ImGui::InputDouble("Center X", &m_mandel_center_x, 0.01, 0.0, "%.8f");
						ImGui::InputDouble("Center Y", &m_mandel_center_y, 0.01, 0.0, "%.8f");

						if (m_adaptive_iterations) {
							ImGui::BeginDisabled();
							ImGui::SliderInt("Max Iterations", &m_mandel_max_iterations, 50, 5000);
							ImGui::EndDisabled();
						}
						else {
							ImGui::SliderInt("Max Iterations", &m_mandel_max_iterations, 50, 5000);
						}
						ImGui::Separator();
						ImGui::Checkbox("Adaptive Iterations", &m_adaptive_iterations);
						ImGui::SliderInt("Base Iterations", &m_base_iterations, 50, 1000);
						ImGui::TextWrapped(("Current Adaptive Iterations: " + std::to_string(final_adaptive_iterations)).c_str());
						if (ImGui::IsItemHovered()) {
							ImGui::SetTooltip("Max iterations will increase with zoom when 'Adaptive' is checked.");
						}

						if (ImGui::Button("Reset View")) {
							m_mandel_zoom = 1.0;
							m_mandel_center_x = -0.75;
							m_mandel_center_y = 0.0;
							m_mandel_max_iterations = 200;
						}
					}
					else if (m_currentFractal == FractalType::JULIA)
					{
						ImGui::Text("Julia Controls");
						ImGui::Separator();

						ImGui::Text("Julia Set Fractal Color Palette");
						ImGui::ColorEdit3("Brightness (a)", (float*)&m_palette_julia_a);
						ImGui::ColorEdit3("Contrast (b)", (float*)&m_palette_julia_b);
						ImGui::SliderFloat3("Frequency (c)", (float*)&m_palette_julia_c, 0.0f, 2.0f);
						ImGui::SliderFloat3("Phase (d)", (float*)&m_palette_julia_d, 0.0f, 1.0f);
						ImGui::Separator();

						ImGui::InputDouble("Zoom", &m_mandel_zoom, 0.1, 0.0, "%.8f");
						ImGui::InputDouble("Center X", &m_mandel_center_x, 0.01, 0.0, "%.8f");
						ImGui::InputDouble("Center Y", &m_mandel_center_y, 0.01, 0.0, "%.8f");

						ImGui::Separator();

						ImGui::InputDouble("Constant c (real)", &m_julia_c_x, 0.001, 0.0, "%.8f");
						ImGui::InputDouble("Constant c (imag)", &m_julia_c_y, 0.001, 0.0, "%.8f");

						ImGui::Checkbox("Interactive Mode (Mouse)", &m_julia_interactive_mode);
						ImGui::Checkbox("Map Julia Const to Mouse Pos", &m_julia_c_map_to_mouse);

						ImGui::Separator();

						if (m_adaptive_iterations) {
							ImGui::BeginDisabled();
							ImGui::SliderInt("Max Iterations", &m_mandel_max_iterations, 50, 5000);
							ImGui::EndDisabled();
						}
						else {
							ImGui::SliderInt("Max Iterations", &m_mandel_max_iterations, 50, 5000);
						}
						ImGui::Separator();
						ImGui::Checkbox("Adaptive Iterations", &m_adaptive_iterations);
						ImGui::SliderInt("Base Iterations", &m_base_iterations, 50, 1000);
						ImGui::TextWrapped(("Current Adaptive Iterations: " + std::to_string(final_adaptive_iterations)).c_str());
						if (ImGui::IsItemHovered()) {
							ImGui::SetTooltip("Max iterations will increase with zoom when 'Adaptive' is checked.");
						}

						if (ImGui::Button("Reset View")) {
							m_mandel_zoom = 1.0;
							m_mandel_center_x = 0.0;
							m_mandel_center_y = 0.0;
							m_julia_c_x = -0.8;
							m_julia_c_y = 0.156;
							m_mandel_max_iterations = 200;
						}
					}
					else if (m_currentFractal == FractalType::BURNING_SHIP)
					{
						ImGui::Text("Burning Ship Controls");
						ImGui::Separator();

						ImGui::Text("Burning Ship Set Fractal Color Palette");
						ImGui::ColorEdit3("Brightness (a)", (float*)&m_palette_burning_ship_a);
						ImGui::ColorEdit3("Contrast (b)", (float*)&m_palette_burning_ship_b);
						ImGui::SliderFloat3("Frequency (c)", (float*)&m_palette_burning_ship_c, 0.0f, 2.0f);
						ImGui::SliderFloat3("Phase (d)", (float*)&m_palette_burning_ship_d, 0.0f, 1.0f);
						ImGui::Separator();

						ImGui::InputDouble("Zoom", &m_mandel_zoom, 0.1, 0.0, "%.8f");
						ImGui::InputDouble("Center X", &m_mandel_center_x, 0.01, 0.0, "%.8f");
						ImGui::InputDouble("Center Y", &m_mandel_center_y, 0.01, 0.0, "%.8f");

						if (m_adaptive_iterations) {
							ImGui::BeginDisabled();
							ImGui::SliderInt("Max Iterations", &m_mandel_max_iterations, 50, 5000);
							ImGui::EndDisabled();
						}
						else {
							ImGui::SliderInt("Max Iterations", &m_mandel_max_iterations, 50, 5000);
						}
						ImGui::Separator();
						ImGui::Checkbox("Adaptive Iterations", &m_adaptive_iterations);
						ImGui::SliderInt("Base Iterations", &m_base_iterations, 50, 1000);
						ImGui::TextWrapped(("Current Adaptive Iterations: " + std::to_string(final_adaptive_iterations)).c_str());
						if (ImGui::IsItemHovered()) {
							ImGui::SetTooltip("Max iterations will increase with zoom when 'Adaptive' is checked.");
						}
						if (ImGui::Button("Reset View")) {
							m_mandel_zoom = 22.0;
							m_mandel_center_x = -1.75;
							m_mandel_center_y = -0.04;
							m_mandel_max_iterations = 200;
						}
					}
					else if (m_currentFractal == FractalType::TRICORN) 
					{
						ImGui::Text("Tricorn Controls");
						ImGui::Separator();

						ImGui::Text("Tricorn Set Fractal Color Palette");
						ImGui::ColorEdit3("Brightness (a)", (float*)&m_palette_tricorn_a);
						ImGui::ColorEdit3("Contrast (b)", (float*)&m_palette_tricorn_b);
						ImGui::SliderFloat3("Frequency (c)", (float*)&m_palette_tricorn_c, 0.0f, 2.0f);
						ImGui::SliderFloat3("Phase (d)", (float*)&m_palette_tricorn_d, 0.0f, 1.0f);
						ImGui::Separator();

						ImGui::InputDouble("Zoom", &m_mandel_zoom, 0.1, 0.0, "%.8f");
						ImGui::InputDouble("Center X", &m_mandel_center_x, 0.01, 0.0, "%.8f");
						ImGui::InputDouble("Center Y", &m_mandel_center_y, 0.01, 0.0, "%.8f");


						if (m_adaptive_iterations) {
							ImGui::BeginDisabled();
							ImGui::SliderInt("Max Iterations", &m_mandel_max_iterations, 50, 5000);
							ImGui::EndDisabled();
						}
						else {
							ImGui::SliderInt("Max Iterations", &m_mandel_max_iterations, 50, 5000);
						}
						ImGui::Separator();
						ImGui::Checkbox("Adaptive Iterations", &m_adaptive_iterations);
						ImGui::SliderInt("Base Iterations", &m_base_iterations, 50, 1000);
						ImGui::TextWrapped(("Current Adaptive Iterations: " + std::to_string(final_adaptive_iterations)).c_str());
						if (ImGui::IsItemHovered()) {
							ImGui::SetTooltip("Max iterations will increase with zoom when 'Adaptive' is checked.");
						}
						if (ImGui::Button("Reset View")) {
							m_mandel_zoom = 1.0;
							m_mandel_center_x = -0.75;
							m_mandel_center_y = 0.0;
							m_mandel_max_iterations = 200;
						}
					}
					else if (m_currentFractal == FractalType::PHOENIX) 
					{
						ImGui::Text("Phoenix Controls");
						ImGui::Separator();

						ImGui::Text("Phoenix Set Fractal Color Palette");
						ImGui::ColorEdit3("Brightness (a)", (float*)&m_palette_phoenix_a);
						ImGui::ColorEdit3("Contrast (b)", (float*)&m_palette_phoenix_b);
						ImGui::SliderFloat3("Frequency (c)", (float*)&m_palette_phoenix_c, 0.0f, 2.0f);
						ImGui::SliderFloat3("Phase (d)", (float*)&m_palette_phoenix_d, 0.0f, 1.0f);
						ImGui::Separator();

						ImGui::InputDouble("Zoom", &m_mandel_zoom, 0.1, 0.0, "%.8f");
						ImGui::InputDouble("Center X", &m_mandel_center_x, 0.01, 0.0, "%.8f");
						ImGui::InputDouble("Center Y", &m_mandel_center_y, 0.01, 0.0, "%.8f");
						ImGui::InputDouble("Constant c (real)", &m_phoenix_c_x, 0.01, 0.0, "%.4f");
						ImGui::InputDouble("Constant c (imag)", &m_phoenix_c_y, 0.01, 0.0, "%.4f");
						ImGui::InputDouble("Constant p", &m_phoenix_p, 0.01, 0.0, "%.4f");

						if (m_adaptive_iterations) {
							ImGui::BeginDisabled();
							ImGui::SliderInt("Max Iterations", &m_mandel_max_iterations, 50, 5000);
							ImGui::EndDisabled();
						}
						else {
							ImGui::SliderInt("Max Iterations", &m_mandel_max_iterations, 50, 5000);
						}

						ImGui::Checkbox("Adaptive Iterations", &m_adaptive_iterations);
						ImGui::SliderInt("Base Iterations", &m_base_iterations, 50, 1000);
						ImGui::TextWrapped(("Current Adaptive Iterations: " + std::to_string(final_adaptive_iterations)).c_str());
						if (ImGui::IsItemHovered()) {
							ImGui::SetTooltip("Max iterations will increase with zoom when 'Adaptive' is checked.");
						}
						if (ImGui::Button("Reset View")) {
							m_mandel_zoom = 1.0;
							m_mandel_center_x = -0.75;
							m_mandel_center_y = 0.0;
							m_mandel_max_iterations = 200;
						}
					} 
					else if (m_currentFractal == FractalType::LYAPUNOV) 
					{
						ImGui::Text("Lyapunov Controls");
						ImGui::Separator();

						ImGui::Text("Lyapunov Set Fractal Color Palette");
						ImGui::ColorEdit3("Brightness (a)", (float*)&m_palette_lyapunov_a);
						ImGui::ColorEdit3("Contrast (b)", (float*)&m_palette_lyapunov_b);
						ImGui::SliderFloat3("Frequency (c)", (float*)&m_palette_lyapunov_c, 0.0f, 2.0f);
						ImGui::SliderFloat3("Phase (d)", (float*)&m_palette_lyapunov_d, 0.0f, 1.0f);
						ImGui::Separator();

						ImGui::Text("View is of (a, b) parameter space.");
						ImGui::InputDouble("Zoom", &m_lyapunov_zoom, 0.1, 0.0, "%.8f");
						ImGui::InputDouble("Center A", &m_lyapunov_center_a, 0.01, 0.0, "%.8f");
						ImGui::InputDouble("Center B", &m_lyapunov_center_b, 0.01, 0.0, "%.8f");
						ImGui::Separator();

						if (m_adaptive_iterations) {
							ImGui::BeginDisabled();
							ImGui::SliderInt("Max Iterations", &m_mandel_max_iterations, 50, 5000);
							ImGui::EndDisabled();
						}
						else {
							ImGui::SliderInt("Max Iterations", &m_mandel_max_iterations, 50, 5000);
						}
						
						ImGui::Checkbox("Adaptive Iterations", &m_adaptive_iterations);
						ImGui::SliderInt("Base Iterations", &m_base_iterations, 50, 1000);
						ImGui::TextWrapped(("Current Adaptive Iterations: " + std::to_string(final_adaptive_iterations)).c_str());
						if (ImGui::IsItemHovered()) {
							ImGui::SetTooltip("Max iterations will increase with zoom when 'Adaptive' is checked.");
						}
						if (ImGui::Button("Reset View")) {
							m_lyapunov_zoom = 1.10000000;
							m_lyapunov_center_a = 2.76486681;
							m_lyapunov_center_b = 4.05390855;
							m_mandel_max_iterations = 200;
						}
					}
					else if (m_currentFractal == FractalType::NEWTON) 
					{
						ImGui::Text("Newton Controls");
						ImGui::Separator();

						ImGui::Text("Newton Set Fractal Color Palette");
						ImGui::ColorEdit3("Brightness (a)", (float*)&m_palette_newton_a);
						ImGui::ColorEdit3("Contrast (b)", (float*)&m_palette_newton_b);
						ImGui::SliderFloat3("Frequency (c)", (float*)&m_palette_newton_c, 0.0f, 2.0f);
						ImGui::SliderFloat3("Phase (d)", (float*)&m_palette_newton_d, 0.0f, 1.0f);
						ImGui::Separator();

						ImGui::InputDouble("Zoom", &m_mandel_zoom, 0.1, 0.0, "%.8f");
						ImGui::InputDouble("Center X", &m_mandel_center_x, 0.01, 0.0, "%.8f");
						ImGui::InputDouble("Center Y", &m_mandel_center_y, 0.01, 0.0, "%.8f");

						ImGui::Separator();

						if (m_adaptive_iterations) {
							ImGui::BeginDisabled();
							ImGui::SliderInt("Max Iterations", &m_mandel_max_iterations, 50, 5000);
							ImGui::EndDisabled();
						}
						else {
							ImGui::SliderInt("Max Iterations", &m_mandel_max_iterations, 50, 5000);
						}
						ImGui::Separator();
						ImGui::Checkbox("Adaptive Iterations", &m_adaptive_iterations);
						ImGui::SliderInt("Base Iterations", &m_base_iterations, 50, 1000);
						ImGui::TextWrapped(("Current Adaptive Iterations: " + std::to_string(final_adaptive_iterations)).c_str());
						if (ImGui::IsItemHovered()) {
							ImGui::SetTooltip("Max iterations will increase with zoom when 'Adaptive' is checked.");
						}

						if (ImGui::Button("Reset View")) {
							m_mandel_zoom = 1.0;
							m_mandel_center_x = 0.0;
							m_mandel_center_y = 0.0;
							m_mandel_max_iterations = 200;
						}
					}
					else if (m_currentFractal == FractalType::MULTIBROT)
					{
						ImGui::Text("Multibrot Controls");
						ImGui::Separator();

						ImGui::Text("Multibrot Set Fractal Color Palette");
						ImGui::ColorEdit3("Brightness (a)", (float*)&m_palette_multibrot_a);
						ImGui::ColorEdit3("Contrast (b)", (float*)&m_palette_multibrot_b);
						ImGui::SliderFloat3("Frequency (c)", (float*)&m_palette_multibrot_c, 0.0f, 2.0f);
						ImGui::SliderFloat3("Phase (d)", (float*)&m_palette_multibrot_d, 0.0f, 1.0f);
						ImGui::Separator();

						ImGui::InputDouble("Zoom", &m_mandel_zoom, 0.1, 0.0, "%.8f");
						ImGui::InputDouble("Center X", &m_mandel_center_x, 0.01, 0.0, "%.8f");
						ImGui::InputDouble("Center Y", &m_mandel_center_y, 0.01, 0.0, "%.8f");
						ImGui::InputDouble("Power", &m_multibrot_power, -10.0f, 10.0f);

						if (m_adaptive_iterations) {
							ImGui::BeginDisabled();
							ImGui::SliderInt("Max Iterations", &m_mandel_max_iterations, 50, 5000);
							ImGui::EndDisabled();
						}
						else {
							ImGui::SliderInt("Max Iterations", &m_mandel_max_iterations, 50, 5000);
						}
						ImGui::Separator();
						ImGui::Checkbox("Adaptive Iterations", &m_adaptive_iterations);
						ImGui::SliderInt("Base Iterations", &m_base_iterations, 50, 1000);
						ImGui::TextWrapped(("Current Adaptive Iterations: " + std::to_string(final_adaptive_iterations)).c_str());
						if (ImGui::IsItemHovered()) {
							ImGui::SetTooltip("Max iterations will increase with zoom when 'Adaptive' is checked.");
						}

						if (ImGui::Button("Reset View")) {
							m_mandel_zoom = 1.0;
							m_mandel_center_x = -0.75;
							m_mandel_center_y = 0.0;
							m_mandel_max_iterations = 200;
						}
					}
					else if (m_currentFractal == FractalType::NOVA) 
					{
						ImGui::Text("Nova Controls");
						ImGui::Separator();
						
						ImGui::Text("Nova Set Fractal Color Palette");
						ImGui::ColorEdit3("Brightness (a)", (float*)&m_palette_nova_a);
						ImGui::ColorEdit3("Contrast (b)", (float*)&m_palette_nova_b);
						ImGui::SliderFloat3("Frequency (c)", (float*)&m_palette_nova_c, 0.0f, 2.0f);
						ImGui::SliderFloat3("Phase (d)", (float*)&m_palette_nova_d, 0.0f, 1.0f);
						ImGui::Separator();

						ImGui::InputDouble("Zoom", &m_mandel_zoom, 0.1, 0.0, "%.8f");
						ImGui::InputDouble("Center X", &m_mandel_center_x, 0.01, 0.0, "%.8f");
						ImGui::InputDouble("Center Y", &m_mandel_center_y, 0.01, 0.0, "%.8f");
						ImGui::SliderFloat("Power", (float*)&m_nova_power, 1.0f, 10.0f);
						ImGui::SliderFloat("Relaxation", (float*)&m_nova_relaxation, 0.1f, 2.0f);
						
						if (m_adaptive_iterations) {
							ImGui::BeginDisabled();
							ImGui::SliderInt("Max Iterations", &m_mandel_max_iterations, 50, 5000);
							ImGui::EndDisabled();
						}
						else {
							ImGui::SliderInt("Max Iterations", &m_mandel_max_iterations, 50, 5000);
						}
						ImGui::Separator();
						ImGui::Checkbox("Adaptive Iterations", &m_adaptive_iterations);
						ImGui::SliderInt("Base Iterations", &m_base_iterations, 50, 1000);
						ImGui::TextWrapped(("Current Adaptive Iterations: " + std::to_string(final_adaptive_iterations)).c_str());
						if (ImGui::IsItemHovered()) {
							ImGui::SetTooltip("Max iterations will increase with zoom when 'Adaptive' is checked.");
						}

						if (ImGui::Button("Reset View")) {
							m_mandel_zoom = 1.0;
							m_mandel_center_x = -0.75;
							m_mandel_center_y = 0.0;
							m_mandel_max_iterations = 200;
							m_nova_power = 3.0f;
							m_nova_relaxation = 1.0f;
						}
					}
					else if (m_currentFractal == FractalType::SPIDER)
					{
						ImGui::Text("Spider Controls");
						ImGui::Separator();

						ImGui::Text("Spider Set Fractal Color Palette");
						ImGui::ColorEdit3("Brightness (a)", (float*)&m_palette_spider_a);
						ImGui::ColorEdit3("Contrast (b)", (float*)&m_palette_spider_b);
						ImGui::SliderFloat3("Frequency (c)", (float*)&m_palette_spider_c, 0.0f, 2.0f);
						ImGui::SliderFloat3("Phase (d)", (float*)&m_palette_spider_d, 0.0f, 1.0f);
						ImGui::Separator();

						ImGui::InputDouble("Zoom", &m_mandel_zoom, 0.1, 0.0, "%.8f");
						ImGui::InputDouble("Center X", &m_mandel_center_x, 0.01, 0.0, "%.8f");
						ImGui::InputDouble("Center Y", &m_mandel_center_y, 0.01, 0.0, "%.8f");

						if (m_adaptive_iterations) {
							ImGui::BeginDisabled();
							ImGui::SliderInt("Max Iterations", &m_mandel_max_iterations, 50, 5000);
							ImGui::EndDisabled();
						}
						else {
							ImGui::SliderInt("Max Iterations", &m_mandel_max_iterations, 50, 5000);
						}
						ImGui::Separator();
						ImGui::Checkbox("Adaptive Iterations", &m_adaptive_iterations);
						ImGui::SliderInt("Base Iterations", &m_base_iterations, 50, 1000);
						ImGui::TextWrapped(("Current Adaptive Iterations: " + std::to_string(final_adaptive_iterations)).c_str());
						if (ImGui::IsItemHovered()) {
							ImGui::SetTooltip("Max iterations will increase with zoom when 'Adaptive' is checked.");
						}

						if (ImGui::Button("Reset View")) {
							m_mandel_zoom = 1.0;
							m_mandel_center_x = -0.75;
							m_mandel_center_y = 0.0;
							m_mandel_max_iterations = 200;
						}
					}
					else
					{
						ImGui::Text("No fractal selected.");
					}
				}
				ImGui::Checkbox("Toggle info window", &info_gui_window_toggle);
				ImGui::Checkbox("Toggle pre-rendering system (EXPERIMENTAL - MAY CAUSE CRASHES)", &m_pre_render_enabled);
				// pre rendering options -->
				if (m_currentFractal == FractalType::NONE)
				{
					if (ImGui::CollapsingHeader("Pre-Rendering Parameters"))
					{
						if (m_pre_render_enabled) {
							ImGui::InputDouble("Pre-Render Zoom", &m_pre_render_zoom_threshold, 0.1, 0.0, "%.8f");
							ImGui::InputDouble("Pre-Render Center X", &m_pre_render_center_x, 0.01, 0.0, "%.8f");
							ImGui::InputDouble("Pre-Render Center Y", &m_pre_render_center_y, 0.01, 0.0, "%.8f");
							ImGui::TextWrapped("Adjust the settings in the fractal display settings tab to change color and color density");
							ImGui::Separator();
							ImGui::TextWrapped("Fractal Specific Parameter Customization");
							ImGui::InputDouble("Pre-Render Julia c (real)", &m_pre_render_julia_c_x, 0.001, 0.0, "%.8f");
							ImGui::InputDouble("Pre-Render Julia c (imag)", &m_pre_render_julia_c_y, 0.001, 0.0, "%.8f");
							ImGui::InputDouble("Pre-Render Phoenix c (real)", &m_pre_render_phoenix_c_x, 0.01, 0.0, "%.4f");
							ImGui::InputDouble("Pre-Render Phoenix c (imag)", &m_pre_render_phoenix_c_y, 0.01, 0.0, "%.4f");
							ImGui::InputDouble("Pre-Render Phoenix p", &m_pre_render_phoenix_p, 0.01, 0.0, "%.4f");
							ImGui::InputDouble("Pre-Render Multibrot Power", &m_pre_render_multibrot_power, -10.0f, 10.0f);
							ImGui::InputDouble("Pre-Render Nova Power", &m_pre_render_nova_power, 1.0f, 10.0f);
							ImGui::InputDouble("Pre-Render Nova Relaxation", &m_pre_render_nova_relaxation, 0.1f, 2.0f);
							ImGui::Separator();
							ImGui::Checkbox("Use Pre-Render Settings", &m_use_pre_render_params);
							ImGui::Separator();
							ImGui::PushTextWrapPos(0.0f);
							ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Don't use pre-rendering if your dont have a powerful GPU with a good amount of VRAM (~6GB)");
							ImGui::PopTextWrapPos();
						}
						else {
							ImGui::Text("Enable Pre-Rendering to adjust these settings.");
						}
					}
				}
				ImGui::End();
			}

			// info GUI window
			if (info_gui_window_toggle && hud_toggle)
			{
				ImGui::Begin("Information");
				ImGui::Text(app_metadata["description"].c_str());
				ImGui::Separator();
				ImGui::Text(app_metadata["author_desc"].c_str());
				ImGui::Text(app_metadata["version"].c_str());
				ImGui::Text(app_metadata["build"].c_str());
				ImGui::Text(app_metadata["license"].c_str());
				ImGui::Separator();
				ImGui::Text("Controls:");
				ImGui::BulletText(app_metadata["control_one"].c_str());
				ImGui::BulletText(app_metadata["control_two"].c_str());
				ImGui::BulletText(app_metadata["control_three"].c_str());
				ImGui::BulletText(app_metadata["control_four"].c_str());
				ImGui::BulletText(app_metadata["control_five"].c_str());
				ImGui::BulletText(app_metadata["control_six"].c_str());
				ImGui::Separator();
				ImGui::PushTextWrapPos(0.0f);
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), app_metadata["warning_one"].c_str());
				ImGui::PopTextWrapPos();
				ImGui::PushTextWrapPos(0.0f);
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), app_metadata["warning_two"].c_str());
				ImGui::PopTextWrapPos();
				ImGui::PushTextWrapPos(0.0f);
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), app_metadata["warning_three"].c_str());
				ImGui::PopTextWrapPos();
				ImGui::Separator();
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s", status);
				ImGui::End();
			}

			//---------------------------------- control panel ---------------------------------- (end)
			// Main Buttons

			if (show_main_buttons) {
				const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
				ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + screenWidth / 2 - 270, main_viewport->WorkPos.y + screenHeight / 2 - 100));
				ImGui::SetNextWindowSize(ImVec2(550, 150));
				ImGui::Begin("Main Buttons", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);
				ImGui::PushFont(m_font_large);
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 20.0f));
				ImGui::Spacing();
				ImGui::Spacing();
				float button_width = 250.0f;
				float spacing = 30.0f;
				float total_width = (button_width * 2) + spacing;
				ImGui::SetCursorPosX((ImGui::GetWindowSize().x - total_width) * 0.5f);
				if (ImGui::Button("Explore", ImVec2(button_width, 80))) {
					show_fractal_selection = !show_fractal_selection;
					spdlog::info("'Explore Fractals' button clicked!");
				}
				ImGui::SameLine(0.0f, spacing);
				if (ImGui::Button("Quit", ImVec2(button_width, 80))) {
					done = true;
				}
				ImGui::PopStyleVar();
				ImGui::PopFont();
				ImGui::End();
			}
			// main fractal buttons ------------------------------------------------------------------
			if (show_fractal_selection)
			{
				const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
				ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + screenWidth / 2 - 330,
					main_viewport->WorkPos.y + screenHeight / 2 + 60));
				ImGui::SetNextWindowSize(ImVec2(680, 300));
				ImGui::Begin("Fractal Selection", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);
				ImGui::PushFont(m_font_large);
				float button_width = 320.0f;
				float spacing = 30.0f;
				float total_width = (button_width * 2) + spacing;

				// --- Row 1: Mandelbrot + Julia ---
				ImGui::SetCursorPosX((ImGui::GetWindowSize().x - total_width) * 0.5f);

				// Mandelbrot
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.3f, 0.8f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.4f, 0.9f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.2f, 0.7f, 1.0f));
				if (ImGui::Button("Mandelbrot", ImVec2(button_width, 80))) {
					spdlog::info("'Mandelbrot' button clicked!");
					m_currentFractal = FractalType::MANDELBROT;
					if (ourShader != nullptr) delete ourShader;
					if (m_pre_render_enabled) {
						ourShader = new Shader("shaders/mandelbrot.vert", "shaders/mandelbrot.frag");
						spdlog::info("Launching pre-render worker for Mandelbrot...");
						m_is_loading = true;
						m_loading_shader = new Shader("shaders/simple.vert", "shaders/loading_screen.frag");
						if (m_pre_render_thread.joinable()) m_pre_render_thread.join();
						m_worker_finished_submission.store(false);
						m_pre_render_thread = std::thread(&Application::preRenderWorker, this);
						m_pre_render_thread.detach();
					}
					else {
						ourShader = new Shader("shaders/mandelbrot.vert", "shaders/mandelbrot.frag");
						spdlog::info("Loaded Mandelbrot shader for real-time rendering.");
					}

					show_fractal_selection = false;
					show_main_buttons = false;
					sub = "Mesmer - Mandelbrot Set";
					title_text_toggle = false;
				}
				ImGui::PopStyleColor(3);

				ImGui::SameLine(0.0f, spacing);

				// Julia
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.2f, 0.8f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.3f, 0.9f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.1f, 0.7f, 1.0f));
				if (ImGui::Button("Julia", ImVec2(button_width, 80))) {
					spdlog::info("'Julia' button clicked!");
					m_currentFractal = FractalType::JULIA;
					if (ourShader != nullptr) delete ourShader;

					if (m_pre_render_enabled) {
						ourShader = new Shader("shaders/julia.vert", "shaders/julia.frag");
						m_is_pre_rendering = true;
						spdlog::info("Loaded (Pre-Render) Julia shader.");
					}
					else
					{
						ourShader = new Shader("shaders/julia.vert", "shaders/julia.frag");
						spdlog::info("Loaded Julia shader.");
					}

					show_fractal_selection = false;
					show_main_buttons = false;
					sub = "Mesmer - Julia Set";
					title_text_toggle = false;
				}
				ImGui::PopStyleColor(3);

				// --- Row 2: Burning Ship + Tricorn ---
				ImGui::NewLine();
				ImGui::SetCursorPosX((ImGui::GetWindowSize().x - total_width) * 0.5f);

				// Burning Ship
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.25f, 0.15f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.35f, 0.20f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.15f, 0.10f, 1.0f));
				if (ImGui::Button("Burning Ship", ImVec2(button_width, 80))) {
					spdlog::info("'Burning Ship' button clicked!");
					m_currentFractal = FractalType::BURNING_SHIP;
					if (ourShader != nullptr) delete ourShader;

					if (m_pre_render_enabled)
					{
						ourShader = new Shader("shaders/burningship.vert", "shaders/burningship.frag");
						m_is_pre_rendering = true;
						spdlog::info("Loaded (Pre-Render) Burning Ship shader.");
					}
					else 
					{
						ourShader = new Shader("shaders/burningship.vert", "shaders/burningship.frag");
						spdlog::info("Loaded Burning Ship shader.");
					}

					show_fractal_selection = false;
					show_main_buttons = false;
					sub = "Mesmer - Burning Ship";
					title_text_toggle = false;

					m_mandel_zoom = 22.0;
					m_mandel_center_x = -1.75;
					m_mandel_center_y = -0.04;
					m_color_density = 0.031f;
				}
				ImGui::PopStyleColor(3);

				ImGui::SameLine(0.0f, spacing);

				// Tricorn
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.5f, 0.6f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.7f, 0.8f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.05f, 0.4f, 0.5f, 1.0f));
				if (ImGui::Button("Tricorn", ImVec2(button_width, 80))) {
					spdlog::info("'Tricorn' button clicked!");
					m_currentFractal = FractalType::TRICORN;
					if (ourShader != nullptr) delete ourShader;

					if (m_pre_render_enabled) {
						ourShader = new Shader("shaders/tricorn.vert", "shaders/tricorn.frag");
						m_is_pre_rendering = true;
						spdlog::info("Loaded (Pre-Render) Tricorn shader.");
					}
					else {
						ourShader = new Shader("shaders/tricorn.vert", "shaders/tricorn.frag");
						spdlog::info("Loaded Tricorn shader.");
					}

					show_fractal_selection = false;
					show_main_buttons = false;
					sub = "Mesmer - Tricorn";
					title_text_toggle = false;

					m_mandel_zoom = 0.5;
					m_mandel_center_x = 0.0;
					m_mandel_center_y = 0.0;
				}
				ImGui::PopStyleColor(3);

				// phoenix (row - 3)
				ImGui::NewLine();
				ImGui::SetCursorPosX((ImGui::GetWindowSize().x - total_width) * 0.5f);

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.4f, 0.1f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.7f, 0.5f, 0.2f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.3f, 0.05f, 1.0f));
				if (ImGui::Button("Phoenix", ImVec2(button_width, 80))) {
					spdlog::info("'Phoenix' button clicked!");
					m_currentFractal = FractalType::PHOENIX;
					if (ourShader != nullptr) delete ourShader;

					if (m_pre_render_enabled) {
						ourShader = new Shader("shaders/phoenix.vert", "shaders/phoenix.frag");
						m_is_pre_rendering = true;
						spdlog::info("Loaded (Pre-Render) Phoenix shader.");
					}
					else {
						ourShader = new Shader("shaders/phoenix.vert", "shaders/phoenix.frag");
						spdlog::info("Loaded Phoenix shader.");
					}

					show_fractal_selection = false;
					show_main_buttons = false;
					sub = "Mesmer - Phoenix";
					title_text_toggle = false;

					m_mandel_zoom = 0.5;
					m_mandel_center_x = 0.0;
					m_mandel_center_y = 0.0;
				}
				ImGui::PopStyleColor(3);

				// lyapunov (row - 3)
				ImGui::SameLine(0.0f, spacing);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.6f, 0.2f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.7f, 0.3f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.5f, 0.1f, 1.0f));
				if (ImGui::Button("Lyapunov", ImVec2(button_width, 80))) {
					spdlog::info("'Lyapunov' button clicked!");
					m_currentFractal = FractalType::LYAPUNOV;
					if (ourShader != nullptr) delete ourShader;

					if (m_pre_render_enabled) {
						ourShader = new Shader("shaders/lyapunov.vert", "shaders/lyapunov.frag");
						m_is_pre_rendering = true;
						spdlog::info("Loaded (Pre-Render) Lyapunov shader.");
					}
					else {
						ourShader = new Shader("shaders/lyapunov.vert", "shaders/lyapunov.frag");
						spdlog::info("Loaded Lyapunov shader.");
					}

					show_fractal_selection = false;
					show_main_buttons = false;
					sub = "Mesmer - Lyapunov";
					title_text_toggle = false;
					
					m_mandel_zoom = 0.5;
					m_mandel_center_x = 0.0;
					m_mandel_center_y = 0.0;
					m_lyapunov_zoom = 1.10000000;
					m_lyapunov_center_a = 2.76486681;
					m_lyapunov_center_b = 4.05390855;
				}
				ImGui::PopStyleColor(3);

				// newton (row - 4 left side)
				ImGui::NewLine();
				ImGui::SetCursorPosX((ImGui::GetWindowSize().x - total_width) * 0.5f);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.3f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.4f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.6f, 0.2f, 1.0f));
				if (ImGui::Button("Newton", ImVec2(button_width, 80))) {
					spdlog::info("'Newton' button clicked!");
					m_currentFractal = FractalType::NEWTON;
					if (ourShader != nullptr) delete ourShader;

					if (m_pre_render_enabled) {
						ourShader = new Shader("shaders/newton.vert", "shaders/newton.frag");
						m_is_pre_rendering = true;
						spdlog::info("Loaded (Pre-Render) Newton shader.");
					}
					else {
						ourShader = new Shader("shaders/newton.vert", "shaders/newton.frag");
						spdlog::info("Loaded Newton shader.");
					}

					show_fractal_selection = false;
					show_main_buttons = false;
					sub = "Mesmer - Newton";
					title_text_toggle = false;

					m_mandel_zoom = 1.0;
					m_mandel_center_x = 0.0;
					m_mandel_center_y = 0.0;
				}
				ImGui::PopStyleColor(3);

				// multibrot (row - 4 right side)
				ImGui::SameLine(0.0f, spacing);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.3f, 0.7f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.4f, 0.8f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.2f, 0.6f, 1.0f));
				if (ImGui::Button("Multibrot", ImVec2(button_width, 80))) {
					spdlog::info("'Multibrot' button clicked!");
					m_currentFractal = FractalType::MULTIBROT;
					if (ourShader != nullptr) delete ourShader;

					if (m_pre_render_enabled) {
						ourShader = new Shader("shaders/multibrot.vert", "shaders/multibrot.frag");
						m_is_pre_rendering = true;
						spdlog::info("Loaded (Pre-Render) Multibrot shader.");
					}
					else {
						ourShader = new Shader("shaders/multibrot.vert", "shaders/multibrot.frag");
						spdlog::info("Loaded Multibrot shader.");
					}

					show_fractal_selection = false;
					show_main_buttons = false;
					sub = "Mesmer - Multibrot";
					title_text_toggle = false;

					m_mandel_zoom = 1.0;
					m_mandel_center_x = -0.75;
					m_mandel_center_y = 0.0;
					m_multibrot_power = 3.0;
				}
				ImGui::PopStyleColor(3);

				// nova (row - 5 left side)
				ImGui::NewLine();
				ImGui::SetCursorPosX((ImGui::GetWindowSize().x - total_width) * 0.5f);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.2f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.6f, 0.3f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.4f, 0.1f, 1.0f));
				if (ImGui::Button("Nova", ImVec2(button_width, 80))) {
					spdlog::info("'Nova' button clicked!");
					m_currentFractal = FractalType::NOVA;
					if (ourShader != nullptr) delete ourShader;

					if (m_pre_render_enabled) {
						ourShader = new Shader("shaders/nova.vert", "shaders/nova.frag");
						m_is_pre_rendering = true;
						spdlog::info("Loaded (Pre-Render) Nova shader.");
					}
					else {
						ourShader = new Shader("shaders/nova.vert", "shaders/nova.frag");
						spdlog::info("Loaded Nova shader.");
					}

					show_fractal_selection = false;
					show_main_buttons = false;
					sub = "Mesmer - Nova";
					title_text_toggle = false;

					m_mandel_zoom = 1.0;
					m_mandel_center_x = 0.0;
					m_mandel_center_y = 0.0;
					m_nova_power = 3.0;
					m_nova_relaxation = 1.0;
				}
				ImGui::PopStyleColor(3);

				// spider (row - 5 right side)
				ImGui::SameLine(0.0f, spacing);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
				if (ImGui::Button("Spider", ImVec2(button_width, 80))) {
					spdlog::info("'Spider' button clicked!");
					m_currentFractal = FractalType::SPIDER;
					if (ourShader != nullptr) delete ourShader;

					if (m_pre_render_enabled) {
						ourShader = new Shader("shaders/spider.vert", "shaders/spider.frag");
						m_is_pre_rendering = true;
						spdlog::info("Loaded (Pre-Render) Spider shader.");
					}
					else {
						ourShader = new Shader("shaders/spider.vert", "shaders/spider.frag");
						spdlog::info("Loaded Spider shader.");
					}

					show_fractal_selection = false;
					show_main_buttons = false;
					sub = "Mesmer - Spider";
					title_text_toggle = false;

					m_mandel_zoom = 1.0;
					m_mandel_center_x = -0.5;
					m_mandel_center_y = 0.0;
					// to do
				}
				ImGui::PopStyleColor(3);
				// to add more fractals

				ImGui::PopFont();
				ImGui::End();
				// ---------------------------------- Main Buttons ---------------------------------- (end)
			}
			// ---------------------------------- HUD / Text Overlay ----------------------------------
				// ImGui::GetBackgroundDrawList() to draw behind everything
				// ImGui::GetForegroundDrawList() to draw in front of everything


			ImGuiIO& io = ImGui::GetIO();
			ImDrawList* draw_list = ImGui::GetBackgroundDrawList();

			if (hud_toggle) {
				draw_list->AddText(
					ImVec2(10, 10),
					IM_COL32(255, 255, 0, 255),
					sub
				);
			}
			if (title_text_toggle) {
				addTextWithStroke(
					draw_list,
					m_font_large,
					68.0f,
					ImVec2((float)screenWidth / 2 - 140, 20),
					IM_COL32(255, 255, 255, 255),
					IM_COL32(0, 0, 0, 255),
					4.0f,
					"MESMER"
				);
			}
			else {
				if (hud_toggle) {
					addTextWithStroke(
						draw_list,
						m_font_large,
						20.0f,
						ImVec2((float)screenWidth / 2 - 170, 20),
						IM_COL32(255, 255, 255, 255),
						IM_COL32(0, 0, 0, 255),
						2.0f,
						"Press Space Bar to exit to main menu"
					);
				}
			}
			
			if (hud_toggle) {
				draw_list->AddText(
					ImVec2((float)screenWidth - 200, 10),
					IM_COL32(0, 255, 0, 255),
					status
				);
			}

			if (show_demo_window) {
				ImGui::ShowDemoWindow(&show_demo_window);
			}

			int drawable_w, drawable_h;
			SDL_GL_GetDrawableSize(window, &drawable_w, &drawable_h);

			if (m_is_loading) {
				glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);

				// Render the animated loading screen background
				m_loading_shader->use();
				m_loading_shader->setVec2("iResolution", (float)drawable_w, (float)drawable_h);
				m_loading_shader->setFloat("iTime", SDL_GetTicks() / 1000.0f);
				glBindVertexArray(VAO);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

				ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
				addTextWithStroke(draw_list, m_font_large, 48.0f, ImVec2((float)screenWidth / 2 - 250.0f, (float)screenHeight / 2 - 80.0f), IM_COL32_WHITE, IM_COL32_BLACK, 2.0f, "Rendering, please wait...");

				if (m_worker_finished_submission && m_pre_render_fence) {
					GLenum wait_result = glClientWaitSync(m_pre_render_fence, 0, 0);
					if (wait_result == GL_ALREADY_SIGNALED || wait_result == GL_CONDITION_SATISFIED) {
						spdlog::info("Main thread: Fence signaled! GPU render is complete.");

						glBindTexture(GL_TEXTURE_2D, m_pre_render_texture);
						glGenerateMipmap(GL_TEXTURE_2D);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
						glDeleteSync(m_pre_render_fence);
						m_pre_render_fence = nullptr;

						m_texture_view_shader = new Shader("shaders/simple.vert", "shaders/texture_view.frag");
						m_pre_render_complete = true;
						m_is_loading = false;

						delete m_loading_shader; m_loading_shader = nullptr;
					}
				}
			}
			else if (m_pre_render_complete) {
				glViewport(0, 0, drawable_w, drawable_h);
				glClearColor(clear_color.x* clear_color.w, clear_color.y* clear_color.w, clear_color.z* clear_color.w, clear_color.w);
				glClear(GL_COLOR_BUFFER_BIT);

				m_texture_view_shader->use();
				m_texture_view_shader->setVec2("iResolution", (float)drawable_w, (float)drawable_h);
				m_texture_view_shader->setDVec2("u_center", m_view_center_x, m_view_center_y);
				m_texture_view_shader->setDouble("u_zoom", m_view_zoom);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, m_pre_render_texture);

				glBindVertexArray(VAO);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}
			else {
				glViewport(0, 0, drawable_w, drawable_h);
				glClearColor(clear_color.x* clear_color.w, clear_color.y* clear_color.w, clear_color.z* clear_color.w, clear_color.w);
				glClear(GL_COLOR_BUFFER_BIT);

				ourShader->use();
				ourShader->setFloat("iTime", SDL_GetTicks() / 1000.0f);
				ourShader->setVec2("iResolution", (float)drawable_w, (float)drawable_h);
				ourShader->setVec3("iColorOne", m_menu_bg_color_one.x, m_menu_bg_color_one.y, m_menu_bg_color_one.z);
				ourShader->setVec3("iColorTwo", m_menu_bg_color_two.x, m_menu_bg_color_two.y, m_menu_bg_color_two.z);
				ourShader->setVec3("iAccentColor", m_menu_bg_accent_color.x, m_menu_bg_accent_color.y, m_menu_bg_accent_color.z);
				ourShader->setFloat("u_color_density", m_color_density);
				if (m_apply_common_color_palette) {
					ourShader->setVec3("u_palette_a", m_palette_a.x, m_palette_a.y, m_palette_a.z);
					ourShader->setVec3("u_palette_b", m_palette_b.x, m_palette_b.y, m_palette_b.z);
					ourShader->setVec3("u_palette_c", m_palette_c.x, m_palette_c.y, m_palette_c.z);
					ourShader->setVec3("u_palette_d", m_palette_d.x, m_palette_d.y, m_palette_d.z);
				}
				if (m_currentFractal == FractalType::MANDELBROT) {
					ourShader->setDVec2("u_center", m_mandel_center_x, m_mandel_center_y);
					ourShader->setDouble("u_zoom", m_mandel_zoom);
					if (m_adaptive_iterations) {
						int final_iterations = m_base_iterations;
						if (m_adaptive_iterations && m_mandel_zoom > 1.0) {
							final_iterations += static_cast<int>(150.0 * log(m_mandel_zoom));
						}
						final_adaptive_iterations = final_iterations;
						ourShader->setInt("u_max_iterations", final_iterations);
					}
					else {
						ourShader->setInt("u_max_iterations", m_mandel_max_iterations);
					}
					if (!m_apply_common_color_palette) {
						ourShader->setVec3("u_palette_a", m_palette_mandelbrot_a.x, m_palette_mandelbrot_a.y, m_palette_mandelbrot_a.z);
						ourShader->setVec3("u_palette_b", m_palette_mandelbrot_b.x, m_palette_mandelbrot_b.y, m_palette_mandelbrot_b.z);
						ourShader->setVec3("u_palette_c", m_palette_mandelbrot_c.x, m_palette_mandelbrot_c.y, m_palette_mandelbrot_c.z);
						ourShader->setVec3("u_palette_d", m_palette_mandelbrot_d.x, m_palette_mandelbrot_d.y, m_palette_mandelbrot_d.z);
					}
				}
				else if (m_currentFractal == FractalType::JULIA)
				{
					ourShader->setDVec2("u_center", m_mandel_center_x, m_mandel_center_y);
					ourShader->setDouble("u_zoom", m_mandel_zoom);
					ourShader->setInt("u_max_iterations", m_mandel_max_iterations);
					ourShader->setDVec2("u_julia_c", m_julia_c_x, m_julia_c_y);
					if (m_adaptive_iterations) {
						int final_iterations = m_base_iterations;
						if (m_adaptive_iterations && m_mandel_zoom > 1.0) {
							final_iterations += static_cast<int>(150.0 * log(m_mandel_zoom));
						}
						final_adaptive_iterations = final_iterations;
						ourShader->setInt("u_max_iterations", final_iterations);
					}
					else {
						ourShader->setInt("u_max_iterations", m_mandel_max_iterations);
					}
					if (!m_apply_common_color_palette) {
						ourShader->setVec3("u_palette_a", m_palette_julia_a.x, m_palette_julia_a.y, m_palette_julia_a.z);
						ourShader->setVec3("u_palette_b", m_palette_julia_b.x, m_palette_julia_b.y, m_palette_julia_b.z);
						ourShader->setVec3("u_palette_c", m_palette_julia_c.x, m_palette_julia_c.y, m_palette_julia_c.z);
						ourShader->setVec3("u_palette_d", m_palette_julia_d.x, m_palette_julia_d.y, m_palette_julia_d.z);
					}
				}
				else if (m_currentFractal == FractalType::BURNING_SHIP)
				{
					ourShader->setDVec2("u_center", m_mandel_center_x, m_mandel_center_y);
					ourShader->setDouble("u_zoom", m_mandel_zoom);
					ourShader->setInt("u_max_iterations", m_mandel_max_iterations);
					if (m_adaptive_iterations) {
						int final_iterations = m_base_iterations;
						if (m_adaptive_iterations && m_mandel_zoom > 1.0) {
							final_iterations += static_cast<int>(150.0 * log(m_mandel_zoom));
						}
						final_adaptive_iterations = final_iterations;
						ourShader->setInt("u_max_iterations", final_iterations);
					}
					else {
						ourShader->setInt("u_max_iterations", m_mandel_max_iterations);
					}
					if (!m_apply_common_color_palette) {
						ourShader->setVec3("u_palette_a", m_palette_burning_ship_a.x, m_palette_burning_ship_a.y, m_palette_burning_ship_a.z);
						ourShader->setVec3("u_palette_b", m_palette_burning_ship_b.x, m_palette_burning_ship_b.y, m_palette_burning_ship_b.z);
						ourShader->setVec3("u_palette_c", m_palette_burning_ship_c.x, m_palette_burning_ship_c.y, m_palette_burning_ship_c.z);
						ourShader->setVec3("u_palette_d", m_palette_burning_ship_d.x, m_palette_burning_ship_d.y, m_palette_burning_ship_d.z);
					}
				}
				else if (m_currentFractal == FractalType::TRICORN)
				{
					ourShader->setDVec2("u_center", m_mandel_center_x, m_mandel_center_y);
					ourShader->setDouble("u_zoom", m_mandel_zoom);
					ourShader->setInt("u_max_iterations", m_mandel_max_iterations);
					if (m_adaptive_iterations) {
						int final_iterations = m_base_iterations;
						if (m_adaptive_iterations && m_mandel_zoom > 1.0) {
							final_iterations += static_cast<int>(150.0 * log(m_mandel_zoom));
						}
						final_adaptive_iterations = final_iterations;
						ourShader->setInt("u_max_iterations", final_iterations);
					}
					else {
						ourShader->setInt("u_max_iterations", m_mandel_max_iterations);
					}
					if (!m_apply_common_color_palette) {
						ourShader->setVec3("u_palette_a", m_palette_tricorn_a.x, m_palette_tricorn_a.y, m_palette_tricorn_a.z);
						ourShader->setVec3("u_palette_b", m_palette_tricorn_b.x, m_palette_tricorn_b.y, m_palette_tricorn_b.z);
						ourShader->setVec3("u_palette_c", m_palette_tricorn_c.x, m_palette_tricorn_c.y, m_palette_tricorn_c.z);
						ourShader->setVec3("u_palette_d", m_palette_tricorn_d.x, m_palette_tricorn_d.y, m_palette_tricorn_d.z);
					}
				}
				else if (m_currentFractal == FractalType::PHOENIX)
				{
					ourShader->setDVec2("u_center", m_mandel_center_x, m_mandel_center_y);
					ourShader->setDouble("u_zoom", m_mandel_zoom);
					ourShader->setDVec2("u_phoenix_c", m_phoenix_c_x, m_phoenix_c_y);
					ourShader->setDouble("u_phoenix_p", m_phoenix_p);
					ourShader->setInt("u_max_iterations", m_mandel_max_iterations);
					if (m_adaptive_iterations) {
						int final_iterations = m_base_iterations;
						if (m_adaptive_iterations && m_mandel_zoom > 1.0) {
							final_iterations += static_cast<int>(150.0 * log(m_mandel_zoom));
						}
						final_adaptive_iterations = final_iterations;
						ourShader->setInt("u_max_iterations", final_iterations);
					}
					else {
						ourShader->setInt("u_max_iterations", m_mandel_max_iterations);
					}
					if (!m_apply_common_color_palette) {
						ourShader->setVec3("u_palette_a", m_palette_phoenix_a.x, m_palette_phoenix_a.y, m_palette_phoenix_a.z);
						ourShader->setVec3("u_palette_b", m_palette_phoenix_b.x, m_palette_phoenix_b.y, m_palette_phoenix_b.z);
						ourShader->setVec3("u_palette_c", m_palette_phoenix_c.x, m_palette_phoenix_c.y, m_palette_phoenix_c.z);
						ourShader->setVec3("u_palette_d", m_palette_phoenix_d.x, m_palette_phoenix_d.y, m_palette_phoenix_d.z);
					}

				}
				else if (m_currentFractal == FractalType::LYAPUNOV)
				{
					ourShader->setDVec2("u_lyapunov_center", m_lyapunov_center_a, m_lyapunov_center_b);
					ourShader->setDouble("u_lyapunov_zoom", m_lyapunov_zoom);
					ourShader->setInt("u_max_iterations", m_mandel_max_iterations);
					if (m_adaptive_iterations) {
						int final_iterations = m_base_iterations;
						if (m_adaptive_iterations && m_lyapunov_zoom > 1.0) {
							final_iterations += static_cast<int>(500.0 * log(m_lyapunov_zoom));
						}
						final_adaptive_iterations = final_iterations;
						ourShader->setInt("u_max_iterations", final_iterations);
					}
					else {
						ourShader->setInt("u_max_iterations", m_mandel_max_iterations);
					}
					if (!m_apply_common_color_palette) {
						ourShader->setVec3("u_palette_a", m_palette_lyapunov_a.x, m_palette_lyapunov_a.y, m_palette_lyapunov_a.z);
						ourShader->setVec3("u_palette_b", m_palette_lyapunov_b.x, m_palette_lyapunov_b.y, m_palette_lyapunov_b.z);
						ourShader->setVec3("u_palette_c", m_palette_lyapunov_c.x, m_palette_lyapunov_c.y, m_palette_lyapunov_c.z);
						ourShader->setVec3("u_palette_d", m_palette_lyapunov_d.x, m_palette_lyapunov_d.y, m_palette_lyapunov_d.z);
					}
				}
				else if (m_currentFractal == FractalType::NEWTON)
				{
					ourShader->setDVec2("u_center", m_mandel_center_x, m_mandel_center_y);
					ourShader->setDouble("u_zoom", m_mandel_zoom);
					ourShader->setInt("u_max_iterations", m_mandel_max_iterations);
					if (m_adaptive_iterations) {
						int final_iterations = m_base_iterations;
						if (m_adaptive_iterations && m_mandel_zoom > 1.0) {
							final_iterations += static_cast<int>(150.0 * log(m_mandel_zoom));
						}
						final_adaptive_iterations = final_iterations;
						ourShader->setInt("u_max_iterations", final_iterations);
					}
					else {
						ourShader->setInt("u_max_iterations", m_mandel_max_iterations);
					}
					if (!m_apply_common_color_palette) {
						ourShader->setVec3("u_palette_a", m_palette_newton_a.x, m_palette_newton_a.y, m_palette_newton_a.z);
						ourShader->setVec3("u_palette_b", m_palette_newton_b.x, m_palette_newton_b.y, m_palette_newton_b.z);
						ourShader->setVec3("u_palette_c", m_palette_newton_c.x, m_palette_newton_c.y, m_palette_newton_c.z);
						ourShader->setVec3("u_palette_d", m_palette_newton_d.x, m_palette_newton_d.y, m_palette_newton_d.z);
					}
				}
				else if (m_currentFractal == FractalType::MULTIBROT)
				{
					ourShader->setDVec2("u_center", m_mandel_center_x, m_mandel_center_y);
					ourShader->setDouble("u_zoom", m_mandel_zoom);
					ourShader->setDouble("u_power", m_multibrot_power);
					if (m_adaptive_iterations) {
						int final_iterations = m_base_iterations;
						if (m_adaptive_iterations && m_mandel_zoom > 1.0) {
							final_iterations += static_cast<int>(150.0 * log(m_mandel_zoom));
						}
						final_adaptive_iterations = final_iterations;
						ourShader->setInt("u_max_iterations", final_iterations);
					}
					else {
						ourShader->setInt("u_max_iterations", m_mandel_max_iterations);
					}
					if (!m_apply_common_color_palette) {
						ourShader->setVec3("u_palette_a", m_palette_mandelbrot_a.x, m_palette_mandelbrot_a.y, m_palette_mandelbrot_a.z);
						ourShader->setVec3("u_palette_b", m_palette_mandelbrot_b.x, m_palette_mandelbrot_b.y, m_palette_mandelbrot_b.z);
						ourShader->setVec3("u_palette_c", m_palette_mandelbrot_c.x, m_palette_mandelbrot_c.y, m_palette_mandelbrot_c.z);
						ourShader->setVec3("u_palette_d", m_palette_mandelbrot_d.x, m_palette_mandelbrot_d.y, m_palette_mandelbrot_d.z);
					}
				}
				else if (m_currentFractal == FractalType::NOVA)
				{
					ourShader->setDVec2("u_center", m_mandel_center_x, m_mandel_center_y);
					ourShader->setDouble("u_zoom", m_mandel_zoom);
					ourShader->setDouble("u_power", m_nova_power);
					ourShader->setDouble("u_relaxation", m_nova_relaxation);
					if (m_adaptive_iterations) {
						int final_iterations = m_base_iterations;
						if (m_adaptive_iterations && m_mandel_zoom > 1.0) {
							final_iterations += static_cast<int>(150.0 * log(m_mandel_zoom));
						}
						final_adaptive_iterations = final_iterations;
						ourShader->setInt("u_max_iterations", final_iterations);
					}
					else {
						ourShader->setInt("u_max_iterations", m_mandel_max_iterations);
					}
					if (!m_apply_common_color_palette) {
						ourShader->setVec3("u_palette_a", m_palette_nova_a.x, m_palette_nova_a.y, m_palette_nova_a.z);
						ourShader->setVec3("u_palette_b", m_palette_nova_b.x, m_palette_nova_b.y, m_palette_nova_b.z);
						ourShader->setVec3("u_palette_c", m_palette_nova_c.x, m_palette_nova_c.y, m_palette_nova_c.z);
						ourShader->setVec3("u_palette_d", m_palette_nova_d.x, m_palette_nova_d.y, m_palette_nova_d.z);
					}
				}
				else if (m_currentFractal == FractalType::SPIDER)
				{
					ourShader->setDVec2("u_center", m_mandel_center_x, m_mandel_center_y);
					ourShader->setDouble("u_zoom", m_mandel_zoom);
					if (m_adaptive_iterations) {
						int final_iterations = m_base_iterations;
						if (m_adaptive_iterations && m_mandel_zoom > 1.0) {
							final_iterations += static_cast<int>(150.0 * log(m_mandel_zoom));
						}
						final_adaptive_iterations = final_iterations;
						ourShader->setInt("u_max_iterations", final_iterations);
					}
					else {
						ourShader->setInt("u_max_iterations", m_mandel_max_iterations);
					}
					if (!m_apply_common_color_palette) {
						ourShader->setVec3("u_palette_a", m_palette_spider_a.x, m_palette_spider_a.y, m_palette_spider_a.z);
						ourShader->setVec3("u_palette_b", m_palette_spider_b.x, m_palette_spider_b.y, m_palette_spider_b.z);
						ourShader->setVec3("u_palette_c", m_palette_spider_c.x, m_palette_spider_c.y, m_palette_spider_c.z);
						ourShader->setVec3("u_palette_d", m_palette_spider_d.x, m_palette_spider_d.y, m_palette_spider_d.z);
					}
				}
				else {
					// no fractal selected
				}

				glBindVertexArray(VAO);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			SDL_GL_SwapWindow(window);
		}
	}
	catch (const std::exception& e) {
		spdlog::error("An exception occurred: {}", e.what());
		cleanup();
	}

	cleanup();
}


void Application::initSDL() {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
		throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());
	}
	spdlog::info("SDL initialized successfully.");
}

void Application::initOpenGL() {
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);

	SDL_DisplayMode current;
	if (SDL_GetCurrentDisplayMode(0, &current) != 0) {
		spdlog::critical("Error getting display mode!");
		throw std::runtime_error(std::string("SDL_GetCurrentDisplayMode Error: ") + SDL_GetError());
	}
	screenWidth = current.w;
	screenHeight = current.h;

	window = SDL_CreateWindow(
		windowTitle,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		screenWidth,
		screenHeight,
		SDL_WINDOW_OPENGL /*| SDL_WINDOW_RESIZABLE*/ | SDL_WINDOW_SHOWN | SDL_WINDOW_MAXIMIZED
	);
	if (!window) {
		throw std::runtime_error("Failed to create SDL window.");
	}
	SDL_SetWindowFullscreen(window, 0);
	//SDL_SetWindowBordered(window, SDL_TRUE);
	gl_context = SDL_GL_CreateContext(window);
	if (!gl_context) {
		throw std::runtime_error(std::string("SDL_GL_CreateContext Error: ") + SDL_GetError());
	}
	m_worker_context = SDL_GL_CreateContext(window);
	if (!m_worker_context) {
		throw std::runtime_error(std::string("SDL_GL_CreateContext Error (worker): ") + SDL_GetError());
	}
	spdlog::info("All OpenGL context created successfully.");
	SDL_GL_MakeCurrent(window, gl_context);
	SDL_GL_SetSwapInterval(1);

	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		throw std::runtime_error("Failed to initialize GLAD.");
	}

	int max_texture_size;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
	spdlog::info("Max texture size supported: {}x{}", max_texture_size, max_texture_size);
	if (m_pre_render_resolution > max_texture_size) {
		spdlog::warn("Requested pre-render resolution ({}) exceeds the maximum supported size ({}).", m_pre_render_resolution, max_texture_size);
		spdlog::warn("Setting pre-render resolution to the maximum supported size.");
		m_pre_render_resolution = max_texture_size;
	}
	else {
		spdlog::info("Pre-render resolution set to {}.", m_pre_render_resolution);
	}

	spdlog::info("OpenGL Vendor: {}", (const char*)glGetString(GL_VENDOR));
	spdlog::info("OpenGL Renderer: {}", (const char*)glGetString(GL_RENDERER));
	spdlog::info("OpenGL Version: {}", (const char*)glGetString(GL_VERSION));
	spdlog::info("GLSL Version: {}", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
	spdlog::info("OpenGL initialized successfully.");
}

void Application::initImGui() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
	ImGui_ImplOpenGL3_Init("#version 460");
	io.Fonts->AddFontDefault();
	m_font_regular = io.Fonts->AddFontFromFileTTF("assets/fonts/cw.otf", 18.0f);
	m_font_large = io.Fonts->AddFontFromFileTTF("assets/fonts/cw.otf", 48.0f);
	spdlog::info("ImGui initialized successfully.");
}

void Application::initBG() {
	ourShader = new Shader("shaders/background.vert", "shaders/background.frag");
	float vertices[] = {
		-1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,  1.0f, 1.0f
	};

	unsigned int indices[] = {
		0, 1, 2,
		0, 2, 3
	};

	//GLuint EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO_vertices);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
	spdlog::info("Screen quad initialized successfully.");
}

void Application::cleanup() {
	delete ourShader;
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO_vertices);
	glDeleteBuffers(1, &EBO);
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	if (gl_context) {
		SDL_GL_DeleteContext(gl_context);
	}
	if (window) {
		SDL_DestroyWindow(window);
	}
	SDL_Quit();
	spdlog::info("Application cleaned up successfully.");
}

void Application::addTextWithStroke(ImDrawList* draw_list, ImFont* font, float size, ImVec2 pos, ImU32 fill_col, ImU32 outline_col, float thickness, const char* text) {
	draw_list->AddText(font, size, ImVec2(pos.x - thickness, pos.y - thickness), outline_col, text);
	draw_list->AddText(font, size, ImVec2(pos.x + thickness, pos.y - thickness), outline_col, text);
	draw_list->AddText(font, size, ImVec2(pos.x - thickness, pos.y + thickness), outline_col, text);
	draw_list->AddText(font, size, ImVec2(pos.x + thickness, pos.y + thickness), outline_col, text);
	draw_list->AddText(font, size, ImVec2(pos.x - thickness, pos.y), outline_col, text);
	draw_list->AddText(font, size, ImVec2(pos.x + thickness, pos.y), outline_col, text);
	draw_list->AddText(font, size, ImVec2(pos.x, pos.y - thickness), outline_col, text);
	draw_list->AddText(font, size, ImVec2(pos.x, pos.y + thickness), outline_col, text);
	draw_list->AddText(font, size, pos, fill_col, text);
}

void Application::performPreRender() {
	spdlog::info("Starting 32K pre-render...");
	spdlog::info("Initializing framebuffers and textures...");
	glGenFramebuffers(1, &m_pre_render_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_pre_render_fbo);
	glGenTextures(1, &m_pre_render_texture);
	glBindTexture(GL_TEXTURE_2D, m_pre_render_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_pre_render_resolution, m_pre_render_resolution, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pre_render_texture, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		spdlog::critical("Framebuffer is not complete!");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteTextures(1, &m_pre_render_texture);
		glDeleteFramebuffers(1, &m_pre_render_fbo);
		m_pre_render_texture = 0;
		m_pre_render_fbo = 0;
		return;
	}
	glViewport(0, 0, m_pre_render_resolution, m_pre_render_resolution);
	glClear(GL_COLOR_BUFFER_BIT);
	spdlog::info("Setting shader uniforms...");
	ourShader->use();
	ourShader->setVec2("iResolution", (float)m_pre_render_resolution, (float)m_pre_render_resolution);
	ourShader->setInt("u_max_iterations", 5000);
	ourShader->setFloat("u_color_density", m_color_density);
	if (m_currentFractal == FractalType::MANDELBROT) {
		if (m_use_pre_render_params) {
			ourShader->setDVec2("u_center", m_pre_render_center_x, m_pre_render_center_y);
			ourShader->setDouble("u_zoom", m_pre_render_zoom_threshold);
		}
		else {
			ourShader->setDVec2("u_center", 0.0, 0.0);
			ourShader->setDouble("u_zoom", 1.0);
		}
		if (!m_apply_common_color_palette) {
			ourShader->setVec3("u_palette_a", m_palette_mandelbrot_a.x, m_palette_mandelbrot_a.y, m_palette_mandelbrot_a.z);
			ourShader->setVec3("u_palette_b", m_palette_mandelbrot_b.x, m_palette_mandelbrot_b.y, m_palette_mandelbrot_b.z);
			ourShader->setVec3("u_palette_c", m_palette_mandelbrot_c.x, m_palette_mandelbrot_c.y, m_palette_mandelbrot_c.z);
			ourShader->setVec3("u_palette_d", m_palette_mandelbrot_d.x, m_palette_mandelbrot_d.y, m_palette_mandelbrot_d.z);
		}
		else {
			ourShader->setVec3("u_palette_a", m_palette_a.x, m_palette_a.y, m_palette_a.z);
			ourShader->setVec3("u_palette_b", m_palette_b.x, m_palette_b.y, m_palette_b.z);
			ourShader->setVec3("u_palette_c", m_palette_c.x, m_palette_c.y, m_palette_c.z);
			ourShader->setVec3("u_palette_d", m_palette_d.x, m_palette_d.y, m_palette_d.z);
		}
	}
	else if (m_currentFractal == FractalType::JULIA) {
		if (m_use_pre_render_params) {
			ourShader->setDVec2("u_center", m_pre_render_center_x, m_pre_render_center_y);
			ourShader->setDouble("u_zoom", m_pre_render_zoom_threshold);
			ourShader->setDVec2("u_julia_c", m_pre_render_julia_c_x, m_pre_render_julia_c_y);
		}
		else {
			ourShader->setDVec2("u_center", 0.0, 0.0);
			ourShader->setDouble("u_zoom", 1.0);
			ourShader->setDVec2("u_julia_c", -0.7, 0.27015);
		}
		if (!m_apply_common_color_palette) {
			ourShader->setVec3("u_palette_a", m_palette_julia_a.x, m_palette_julia_a.y, m_palette_julia_a.z);
			ourShader->setVec3("u_palette_b", m_palette_julia_b.x, m_palette_julia_b.y, m_palette_julia_b.z);
			ourShader->setVec3("u_palette_c", m_palette_julia_c.x, m_palette_julia_c.y, m_palette_julia_c.z);
			ourShader->setVec3("u_palette_d", m_palette_julia_d.x, m_palette_julia_d.y, m_palette_julia_d.z);
		}
		else {
			ourShader->setVec3("u_palette_a", m_palette_a.x, m_palette_a.y, m_palette_a.z);
			ourShader->setVec3("u_palette_b", m_palette_b.x, m_palette_b.y, m_palette_b.z);
			ourShader->setVec3("u_palette_c", m_palette_c.x, m_palette_c.y, m_palette_c.z);
			ourShader->setVec3("u_palette_d", m_palette_d.x, m_palette_d.y, m_palette_d.z);
		}
	}
	else if (m_currentFractal == FractalType::BURNING_SHIP) {
		if (m_use_pre_render_params) {
			ourShader->setDVec2("u_center", m_pre_render_center_x, m_pre_render_center_y);
			ourShader->setDouble("u_zoom", m_pre_render_zoom_threshold);
		}
		else {
			ourShader->setDVec2("u_center", -1.75, -0.04);
			ourShader->setDouble("u_zoom", 22.0);
		}
		if (!m_apply_common_color_palette) {
			ourShader->setVec3("u_palette_a", m_palette_burning_ship_a.x, m_palette_burning_ship_a.y, m_palette_burning_ship_a.z);
			ourShader->setVec3("u_palette_b", m_palette_burning_ship_b.x, m_palette_burning_ship_b.y, m_palette_burning_ship_b.z);
			ourShader->setVec3("u_palette_c", m_palette_burning_ship_c.x, m_palette_burning_ship_c.y, m_palette_burning_ship_c.z);
			ourShader->setVec3("u_palette_d", m_palette_burning_ship_d.x, m_palette_burning_ship_d.y, m_palette_burning_ship_d.z);
		}
		else {
			ourShader->setVec3("u_palette_a", m_palette_a.x, m_palette_a.y, m_palette_a.z);
			ourShader->setVec3("u_palette_b", m_palette_b.x, m_palette_b.y, m_palette_b.z);
			ourShader->setVec3("u_palette_c", m_palette_c.x, m_palette_c.y, m_palette_c.z);
			ourShader->setVec3("u_palette_d", m_palette_d.x, m_palette_d.y, m_palette_d.z);
		}
	}
	else if (m_currentFractal == FractalType::TRICORN) {
		if (m_use_pre_render_params) {
			ourShader->setDVec2("u_center", m_pre_render_center_x, m_pre_render_center_y);
			ourShader->setDouble("u_zoom", m_pre_render_zoom_threshold);
		}
		else {
			ourShader->setDVec2("u_center", 0.0, 0.0);
			ourShader->setDouble("u_zoom", 0.5);
		}
		if (!m_apply_common_color_palette) {
			ourShader->setVec3("u_palette_a", m_palette_tricorn_a.x, m_palette_tricorn_a.y, m_palette_tricorn_a.z);
			ourShader->setVec3("u_palette_b", m_palette_tricorn_b.x, m_palette_tricorn_b.y, m_palette_tricorn_b.z);
			ourShader->setVec3("u_palette_c", m_palette_tricorn_c.x, m_palette_tricorn_c.y, m_palette_tricorn_c.z);
			ourShader->setVec3("u_palette_d", m_palette_tricorn_d.x, m_palette_tricorn_d.y, m_palette_tricorn_d.z);
		}
		else {
			ourShader->setVec3("u_palette_a", m_palette_a.x, m_palette_a.y, m_palette_a.z);
			ourShader->setVec3("u_palette_b", m_palette_b.x, m_palette_b.y, m_palette_b.z);
			ourShader->setVec3("u_palette_c", m_palette_c.x, m_palette_c.y, m_palette_c.z);
			ourShader->setVec3("u_palette_d", m_palette_d.x, m_palette_d.y, m_palette_d.z);
		}
	}
	else if (m_currentFractal == FractalType::PHOENIX) {
		if (m_use_pre_render_params) {
			ourShader->setDVec2("u_center", m_pre_render_center_x, m_pre_render_center_y);
			ourShader->setDouble("u_zoom", m_pre_render_zoom_threshold);
			ourShader->setDVec2("u_phoenix_c", m_pre_render_phoenix_c_x, m_pre_render_phoenix_c_y);
			ourShader->setDouble("u_phoenix_p", m_pre_render_phoenix_p);
		}
		else {
			ourShader->setDVec2("u_center", 0.0, 0.0);
			ourShader->setDouble("u_zoom", 0.5);
			ourShader->setDVec2("u_phoenix_c", -0.5, 0.0);
			ourShader->setDouble("u_phoenix_p", 0.56667);
		}
		if (!m_apply_common_color_palette) {
			ourShader->setVec3("u_palette_a", m_palette_phoenix_a.x, m_palette_phoenix_a.y, m_palette_phoenix_a.z);
			ourShader->setVec3("u_palette_b", m_palette_phoenix_b.x, m_palette_phoenix_b.y, m_palette_phoenix_b.z);
			ourShader->setVec3("u_palette_c", m_palette_phoenix_c.x, m_palette_phoenix_c.y, m_palette_phoenix_c.z);
			ourShader->setVec3("u_palette_d", m_palette_phoenix_d.x, m_palette_phoenix_d.y, m_palette_phoenix_d.z);
		}
		else {
			ourShader->setVec3("u_palette_a", m_palette_a.x, m_palette_a.y, m_palette_a.z);
			ourShader->setVec3("u_palette_b", m_palette_b.x, m_palette_b.y, m_palette_b.z);
			ourShader->setVec3("u_palette_c", m_palette_c.x, m_palette_c.y, m_palette_c.z);
			ourShader->setVec3("u_palette_d", m_palette_d.x, m_palette_d.y, m_palette_d.z);
		}
	}
	else if (m_currentFractal == FractalType::LYAPUNOV) {
		if (m_use_pre_render_params) {
			ourShader->setDVec2("u_lyapunov_center", m_pre_render_lyapunov_center_a, m_pre_render_lyapunov_center_b);
			ourShader->setDouble("u_lyapunov_zoom", m_pre_render_zoom_threshold);
		}
		else {
			ourShader->setDVec2("u_lyapunov_center", 3.0, 3.0);
			ourShader->setDouble("u_lyapunov_zoom", 1.0);
		}
		if (!m_apply_common_color_palette) {
			ourShader->setVec3("u_palette_a", m_palette_lyapunov_a.x, m_palette_lyapunov_a.y, m_palette_lyapunov_a.z);
			ourShader->setVec3("u_palette_b", m_palette_lyapunov_b.x, m_palette_lyapunov_b.y, m_palette_lyapunov_b.z);
			ourShader->setVec3("u_palette_c", m_palette_lyapunov_c.x, m_palette_lyapunov_c.y, m_palette_lyapunov_c.z);
			ourShader->setVec3("u_palette_d", m_palette_lyapunov_d.x, m_palette_lyapunov_d.y, m_palette_lyapunov_d.z);
		}
		else {
			ourShader->setVec3("u_palette_a", m_palette_a.x, m_palette_a.y, m_palette_a.z);
			ourShader->setVec3("u_palette_b", m_palette_b.x, m_palette_b.y, m_palette_b.z);
			ourShader->setVec3("u_palette_c", m_palette_c.x, m_palette_c.y, m_palette_c.z);
			ourShader->setVec3("u_palette_d", m_palette_d.x, m_palette_d.y, m_palette_d.z);
		}
	}
	else if (m_currentFractal == FractalType::NEWTON) {
		if (m_use_pre_render_params) {
			ourShader->setDVec2("u_center", m_pre_render_center_x, m_pre_render_center_y);
			ourShader->setDouble("u_zoom", m_pre_render_zoom_threshold);
		}
		else {
			ourShader->setDVec2("u_center", 0.0, 0.0);
			ourShader->setDouble("u_zoom", 0.5);
		}
		if (!m_apply_common_color_palette) {
			ourShader->setVec3("u_palette_a", m_palette_newton_a.x, m_palette_newton_a.y, m_palette_newton_a.z);
			ourShader->setVec3("u_palette_b", m_palette_newton_b.x, m_palette_newton_b.y, m_palette_newton_b.z);
			ourShader->setVec3("u_palette_c", m_palette_newton_c.x, m_palette_newton_c.y, m_palette_newton_c.z);
			ourShader->setVec3("u_palette_d", m_palette_newton_d.x, m_palette_newton_d.y, m_palette_newton_d.z);
		}
		else {
			ourShader->setVec3("u_palette_a", m_palette_a.x, m_palette_a.y, m_palette_a.z);
			ourShader->setVec3("u_palette_b", m_palette_b.x, m_palette_b.y, m_palette_b.z);
			ourShader->setVec3("u_palette_c", m_palette_c.x, m_palette_c.y, m_palette_c.z);
			ourShader->setVec3("u_palette_d", m_palette_d.x, m_palette_d.y, m_palette_d.z);
		}
	}
	else if (m_currentFractal == FractalType::NOVA) {
		if (m_use_pre_render_params) {
			ourShader->setDVec2("u_center", m_pre_render_center_x, m_pre_render_center_y);
			ourShader->setDouble("u_zoom", m_pre_render_zoom_threshold);
			ourShader->setDouble("u_power", m_pre_render_nova_power);
			ourShader->setDouble("u_relaxation", m_pre_render_nova_relaxation);
		}
		else {
			ourShader->setDVec2("u_center", 0.0, 0.0);
			ourShader->setDouble("u_zoom", 0.5);
			ourShader->setDouble("u_power", 3.0);
			ourShader->setDouble("u_relaxation", 1.0);
		}
		if (!m_apply_common_color_palette) {
			ourShader->setVec3("u_palette_a", m_palette_nova_a.x, m_palette_nova_a.y, m_palette_nova_a.z);
			ourShader->setVec3("u_palette_b", m_palette_nova_b.x, m_palette_nova_b.y, m_palette_nova_b.z);
			ourShader->setVec3("u_palette_c", m_palette_nova_c.x, m_palette_nova_c.y, m_palette_nova_c.z);
			ourShader->setVec3("u_palette_d", m_palette_nova_d.x, m_palette_nova_d.y, m_palette_nova_d.z);
		}
		else {
			ourShader->setVec3("u_palette_a", m_palette_a.x, m_palette_a.y, m_palette_a.z);
			ourShader->setVec3("u_palette_b", m_palette_b.x, m_palette_b.y, m_palette_b.z);
			ourShader->setVec3("u_palette_c", m_palette_c.x, m_palette_c.y, m_palette_c.z);
			ourShader->setVec3("u_palette_d", m_palette_d.x, m_palette_d.y, m_palette_d.z);
		}
	}
	else if (m_currentFractal == FractalType::MULTIBROT) {
		if (m_use_pre_render_params) {
			ourShader->setDVec2("u_center", m_pre_render_center_x, m_pre_render_center_y);
			ourShader->setDouble("u_zoom", m_pre_render_zoom_threshold);
			ourShader->setDouble("u_power", m_pre_render_multibrot_power);
		}
		else {
			ourShader->setDVec2("u_center", 0.0, 0.0);
			ourShader->setDouble("u_zoom", 0.5);
			ourShader->setDouble("u_power", 3.0);
		}
		if (!m_apply_common_color_palette) {
			ourShader->setVec3("u_palette_a", m_palette_mandelbrot_a.x, m_palette_mandelbrot_a.y, m_palette_mandelbrot_a.z);
			ourShader->setVec3("u_palette_b", m_palette_mandelbrot_b.x, m_palette_mandelbrot_b.y, m_palette_mandelbrot_b.z);
			ourShader->setVec3("u_palette_c", m_palette_mandelbrot_c.x, m_palette_mandelbrot_c.y, m_palette_mandelbrot_c.z);
			ourShader->setVec3("u_palette_d", m_palette_mandelbrot_d.x, m_palette_mandelbrot_d.y, m_palette_mandelbrot_d.z);
		}
		else {
			ourShader->setVec3("u_palette_a", m_palette_a.x, m_palette_a.y, m_palette_a.z);
			ourShader->setVec3("u_palette_b", m_palette_b.x, m_palette_b.y, m_palette_b.z);
			ourShader->setVec3("u_palette_c", m_palette_c.x, m_palette_c.y, m_palette_c.z);
			ourShader->setVec3("u_palette_d", m_palette_d.x, m_palette_d.y, m_palette_d.z);
		}
	}
	else if (m_currentFractal == FractalType::SPIDER) {
		if (m_use_pre_render_params) {
			ourShader->setDVec2("u_center", m_pre_render_center_x, m_pre_render_center_y);
			ourShader->setDouble("u_zoom", m_pre_render_zoom_threshold);
		}
		else {
			ourShader->setDVec2("u_center", 0.0, 0.0);
			ourShader->setDouble("u_zoom", 0.5);
		}
		if (!m_apply_common_color_palette) {
			ourShader->setVec3("u_palette_a", m_palette_spider_a.x, m_palette_spider_a.y, m_palette_spider_a.z);
			ourShader->setVec3("u_palette_b", m_palette_spider_b.x, m_palette_spider_b.y, m_palette_spider_b.z);
			ourShader->setVec3("u_palette_c", m_palette_spider_c.x, m_palette_spider_c.y, m_palette_spider_c.z);
			ourShader->setVec3("u_palette_d", m_palette_spider_d.x, m_palette_spider_d.y, m_palette_spider_d.z);
		}
		else {
			ourShader->setVec3("u_palette_a", m_palette_a.x, m_palette_a.y, m_palette_a.z);
			ourShader->setVec3("u_palette_b", m_palette_b.x, m_palette_b.y, m_palette_b.z);
			ourShader->setVec3("u_palette_c", m_palette_c.x, m_palette_c.y, m_palette_c.z);
			ourShader->setVec3("u_palette_d", m_palette_d.x, m_palette_d.y, m_palette_d.z);
		}
	}
	else {
		spdlog::critical("No fractals selected, pre-render aborted ...");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteTextures(1, &m_pre_render_texture);
		glDeleteFramebuffers(1, &m_pre_render_fbo);
		m_pre_render_texture = 0;
		m_pre_render_fbo = 0;
		return;
	}

	ourShader->setFloat("u_color_density", m_color_density);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D, m_pre_render_texture);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if (ourShader != nullptr) {
		delete ourShader;
		ourShader = nullptr;
	}
	m_texture_view_shader = new Shader("shaders/texture_view.vert", "shaders/texture_view.frag");
	spdlog::info("Pre-render complete.");
}

void Application::preRenderWorker()
{
	if (SDL_GL_MakeCurrent(window, m_worker_context) != 0) {
		spdlog::critical("Worker thread could not set GL context! Error: {}", SDL_GetError());
		m_worker_finished_submission.store(true);
		return;
	}

	spdlog::info("Worker thread: Starting {}K pre-render submission...", (m_pre_render_resolution / 1024));
	Shader* workerShader;
	if (m_currentFractal == FractalType::MANDELBROT) {
		workerShader = new Shader("shaders/prerender.vert", "shaders/mandelbrot_prerender.frag");
	}
	else if (m_currentFractal == FractalType::BURNING_SHIP) {
		workerShader = new Shader("shaders/prerender.vert", "shaders/burningship_prerender.frag");
	}
	// more fractal cases
	else {
		spdlog::critical("Worker thread: No valid fractal type set for pre-render!");
		m_worker_finished_submission.store(true);
		return;
	}

	if (workerShader == nullptr || workerShader->ID == 0) {
		spdlog::critical("Worker thread: Failed to create or link the shader program!");
		delete workerShader;
		m_worker_finished_submission.store(true);
		return;
	}

	GLuint workerVAO;
	glGenVertexArrays(1, &workerVAO);
	glBindVertexArray(workerVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_vertices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glGenFramebuffers(1, &m_pre_render_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_pre_render_fbo);
	glGenTextures(1, &m_pre_render_texture);
	glBindTexture(GL_TEXTURE_2D, m_pre_render_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_pre_render_resolution, m_pre_render_resolution, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pre_render_texture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		spdlog::critical("Worker thread: Framebuffer is not complete!");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteTextures(1, &m_pre_render_texture);
		glDeleteFramebuffers(1, &m_pre_render_fbo);
		glDeleteVertexArrays(1, &workerVAO);
		m_worker_finished_submission.store(true);
		return;
	}

	glViewport(0, 0, m_pre_render_resolution, m_pre_render_resolution);
	glClear(GL_COLOR_BUFFER_BIT);
	workerShader->use();
	/*workerShader->setVec2("iResolution", (float)m_pre_render_resolution, (float)m_pre_render_resolution);*/
	workerShader->setInt("u_max_iterations", 5000);
	workerShader->setFloat("u_color_density", m_color_density);

	if (m_currentFractal == FractalType::MANDELBROT) {
		if (m_use_pre_render_params) {
			workerShader->setDVec2("u_center", m_pre_render_center_x, m_pre_render_center_y);
			workerShader->setDouble("u_zoom", m_pre_render_zoom_threshold);
		}
		else {
			workerShader->setDVec2("u_center", -0.75, 0.0);
			workerShader->setDouble("u_zoom", 1.0);
		}
		if (!m_apply_common_color_palette) {
			workerShader->setVec3("u_palette_a", m_palette_mandelbrot_a.x, m_palette_mandelbrot_a.y, m_palette_mandelbrot_a.z);
			workerShader->setVec3("u_palette_b", m_palette_mandelbrot_b.x, m_palette_mandelbrot_b.y, m_palette_mandelbrot_b.z);
			workerShader->setVec3("u_palette_c", m_palette_mandelbrot_c.x, m_palette_mandelbrot_c.y, m_palette_mandelbrot_c.z);
			workerShader->setVec3("u_palette_d", m_palette_mandelbrot_d.x, m_palette_mandelbrot_d.y, m_palette_mandelbrot_d.z);
		}
		else {
			workerShader->setVec3("u_palette_a", m_palette_a.x, m_palette_a.y, m_palette_a.z);
			workerShader->setVec3("u_palette_b", m_palette_b.x, m_palette_b.y, m_palette_b.z);
			workerShader->setVec3("u_palette_c", m_palette_c.x, m_palette_c.y, m_palette_c.z);
			workerShader->setVec3("u_palette_d", m_palette_d.x, m_palette_d.y, m_palette_d.z);
		}
	}

	// tiled rendering parameters
	const int TILE_SIZE = 256;
	const int BATCH_SIZE = 4;
	const int num_tiles = m_pre_render_resolution / TILE_SIZE;
	int batch_counter = 0;
	glEnable(GL_SCISSOR_TEST);
	for (int tile_y = 0; tile_y < num_tiles; ++tile_y) {
		for (int tile_x = 0; tile_x < num_tiles; ++tile_x) {
			int x_pos = tile_x * TILE_SIZE;
			int y_pos = tile_y * TILE_SIZE;
			glViewport(x_pos, y_pos, TILE_SIZE, TILE_SIZE);
			glScissor(x_pos, y_pos, TILE_SIZE, TILE_SIZE);
			workerShader->setIVec4("u_tile_info", tile_x, tile_y, num_tiles, num_tiles);
			glBindVertexArray(workerVAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glFlush();
			GLsync tileFence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
			GLenum waitRes;
			do {
				waitRes = glClientWaitSync(tileFence, GL_SYNC_FLUSH_COMMANDS_BIT, 5'000'000);
			} while (waitRes == GL_TIMEOUT_EXPIRED);
			glDeleteSync(tileFence);
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
	spdlog::info("Worker thread: All tiles rendered.");
	glDisable(GL_SCISSOR_TEST);
	if (m_pre_render_fence) { glDeleteSync(m_pre_render_fence); }
	m_pre_render_fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	glFlush();
	delete workerShader;
	glDeleteVertexArrays(1, &workerVAO);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	SDL_GL_MakeCurrent(window, nullptr);
	m_worker_finished_submission.store(true);
	spdlog::info("Worker thread: Render commands submitted.");
	spdlog::info("Worker thread: Pre-render worker completed.");
}