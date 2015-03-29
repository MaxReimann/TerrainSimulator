#include "stdafx.h"
#include "simulation.h"
#include "util/tga.h"
#include "voronoi.h"
#include <wx/image.h>
#include "util/random.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//2d to 1d conversion
#define ALT(x,y) (map[(x)*ysize+(y)])


#define SEDIMENT(x,y) (sedimentMap[(x)*ysize+(y)])

using namespace std;


AltitudeMap::AltitudeMap(int _xsize, int _ysize)
	: xsize(_xsize), ysize(_ysize), sealevel(0.0),
	FlagX(static_cast<int>((double)_xsize / 2.0)),
	FlagY(static_cast<int>((double)_ysize / 2.0))
{
	change = 0.0;
	errorstream.open("error_sim.txt");
	initRandom(); //random seed
	map = new double[xsize*ysize]; //heightmap
	sedimentMap = new double[xsize*ysize]; 


	diffMap = new double**[xsize];
	for (int i = 0; i < xsize; i++)
	{
		diffMap[i] = new double*[ysize]; 
		for (int j = 0; j < ysize; j++)
		{
			diffMap[i][j] = new double[4];
		}
	}


	previewImage = new wxImage(xsize, ysize, true);
	for (int x = 0; x < xsize; ++x){
		for (int y = 0; y < ysize; ++y)
		{
			ALT(x, y) = 0.0;
		}
	}

	showflag = true; // positionflag
}

AltitudeMap::AltitudeMap(void) {}

AltitudeMap::~AltitudeMap(void){
	if (terrainColors){ delete[] terrainColors; }
	if (terrainNormals){ delete[] terrainNormals; }
	delete[] map;
	delete[] sedimentMap;
	deleteDiffMap();

	errorstream.close();
	previewImage->Destroy();
	delete previewImage;
}

void AltitudeMap::deleteDiffMap()
{
	for (int i = 0; i < xsize; i++)
		for (int j = 0; j < ysize; j++)
			delete diffMap[i][j];

	for (int i = 0; i < xsize; i++)
		delete[] diffMap[i];

	delete[] diffMap;
}

int AltitudeMap::getXSize() {
	return xsize;
}

int AltitudeMap::getYSize() {
	return ysize;
}

double AltitudeMap::getSeaLevel() {
	return sealevel;
}
void AltitudeMap::getSeaLevel(double sl) {
	sealevel = sl;
}
double AltitudeMap::getAltitude(int x, int y) {
	return ALT(x%xsize, y%ysize);
}
void  AltitudeMap::writeAltitude(int x, int y, double z){
	if (x < xsize && y < ysize){
		map[(x)*ysize + (y)] = z;
	}
}
void  AltitudeMap::add(int x, int y, double z){
	map[x*ysize + y] = map[(x)*ysize + (y)] + z;
}
void  AltitudeMap::multiply(int x, int y, double z){
	ALT(x, y) *= z + 0.5;
}

void AltitudeMap::subdivision(double coeff, double lt, double rt, double lb, double rb) {
	// coeff in interval 0.0>0.5 
	//lt = lefttop, rt = righttop, lb=leftbottom,rb=rightbottom

	ALT(0, 0) = lt; //map[0] = ALT(0,0)
	ALT(xsize - 1, 0) = rt;
	ALT(0, ysize - 1) = lb;
	ALT(xsize - 1, ysize - 1) = rb;

	for (int x = 0; x < xsize; ++x){
		for (int y = 0; y < ysize; ++y)
		{
			ALT(x, y) = 0.0;
			SEDIMENT(x, y) = 0.0;
		}
	}

	initRandom(); //random seed setzen
	_subdivide(coeff, 0, 0, xsize - 1, ysize - 1);
	
	for (int x = 0; x < xsize; ++x){
		for (int y = 0; y < ysize; ++y)
		{
			ALT(x, y) += 0.4;
		}
	}

	normalize();
}


void inline AltitudeMap::_subdivide(double coeff, int x1, int y1, int x2, int y2, double* inputMap){
	//if inputMap is left out, standard map is used
	//coeff is degree of terrain folding in [0,0.5]
	/*
	Subdivision
	o------o		o---o---o
	|	   |		|   |   |
	|	   | ---->  o---o---o
	|      |		|   |   |
	o------o		O---o---O
	*/

	//use standard or custom map
	double* usedMap;
	if (inputMap == nullptr)
		usedMap = map;
	else
		usedMap = inputMap;

#define MAP(x,y) usedMap[(x)*ysize+(y)]


	// stop if no points between corners anymore
	if (abs(x1 - x2) <= 1 || abs(y1 - y2) <= 1) {
		return;
	}

	int xmiddle = x1 + (int)floor(double(x2 - x1) / 2.0);
	int ymiddle = y1 + (int)floor(double(y2 - y1) / 2.0);

	// add random height to middle points
	if (MAP(xmiddle, y1) == 0){ 
		MAP(xmiddle, y1) = (MAP(x1, y1) + MAP(x2, y1)) / 2.0 + random(-0.5, 0.5) * coeff;
	}

	if (MAP(x1, ymiddle) == 0){
		MAP(x1, ymiddle) = (MAP(x1, y1) + MAP(x1, y2)) / 2.0 + random(-0.5, 0.5) * coeff;
	}

	if (MAP(xmiddle, ymiddle) == 0){
		MAP(xmiddle, ymiddle) = (MAP(x1, y1) + MAP(x2, y2)) / 2.0 + random(-0.5, 0.5) * coeff;
	}

	if (MAP(xmiddle, y2) == 0){
		MAP(xmiddle, y2) = (MAP(x1, y2) + MAP(x2, y2)) / 2.0 + random(-0.5, 0.5) * coeff;
	}

	if (MAP(x2, ymiddle) == 0){
		MAP(x2, ymiddle) = (MAP(x2, y1) + MAP(x2, y2)) / 2.0 + random(-0.5, 0.5) * coeff;
	}

	// Decreases folding per iteration, results in smoothed small features
	coeff *= coeff;

	//Rekursiv call for each quadrant
	_subdivide(coeff, x1, y1, xmiddle, ymiddle);
	_subdivide(coeff, xmiddle, y1, x2, ymiddle);
	_subdivide(coeff, x1, ymiddle, xmiddle, y2);
	_subdivide(coeff, xmiddle, ymiddle, x2, y2);

}



void AltitudeMap::subdivision(double coeff, double *inputMap) {
	/* Method for arbirtary heightmap */
#define ALT_2(x,y) (inputMap[(x)*ysize+(y)])

	m_inputMap = inputMap;
	for (int x = 0; x < xsize; ++x){
		for (int y = 0; y < ysize; ++y)
		{
			ALT_2(x, y) = 0.0;
		}
	}
	initRandom(); //random seed
	ALT_2(0, 0) = 0.0; //map[0] = ALT(0,0)
	ALT_2(xsize - 1, 0) = 0.0;
	ALT_2(0, ysize - 1) = 0.0;
	ALT_2(xsize - 1, ysize - 1) = 0.0;

	for (int x = 0; x < xsize; ++x){
		for (int y = 0; y < ysize; ++y)
		{
			ALT_2(x, y) = 0.0;
		}
	}

	_subdivide(coeff, 0, 0, xsize - 1, ysize - 1, inputMap);

	for (int x = 0; x < xsize; ++x) { //normalize
		for (int y = 0; y < ysize; ++y) {
			ALT(x, y) += 0.4;
			if (ALT_2(x, y) < 0.0)
				ALT_2(x, y) = 0.0;
			else if (ALT_2(x, y) > 1.0)
				ALT_2(x, y) = 1.0;
		}
	}

}


void AltitudeMap::initRandom(void){
	unsigned long init[4] = { static_cast<long>(random(100, 300)), 
		static_cast<long>(random(100, 300)), 
		static_cast<long>(random(100, 300)), static_cast<long>(random(100, 300)) };
	int length = 4;
	init_by_array(init, length);
}

double inline AltitudeMap::random(double a, double b){
	return  a + (genrand_real2() * (abs(a) + abs(b))); // return number in interval a,b
}


void  AltitudeMap::error_output(){
	for (int i = 0; i < xsize * ysize; ++i)
		errorstream << map[i] << " ";

	errorstream << "end of map \n \n \n \n \n \n \n \n \n" << std::endl;
}

void AltitudeMap::randomize(double r) {
	long double xrange = 0, yrange = 0, mrange = 0, rand = 0;
	double f;
	initRandom(); 
	bool showDialog = false;
	if (xsize * ysize * r > 5000) { 
		showProgressbar();
		showDialog = true;
		f = 0;
	}

	for (int i = 0; i < (r); i++) {
		if (showDialog) {
			f += 100.0 / r;
			dialog->Update((int)f);
		}
		for (int x = 0; x < xsize; ++x) {
			for (int y = 0; y < ysize; ++y) {
				xrange = abs(ALT((x - 1 < 0) ? (0) : (x - 1), y)
					- ALT((x + 1 > xsize) ? (xsize) : (x + 1), y)); 

				yrange = abs(ALT(x, (y + 1 > ysize) ? (ysize) : (y + 1)) 
					- ALT(x, (y - 1 < 0) ? (0) : (y - 1)));

				mrange = (xrange + yrange) / 100.0 * r / 10.0;
				rand = ((random(mrange, 2 * mrange) - 2 * mrange));
				ALT(x, y) = ALT(x, y) + rand;
			}
		}
	}
	normalize();

	if (showDialog == true) { dialog->Destroy(); } //fenster zerstoeren, wird nicht automatisch gemacht
}

void AltitudeMap::randomize2(double r) {
	double dist = 0;
	if (r > xsize) { r = static_cast<double>(xsize)-1.0; }
	if (r > ysize) { r = static_cast<double>(ysize)-1.0; }

	initRandom(); 
	double strength = 0.5;

	for (int y = 0; y < ysize; ++y) {
		for (int x = 0; x < xsize; ++x){
			if (((x * y) % static_cast<int>(r)) == 0) 
			{
				change = abs(random(strength, strength * 2)) / 50.0; 
				dist = 0;
				double b = change;
				ALT(x, y) = ALT(x, y) + change;

				for (int i = 1; i <= static_cast<int>(r); i++) {
					if (b == 0) { break; }
					b *= 0.99;

					if (x - i >= 0)
						ALT(x - i, y) -= b;

					if (x + i < xsize)
						ALT(x + i, y) -= b;

					if (y + i < ysize)
						ALT(x, y + i) -= b; 

					if (y - i >= 0)
						ALT(x, y - i) -= b;

					if ((x + i) < xsize && y + i < ysize)
						ALT(x + i, y + i) -= b; 

					if (x - i >= 0 && y - i >= 0)
						ALT(x - i, y - i) -= b;

					if (x - i >= 0 && y + i < ysize)
						ALT(x - i, y + i) -= b; 

					if (x + i < xsize && y - i >= 0)
						ALT(x + i, y - i) -= b; 
				}
			}
		}
	}

	normalize();
}

// Ground erosion
void AltitudeMap::erosion(int r, int iter){
	/*
	r = neighbourhood radius of erosion
	iter = number of erosion iterations

	small radius and many iterations -> peaks are smoothed more
	big radius and less steps -> generally more uniform terrain
	*/

	double *copy = new double[xsize * ysize];
	bool showDialog = false;
	double f;
	double sum = 0.0;
	if (xsize * ysize * iter * r > 5000) {
		showProgressbar();
		showDialog = true;
		f = 0;
	}


	while (iter > 0){
		--iter;
		for (int x = 0; x < xsize; ++x) {

			if (showDialog == true) {
				f += 100.0 / (xsize * (iter - 1));
				dialog->Update((int)f);
			}

			for (int y = 0; y < ysize; ++y){
				sum = 0.0;
				int offx;
				if (x >= r) { offx = -r; }
				else { offx = -x; }
				for (; (offx <= r) && (offx + x < xsize); ++offx){
					int offy;
					if (y >= r) { offy = -r; }
					else { offy = -y; }
					for (; (offy <= r) && (offy + y < ysize); ++offy){
						sum += ALT(x + offx, y + offy);
					}
				}
				copy[x*ysize + y] = sum / float(4 * r*r + 4.0*r + 1);
			}
		}

		//write to heightmap
		for (int x = 0; x < xsize; ++x) {
			for (int y = 0; y < ysize; ++y) {
				ALT(x, y) = copy[x*ysize + y];
				SEDIMENT(x, y) = 0.0;
			}
		}

	}
	normalize();
	delete[] copy;
	if (showDialog == true) { dialog->Destroy(); }
}
void AltitudeMap::normalize(){
	normalize(map);
}

void AltitudeMap::normalize(double *inputMap){
#define MAP(x,y) inputMap[(x)*ysize+(y)]
	
	for (int x = 0; x < xsize; ++x) {
		for (int y = 0; y < ysize; ++y) {
			if (MAP(x, y) < 0.0)
				MAP(x, y) = 0.0;
			else if (MAP(x, y) > 1.0)
				MAP(x, y) = 1.0;
		}
	}
}

void AltitudeMap::plateau(int x, int y, int r){
	int offx;
	double f;
	bool showDialog = false;
	if (r * r > 5000) {
		showProgressbar();
		showDialog = true;
		f = 0;
	}
	if (r >= xsize) { r = xsize - 1; }
	if (r >= ysize) { r = ysize - 1; }
	if (x >= r) { offx = -r; }
	else { offx = -x; }
	for (; (offx <= r) && (offx + x < xsize); ++offx){
		int offy;
		if (y >= r) { offy = -r; }
		else { offy = -y; }

		if (showDialog == true) {
			f += 100.0 / (abs(offy * offx));
			dialog->Update((int)f);
		}

		for (; offy <= r && offy + y < ysize; ++offy){
			double distance = sqrt((double)offx*offx + (double)offy*offy) / (double)r;
			if (distance > 1.0) { continue; }
			//smoothing power depends on distance from center
			ALT(x + offx, y + offy) = distance * ALT(x + offx, y + offy) + (1.0 - distance) * ALT(x, y);

			if (x - offx > 0 && y - offy > 0) {
				ALT(x - offx, y - offy) = distance * ALT(x - offx, y - offy) + (1.0 - distance) * ALT(x, y);
			}
		}
	}

	if (showDialog == true) { dialog->Destroy(); } 
}


double AltitudeMap::slopeX(int x, int y){
	if (x == 0) { ++x; }
	else if (x == xsize - 1) { --x; }

	if (y == 0) { ++y; }
	else if (y == ysize - 1){ --y; }
	double d = (ALT(x+1, y-1) + ALT(x+1, y+1) - ALT(x-1, y-1) - ALT(x-1, y+1)) / 2.0;
	return d / sqrt(d*d + 1.0 / (double)(xsize*xsize));
}

double AltitudeMap::angleSlopeX(int x, int y){
	if (x == 0) { ++x; }
	else if (x == xsize - 1) { --x; }

	if (y == 0) { ++y; }
	else if (y == ysize - 1){ --y; }
	double d = (ALT(x+1, y-1) + ALT(x+1, y+1) - ALT(x-1, y-1) - ALT(x-1, y+1));
	return atan(d / 2.0);
}

double AltitudeMap::angleSlopeY(int x, int y){
	if (x == 0) { ++x; }
	else if (x == xsize - 1) { --x; }

	if (y == 0) { ++y; }
	else if (y == ysize - 1){ --y; }
	double d = (ALT(x-1, y+1) + ALT(x+1, y+1) - ALT(x-1, y-1) - ALT(x+1, y-1)) / 2.0;
	return atan(d / 2.0);
}



double AltitudeMap::slopeY(int x, int y){
	if (x == 0) { ++x; }
	else if (x == xsize - 1) { --x; }

	if (y == 0) { ++y; }
	else if (y == ysize - 1){ --y; }
	double d = (ALT(x-1, y+1) + ALT(x+1, y+1) - ALT(x-1, y-1) - ALT(x+1, y-1)) / 2.0;
	return d / sqrt(d*d + 1.0 / (double)(ysize*ysize));
}




int AltitudeMap::terrainLoadFromImage(char *filename, int normals) {

	std::string path = std::string(filename);

	if (path.find(".tga") == string::npos){
		bool res = loadImage(filename, normals); 
		if (res){
			return TERRAIN_OK;
		}
		else {
			return TERRAIN_ERROR_LOADING_IMAGE;
		}
	}

	tgaInfo *info;
	int mode;
	double pointHeight;

	/* if a terrain already exists, destroy it. */
	delete[] map;
	info = tgaLoad(filename);

	/* check to see if the image was properly loaded
	   only greyscale, RGB or RGBA noncompressed images */
	if (info->status != TGA_OK)
		return(TERRAIN_ERROR_LOADING_IMAGE);

	/* if the image is RGB, convert it to greyscale
	   mode will store the image's number of components */
	mode = info->pixelDepth / 8;
	if (mode == 3) {
		tgaRGBtoGreyscale(info);
		mode = 1;
	}

	xsize = info->width;
	ysize = info->height;

	map = new double[xsize * ysize];
	if (map == NULL)
		return(TERRAIN_ERROR_MEMORY_PROBLEM);
	
	if (normals) {
		terrainNormals = new double[xsize * ysize * 3];
		if (terrainNormals == NULL)
			return(TERRAIN_ERROR_MEMORY_PROBLEM);
	}
	else
		terrainNormals = NULL;

	/* if mode = RGBA then allocate memory for colors, and check for errors */
	if (mode == 4) {
		terrainColors = new double[xsize * ysize * 3];
		if (terrainColors == NULL)
			return(TERRAIN_ERROR_MEMORY_PROBLEM);
	}
	else
		terrainColors = NULL;

	/* fill arrays */
	for (int i = 0; i < ysize; i++)
		for (int j = 0; j < xsize; j++) {

		/* compute the height as a value between 0.0 and 1.0 */
		pointHeight = info->imageData[mode*(i*xsize + j) + (mode - 1)] / 255.0;
		map[i*xsize + j] = pointHeight;
		/* if mode = RGBA then fill the colors array as well */
		if (mode == 4) {
			terrainColors[3 * (i*xsize + j)] = (info->imageData[mode*(i*xsize + j)]) / 255.0;
			terrainColors[3 * (i*xsize + j) + 1] = (info->imageData[mode*(i*xsize + j) + 1]) / 255.0;
			terrainColors[3 * (i*xsize + j) + 2] = (info->imageData[mode*(i*xsize + j) + 2]) / 255.0;
		}
		}

	/* free the image's memory  */
	tgaDestroy(info);

	return TERRAIN_OK;
}


int AltitudeMap::terrainLoadFromImage(char *filename, int normals, int c) {

	std::string path = std::string(filename);
	std::string sub = path.substr(path.length() - 4, path.length());

	if (sub.compare(".tga") != 0){
		bool res = loadImage(filename, normals); 
		if (res){
			return TERRAIN_OK;
		}
		else {
			return TERRAIN_ERROR_LOADING_IMAGE;
		}
	}

	STGA tgaFile;
	int mode;
	double pointHeight;
	int stat;


	if (stat = (loadTGA(filename, tgaFile) != 0))
		return stat;


	/* if a terrain already exists, destroy it. */
	delete[] map;

	mode = tgaFile.type[2];
	if (mode == 3) {
		tgaRGBtoGreyscale(tgaFile);
		mode = 1;
	}

	/* set the width and height of the terrain */
	xsize = tgaFile.width;
	ysize = tgaFile.height;

	/* alocate memory for the terrain, and check for errors */
	map = new double[xsize * ysize];
	if (map == NULL)
		return(TERRAIN_ERROR_MEMORY_PROBLEM);

	/* allocate memory for the normals, and check for errors */
	if (normals) {
		terrainNormals = new double[xsize * ysize * 3];
		if (terrainNormals == NULL)
			return(TERRAIN_ERROR_MEMORY_PROBLEM);
	}
	else
		terrainNormals = NULL;

	/* if mode = RGBA then allocate memory for colors, and check for errors */
	if (mode == 4) {
		terrainColors = new double[xsize * ysize * 3];
		if (terrainColors == NULL)
			return(TERRAIN_ERROR_MEMORY_PROBLEM);
	}
	else
		terrainColors = NULL;

	/* fill arrays */
	for (int i = 0; i < ysize; i++)
		for (int j = 0; j < xsize; j++) {

		/* compute the height as a value between 0.0 and 1.0 */
		pointHeight = tgaFile.data[mode*(i*xsize + j) + (mode - 1)] / 255.0;
		map[i*xsize + j] = pointHeight;
		/* if mode = RGBA then fill the colors array as well */
		if (mode == 4) {
			terrainColors[3 * (i*xsize + j)] = (tgaFile.data[mode*(i*xsize + j)]) / 255.0;
			terrainColors[3 * (i*xsize + j) + 1] = (tgaFile.data[mode*(i*xsize + j) + 1]) / 255.0;
			terrainColors[3 * (i*xsize + j) + 2] = (tgaFile.data[mode*(i*xsize + j) + 2]) / 255.0;
		}
		}

	return TERRAIN_OK;
}


void AltitudeMap::showProgressbar(void){
	dialog = new wxProgressDialog(wxT("Progress of Operation"),
		wxT("Progress of Operation"), 100, NULL, wxPD_AUTO_HIDE);
}

void AltitudeMap::showProgressbar(wxString Text){
	dialog = new wxProgressDialog(Text,
		Text, 100, NULL, wxPD_AUTO_HIDE);
}


void AltitudeMap::scaleTerrain(double factor){
	wxImage original = makePreviewImage();
	deleteDiffMap();

	xsize = int(xsize * factor);
	ysize = int(ysize * factor);
	if (xsize < 4 || ysize < 4){
		return; // smaller than 4*4 will not be considered
	}

	original.Rescale(xsize + 1, ysize + 1); //rescale in-place
	delete[] map;
	map = new double[xsize*ysize];
	sedimentMap = new double[xsize*ysize];
	diffMap = new double**[xsize]; 

	for (int i = 0; i < xsize; i++)
	{
		diffMap[i] = new double*[ysize]; 
		for (int j = 0; j < ysize; j++)
		{
			diffMap[i][j] = new double[4];
		}
	}


	for (int x = 0; x < xsize; x++){
		for (int y = 0; y < ysize; y++){
			ALT(x, y) = (double(original.GetRed(x, y)) + double(original.GetBlue(x, y)) + double(original.GetGreen(x, y))) / (255.0 * 3.0);
		}
	}
	original.Destroy();
	resetFlag(); //put flag into middle (prevents out of bounds exception)
}

void AltitudeMap::resetFlag(void){
	FlagX = int(double(xsize) / 2.0);
	FlagY = int(double(ysize) / 2.0);
}


void AltitudeMap::normalAtPoint(int x, int z, Vector3 &n)
{
	// Returns the normal at the specified location on the height map.
	// The normal is calculated using the properties of the height map.
	// This approach is much quicker and more elegant than triangulating the
	// height map and averaging triangle surface normals.

	if (x > 0 && x < xsize - 1)
		n.x = getAltitude(x - 1, z) - getAltitude(x + 1, z);
	else if (x > 0)
		n.x = 2.0f * (getAltitude(x - 1, z) - getAltitude(x, z));
	else
		n.x = 2.0f * (getAltitude(x, z) - getAltitude(x + 1, z));

	if (z > 0 && z < ysize - 1)
		n.z = getAltitude(x, z - 1) - getAltitude(x, z + 1);
	else if (z > 0)
		n.z = 2.0f * (getAltitude(x, z - 1) - getAltitude(x, z));
	else
		n.z = 2.0f * (getAltitude(x, z) - getAltitude(x, z + 1));

	n.y = 2.0f * 16.0f;
	n.normalize();
}


// Height difference to surrounding cells
// 0 = (x, y+1), 1 = (x-1, y), 2 = (x+1, y), 3 = (x, y-1) 
void AltitudeMap::heightDiff()
{
	for (int y = 1; y < ysize - 1; y++)
	{
		for (int x = 1; x < xsize - 1; x++)
		{

			diffMap[x][y][0] = 0;
			diffMap[x][y][1] = 0;
			diffMap[x][y][2] = 0;
			diffMap[x][y][3] = 0;

			if (ALT(x, y) > ALT(x, y + 1))
				diffMap[x][y][0] = ALT(x, y) - ALT(x, y + 1);

			if (ALT(x, y) > ALT(x - 1, y))
				diffMap[x][y][1] = ALT(x, y) - ALT(x - 1, y);

			if (ALT(x, y) > ALT(x + 1, y))
				diffMap[x][y][2] = ALT(x, y) - ALT(x + 1, y);

			if (ALT(x, y) > ALT(x, y - 1))
				diffMap[x][y][3] = ALT(x, y) - ALT(x, y - 1);

		}
	}
}

// MaterialMap is generated using perlinnoise
void AltitudeMap::thermalErosion(int iter, double Talus)
{
	float diffTotal;
	float diffMax;
	float c = 0.5; 
	float T = Talus;///*T = Talus angle*/
	int duration = iter; 
	float progress;
	
	showProgressbar();


	for (int m = 0; m < duration; m++)
	{

		heightDiff();

		for (int y = 1; y < ysize - 1; ++y)
		{
			for (int x = 1; x < xsize - 1; ++x)
			{
				diffTotal = 0.0f;
				diffMax = T;

				for (int n = 0; n < 4; n++)
				{
					if (diffMap[x][y][n] > T)
					{
						diffTotal += diffMap[x][y][n];

						if (diffMap[x][y][n] > diffMax)
							diffMax = diffMap[x][y][n];
					}
				}

				if (diffTotal != 0) // If therea re differences, shift material
				{
					ALT(x, y + 1) = ALT(x, y + 1) + c * (diffMax - T) * (diffMap[x][y][0] / diffTotal); // shift material to lower points
					ALT(x - 1, y) = ALT(x - 1, y) + c * (diffMax - T) * (diffMap[x][y][1] / diffTotal);
					ALT(x + 1, y) = ALT(x + 1, y) + c * (diffMax - T) * (diffMap[x][y][2] / diffTotal);
					ALT(x, y - 1) = ALT(x, y - 1) + c * (diffMax - T) * (diffMap[x][y][3] / diffTotal);

					ALT(x, y) -= c * (diffMax - T); // remove material from index cell
				}
			}
		}

		progress = double(m) / double(duration) * 100.0;
		dialog->Update(int(progress));
	}

	dialog->Destroy();
}




void AltitudeMap::voronoiTex(int fValue, int blockSize,
	bool negative, float clip, int distance, float coefficients[], int elements)
{
	voronoiTex(fValue, blockSize, negative, clip, distance, map, coefficients, elements);
}

void AltitudeMap::voronoiTex(int fValue, int blockSize,
	bool negative, float clip, int distance, double inputMap[], float coefficients[], int elements)
{
	Voronoi Vo = Voronoi(fValue, blockSize, NULL, 0, negative, clip);
	Vo.calculate(inputMap, distance, coefficients, elements);
	normalize(inputMap);
}