#pragma once

#include <iostream>
#include <math.h>


typedef struct {
	float x, y;
} vector2;

class Perlin
{
public:
	Perlin();
	float perlin(float x, float y);
	float dotGridGradient(int ix, int iy, float x, float y);
	float interpolate(float a0, float a1, float w);
	vector2 randomGradient(int ix, int iy);








};
