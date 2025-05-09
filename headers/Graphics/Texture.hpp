#ifndef TEXTURE_CLASS
#define TEXTURE_CLASS

#include <glad/glad.h>
#include <stb/stb_image.h>

class Texture2D
{
	public:
		unsigned int ID;
		unsigned int width, height;
		unsigned int internalFormat;
		unsigned int imageFormat;
		unsigned int wrapS;
		unsigned int wrapT;
		unsigned int filterMin;
		unsigned int filterMax;

		Texture2D();

		void Generate(unsigned int width, unsigned int height, unsigned char* data);
		void Bind() const;
};

#endif
