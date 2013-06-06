#ifndef PROCEDURALTEXTURE_H
#define PROCEDURALTEXTURE_H

struct ColorRGB{
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

struct ColorRG{
	unsigned char r;
	unsigned char g;
};

struct ColorHSV{
	short h; // [0..360)
	char s;  // [0..100]
	char v;  // [0..100]
};

class ProceduralTexture{
private:
	int distance(int x1, int y1, int x2, int y2); //quad
	int pointOfPerlinNoise(int x, int y, int cellSize);
	int getRnd(int x, int y);
	void brightnessContrast(unsigned char *img, float brightness, float contrast, bool onePass=false);
	void mix(unsigned char *img1, unsigned char *img2, int valueTest, float opacity);
	void mix(ColorRGB *img1, unsigned char *img2);
	void postEffect(unsigned char *img, int iterations, float smooth);
	ColorRGB hsvToRgb(ColorHSV &hsv);
	unsigned char *generateCelluarTexture(int size);
	ColorRGB *generateCelluarTexture(int size, ColorHSV color, int hueRange);
	unsigned char *generatePerlinNoise(int octaves);
	void generateNormalMap(unsigned char *img);
	void gaussianBlur(unsigned char *img, float value);

	int w, h, n;
	int seed;
	int maxOctaves;
	float *powTable;
	float *interpolateTable;
	ColorRG *normalMap;

public:
	ProceduralTexture();
	~ProceduralTexture();
	ColorRGB *cobblestone(int stoneSize=50,
							int stoneNoise=5,
							ColorHSV color=ColorHSV{60, 15, 70},
							int colorRange=80,
							float edgeIntensity=0.65,
							int edgeSize=50,
							float edgeOpacity=0.7,
							float edgeSmooth=0.7,
							int stoneLayers=5,
							float smoothness=1.5,
							float stoneBrightness=0.3,
							bool createNormalMap=true);
	ColorRG *getNormal();

	void setSize(int width, int height);
	void setSeed(int seed);
	int width();
	int height();
};

#endif // PROCEDURALTEXTURE_H
