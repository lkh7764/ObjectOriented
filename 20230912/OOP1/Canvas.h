#ifndef _CANVAS_H_
#define _CANVAS_H_

#include <iostream>

#include "Utils.h"

class Canvas
{
	char* frameBuffer;
	int		size;
	int width;
	int height;

public:

	Canvas(int width, int height) : width(width), height(height), size((width+1)*height), frameBuffer(new char[(size_t)size])
	{
		Borland::Initialize();
		clear();
	}

	int getSize() const { return size; }

	void clear()
	{
		memset(frameBuffer, '*', size);
		for (int i = 0; i < height; i++)
			frameBuffer[i*width] = '\n';
		//for (int i = 0; i < size; i++)
		//	frameBuffer[i] = ' ';
		frameBuffer[size - 1] = '\0';
	}

	void draw(const char* shape, int pos, bool visible)
	{
		if (visible == false) return;

		for (int i = 0; i < strlen(shape); i++)
		{
			if (pos + i < 0) continue;
			if (pos + i > size - 1) continue;

			frameBuffer[pos + i] = shape[i];
		}
	}

	void render() const
	{
		Borland::GotoXY(0, 0);
		printf("%s", frameBuffer);
	}

	~Canvas()
	{
		delete[] frameBuffer;
		frameBuffer = nullptr;
		size = 0;
	}
};

#endif