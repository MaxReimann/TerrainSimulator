#include "globals.h"
#include "voronoi.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



Voronoi::Voronoi(int _fValue, int _blockSize,
	int* _pointsList, int _seed, bool _negative, float _clip)
	:fValue(_fValue),
	blockSize(_blockSize),
	seed(_seed),
	negative(_negative),
	clip(_clip)
{

	xsize = Alti.getXSize();
	ysize = Alti.getYSize();
	initRandom();
	dialogProgress = 0.0; //progressbar progress length

	showProgressbar();
	numToBlocksize = (blockSize - xsize % blockSize)*(blockSize - ysize%blockSize); //square of difference to next blocksize position
	bigXSize = xsize + (blockSize - xsize % blockSize);
	bigYSize = ysize + (blockSize - ysize % blockSize);
	pixelColorNumbers = new int[bigXSize*bigYSize + 1];

	pixelColorNumerate();

	totalBlocks = (bigXSize / blockSize) * (bigXSize / blockSize);

	for (int i = 0; i < 9; ++i)
		pixelDist.push_back(0.0);
	for (int i = 0; i < fValue; ++i)
		shortest.push_back(0.0);

	if (_pointsList == NULL)
	{
		generatePoints(totalBlocks);
		xyPoints(pointsList);
	}
	else{
		for (int i = 0; i < sizeof(_pointsList) / sizeof(int); i += 2){
			pointsList.push_back(pair<int, int>(_pointsList[i], _pointsList[i + 1]));
		}
	}

}

void Voronoi::calculate(double inputMap[], int distType, float coefficients[], int elements){
	pvec neighbors(9);
	m_inputMap = inputMap;

	for (int pixels = 0; pixels < bigXSize*bigYSize; pixels++){
		pair<int, int> XY = getLocation(pixels);
		neighborCheck(getBlock(XY), neighbors);
		pixelColorNumbers[pixels] = findNearestPoints(XY.first, XY.second, neighbors, distType, coefficients, elements);

		if (pixels % bigXSize == 0){
			dialogProgress += 100.0 / (bigXSize*2.0);
			dialog->Update(int(dialogProgress));
		}
	}

	draw();
}


void Voronoi::initRandom(void){
	unsigned long init[4] = { static_cast<long>(random(100, 300)), static_cast<long>(random(100, 300)),
		static_cast<long>(random(100, 300)), static_cast<long>(random(100, 300)) };
	int length = 4;
    init_by_array(init, length);
}

int inline Voronoi::random(int a, int b){
	return int(a + (genrand_real1() * (abs(a) + abs(b)))); // returns int in interval [a,b]
}


void Voronoi::range(int range, int arr[]){
	for (int i = 0; i < range; i++){
		arr[i] = i;
	}
}

void Voronoi::pixelColorNumerate(){
	for (int i = 0; i < bigXSize*bigXSize; i++){
		pixelColorNumbers[i] = i;
	}
}

int Voronoi::findNearestPoints(int x, int y, pvec& neighbor, int distType, float coefficients[], int elements){
	int elem = neighbor.size();
	int blockID;
	float distance;
	pixelDist.clear();
	shortest.clear();

	for (int i = 0; i < elem; i++){
		blockID = getBlockID(neighbor[i]);
		distance = length(x, y, pointsList[blockID].first, pointsList[blockID].second, distType);
		pixelDist.push_back(distance);
	}
	for (int amount = 0; amount < fValue; amount++){
		minNum = min_element(pixelDist.begin(), pixelDist.end()); //min_element returns forward_iterator, dereferenced by *
		shortest.push_back(*minNum);
		pixelDist.erase(minNum);
	}

	float sum = 0.0;
	float coeff;
	for (size_t i = 0; i < shortest.size(); ++i){
		if (signed(i) < elements)
			coeff = coefficients[i];
		else
			coeff = 1.0;

		sum += shortest[i] * coeff;
	}

	return(sum);
}

float Voronoi::length(int x1, int y1, int x2, int y2, int method){
	int dx = x2 - x1;
	int dy = y2 - y1;
	float distance;
	
	switch (method)
	{
	case 1:
		//"Linear"
		distance = sqrt(float(dx*dx + dy*dy)); break;
	case 2:
		//"SqLen"
		distance = (dx*dx + dy*dy); break;
	case 3:
		//"Manhattan"
		distance = (dx + dy); break;
	case 4:
		//"Chebyshev"
		if (dx == dy || dx > dy)
			distance = dx;
		else
			distance = dy;
		break;
	case 5:
		//"Quadratic"
		distance = (dx*dx + dy*dy + dx*dy); break;
	case 6:
		//My method
		distance = cos(double(dx*dx)) + sin(double(dy*dy)); break;
	}

	return(distance);
}

void Voronoi::generatePoints(int totalBlocks){
	for (int number = 0; number < totalBlocks; number++){
		if (seed == 0){
			seed = random(0, 1000000000);
		}
		init_genrand(seed);
		pointsList.push_back(make_pair(random(0, blockSize - 1), random(0, blockSize - 1)));
		seed += 1;
	}
}

pair<int, int> Voronoi::getLocation(int idNumber){
	int xloc = idNumber % bigXSize;
	int yloc = idNumber / bigYSize;
	return make_pair(xloc, yloc);
}


pair<int, int> Voronoi::getLocation_Alti(int idNumber){//fur die richtige (kleinere) map
	int xloc = idNumber % xsize;
	int yloc = idNumber / ysize;
	return make_pair(xloc, yloc);
}

pair<int, int> Voronoi::getBlock(pair<int, int> XY){
	XY.first /= blockSize;
	XY.second /= blockSize;
	return XY;
}

pair<int, int> Voronoi::getBlockStartXY(int ID){
	int x = (ID % (bigXSize / blockSize)) * blockSize;
	int y = (ID / (bigYSize / blockSize)) * blockSize;
	return make_pair(x, y);
}

pair<int, int> Voronoi::getBlockEndXY(int ID){
	int x = (ID % (bigXSize / blockSize)) * blockSize + blockSize;
	int y = (ID / (bigYSize / blockSize)) * blockSize + blockSize;
	return make_pair(x, y);
}

int  Voronoi::getBlockID(pair<int, int> blocks){
	int blockID = blocks.second * (bigXSize / blockSize) + blocks.first;
	return blockID;
}

void Voronoi::neighborCheck(pair<int, int> xyBlock, pvec& neighbors){
	int x = xyBlock.first;
	int y = xyBlock.second;

	Vector2 tmp[] = { Vector2(x - 1, y + 1), Vector2(x, y + 1), Vector2(x + 1, y + 1),
		Vector2(x - 1, y), Vector2(x, y), Vector2(x + 1, y),
		Vector2(x - 1, y - 1), Vector2(x, y - 1), Vector2(x + 1, y - 1) };
	

	int maxX, maxY;
	pair<int, int> neig;
	for (int i = 0; i < neighbors.size(); i++){
		Vector2 vec = tmp[i];
		maxX = (bigXSize / blockSize) - 1;
		maxY = (bigYSize / blockSize) - 1;
		if (vec.x < 0){
			vec.x = maxX;
		}
		if (vec.y < 0){
			vec.y = maxY;
		}
		if (vec.x > maxX){
			vec.x = 0;
		}
		if (vec.y > maxY){
			vec.y = 0;
		}

		tmp[i] = vec;
	}

	neighbors.clear();
	for (int i = 0; i < 9; i++){
		neighbors.push_back(make_pair(tmp[i].x,tmp[i].y));
	}
}

void Voronoi::xyPoints(pvec& numList){
	pair_i XY;
	//int xB, yB;
	pair_i blocks;
	for (int number = 0; number < int(numList.size()); number++){
		XY = numList[number];
		blocks = getBlockStartXY(number);
		XY.first += blocks.first;
		XY.second += blocks.second;
		numList[number] = XY;
	}
}

void Voronoi::showIt(char* color = "color", bool pointsOn = false, bool border = false){
	pair_i start, end;
	for (int number = 0; number < int(pointsList.size()); number++){
		start = getBlockStartXY(number);
		end = getBlockEndXY(number);
	}
}



template<class T> 
T Voronoi::maximum(T* list, int numElements)
{
	T biggest=NULL;
	for (int i = 0; i < numElements; ++i){
		biggest = max(list[i], biggest);
	}
	return biggest;
}

void Voronoi::draw(){
	int type = 1;
	float  maxNumber = maximum(pixelColorNumbers, bigXSize*bigYSize) * (1.0 - clip);
	float  colorSpread = 255.0 / maxNumber;
	pair<int, int> XY;
	double color;

	for (int pixel = 0; pixel < bigXSize*bigYSize; ++pixel){
		XY = getLocation(pixel);
		color = double(colorSpread * pixelColorNumbers[pixel] / 255.0);
		if (negative != false){
			color = 1.0 - color;
		}
		if (XY.first < xsize && XY.second < ysize){
			if (m_inputMap == NULL)
				Alti.writeAltitude(XY.first, XY.second, color);
			else
				m_inputMap[XY.first*ysize + XY.second] = color;
		}

		if (pixel%bigXSize == 0){
			dialogProgress += 100.0 / bigXSize / 2.0;
			dialog->Update(int(dialogProgress));
		}
	}

	dialog->Destroy();
}



void Voronoi::showProgressbar(void){
	dialog = new wxProgressDialog(wxT("Voronoi"),
		wxT("Voronoi"), 100, NULL, wxPD_AUTO_HIDE);
}



Voronoi::~Voronoi(void){
	delete[] pixelColorNumbers;
}
