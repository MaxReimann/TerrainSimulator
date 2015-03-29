#ifndef  _VORONOI_H
#define _VORONOI_H

#include "perturbation.h"

#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
#endif


typedef list< pair<int,int> > plist;
typedef vector< pair<int,int> > pvec;
typedef pair<int,int> pair_i;

class Voronoi {
public:
	int xsize;
	int ysize;
	int bigXSize;
	int bigYSize;
	int* pixelColorNumbers;
	wxProgressDialog* dialog;
	int totalBlocks;
	int fValue;
	int blockSize;
	pvec pointsList;
	bool negative;
	float  clip;
	int seed;
	double dialogProgress;
	int numToBlocksize;
	vector<float> pixelDist;
	vector<float> shortest;
	vector<float >::iterator minNum;
	double *m_inputMap;
	
	




	Voronoi(int _fValue, int _blockSize,
		int* _pointsList = NULL , int _seed=0, bool _negative=false, float _clip=0);
	~Voronoi(void);

	void calculate(double Inp_map[]=NULL,int distType=1,float coefficients[] =NULL,int elements=0);//1=Linear
	void range(int range,int arr[]);
	void pixelColorNumerate();
	int findNearestPoints(int x,int y,pvec& neighbor,int distType,float coefficients[]  = NULL,int elements = 0);
	void initRandom();
	int random(int a, int b);
	float length(int x1, int y1, int x2,int y2,int method=1);
	void generatePoints(int totalBlocks);
	pair<int,int> getBlock(pair<int,int> XY);
	pair<int,int> getLocation(int idNumber);
	pair<int,int> getLocation_Alti(int idNumber);
	pair<int,int> getBlockStartXY(int ID);
	pair<int,int> getBlockEndXY(int ID);
	int getBlockID(pair<int,int> blocks);

	void neighborCheck(pair<int,int> XYblock,pvec&	 neighbors);

	void xyPoints(pvec& numList);
	void showIt(char* color, bool pointsOn,bool border);
	void showProgressbar();


	template<class T> T maximum(T* liste, int numElements);
	void draw();
};
#endif
