#include "ImageReader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../Include/stb_image.h"

ImageReader::ImageReader(const char* path, int ImageType, bool flipImage) {
	

	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (flipImage)
		stbi_set_flip_vertically_on_load(true);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);

	cout << "Width" << width << endl;
	cout << "Height" << width << endl;
	cout << "nrChannels" << width << endl;

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, ImageType, width, height, 0, ImageType, GL_UNSIGNED_BYTE, data);//GL_RGB
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
}