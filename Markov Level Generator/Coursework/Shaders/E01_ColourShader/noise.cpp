#include "noise.h"
#include <math.h>


noise::noise()
{
	for (int i = 0; i < 256; i++)
	{
		//x = y
		// y = 4
		//x = y = 4;
		p[i] = permutation[i];
		p[256 + i] = p[i];
	}
}

noise::~noise()
{
}

double noise::create_noise(double x, double y, double z)
{
	int X = (int)floor(x) & 255,
	Y = (int)floor(y) & 255,
	Z = (int)floor(z) & 255;

	x -= floor(x);
	y -= floor(y);
	z -= floor(z);

	double u = fade(x),
	v = fade(y),
	w = fade(z);

	int A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z,
	B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;

	return lerp(w, lerp(v, lerp(u, grad(p[AA], x, y, z),
	grad(p[BA], x - 1, y, z)), 
	lerp(u, grad(p[AB], x, y - 1, z), 
	grad(p[BB], x - 1, y - 1, z))),
	lerp(v, lerp(u, grad(p[AA + 1], x, y, z - 1), 
	grad(p[BA + 1], x - 1, y, z - 1)),
	lerp(u, grad(p[AB + 1], x, y - 1, z - 1),
	grad(p[BB + 1], x - 1, y - 1, z - 1))));
}

double noise::fade(double t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

double noise::lerp(double t, double a, double b)
{
	return a + t * (b - a);
}

double noise::grad(int hash, double x, double y, double z)
{
	int h = hash & 15;         
	
	double u = h < 8 ? x : y,v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}
