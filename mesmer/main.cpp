/*
	Mesmer - An interactive and high-performance fractal generator and explorer.
	Made by -> Titan // GH: https://github.com/titan3755/mesmer
	[main.cpp]
*/

#include <application.hpp>

const char* WINDOW_TITLE = "Mesmer";
const char* SETTINGS_FILE = "settings.mesmer";

// application metadata
std::map<std::string, std::string> app_metadata = {
	{"name", "Mesmer"},
	{"version", "1.0.0"},
	{"author", "Mahmud Al Muhaimin"},
	{"license", "MIT"},
	{"description", "A fractal explorer application"}
};

int main(int argc, char* argv[]) {
	Application app(WINDOW_TITLE, SETTINGS_FILE, argc, argv);
	try {
		app.run();
	}
	catch (const std::runtime_error& e) {
		std::cerr << "Runtime error: " << e.what() << std::endl;
		return 1;
	}
	catch (const std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
		return 1;
	}
	catch (...) {
		std::cerr << "An unknown error occurred." << std::endl;
		return 1;
	}
    return 0;
}