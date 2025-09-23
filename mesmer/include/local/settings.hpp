/*
	Mesmer - An interactive and high-performance fractal generator and explorer.
	Made by -> Titan // GH: https://github.com/titan3755/mesmer
	[settings.hpp]
*/

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

	void checkFileExists();
	void loadSettings();
	void appendSettings(std::map<std::string, std::string>);
	void updateSetting(const std::string& key, const std::string& value);
	std::string getSetting(const std::string& key) const;
	void saveSettings() const;
	void deleteSetting(const std::string& key);
	void clearSettings();
	void printSettings();
	bool exists() const { return fileExists; }

private:
	bool fileExists = false;
	std::string filename;
	std::map<std::string, std::string> settingsMap;
	std::map <std::string, std::string> defaults = {
		{
		{"window_width", "1920"},
		{"window_height", "1080"},
		{"fullscreen", "true"},
		{"vsync", "true"},
		{"max_fps", "120"},
		{"theme", "dark"},
		{"language", "en"},
		{"autosave_interval", "10"},
		{"recent_files", ""},
		{"default_save_path", "./saves"},
		}
	};

};

#endif
