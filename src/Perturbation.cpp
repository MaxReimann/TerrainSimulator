#include "Perturbation.h"
#include "gl_object.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Perturbation::Perturbation(float m)
{
	max_p = m;
	xsize = Alti.getxsize();
	ysize = Alti.getysize();
	noise_map_x = new double [xsize * ysize + 1];
	noise_map_y = new double [xsize * ysize + 1];
	copy_map = new double [xsize * ysize +1 ];

}

void Perturbation::Apply(int Noise_method)
{
	Progressbar();
	
	if (Noise_method == 0 )
	{
		Alti.Inp_subdivision(0.3,noise_map_x);
		dialog->Update(25);
		Alti.Inp_subdivision(0.3,noise_map_y);
		dialog->Update(50);
	}
	else if (Noise_method == 1 )
	{
		Alti.Inp_Perlin(noise_map_x);
		dialog->Update(25);
		Alti.Inp_Perlin(noise_map_y);
		dialog->Update(50);
	}

	float f = 50.0;
	float xnoise, ynoise;
	int x_lo, x_hi;
	int y_lo, y_hi;
	float x_frac,y_frac;
	float val1, val2 ;
	double result;
	for (int x = 0; x < xsize;++x) {
		for (int y = 0; y < ysize;++y) {
			/* xnoise ist der versetungswert, max_p die maximale verschiebenung auf der karte mit der
			kantengröße xsize*/
			xnoise = x + noise_map_x[x*ysize+y] * max_p * xsize ;
			x_lo = int(xnoise);
			x_hi = int(xnoise) +1;
			x_frac = xnoise - x_lo;
			
			ynoise = y + noise_map_y[x*ysize+y] * max_p * ysize ;
			y_lo = int(ynoise);
			y_hi = int(ynoise) +1;
			y_frac = ynoise - y_lo;
			
			/*interpolation scheme:
			x_lo/y_hi---val2-----x_hi/y_hi
			|			 |			|
			|			 v			|
			|		   result		|
			|			 ^          |
			|			 |			|
			x_lo/y_lo---val1----x_hi/y_lo
			*/
			/*punkte werden um verzerrung verschoben, % schreibt die punkte auf die andere seite wenn karte zu klein*/



			//val1 = cosin_interpolate( Alti.getaltitude(x_lo%xsize,y_lo%ysize), Alti.getaltitude(x_hi%xsize,y_lo%ysize), x_frac);
			//val2 = cosin_interpolate( Alti.getaltitude(x_lo%xsize,y_hi%ysize), Alti.getaltitude(x_hi%xsize,y_hi%ysize), x_frac);
			val1 = cosin_interpolate( GetWrap(x_lo,y_lo), GetWrap(x_hi,y_lo), x_frac);
			val2 = cosin_interpolate( GetWrap(x_lo,y_hi), GetWrap(x_hi,y_hi), x_frac);
			result = cosin_interpolate(val1,val2,y_frac);
			copy_map[x*ysize+y] = result;
		}
		if (x % 10 == 0){
			f += 100.0/(xsize*ysize) *  10.0 / 0.5; /*haelfte wurde schon verarbeitet*/
			dialog->Update(int(f));
		}
	}
	for (int x = 0; x < xsize;++x) {
		for (int y = 0; y < ysize;++y) {
			Alti.Alt(x,y,copy_map[x*ysize+y]);
		}
	}
	dialog->Destroy();

}
Perturbation::~Perturbation()
{
	delete [] noise_map_x;
	delete [] noise_map_y;
	delete [] copy_map;
}

void Perturbation::Progressbar(void){
	dialog = new wxProgressDialog(wxT("Perturbation"),
		wxT("Fortschritt der Perturbation"),100,NULL, wxPD_AUTO_HIDE );
}

double Perturbation::GetWrap(int x, int y)
{
	//x_ch sind die x_lo und x_hi und x ist das schleifen x
/*
	if (x>= xsize && y < ysize)
	{
		//return (Alti.getaltitude(x,y_ch)*3 + Alti.getaltitude(x_ch%xsize,y_ch%ysize)/3.0)/2.0;
		return Alti.getaltitude(xsize,y_ch);
	}
	else if (x< xsize && y >= ysize)
	{
		//return (Alti.getaltitude(x_ch,y)*3 + Alti.getaltitude(x_ch%xsize,y_ch%ysize)/3.0)/2.0;
		return Alti.getaltitude(x_ch,ysize);
	}
	else if (x >= xsize && y >= ysize)
	{
		//return (Alti.getaltitude(x,y)*3.0 + Alti.getaltitude(x_ch%xsize,y_ch%ysize)/3.0)/2.0;
		return Alti.getaltitude(xsize,ysize);
	}
	else
	{
		return Alti.getaltitude(x_ch,y_ch);
	}
	*/
	if (x < 0 || x >= xsize || y < 0 || y >= ysize) 
	{
		return (Alti.getaltitude((y + ysize) % ysize,(x + xsize) % xsize ));
	} 
	else 
	{
		return (Alti.getaltitude(x,y));
	}
}



double inline Perturbation::cosin_interpolate(double a,double b,double x){
    double ft = x * 3.1415927;
    double f = ( 1 - cos(ft)) * 0.5;
    return a*(1-f) + b*f;
}

//void inline Perturbation::Wrap_pixel(


