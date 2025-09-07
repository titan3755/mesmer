#pragma once
#ifndef MESMER_SETTINGS_HPP
#define MESMER_SETTINGS_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <spdlog/spdlog.h>

class Settings {
public:
	Settings(const char* filename);
	~Settings();

	bool exists() const { return fileExists; }

private:
	bool fileExists = false;
	std::string filename;
	std::map<std::string, std::string> settingsMap;

	void loadSettings();
	void appendSettings(std::map<std::string, std::string>);

};

#endif
