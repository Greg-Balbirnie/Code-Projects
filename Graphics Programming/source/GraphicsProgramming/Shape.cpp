#include "shape.h"
#include <vector>

extern GLubyte indices[]=	{0,   1,   2,	//front
							0,   2,   3,

							4,   5,   6,
							4,   6,   7,

							8,	9,	10,
							8,	10,	11,

							12,	13,	14,
							12,	14,	15,

							16,	17,	18,
							16,	18,	19,

							20,	21,	22,
							20,	22,	23
							};

extern float verts[] = {	-1.0, 1.0, 1.0,			// Vertex #0
							-1.0, -1.0, 1.0,		// Vertex #1
							1.0,  -1.0, 1.0,		// Vertex #2
							1.0,  1.0, 1.0,			// Vertex #3

							1.0,  1.0, 1.0,			//You think I'm gonna number 'em all?!
							1.0,  -1.0, 1.0,		//front, right, top, back, left, bottom
							1.0,  -1.0, -1.0,
							1.0,  1.0, -1.0,

							-1.0,  1.0, -1.0,
							-1.0,  1.0, 1.0,
							1.0,  1.0, 1.0,
							1.0,  1.0, -1.0,

							1.0,  1.0, -1.0,
							1.0,  -1.0, -1.0,
							-1.0,  -1.0, -1.0,
							-1.0,  1.0, -1.0,

							-1.0,  1.0, -1.0,
							-1.0,  -1.0, -1.0,
							-1.0,  -1.0, 1.0,
							-1.0,  1.0, 1.0,

							-1.0,  -1.0, 1.0,
							-1.0,  -1.0, -1.0,
							1.0,  -1.0, -1.0,
							1.0,  -1.0, 1.0,
						};

extern float norms[] = { 0.0, 0.0, 1.0,		//0
						0.0, 0.0, 1.0,		//1
 						0.0, 0.0, 1.0,		//2
						0.0, 0.0, 1.0,		//3
						1.0, 0.0, 0.0,
						1.0, 0.0, 0.0,
						1.0, 0.0, 0.0,
						1.0, 0.0, 0.0,
						0.0, 1.0, 0.0,
						0.0, 1.0, 0.0,
						0.0, 1.0, 0.0,
						0.0, 1.0, 0.0,
						0.0, 0.0, -1.0,
						0.0, 0.0, -1.0,
						0.0, 0.0, -1.0,
						0.0, 0.0, -1.0,
						-1.0, 0.0, 0.0,
						-1.0, 0.0, 0.0,
						-1.0, 0.0, 0.0,
						-1.0, 0.0, 0.0,
						0.0, -1.0, 0.0,
						0.0, -1.0, 0.0,
						0.0, -1.0, 0.0,
						0.0, -1.0, 0.0,
						};		

extern float skyTexCoords[] = { 0.25, 0.25, 	//0
								0.25, 0.5, 		//1
								0.5, 0.5, 		//2
								0.5, 0.25, 		//3

								0.5, 0.25, 		//0
								0.5, 0.5, 		//1
								0.75, 0.5, 		//2
								0.75, 0.25, 	//3
	
								0.25, 0.0, 		//0
								0.25, 0.25, 	//1
								0.5, 0.25, 		//2
								0.5, 0.0, 		//3

								0.75, 0.25, 	//0
								0.75, 0.5, 		//1
								1.0, 0.5, 		//2
								1.0, 0.25, 		//3

								0.0, 0.25, 		//0
								0.0, 0.5, 		//1
								0.25, 0.5, 		//2
								0.25, 0.25, 	//3

								0.25, 0.5, 		//0
								0.25, 0.75, 	//1
								0.5, 0.75, 		//2
								0.5, 0.5, 		//3
								};

void Shape::renderSky()
{
	// glDrawElements()
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_INDEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, verts);
	glNormalPointer(GL_FLOAT, 0, norms);
	glTexCoordPointer(2, GL_FLOAT, 0, skyTexCoords);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_INDEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Shape::floor()
{
	//creates a 10 by 10 plane with many smaller planes
	float resolution = 15;							//amount of planes along length and width 
	float division = (10 / resolution);				//the distance between each plane start point
	float total = (resolution * resolution * 4);	//total number of vertices
	std::vector<float> vertices;
	std::vector<float> normies;
	std::vector<float> texture;

	for (float z = -5; z < 5; z += division)
	{
		for (float x = -5; x < 5; x += division)
		{
			normies.push_back(0);
			normies.push_back(1);
			normies.push_back(0);
				vertices.push_back(x);
				vertices.push_back(0);
				vertices.push_back(z);

			normies.push_back(0);
			normies.push_back(1);
			normies.push_back(0);
				vertices.push_back(x);
				vertices.push_back(0);
				vertices.push_back(z + division);

			normies.push_back(0);
			normies.push_back(1);
			normies.push_back(0);
				vertices.push_back(x + division);
				vertices.push_back(0);
				vertices.push_back(z + division);
					
			normies.push_back(0);
			normies.push_back(1);
			normies.push_back(0);
				vertices.push_back(x + division);
				vertices.push_back(0);
				vertices.push_back(z);
		}
	}
	
	for (float z = 0; z < 10; z += division)
	{
		for (float x = 0; x < 10; x += division)
		{
			texture.push_back(x);
			texture.push_back(z);

			texture.push_back(x);
			texture.push_back(z + division);

			texture.push_back(x + division);
			texture.push_back(z + division);

			texture.push_back(x + division);
			texture.push_back(z);
		}
	}
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	glTexCoordPointer(2, GL_FLOAT, 0, texture.data());
	glNormalPointer(GL_FLOAT, 0, normies.data());
	glVertexPointer(3, GL_FLOAT, 0, vertices.data());

	glDrawArrays(GL_QUADS, 0, total);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Shape::cube()
{
	// add code to render the cube (above) using method 3
	// glDrawElements()
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_INDEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, verts);
	glNormalPointer(GL_FLOAT, 0, norms);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_INDEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}
