#pragma once
#ifndef MESMER_SETTINGS_HPP
#define MESMER_SETTINGS_HPP

#include <iostream>
#include <fstream>

class Settings {
public:
	Settings(const char* filename);
	~Settings();
};

#endif
