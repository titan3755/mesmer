#pragma once
#ifndef MESMER_SHADER_PROGRAM_HPP
#define MESMER_SHADER_PROGRAM_HPP

#include <iostream>

class ShaderProgram {
public:
	ShaderProgram(const char* vertexShaderPath, const char* fragmentShaderPath);
	~ShaderProgram() = default;
};

#endif