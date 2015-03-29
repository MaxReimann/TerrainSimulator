#include "perturbation.h"
#include "globals.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Perturbation::Perturbation(float m)
{
	max_p = m;
	xsize = Alti.getXSize();
	ysize = Alti.getYSize();
	noiseMapX = new double[xsize * ysize + 1];
	noiseMapY = new double[xsize * ysize + 1];
	copyMap = new double[xsize * ysize + 1];
}

void Perturbation::apply(int noiseMethod)
{
	showProgressbar();
	
	if (noiseMethod == 0 )
	{
		Alti.subdivision(0.3,noiseMapX);
		dialog->Update(25);
		Alti.subdivision(0.3,noiseMapY);
		dialog->Update(50);
	}
	else if (noiseMethod == 1 )
	{
		Alti.perlin(noiseMapX);
		dialog->Update(25);
		Alti.perlin(noiseMapY);
		dialog->Update(50);
	}

	float f = 50.0;
	float xnoise, ynoise;
	int xLow, xHigh;
	int yLow, yHigh;
	float xFrac,yFrac;
	float intpD, intpU ;
	double result;
	for (int x = 0; x < xsize;++x) {
		for (int y = 0; y < ysize;++y) {
			//xnoise is shift, max_p is maximal shift on map
			xnoise = x + noiseMapX[x*ysize+y] * max_p * xsize ;
			xLow = int(xnoise);
			xHigh = int(xnoise) +1;
			xFrac = xnoise - xLow;
			
			ynoise = y + noiseMapY[x*ysize+y] * max_p * ysize ;
			yLow = int(ynoise);
			yHigh = int(ynoise) +1;
			yFrac = ynoise - yLow;
			
			/*interpolation scheme:
			x_lo/y_hi---intpU-----x_hi/y_hi
			|			 |			|
			|			 v			|
			|		   result		|
			|			 ^          |
			|			 |			|
			x_lo/y_lo---intpD----x_hi/y_lo
			*/

			intpD = cosin_interpolate( getWrapped(xLow,yLow), getWrapped(xHigh,yLow), xFrac);
			intpU = cosin_interpolate( getWrapped(xLow,yHigh), getWrapped(xHigh,yHigh), xFrac);
			result = cosin_interpolate(intpD,intpU,yFrac);
			copyMap[x*ysize+y] = result;
		}
		if (x % 10 == 0){
			f += 100.0/(xsize*ysize) *  10.0 / 0.5;
			dialog->Update(int(f));
		}
	}

	for (int x = 0; x < xsize;++x) {
		for (int y = 0; y < ysize;++y) {
			Alti.writeAltitude(x,y,copyMap[x*ysize+y]);
		}
	}
	dialog->Destroy();

}
Perturbation::~Perturbation()
{
	delete [] noiseMapX;
	delete [] noiseMapY;
	delete [] copyMap;
}

void Perturbation::showProgressbar(void){
	dialog = new wxProgressDialog(wxT("Perturbation"),
		wxT("Progress of Perturbation"),100,NULL, wxPD_AUTO_HIDE );
}

double Perturbation::getWrapped(int x, int y)
{
	if (x < 0 || x >= xsize || y < 0 || y >= ysize) 
	{
		return (Alti.getAltitude((y + ysize) % ysize,(x + xsize) % xsize ));
	} 
	else 
	{
		return (Alti.getAltitude(x,y));
	}
}


double inline Perturbation::cosin_interpolate(double a,double b,double x){
    double ft = x * 3.1415927;
    double f = ( 1 - cos(ft)) * 0.5;
    return a*(1-f) + b*f;
}

