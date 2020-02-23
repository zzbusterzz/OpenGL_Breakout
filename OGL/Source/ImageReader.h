#pragma once
#include<iostream>
#include<glad/glad.h>

using namespace std;

class ImageReader
{
	public :
		unsigned int ID;
		ImageReader(const char* path, int ImageType, bool flipImage);
};

