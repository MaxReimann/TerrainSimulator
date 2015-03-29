#pragma once
#include "imports.h"

struct WaterConstants
{
	float KC;
	float KD;
	float KE;
	float KS;
	float KR;
	float Pipe_length_x;
	float Pipe_length_y;
	float Radius;
	float Water_Amount;
	float DT;
	float Min_alpha;
	bool Rain_on;
	bool Watersource_on;
	int Iterations;
};



  struct WaterVertex
  {
	  double z;
	  	double Waterdistribution;
		double d;
		double d1;
		double d2;
		double fluxLeft, fluxRight, fluxBottom, fluxTop;
		double Volume;
		double Vx, Vy;
		double sediment_amount;
		double sediment_amount1;
		double w; // w= d
  };

class WaterSimulation
{
public:
	WaterSimulation(const struct WaterConstants *constants,int _method);
	~WaterSimulation();
	void waterflowDistribution(double Water_Amount);
	void rainDistribution();
	void increaseWater();
	void simLoop();
	void fluxSim();
	void surfaceAndVelocity();
	void erosionDeposition();
	void sedimentTransport();
	void evaporation();
	void normalizeHeight();
	void visualizeWater2D();
	void visualizeSediment2D();


	void strictSolver();
	double averageHeight(int x,int y);
	double dTotal(int x, int y);
	void fastSolver();

	void hydraulicErosion();


	void showProgressbar();

private:
	WaterVertex *vert_array;
	
	std::fstream errorstream;
	double pipeArea;
	double pipeLength; 
	int ysize;
	int xsize;
	double diffHeightL, diffHeightR, diffHeightT, diffHeightB;//delta height: left right top bottom
	double dAvg;
	double waterpassX, waterpassY;
	double inflowSum, outflowSum;
	wxProgressDialog* dialog;
	float RADIUS;
	float KC, KD, KS, KE, KR;
	float waterAmount;
	float PIPE_LENGTH;
	float DT;
	int iterations;
	wxImage tempImg;
	int normalized;
	bool RainOn;
	bool WatersourceOn;
	time_t seconds;
	int method;



};
