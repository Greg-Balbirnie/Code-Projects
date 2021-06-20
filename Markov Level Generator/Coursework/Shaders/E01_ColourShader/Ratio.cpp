#include "Ratio.h"
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <iostream>
#include <fstream>
#include <windows.h>
#include <vector>
#include <chrono>
#include "GdiplusHelperFunctions.h"

#include <gdiplus.h>

using namespace Gdiplus;
using namespace std;

#pragma comment (lib,"Gdiplus.lib")

Ratio::Ratio()
{
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;

	// Initialize GDI+.
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}


Ratio::~Ratio()
{
}

Ratio Ratio::Return_Ratio()
{
	Ratio ratio;

	Bitmap* image = new Bitmap(L"../generated_level.png");

	// Find the width and height
	width = image->GetWidth();	// The width of the image in pixels
	height = image->GetHeight();	// The height of the image in pixels

	if (height > width)
	{
		ratio.x = 1;
		ratio.y = height / width;
	}
	else
	{
		ratio.x = width / height;;
		ratio.y = 1;
	}

	return ratio;
}
