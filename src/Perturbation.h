#ifndef  _PERTURBATION_H
#define _PERTURBATION_H

#include "imports.h"

#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
#endif


class Perturbation {
	public:
		float max_p; //maximale perturbation
		double *noise_map_x;
		double *noise_map_y;
		double *copy_map;
		int xsize;
		int ysize;
		wxProgressDialog* dialog;


		Perturbation(float m);
		void Apply(int Noise_method);
		~Perturbation();
		void Progressbar();
		double cosin_interpolate(double a,double b,double x);
		double GetWrap(int x, int y);
		
};
#endif
