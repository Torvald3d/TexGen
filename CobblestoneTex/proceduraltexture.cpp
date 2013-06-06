#include "proceduraltexture.h"
#include <math.h>
#include <stdlib.h>
#include <limits.h>

ProceduralTexture::ProceduralTexture(){
	seed = 0;
	w = h = 64;
	maxOctaves = 20;
	interpolateTable = NULL;
	powTable = new float[maxOctaves];
	for (int i=0; i<maxOctaves; i++)
		powTable[i] = pow(0.5, i);
}

ProceduralTexture::~ProceduralTexture(){
	delete[] powTable;
	delete[] interpolateTable;
}

inline int ProceduralTexture::distance(int x1, int y1, int x2, int y2){
	return (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2);
}

inline int ProceduralTexture::getRnd(int x, int y){
	return (0x6C078965*(seed^((x*2971902361)^(y*3572953751))))&0x7FFFFFFF;
}

inline int ProceduralTexture::pointOfPerlinNoise(int x, int y, int cellSize){
	int dx = x-(x/cellSize)*cellSize;
	int dy = y-(y/cellSize)*cellSize;
	int z1 = getRnd(x-dx, y-dy);					// z3 --- z4
	int z2 = getRnd(x-dx+cellSize, y-dy);			//  |     |
	int z3 = getRnd(x-dx, y-dy+cellSize);			//  + z   +
	int z4 = getRnd(x-dx+cellSize, y-dy+cellSize);	// z1 --- z2

	int z1z3 = z1*(1.0f-interpolateTable[dy])+z3*interpolateTable[dy];
	int z2z4 = z2*(1.0f-interpolateTable[dy])+z4*interpolateTable[dy];
	return z1z3*(1.0f-interpolateTable[dx])+z2z4*interpolateTable[dx];
}

unsigned char *ProceduralTexture::generateCelluarTexture(int size){
	if (size<2)
		size = 2;
	int cellX = w/size+2;
	int cellY = h/size+2;
	int pointsX[cellX][cellY];
	int pointsY[cellX][cellY];
	srand(seed);
	for (int i=0; i<cellX; i++)
		for (int j=0; j<cellY; j++){
			pointsX[i][j] = i*size+rand()%((int)(size*0.7))+size*0.15-size;
			pointsY[i][j] = j*size+rand()%((int)(size*0.7))+size*0.15-size;
		}

	int distBuff[n];
	int maxDist = INT_MIN;

	for (int i=0; i<n; i++){
		int x = i%w;
		int y = i/w;
		int min = INT_MAX;
		int min2 = INT_MAX;
		int startX = x/size;
		int finishX = startX+3;
		for (int cp=-1, point=0; startX<finishX; startX++){
			int startY = y/size;
			int finishY = startY+3;
			for (; startY<finishY; startY++, point++){
				if (startX<0 || startX>=cellX || startY<0 || startY>=cellY)
					continue;
				int d = distance(x, y, pointsX[startX][startY], pointsY[startX][startY]);
				if (d<min){
					cp = point;
					min2 = min;
					min = d;
				}
				if (d<min2 && cp!=point)
					min2 = d;
			}
		}
		distBuff[i] = min2-min;
		if (maxDist<distBuff[i])
			maxDist = distBuff[i];
	}

	unsigned char *img = new unsigned char[n];

	for (int i=0; i<n; i++)
		img[i] = (distBuff[i]*255)/maxDist;
	return img;
}

inline ColorRGB ProceduralTexture::hsvToRgb(ColorHSV &hsv){
	ColorRGB rgb;
	if (hsv.s){
		hsv.h %= 360;
		if (hsv.h<0)
			hsv.h += 360;
		int i = hsv.h/60;
		float f = ((float)hsv.h/60.0f)-(float)i;
		unsigned char c1 = (hsv.v*(100-hsv.s))*0.0255f;
		unsigned char c2 = (hsv.v*(100-hsv.s*f))*0.0255f;
		unsigned char c3 = (hsv.v*(100-hsv.s*(1.0f-f)))*0.0255f;
		hsv.v *= 2.55f;
		switch (i){
			case 0:
				rgb.r = hsv.v;
				rgb.g = c3;
				rgb.b = c1;
				break;
			case 1:
				rgb.r = c2;
				rgb.g = hsv.v;
				rgb.b = c1;
				break;
			case 2:
				rgb.r = c1;
				rgb.g = hsv.v;
				rgb.b = c3;
				break;
			case 3:
				rgb.r = c1;
				rgb.g = c2;
				rgb.b = hsv.v;
				break;
			case 4:
				rgb.r = c3;
				rgb.g = c1;
				rgb.b = hsv.v;
				break;
			case 5:
				rgb.r = hsv.v;
				rgb.g = c1;
				rgb.b = c2;
				break;
		}
	} else
		rgb.r = rgb.g = rgb.b = hsv.v;
	return rgb;
}

ColorRGB *ProceduralTexture::generateCelluarTexture(int size, ColorHSV color, int hueRange){
	if (size<2)
		size = 2;
	int cellX = w/size+2;
	int cellY = h/size+2;
	int pointsX[cellX][cellY];
	int pointsY[cellX][cellY];
	ColorRGB cellColor[cellX][cellY];
	srand(seed);
	for (int i=0; i<cellX; i++)
		for (int j=0; j<cellY; j++){
			pointsX[i][j] = i*size+rand()%((int)(size*0.7))+size*0.15-size;
			pointsY[i][j] = j*size+rand()%((int)(size*0.7))+size*0.15-size;
		}
	color.h -= (hueRange/2);
	for (int i=0; i<cellX; i++)
		for (int j=0; j<cellY; j++){
			ColorHSV c = color;
			c.h += rand()%hueRange;
			cellColor[i][j] = hsvToRgb(c);
		}

	ColorRGB *img = new ColorRGB[n];
	for (int i=0; i<n; i++){
		int x = i%w;
		int y = i/w;
		int px = 0;
		int py = 0;
		int min = INT_MAX;
		int startX = x/size;
		int finishX = startX+3;
		for (; startX<finishX; startX++){
			int startY = y/size;
			int finishY = startY+3;
			for (; startY<finishY; startY++){
				if (startX<0 || startX>=cellX || startY<0 || startY>=cellY)
					continue;
				int d = distance(x, y, pointsX[startX][startY], pointsY[startX][startY]);
				if (d<min){
					px = startX;
					py = startY;
					min = d;
				}
			}
		}
		img[i] = cellColor[px][py];
	}
	return img;
}

unsigned char *ProceduralTexture::generatePerlinNoise(int octaves){
	unsigned char *img = new unsigned char[n];
	if (octaves<1)
		octaves = 1;
	if (octaves>maxOctaves)
		octaves = maxOctaves;
	for (int i=0; i<n; i++)
		img[i] = 0;

	float norm = 255.0f/INT_MAX;
	for (int j=1; j<=octaves; j++){
		int f = 1<<(octaves-j);
		delete[] interpolateTable;
		interpolateTable = new float[f];
		for (int i=0; i<f; i++){
			float a = ((float)i/(float)f)*M_PI;
			interpolateTable[i] = (1.0f-cosf(a))*0.5f;
		}
		for (int i=0; i<n; i++)
			img[i] += pointOfPerlinNoise(i%w, i/w, f)*powTable[j]*norm;
	}
	return img;
}

void ProceduralTexture::brightnessContrast(unsigned char *img, float brightness, float contrast, bool onePass){
	if (brightness!=0){
		if (brightness>0){
			if (brightness>1)
				brightness = 1;
			brightness = 1.0f-brightness;
			for (int i=0; i<n; i++){
				int r = 255-img[i];
				r *= brightness;
				img[i] = 255-r;
			}
		}
		if (brightness<0){
			if (brightness<-1)
				brightness = -1;
			brightness = 1.0f+brightness;
			for (int i=0; i<n; i++)
				img[i] *= brightness;
		}
	}

	if (contrast!=1){
		if (contrast<0)
			contrast = 0;
		int avbr = 0;
		if (!onePass){
			for (int i=0; i<n; i++)
				avbr += img[i];
			avbr /= n;
		} else
			avbr = 127;
		for (int i=0; i<n; i++){
			int res = contrast*(img[i]-avbr)+avbr;
			if (res<0)
				res = 0;
			if (res>255)
				res = 255;
			img[i] = res;
		}
	}
}

void ProceduralTexture::mix(unsigned char *img1, unsigned char *img2, int valueTest, float opacity){
	for (int i=0; i<n; i++)
		if (img2[i]<=valueTest){
			int b = img2[i];
			int r = img1[i];
			b = ((float)b/valueTest)*255.0f;
			r = r-(255-b)*opacity;
			if (r<0)
				r = 0;
			img1[i] = r;
		}
}

void ProceduralTexture::mix(ColorRGB *img1, unsigned char *img2){
	for (int i=0; i<n; i++){
		img1[i].r = (img1[i].r*img2[i])/255;
		img1[i].g = (img1[i].g*img2[i])/255;
		img1[i].b = (img1[i].b*img2[i])/255;
	}
}

void ProceduralTexture::postEffect(unsigned char *img, int iterations, float smooth){
	for (int i=0; i<n; i++){
		float s = (float)img[i]/255.0f;
		float ds = s*(float)iterations-(float)((int)(s*iterations));
		ds = smooth*(ds-0.5f)+0.5f;
		if (ds>1)
			ds = 1;
		if (ds<0)
			ds = 0;
		s = ((float)((int)(s*(float)iterations))+ds)/(float)iterations;
		img[i] = s*255;
	}
}

void ProceduralTexture::generateNormalMap(unsigned char *img){
	normalMap = new ColorRG[n];
	int maxR = 0;
	int maxG = 0;

	for (int i=0; i<n; i++){
		int x = i%w;
		int y = i/w;
		if (x>0){
			int dr = (int)img[y*w+x-1]-(int)img[y*w+x];
			if (dr>maxR)
				maxR = dr;
		}
		if (y>0){
			int dg = (int)img[(y-1)*w+x]-(int)img[y*w+x];
			if (dg>maxG)
				maxG = dg;
		}
	}
	maxR *= 2;
	maxG *= 2;
	for (int i=0; i<n; i++){
		int x = i%w;
		int y = i/w;
		if (x>0){
			int r = 127+(((int)img[y*w+x-1]-(int)img[y*w+x])*255)/maxR;
			if (r>255)
				r = 255;
			if (r<0)
				r = 0;
			normalMap[y*w+x].r = r;
		} else {
			int r = 127-(((int)img[y*w+x+1]-(int)img[y*w+x])*255)/maxR;
			if (r>255)
				r = 255;
			if (r<0)
				r = 0;
			normalMap[y*w+x].r = r;
		}
		if (y>0){
			int g = 127+(((int)img[(y-1)*w+x]-img[y*w+x])*255)/maxG;
			if (g>255)
				g = 255;
			if (g<0)
				g = 0;
			normalMap[y*w+x].g = g;
		} else {
			int g = 127-(((int)img[(y+1)*w+x]-img[y*w+x])*255)/maxG;
			if (g>255)
				g = 255;
			if (g<0)
				g = 0;
			normalMap[y*w+x].g = g;
		}
	}
}

void ProceduralTexture::gaussianBlur(unsigned char *img, float value){
	if (value<=0)
		return;
	//еще пока не реализованно

}

void ProceduralTexture::setSize(int width, int height){
	if (width<2)
		width = 2;
	if (height<2)
		height = 2;
	w = width;
	h = height;
	n = width*height;
}

void ProceduralTexture::setSeed(int seed){
	this->seed = seed;
}

int ProceduralTexture::width(){
	return w;
}

int ProceduralTexture::height(){
	return h;
}

ColorRGB *ProceduralTexture::cobblestone(int stoneSize,
										 int stoneNoise,
										 ColorHSV color,
										 int colorRange,
										 float edgeIntensity,
										 int edgeSize,
										 float edgeOpacity,
										 float edgeSmooth,
										 int stoneLayers,
										 float smoothness,
										 float stoneBrightness,
										 bool createNormalMap){
	unsigned char *ct = generateCelluarTexture(stoneSize);
	gaussianBlur(ct, edgeSmooth);
	brightnessContrast(ct, 0, edgeIntensity);
	unsigned char *pn = generatePerlinNoise(stoneNoise);
	postEffect(pn, stoneLayers, smoothness);
	brightnessContrast(pn, stoneBrightness, 1);
	mix(pn, ct, edgeSize, edgeOpacity);
	delete[] ct;
	if (createNormalMap)
		generateNormalMap(pn);
	ColorRGB *cct = generateCelluarTexture(stoneSize, color, colorRange);
	mix(cct, pn);
	delete[] pn;

	return cct;
}

ColorRG *ProceduralTexture::getNormal(){
	return normalMap;
}
