#ifndef RESOURCE_MANAGER_CLASS
#define RESOURCE_MANAGER_CLASS

#include <map>
#include <string>
#include <glad/glad.h>
#include "Texture.hpp"
#include "Shader.hpp"

class ResourceManager
{
	public:
		static std::map<std::string, Shader> Shaders;
		static std::map<std::string, Texture2D> Textures;

		static Shader LoadShader(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile, std::string name);
		static Shader GetShader(std::string name);
		static Texture2D LoadTexture(const char *file, bool alpha, std::string name);
		static Texture2D GetTexture(std::string name);
		static glm::mat4 SetProjection(float width, float height);
		static void Clear();
	private:
		ResourceManager() {}

		static Shader LoadShaderFromFile(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile = nullptr);
		static Texture2D LoadTextureFromFile(const char *file, bool alpha);
};

#endif
