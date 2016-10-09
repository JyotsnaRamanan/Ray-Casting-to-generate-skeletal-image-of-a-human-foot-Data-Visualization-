#include<iostream>
#include<fstream>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

using namespace std;

int width = 256;
int height = 256;
int X = 256;
int Y = 256;
int Z = 118;

int sampling_step = 1;

typedef struct{
	float r, g, b, a;
}color_t;

typedef struct{
	int x;
	int y;
	int z;
}point3_t;

typedef struct{
	int x, y, z;
}vec3_t;

unsigned char *vol = new unsigned char[X*Y*Z];
void saveppm(int width, int height, color_t *image, char *filename)
//void saveppm(int width, int height, color_t *image)
{
	uint64_t i;
	uint8_t r = 0, g = 0, b = 0;
	int x, y;

	FILE *fp;

	/* open in text mode */
	fp = fopen(filename, "ab");
	if (!fp) {
		fprintf(stderr, "Cannot open file %s.\n", filename);

	}

	fprintf(fp, "P6\n");
	fprintf(fp, "%i %i\n", width, height);
	fprintf(fp, "255\n");
	fclose(fp);

	/* reopen in binary append mode */
	fp = fopen(filename, "ab");
	if (!fp) {
		fprintf(stderr, "Cannot open file %s.\n", filename);

	}

	for (y = height - 1; y >= 0; y--) {
		for (x = 0; x < width; x++) {
			i = x + y * width;

			r = (uint8_t)(image[i].r * 0xFF);
			g = (uint8_t)(image[i].g * 0xFF);
			b = (uint8_t)(image[i].b * 0xFF);

			fputc(r, fp);
			fputc(g, fp);
			fputc(b, fp);
		}
	}

	fclose(fp);

}

color_t ray_casting(point3_t val, vec3_t dir, int step){
	point3_t sampling_point = val;
	color_t clr;
	color_t composition;
	composition.r = 0;
	composition.g = 0;
	composition.b = 0;
	composition.a = 0;

	color_t fcomposition;

	fcomposition.r = 0;
	fcomposition.g = 0;
	fcomposition.b = 0;
	fcomposition.a = 0;

	while (sampling_point.x >= 0 && sampling_point.x < X && sampling_point.y >= 0 && sampling_point.y < Y && sampling_point.z >= 0 && sampling_point.z < Z){
		float v = (float)vol[sampling_point.x + sampling_point.y*X + sampling_point.z*X*Y] * (1.f / 255.f);

		if (v < 0.2){

			clr.r = clr.g = clr.b = clr.a = 0.0;

		}
		else if (v >= 0.2 && v < 0.45){

			clr.r = 0.0f;
			clr.g = 0.1f;
			clr.b = 0.9f;
			clr.a = 0.5f;
		}
		else{
			clr.r = 0.9;
			clr.b = 0.9;
			clr.g = 0.9;
			clr.a = 0.9;
		}

		composition.r = (float)clr.r*clr.a*(1 - fcomposition.a);
		composition.g = (float)clr.g*clr.a*(1 - fcomposition.a);
		composition.b = (float)clr.b*clr.a*(1 - fcomposition.a);
		fcomposition.r = (float)(fcomposition.r + composition.r);
		fcomposition.g = (float)(fcomposition.g + composition.g);
		fcomposition.b = (float)(fcomposition.r + composition.b);
		fcomposition.a = (float)(clr.a*(1 - fcomposition.a) + fcomposition.a);

		sampling_point.z = sampling_point.z + (dir.z * step);
		sampling_point.y = sampling_point.y + (dir.y * step);
		sampling_point.x = sampling_point.x + (dir.x * step);


	}
	return fcomposition;
}


int main(){

	color_t *image = new color_t[width*height];
	//unsigned char *vol=new unsigned char[X*Y*Z];
	ifstream inf("VH_foot.dat", ios::binary);
	inf.read(reinterpret_cast<char *>(vol), X*Y*Z*sizeof(unsigned char));
	inf.close();


	vec3_t direction;
	direction.x = 1;
	direction.y = 0;
	direction.z = 0;


	for (int y = 0; y < height; y++){
		for (int x = 0; x < width; x++){
			int index = x + y*width;
			point3_t pt;
			pt.x = 0;
			pt.y = x;
			pt.z = y;

			image[index] = ray_casting(pt, direction, sampling_step);
		}
		cout << (int)(((double)y / (double)width) * 100) << endl;
	}

	saveppm(width, height, image, "sideview_opac1.ppm");
	//saveppm(width, height, image);
	free(image);

	//char * c = "";
	//cin >> c;
}
