#include <settings.hpp>

Settings::Settings(const char* filename) {
	this->appendSettings({
		{"window_width", "1280"},
		{"window_height", "720"},
		{"fullscreen", "false"},
		{"vsync", "true"},
		{"max_fps", "60"},
		{"theme", "dark"},
		{"language", "en"},
		{"autosave_interval", "10"}, // in minutes
		{"recent_files", ""}, // comma-separated list
		{"default_save_path", "./saves"},
		});
	this->filename = filename;
	this->checkFileExists();
	if (this->fileExists) {
		this->loadSettings();
	} else {
		this->saveSettings();
	}
}

Settings::~Settings() {

}

void Settings::checkFileExists() {
	
}

void Settings::loadSettings() {

}

void Settings::appendSettings(std::map<std::string, std::string>) {

}

void Settings::updateSetting(const std::string& key, const std::string& value) {

}

std::string Settings::getSetting(const std::string& key) const {
	return "";
}

void Settings::saveSettings() const {

}

