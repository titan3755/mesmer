/*
	Mesmer - An interactive and high-performance fractal generator and explorer.
	Made by -> Titan // GH: https://github.com/titan3755/mesmer
	[settings.cpp]
*/

#include <settings.hpp>

Settings::Settings(const char* filename) {
	this->appendSettings(defaults);
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
	std::ifstream file(this->filename);
	this->fileExists = file.good();
}

void Settings::loadSettings() {
	std::ifstream file(this->filename);
	if (!file.is_open()) {
		spdlog::error("Could not open settings file {} for reading.", this->filename);
		std::cerr << "Could not open " << this->filename << " for reading.\n";
		return;
	}

	std::string line;
	while (std::getline(file, line)) {
		if (line.empty() || line[0] == '#') continue;
		std::istringstream iss(line);
		std::string key, value;
		if (std::getline(iss, key, '=') && std::getline(iss, value)) {
			this->settingsMap[key] = value;
		}
	}
}

void Settings::appendSettings(std::map<std::string, std::string>) {
	for (auto& kv : defaults) {
		if (this->settingsMap.find(kv.first) == this->settingsMap.end()) {
			this->settingsMap[kv.first] = kv.second;
		}
	}
}

void Settings::updateSetting(const std::string& key, const std::string& value) {
	this->settingsMap[key] = value;
}

std::string Settings::getSetting(const std::string& key) const {
	auto it = this->settingsMap.find(key);
	if (it != this->settingsMap.end()) {
		return it->second;
	}
	return "";
}

void Settings::saveSettings() const {
	std::ofstream file(this->filename);
	if (!file.is_open()) {
		spdlog::error("Could not open settings file {} for writing.", this->filename);
		std::cerr << "Could not open " << this->filename << " for writing.\n";
		return;
	}

	for (const auto& kv : this->settingsMap) {
		file << kv.first << "=" << kv.second << "\n";
	}
}

void Settings::deleteSetting(const std::string& key) {
	this->settingsMap.erase(key);
}

void Settings::clearSettings() {
	this->settingsMap.clear();
}

void Settings::printSettings() {
	for (const auto& kv : this->settingsMap) {
		spdlog::info("Printing All Settings: ");
		std::cout << kv.first << " = " << kv.second << std::endl;
	}
}

// will be applied to application.cpp later (...)
