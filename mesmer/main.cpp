#include <application.hpp>

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const char* WINDOW_TITLE = "Mesmer";

int main(int argc, char* argv[]) {
	Application app(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE, argc, argv);
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