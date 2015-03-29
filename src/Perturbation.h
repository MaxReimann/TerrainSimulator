#ifndef  _PERTURBATION_H
#define _PERTURBATION_H

#include "imports.h"

#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
#endif


class Perturbation {
public:
	float max_p; //max perturbation
	double *noiseMapX;
	double *noiseMapY;
	double *copyMap;
	int xsize;
	int ysize;
	wxProgressDialog* dialog;


	Perturbation(float m);
	void apply(int Noise_method);
	~Perturbation();
	void showProgressbar();
	double cosin_interpolate(double a, double b, double x);
	double getWrapped(int x, int y);

};
#endif
