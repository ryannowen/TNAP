#pragma once

#include <memory>
#include <string>

#include "ImageLoader.h"
#include "ExternalLibraryHeaders.h"

struct STextureData
{
	std::unique_ptr<Helpers::ImageLoader> m_textureData;
	GLuint m_textureBinding{ 0 };
	std::string m_filePath{ "" };

	STextureData()
	{}

	STextureData(std::unique_ptr<Helpers::ImageLoader> argTextureData, const GLuint argTextureBinding, const std::string& argFilePath)
		: m_textureData(std::move(argTextureData)), m_textureBinding(argTextureBinding), m_filePath(argFilePath)
	{}
};