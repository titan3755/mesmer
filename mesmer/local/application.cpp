/*
	Mesmer - An interactive and high-performance fractal generator and explorer.
	Made by -> Titan // GH: https://github.com/titan3755/mesmer
	[application.cpp]
*/

#include <application.hpp>

Application::Application(const char* title, const char* settings_file, int argc, char* argv[]) {
	this->screenWidth = 0;
	this->screenHeight = 0;
	this->windowTitle = title;
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
		initTriangle();
		std::cout << "\n\033[30;42m Mesmer application core has been initialized \033[0m\n" << std::endl;

		bool done = false;
		static ImVec4 clear_color = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);
		static bool show_demo_window = false;
		const char* status = "Status: All Systems OK";
		const char* sub = "Mesmer - Main Menu";
		static bool title_text_toggle = true;

		while (!done) {
			SDL_Event event;
			while (SDL_PollEvent(&event)) {
				ImGuiIO& io = ImGui::GetIO();
				ImGui_ImplSDL2_ProcessEvent(&event);
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

			{
				ImGui::Begin("Control Panel");
				if (ImGui::CollapsingHeader("Menu BG Settings")) 
				{
					ImGui::SliderFloat3("Menu BG Color One", (float*)&m_menu_bg_color_one, 0.0f, 1.0f);
					ImGui::SliderFloat3("Menu BG Color Two", (float*)&m_menu_bg_color_two, 0.0f, 1.0f);
					ImGui::SliderFloat3("Menu BG Accent Color", (float*)&m_menu_bg_accent_color, 0.0f, 1.0f);
				}
				if (ImGui::CollapsingHeader("Fractal Display Settings"))
				{
					ImGui::ColorEdit3("Background Color", (float*)&clear_color); // maybe useless

					ImGui::Separator();
					ImGui::Text("Fractal Color Palette");
					ImGui::ColorEdit3("Brightness (a)", (float*)&m_palette_a);
					ImGui::ColorEdit3("Contrast (b)", (float*)&m_palette_b);
					ImGui::SliderFloat3("Frequency (c)", (float*)&m_palette_c, 0.0f, 2.0f);
					ImGui::SliderFloat3("Phase (d)", (float*)&m_palette_d, 0.0f, 1.0f);
					ImGui::Separator();
				}

				if (ImGui::CollapsingHeader("Fractal Parameters"))
				{
					if (m_currentFractal == FractalType::MANDELBROT)
					{
						ImGui::Text("Mandelbrot Controls");
						ImGui::Separator();

						ImGui::InputDouble("Zoom", &m_mandel_zoom, 0.1, 0.0, "%.8f");
						ImGui::InputDouble("Center X", &m_mandel_center_x, 0.01, 0.0, "%.8f");
						ImGui::InputDouble("Center Y", &m_mandel_center_y, 0.01, 0.0, "%.8f");

						ImGui::SliderInt("Max Iterations", &m_mandel_max_iterations, 50, 5000);

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

						ImGui::InputDouble("Zoom", &m_mandel_zoom, 0.1, 0.0, "%.8f");
						ImGui::InputDouble("Center X", &m_mandel_center_x, 0.01, 0.0, "%.8f");
						ImGui::InputDouble("Center Y", &m_mandel_center_y, 0.01, 0.0, "%.8f");

						ImGui::Separator();

						ImGui::InputDouble("Constant c (real)", &m_julia_c_x, 0.001, 0.0, "%.8f");
						ImGui::InputDouble("Constant c (imag)", &m_julia_c_y, 0.001, 0.0, "%.8f");

						ImGui::Checkbox("Interactive Mode (Mouse)", &m_julia_interactive_mode);
						ImGui::Checkbox("Map Julia Const to Mouse Pos", &m_julia_c_map_to_mouse);

						ImGui::Separator();
						ImGui::SliderInt("Max Iterations", &m_mandel_max_iterations, 50, 5000);

						if (ImGui::Button("Reset View")) {
							m_mandel_zoom = 1.0;
							m_mandel_center_x = 0.0;
							m_mandel_center_y = 0.0;
							m_julia_c_x = -0.8;
							m_julia_c_y = 0.156;
							m_mandel_max_iterations = 200;
						}
					}
					else
					{
						ImGui::Text("No fractal selected.");
					}
				}


				ImGui::End();
			}
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
			if (show_fractal_selection)
			{
				const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
				ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + screenWidth / 2 - 320, main_viewport->WorkPos.y + screenHeight / 2 + 60));
				ImGui::SetNextWindowSize(ImVec2(650, 150));
				ImGui::Begin("Fractal Selection", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);
				ImGui::PushFont(m_font_large);
				float button_width = 320.0f;
				float spacing = 30.0f;
				float total_width = (button_width * 2) + spacing;
				ImGui::SetCursorPosX((ImGui::GetWindowSize().x - total_width) * 0.5f);

				// Mandelbrot
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.3f, 0.8f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.4f, 0.9f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.2f, 0.7f, 1.0f));
				if (ImGui::Button("Mandelbrot", ImVec2(button_width, 80))) {
					spdlog::info("'Mandelbrot' button clicked!");
					m_currentFractal = FractalType::MANDELBROT;

					if (ourShader != nullptr) {
						delete ourShader;
					}

					ourShader = new Shader("shaders/mandelbrot.vert", "shaders/mandelbrot.frag");
					spdlog::info("Loaded Mandelbrot shader.");

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

					if (ourShader != nullptr) {
						delete ourShader;
					}
					ourShader = new Shader("shaders/julia.vert", "shaders/julia.frag");
					spdlog::info("Loaded Julia shader.");

					show_fractal_selection = false;
					show_main_buttons = false;
					sub = "Mesmer - Julia Set";
					title_text_toggle = false;
				}

				ImGui::PopStyleColor(3);
				ImGui::SameLine(0.0f, spacing);

				// Burning Ship

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.25f, 0.15f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.35f, 0.20f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.15f, 0.10f, 1.0f));
				if (ImGui::Button("Burning Ship", ImVec2(button_width, 80))) {
					spdlog::info("'Burning Ship' button clicked!");
					m_currentFractal = FractalType::BURNING_SHIP;

					if (ourShader != nullptr) {
						delete ourShader;
					}

					ourShader = new Shader("shaders/burningship.vert", "shaders/burningship.frag");
					spdlog::info("Loaded Burning Ship shader.");

					show_fractal_selection = false;
					show_main_buttons = false;
					sub = "Mesmer - Burning Ship";
					title_text_toggle = false;

					m_mandel_zoom = 0.5;
					m_mandel_center_x = -0.5;
					m_mandel_center_y = -0.5;
				}

				ImGui::PopStyleColor(3);
				ImGui::SameLine(0.0f, spacing);

				// Tricorn
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.5f, 0.6f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.7f, 0.8f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.05f, 0.4f, 0.5f, 1.0f));
				if (ImGui::Button("Tricorn", ImVec2(button_width, 80))) {
					spdlog::info("'Tricorn' button clicked!");
					// todo: implement tricorn shader
				}

				ImGui::PopFont();
				ImGui::End();
			}

			ImGuiIO& io = ImGui::GetIO();
			ImDrawList* draw_list = ImGui::GetBackgroundDrawList();

			draw_list->AddText(
				ImVec2(10, 10),
				IM_COL32(255, 255, 0, 255),
				sub
			);
			if (title_text_toggle) {
				draw_list->AddText(
					m_font_large,
					68.0f,
					ImVec2((float)screenWidth / 2 - 140, 20),
					IM_COL32(255, 255, 255, 255),
					"MESMER"
				);
			}
			else {
				draw_list->AddText(
					m_font_large,
					20.0f,
					ImVec2((float)screenWidth / 2 - 170, 20),
					IM_COL32(255, 255, 255, 255),
					"Press Space Bar to exit to main menu"
				);
			}
			draw_list->AddText(
				ImVec2((float)screenWidth - 200, 10),
				IM_COL32(0, 255, 0, 255),
				status
			);

			if (show_demo_window) {
				ImGui::ShowDemoWindow(&show_demo_window);
			}

			ImGui::Render();
			int drawable_w, drawable_h;
			SDL_GL_GetDrawableSize(window, &drawable_w, &drawable_h);
			glViewport(0, 0, drawable_w, drawable_h);
			glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
			glClear(GL_COLOR_BUFFER_BIT);

			ourShader->use();
			ourShader->setFloat("iTime", SDL_GetTicks() / 1000.0f);
			ourShader->setVec2("iResolution", (float)drawable_w, (float)drawable_h);
			ourShader->setVec3("iColorOne", m_menu_bg_color_one.x, m_menu_bg_color_one.y, m_menu_bg_color_one.z);
			ourShader->setVec3("iColorTwo", m_menu_bg_color_two.x, m_menu_bg_color_two.y, m_menu_bg_color_two.z);
			ourShader->setVec3("iAccentColor", m_menu_bg_accent_color.x, m_menu_bg_accent_color.y, m_menu_bg_accent_color.z);
			ourShader->setVec3("u_palette_a", m_palette_a.x, m_palette_a.y, m_palette_a.z);
			ourShader->setVec3("u_palette_b", m_palette_b.x, m_palette_b.y, m_palette_b.z);
			ourShader->setVec3("u_palette_c", m_palette_c.x, m_palette_c.y, m_palette_c.z);
			ourShader->setVec3("u_palette_d", m_palette_d.x, m_palette_d.y, m_palette_d.z);
			if (m_currentFractal == FractalType::MANDELBROT) {
				ourShader->setDVec2("u_center", m_mandel_center_x, m_mandel_center_y);
				ourShader->setDouble("u_zoom", m_mandel_zoom);
				ourShader->setInt("u_max_iterations", m_mandel_max_iterations);
			}
			else if (m_currentFractal == FractalType::JULIA)
			{
				ourShader->setDVec2("u_center", m_mandel_center_x, m_mandel_center_y);
				ourShader->setDouble("u_zoom", m_mandel_zoom);
				ourShader->setInt("u_max_iterations", m_mandel_max_iterations);
				ourShader->setDVec2("u_julia_c", m_julia_c_x, m_julia_c_y);
			}
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_WINDOW_MAXIMIZED
	);
	if (!window) {
		throw std::runtime_error("Failed to create SDL window.");
	}
	SDL_SetWindowFullscreen(window, 0);
	SDL_SetWindowBordered(window, SDL_TRUE);

	gl_context = SDL_GL_CreateContext(window);
	if (!gl_context) {
		throw std::runtime_error(std::string("SDL_GL_CreateContext Error: ") + SDL_GetError());
	}
	SDL_GL_MakeCurrent(window, gl_context);
	SDL_GL_SetSwapInterval(1);

	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		throw std::runtime_error("Failed to initialize GLAD.");
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

void Application::initTriangle() {
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

	GLuint EBO;
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