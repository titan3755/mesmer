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
	{"version", "Version: 0.5.0 Alpha (Dev)"},
	{"author_desc", "Made by Titan // GH: https://github.com/titan3755/mesmer"},
	{"build", "Build Date: September 2025"},
	{"description", "Mesmer - Fractal Generator and Explorer"},
	{"license", "GPL-3.0"},
	{"control_one", "H - Toggle HUD"},
	{"control_two", "Space - Toggle Main Menu"},
	{"control_three", "Mouse Wheel - Zoom In/Out"},
	{"control_four", "Click + Drag - Pan View"},
	{"control_five", "M (Julia) - Toggle mapping constant to mouse"},
	{"control_six", "I (Julia) - Toggle interactive mode"}
};

int main(int argc, char* argv[]) {
	Application app(WINDOW_TITLE, SETTINGS_FILE, app_metadata, argc, argv);
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