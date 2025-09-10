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
		initSDL();
		initOpenGL();
		initImGui();
		initTriangle();
		std::cout << "\n\033[30;42m Mesmer application core has been initialized \033[0m\n" << std::endl;

		bool done = false;
		ImVec4 clear_color = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);
		bool show_demo_window = true;

		while (!done) {
			SDL_Event event;
			while (SDL_PollEvent(&event)) {
				ImGui_ImplSDL2_ProcessEvent(&event);
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

			ImGuiIO& io = ImGui::GetIO();
			ImDrawList* draw_list = ImGui::GetBackgroundDrawList();

			draw_list->AddText(
				ImVec2(10, 10),
				IM_COL32(255, 255, 0, 255),
				"Mesmer - Main Menu"
			);
			draw_list->AddText(
				m_font_large,
				68.0f,
				ImVec2((float)screenWidth / 2 - 140, 20),
				IM_COL32(255, 255, 255, 255),
				"MESMER"
			);
			const char* status = "All Systems OK - No errors detected";
			draw_list->AddText(
				ImVec2((float)screenWidth - 300, 10),
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
			ourShader->setVec2("iResolution", (float)screenWidth, (float)screenHeight);
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