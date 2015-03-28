#include "simulation.h"
#include "tga.h"

//#include "load_obj.h"
//#include "global_class_function.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//Makro um eine Hoenkarte in ein eindimensionales Array zu packen
#define ALT(x,y) (map[(x)*ysize+(y)])
#define ALT_2(x,y) (inp_map[(x)*ysize+(y)])

#define SEDIMENT(x,y) (sediment_map[(x)*ysize+(y)])

using namespace std;


//Basismethoden
AltitudeMap::AltitudeMap(int _xsize, int _ysize)
: xsize(_xsize),ysize(_ysize),sealevel(0.0) ,
FlagX(static_cast<int>((double)_xsize / 2.0)), FlagY(static_cast<int>((double)_ysize / 2.0))
{
	change = 0.0;
	errorstream.open("error_sim.txt");
	init_random(); //random seed setzen
	map =  new   double [xsize*ysize]; //die heohenkarte
	sediment_map = new   double [xsize*ysize]; //karte mit sedimetn_ablagen
	
	
	DiffMap = new double** [xsize]; //dreidimensionales array
	
	for (int i = 0; i < xsize; i++)
	{
		DiffMap[i] = new double* [ysize]; // speicher 2 dimension
		for (int j = 0; j < ysize; j++)
		{
			DiffMap[i][j] = new double [4]; // speicher 3 dimension anfordern
		}
	}


	preview_image = new wxImage(xsize,ysize,true);//image initalisieren
	for ( int x = 0;x < xsize; ++x){
		for (int y=0; y < ysize;++y)
		{
			ALT(x,y) = 0.0;
		}
	}
	showflag = true; // der positionsmarker

}

AltitudeMap::AltitudeMap(void) {} //default konstruktor

AltitudeMap::~AltitudeMap(void){
	if (terrainColors){delete [] terrainColors;}
	if (terrainNormals){delete [] terrainNormals;}
	delete [] map;
	delete [] sediment_map;
	delete_diffMap();

	errorstream.close();
	preview_image->Destroy();
	delete preview_image;
}

void AltitudeMap::delete_diffMap()
{
	
	for (int i = 0; i < xsize; i++)
	{
		for (int j = 0; j < ysize; j++)
		{
			delete DiffMap[i][j];// speicher 3 dimension löschen
		}
	}
	for (int i = 0; i < xsize; i++)
	{
		delete [] DiffMap[i];
	}
	
	delete [] DiffMap;
}

int AltitudeMap::getxsize() {
	return xsize;
}

int AltitudeMap::getysize() {
	return ysize;
}

double AltitudeMap::getsealevel() {
	return sealevel;
}
void AltitudeMap::getsealevel(double sl) {
	sealevel = sl;
}
double AltitudeMap::getaltitude(int x,int y) {
	return ALT(x%xsize,y%ysize);
}
void  AltitudeMap::Alt(int x,int y, double z){
	if(x<xsize && y<ysize){
	map[(x)*ysize+(y)] = z;
	}
}
void  AltitudeMap::add(int x,int y, double z){
	map[x*ysize+y] = map[(x)*ysize+(y)]+ z;
}
void  AltitudeMap::multiply(int x,int y, double z){
	ALT(x,y) *= z + 0.5;
}

// Subdivision
void AltitudeMap::subdivision(double coeff,double lt,double rt, double lb, double rb) {
	// coeff im intervall 0.0>0.5 
	//hoehe der ecken: lt = lefttop,rt = righttop,lb=leftbottom,rb=rightbottom
	ALT(0,0) = lt; //map[0] = ALT(0,0) : enstpricht der linken oberen ecke
	ALT(xsize-1,0) = rt;
	ALT(0,ysize-1) = lb;
	ALT(xsize-1,ysize -1) = rb;
	
	for ( int x = 0;x < xsize; ++x){
		for (int y=0; y < ysize;++y)
		{
			ALT(x,y) = 0.0;
			SEDIMENT(x,y) = 0.0;
		}
	}// Alles auf 0 setzen


	//rekursive Funktion wird ausgefuehrt, linke obere und rechte untere ecke werden uebergeben
	init_random(); //random seed setzen

	subdiv_private(coeff,0,0,xsize - 1,ysize - 1);
	for ( int x = 0;x < xsize; ++x){
		for (int y=0; y < ysize;++y)
		{
			ALT(x,y) += 0.4;
		}
	}


	normalize();



}



void AltitudeMap::Inp_subdivision(double coeff,double *_inp_map) {
	/* methode auf irgendein terrain, z.b als noisefunktion für perturbance filter*/
	//hoehe der ecken: lt = lefttop,rt = righttop,lb=leftbottom,rb=rightbottom
	inp_map = _inp_map;
	for ( int x = 0;x < xsize; ++x){
		for (int y=0; y < ysize;++y)
		{
			ALT_2(x,y) = 0.0;
		}
	}
	init_random(); //random seed setzen
	ALT_2(0,0) = 0.0; //map[0] = ALT(0,0) : enstpricht der linken oberen ecke
	ALT_2(xsize-1,0) = 0.0;
	ALT_2(0,ysize-1) = 0.0;
	ALT_2(xsize-1,ysize -1) = 0.0;
	
	for ( int x = 0;x < xsize; ++x){
		for (int y=0; y < ysize;++y)
		{
			ALT_2(x,y) = 0.0;
		}
	}// Alles auf 0 setzen


	//rekursive Funktion wird ausgefuehrt, linke obere und rechte untere ecke werden uebergeben
	subdiv_inp_map(coeff,0,0,xsize - 1,ysize - 1);


	for (int x = 0; x < xsize;++x) { //normalisieren, wichtig, da sonst uebertretungegn stattfinden, die die karte sabotieren
			for (int y = 0; y < ysize;++y) {
				ALT(x,y) += 0.4;
				if (ALT_2(x,y) < 0.0) {
					ALT_2(x,y) = 0.0;
				}
				else if (ALT_2(x,y) > 1.0) {
					ALT_2(x,y) = 1.0;}
			}
		  }

}
void AltitudeMap::init_random(void){
	unsigned long init[4]={static_cast<long>(random(100,300)),static_cast<long>( random(100,300)),static_cast<long>(random(100,300)),static_cast<long>(random(100,300))}, length=4;
    init_by_array(init, length);
}

double inline AltitudeMap::random(double a, double b){
	double result = a + (genrand_real2() * (abs(a) + abs(b))); // gibt eine zahl im intervall a,b
	return result;//genrand
	

	// gibt eine zahl im intervall a,b
	/*
	srand( TT800() + (unsigned) time(NULL) ); 
	a *= 10;
	b *= 10;
	double c = a + ( rand() % (int)( b - a + 1 ) ) ;
	return ( c /10.0);
	*/
}
void  AltitudeMap::error_output(){
	for (int i = 0; i < xsize *  ysize;++i){
		errorstream << map[i] << " ";
	}
	errorstream <<  "end of map \n \n \n \n \n \n \n \n \n" << std::endl;
}

void inline AltitudeMap::subdiv_inp_map(double coeff,int x1, int y1, int x2, int y2){
	
	//wenn es zwischen den Ecken keine Punkte mehr gibt, wird die Rekursion beendet
	if(abs(x1-x2) <= 1 || abs(y1-y2) <= 1) {
		return; }
	

	// Mittelpunkt zwischen (x1,y1) und (y1,y2)
	int xmiddle = (x1 + x2)/2;
	int ymiddle = (y1 + y2)/2;

	// neue Hoehe = Hoehe in der Mitte des Abschnittes zwischen Punkten + der zufaellige Wert aus 
	// dem Intervall <-1,+1> * coeff
	if  (ALT_2(xmiddle,y1) == 0){ // wenn hebung schon definiert wurde keine neue berechnen
		ALT_2(xmiddle,y1) = (ALT_2(x1,y1) + ALT_2(x2,y1))/2.0 + random(-0.5,0.5) * coeff;}

	if (ALT_2(x1,ymiddle) == 0){
		ALT_2(x1,ymiddle) = (ALT_2(x1,y1) + ALT_2(x1,y2))/2.0 + random(-0.5,0.5) * coeff;}

	if (ALT_2(xmiddle,ymiddle) == 0){
		ALT_2(xmiddle,ymiddle) = (ALT_2(x1,y1) + ALT_2(x2,y2))/2.0 + random(-0.5,0.5) * coeff;}

	if (ALT_2(xmiddle,y2) == 0){
		ALT_2(xmiddle,y2) = (ALT_2(x1,y2) + ALT_2(x2,y2))/2.0 + random(-0.5,0.5) * coeff;}

	if (ALT_2(x2,ymiddle) == 0){
		ALT_2(x2,ymiddle) = (ALT_2(x2,y1) + ALT_2(x2,y2))/2.0 + random(-0.5,0.5) * coeff;}
	
	// verkleintert die hebung mit jedem rekrursionschritt-> weicheres gelaende
	coeff *= coeff;
	
	//rekursive aufrufe
	subdiv_inp_map(coeff,x1,y1,xmiddle,ymiddle);
	subdiv_inp_map(coeff,xmiddle,y1,x2,ymiddle);
	subdiv_inp_map(coeff,x1,ymiddle,xmiddle,y2);
	subdiv_inp_map(coeff,xmiddle,ymiddle,x2,y2);
	
}


void inline AltitudeMap::subdiv_private(double coeff,int x1, int y1, int x2, int y2){
	//coeff ist Grad der Gelaendeunterscheidung intervall 0-0.5 
	//hohe werte ergeben fast zufällige werte
	/*
	Subdivion
	o------o		o---o---o
	|	   |		|   |   |
	|	   | ---->  o---o---o
	|      |		|   |   |
	o------o		O---o---O
	*/


	//wenn es zwischen den Ecken keine Punkte mehr gibt, wird die Rekursion beendet
	if(abs(x1-x2) <= 1 || abs(y1-y2) <= 1) {
		return; }
	

	// Mittelpunkt zwischen (x1,y1) und (y1,y2)
	int xmiddle = x1 + (int)floor(double (x2-x1)/2.0); 
	int ymiddle = y1 + (int)floor(double(y2-y1)/2.0);

	// neue Höhe = Höhe in der Mitte des Abschnittes zwischen Punkten + der zufällige Wert aus 
	// dem Intervall <-1,+1> * coeff
	if  (ALT(xmiddle,y1) == 0){ // wenn Hebung schon definiert wurde keine neue berechnen
		ALT(xmiddle,y1) = (ALT(x1,y1) + ALT(x2,y1))/2.0 + random(-0.5,0.5) * coeff;}

	if (ALT(x1,ymiddle) == 0){
		ALT(x1,ymiddle) = (ALT(x1,y1) + ALT(x1,y2))/2.0 + random(-0.5,0.5) * coeff;}

	if (ALT(xmiddle,ymiddle) == 0){
		ALT(xmiddle,ymiddle) = (ALT(x1,y1) + ALT(x2,y2))/2.0 + random(-0.5,0.5) * coeff;}

	if (ALT(xmiddle,y2) == 0){
		ALT(xmiddle,y2) = (ALT(x1,y2) + ALT(x2,y2))/2.0 + random(-0.5,0.5) * coeff;}

	if (ALT(x2,ymiddle) == 0){
		ALT(x2,ymiddle) = (ALT(x2,y1) + ALT(x2,y2))/2.0 + random(-0.5,0.5) * coeff;}
	
	// Verkleintert die Erhebung mit jedem Rekursionschritt-> weicheres gelaende
	coeff *= coeff;
	
	//Rekursive aufrufe
	subdiv_private(coeff,x1,y1,xmiddle,ymiddle);
	subdiv_private(coeff,xmiddle,y1,x2,ymiddle);
	subdiv_private(coeff,x1,ymiddle,xmiddle,y2);
	subdiv_private(coeff,xmiddle,ymiddle,x2,y2);
	
}
void AltitudeMap::randomize(double r) {
	long double xrange = 0, yrange = 0, mrange = 0,  rand = 0;
	double f;
	init_random(); //bei jedem start eine andere seed nutzen
	bool show_dialog = false;
	if (xsize * ysize * r > 5000) { //nur progressbar erschaffen, wenn werte hoch genug
		showProgressbar();
		show_dialog = true;
		f = 0;} 

	for ( int i = 0; i < (r); i++) {
		if (show_dialog == true) {
			f += 100.0/r;
			dialog->Update((int)f); }
		for (int x = 0; x < xsize;++x) {
			for (int y = 0; y < ysize;++y) {
				xrange = abs( ALT((x-1 < 0) ? (0) : (x-1),y) - ALT((x+1 > xsize) ? (xsize) : (x+1),y) ); //nimmt die werte des linken und rechten nachbarn (wenn nicht groesser als array)
				yrange = abs(ALT(x,(y+1 > ysize) ? (ysize) : (y+1)) - ALT(x,(y-1 < 0) ? (0) : (y-1)));
				mrange = (xrange + yrange) / 100.0 * r / 10.0 ;
				rand = ((random(mrange,2*mrange)-2*mrange));
				ALT(x,y) = ALT(x,y) + rand ;
			}
	  }
	}
	normalize2();
	/// test block ////
	//std::string path("testcube.obj");
	//Load_obj("testcube.obj");

	if (show_dialog == true) { dialog->Destroy(); } //fenster zerstoeren, wird nicht automatisch gemacht
}

void AltitudeMap::randomize2( double r ) {
	double dist = 0;
	if (r > xsize) {r = static_cast<double>(xsize) - 1.0;}
	if (r > ysize) {r = static_cast<double>(ysize) - 1.0;}

	init_random(); //bei jedem start eine andere seed nutzen
	double strength = 0.5;
	// fuer randomize in x richtung
	
		for (int y = 0; y < ysize; ++y) {
			for (int x = 0; x < xsize;++x){
				if (((x * y)  % static_cast<int>(r))  == 0) //jedes rte element
				{
					change = abs(random(strength,strength*2)) / 50.0; //r wird abgezogen, damit auch - werte, (bei insetzen von - werten gibst fehler)
					dist = 0;
					double b = change;
					ALT(x,y) = ALT(x,y) + change;
					
					for (int i = 1; i <= static_cast<int>(r);i++) {
						if (b == 0) {break;}
						b *= 0.99 ;//(b > 0) ? (b-(1/(b*i))) : (b+(1/(b*i)));
						//< als ysize /xsize wichtig
						if (x-i >= 0)
							ALT(x-i,y) -= b; // werte in linksrichtun
						if (x+i < xsize)
							ALT(x+i,y) -= b; // werte in linksrichtung
						if (y+ i < ysize)
							ALT(x,y+i) -= b; // werte in linksrichtung
						if (y-i >= 0)
							ALT(x,y-i) -= b; // werte in linksrichtung
						if ((x+i) < xsize && y+i < ysize)
							ALT(x+i,y+i) -= b; // werte in linksrichtung
						if (x-i >= 0 && y-i >= 0)
							ALT(x-i,y-i) -= b; // werte in linksrichtung
						if (x-i >= 0 && y+i < ysize)
							ALT(x-i,y+i) -= b; // werte in linksrichtung
						if (x+i < xsize && y-i >= 0)
							ALT(x+i,y-i) -= b; // werte in linksrichtung
					 }
				  }
			}
		}// schleifen abgeschlossen
	//werte normalisieren
	
	normalize2();
}

// Bodenerosion
void AltitudeMap::erosion(int r,int iter){
	/*r = intervall <1,(Haelfte - 0.5 der Quadratseite  wovon der der Mittelwert berechtnet wird)>,
	r steht fuer Nachbarschaftradius, also wie weit die Erosion reicht
	iter = Anzahl der Erosionsschritte

	kleiner Radius und viele Schritte -> gipfel werden mehr geglaettet
	grosser Radius wenifg schritte -> einheitlicheres terrain
	*/
	
	//Kopie der Hoehenkarte, da die alten Daten mehrmals benutzt werden
	double *copy = new double [xsize * ysize];
	bool show_dialog = false;
	double f;
	double sum = 0.0;
	if (xsize * ysize * iter * r > 5000) { //nur progressbar erschaffen, wenn werte hoch genug
		showProgressbar();
		show_dialog = true;
		f = 0;} 

	
	while(iter > 0){
		--iter;
		for(int x = 0; x < xsize; ++x) {

			if (show_dialog == true) {
				f += 100.0/(xsize * (iter-1)); //(die endzahl muss 100 sein
				dialog->Update((int)f); 
			}
			

			for (int y = 0; y < ysize;++y){ //fuer jeden punkt
				
				/*if (x < r || y < r || x > xsize - r || y > ysize - r){
				sum= ALT(x,y) * float(4*r*r+4*r + 1.0);
				}
				else {sum= 0.0;}*/

				sum= 0.0;
				int offx;
				if(x >= r) { offx = -r; }
				else {offx = -x;}
				for (;(offx <= r) && (offx+x < xsize);++offx){
					int offy; 
					if (y >= r) {offy = -r;}
					else {offy = -y;}
					for (;(offy <= r) && (offy+y < ysize);++offy){
						sum += ALT(x+offx,y+offy);
					}
				}
				copy[x*ysize+y] = sum /float(4*r*r+4.0*r + 1); 
			}
		}
			//am Ende jedes Erosionsschrittes wird der Inhalt der Kopie in 
			// die Hoehenkarte geschrieben
			for (int x = 0; x < xsize; ++x) {
				for ( int y = 0; y < ysize; ++y) {
					ALT(x,y) = copy[x*ysize+y];
					SEDIMENT(x,y) = 0.0;
				}
			}

	}
	normalize2();
	delete [] copy;
	if (show_dialog == true) { dialog->Destroy(); } //fenster zerstoeren, wird nicht automatisch gemacht
}
void AltitudeMap::normalize(){
			for (int x = 0; x < xsize;++x) {
			for (int y = 0; y < ysize;++y) {
				//ALT(x,y) += 0.2;
				if (ALT(x,y) < 0.0) {
					ALT(x,y) = 0.0;
				}
				else if (ALT(x,y) > 1.0) {
					ALT(x,y) = 1.0;}
			}
		  }
}

void AltitudeMap::normalize2(){ //fuer erosion und random
			for (int x = 0; x < xsize;++x) {
			for (int y = 0; y < ysize;++y) {
				if (ALT(x,y) < 0.0) {
					ALT(x,y) = 0.0;
				}
				else if (ALT(x,y) > 1.0) {
					ALT(x,y) = 1.0;}
			}
		  }
}


void AltitudeMap::plateau( int x, int y ,int r ){
	int offx;
	double f;
	bool show_dialog = false;
	if (r * r > 5000) { //nur progressbar erschaffen, wenn werte hoch genug
		showProgressbar();
		show_dialog = true;
		f = 0;} 
	if (r >= xsize) {r=  xsize - 1; }
	if (r >= ysize) {r= ysize -1; }
	if(x >= r) { offx = -r;}
	else {offx = -x; } // offx darf maximal an die kartenraender reichen
	for(;(offx <= r) && (offx+x < xsize);++offx){
		int offy;
		if ( y >= r) { offy = -r;}
		else { offy = -y;}

		if (show_dialog == true) {
				f += 100.0/(abs(offy * offx)); //(die endzahl muss 100 sein
				dialog->Update((int)f); 
		}

		for(;offy <= r && offy+y < ysize;++offy){
			double distance =  sqrt((double)offx*offx + (double)offy*offy) / (double)r;
			if(distance > 1.0) {continue;}
			//die Glaettungskraft haengt von der Entfernung zum Plateauzentrum ab
			ALT(x+offx,y+offy) = distance * ALT(x+offx,y+offy) + (1.0 - distance) * ALT(x,y);
			
			if (x-offx > 0 && y - offy > 0) {
				ALT(x-offx,y-offy) = distance * ALT(x-offx,y-offy) + (1.0 - distance) * ALT(x,y);
			}
		}
	}

	if (show_dialog == true) { dialog->Destroy(); } //fenster zerstoeren, wird nicht automatisch gemacht
}


//Messung des Gelaendeanstiegs
double AltitudeMap::slope_x(int x, int y){
	
	
	/* Berechnung des Antiegs an den Kanten ist nicht moeglich
	deshalb wird ihnen die selben Werte wie deren Nachbarn auf der inneren
	Seite zugewiesen
	*/
	if(x == 0) {++x;}
	else if (x== xsize-1) {--x;}

	if(y == 0) {++y;}
	else if (y == ysize-1){--y;}
	double d = (ALT(x+1,y-1) + ALT(x+1,y+1) - ALT(x-1,y-1) - ALT(x-1,y+1)) / 2.0;
	return d / sqrt(d*d + 1.0/(double)(xsize*xsize) );
}

double AltitudeMap::angle_slope_x(int x, int y){
	if(x == 0) {++x;}
	else if (x== xsize-1) {--x;}

	if(y == 0) {++y;}
	else if (y == ysize-1){--y;}
	double d = (ALT(x+1,y-1) + ALT(x+1,y+1) - ALT(x-1,y-1) - ALT(x-1,y+1));
	return atan(d/2.0);//2.0 = ankathethe (delta_x), d = delta_z = gegenkathete
}

double AltitudeMap::angle_slope_y(int x, int y){
	
	
	/* Berechnung des Antiegs an den Kanten ist nicht moeglich
	deshalb wird ihnen die selben Werte wie deren Nachbarn auf der inneren
	Seite zugewiesen
	*/
	if(x == 0) {++x;}
	else if (x== xsize-1) {--x;}

	if(y == 0) {++y;}
	else if (y == ysize-1){--y;}
	double d = (ALT(x-1,y+1) + ALT(x+1,y+1) - ALT(x-1,y-1) - ALT(x+1,y-1)) / 2.0;
	return atan(d/2.0);//2.0 = ankathethe (delta_x), d = delta_z = gegenkathete

}

	


double AltitudeMap::slope_y(int x, int y){
	
	
	/* Berechnung des Antiegs an den Kanten ist nicht moeglich
	deshalb wird ihnen die selben Werte wie deren Nachbarn auf der inneren
	Seite zugewiesen
	*/
	if(x == 0) {++x;}
	else if (x== xsize-1) {--x;}

	if(y == 0) {++y;}
	else if (y == ysize-1){--y;}
	double d = (ALT(x-1,y+1) + ALT(x+1,y+1) - ALT(x-1,y-1) - ALT(x+1,y-1)) / 2.0;
	return d / sqrt(d*d + 1.0/(double)(ysize*ysize) );

}




int AltitudeMap::terrainLoadFromImage(char *filename, int normals) {
	
	std::string path = std::string(filename);

	if (path.find(".tga") == string::npos ){ //wenn die dateiendung nicht gefunden wurde
		bool res = load_image(filename,normals); //versuchen andere Dateiformate zu laden
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
	//if (terrainHeights != NULL)
	//	terrainDestroy();
	delete [] map;
		
/* load the image, using the tgalib */
	info = tgaLoad(filename);

/* check to see if the image was properly loaded
   remember: only greyscale, RGB or RGBA noncompressed images */
	if (info->status != TGA_OK)
		return(TERRAIN_ERROR_LOADING_IMAGE);

/* if the image is RGB, convert it to greyscale
   mode will store the image's number of components */
	mode = info->pixelDepth / 8;
	if (mode == 3) {
		tgaRGBtoGreyscale(info);
		mode = 1;
	}
	
/* set the width and height of the terrain */
	xsize = info->width;
	ysize = info->height;

/* alocate memory for the terrain, and check for errors */
	map = new double [ xsize * ysize ];
	if (map == NULL)
		return(TERRAIN_ERROR_MEMORY_PROBLEM);

/* allocate memory for the normals, and check for errors */
	if (normals) {
		terrainNormals = new double [xsize * ysize  * 3];
		if (terrainNormals == NULL)
			return(TERRAIN_ERROR_MEMORY_PROBLEM);
	}
	else
			terrainNormals = NULL;

/* if mode = RGBA then allocate memory for colors, and check for errors */
	if (mode == 4) {
		terrainColors = new double [xsize * ysize  * 3];
		if (terrainColors == NULL)
			return(TERRAIN_ERROR_MEMORY_PROBLEM);
	}
	else
		terrainColors = NULL;

/* fill arrays */
	for (int i = 0 ; i < ysize; i++)
		for (int j = 0;j < xsize; j++) {

/* compute the height as a value between 0.0 and 1.0 */
			pointHeight = info->imageData[mode*(i*xsize + j)+(mode-1)] / 255.0;
			map[i*xsize + j] = pointHeight;
/* if mode = RGBA then fill the colors array as well */
			if (mode==4) {
				terrainColors[3*(i*xsize + j)]   = (info->imageData[mode*(i*xsize + j)])/255.0;
				terrainColors[3*(i*xsize + j)+1] = (info->imageData[mode*(i*xsize + j)+1])/255.0;
				terrainColors[3*(i*xsize + j)+2] = (info->imageData[mode*(i*xsize + j)+2])/255.0;
			}
		}
/* if we want normals then compute them		*/
	//if (normals)
	//	terrainComputeNormals();
/* free the image's memory  */
	tgaDestroy(info);
	
	return TERRAIN_OK; 
}




//alternativ
int AltitudeMap::terrainLoadFromImage(char *filename, int normals, int ungebraucht) {
	
	//tgaInfo *info;
	std::string path = std::string(filename);
	std::string sub = path.substr(path.length()-4,path.length());

	if ( sub.compare(".tga") != 0 ){ //wenn die dateiendung nicht .tga ist
		bool res = load_image(filename,normals); //versuchen andere Dateiformate zu laden
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
	
	
	if (stat = (loadTGA(filename, tgaFile) != 0 ))
	{
		return stat;
	}


/* if a terrain already exists, destroy it. */
	delete [] map;


/* if the image is RGB, convert it to greyscale
   mode will store the image's number of components */
	
	//image type either 2 (color) or 3 (greyscale)
	mode = tgaFile.type[2];
	if (mode == 3) {
		tgaRGBtoGreyscale(tgaFile);
		mode = 1;
	}
	
/* set the width and height of the terrain */
	xsize = tgaFile.width;
	ysize = tgaFile.height;

/* alocate memory for the terrain, and check for errors */
	map = new double [ xsize * ysize ];
	if (map == NULL)
		return(TERRAIN_ERROR_MEMORY_PROBLEM);

/* allocate memory for the normals, and check for errors */
	if (normals) {
		terrainNormals = new double [xsize * ysize  * 3];
		if (terrainNormals == NULL)
			return(TERRAIN_ERROR_MEMORY_PROBLEM);
	}
	else
			terrainNormals = NULL;

/* if mode = RGBA then allocate memory for colors, and check for errors */
	if (mode == 4) {
		terrainColors = new double [xsize * ysize  * 3];
		if (terrainColors == NULL)
			return(TERRAIN_ERROR_MEMORY_PROBLEM);
	}
	else
		terrainColors = NULL;

/* fill arrays */
	for (int i = 0 ; i < ysize; i++)
		for (int j = 0;j < xsize; j++) {

/* compute the height as a value between 0.0 and 1.0 */
			pointHeight = tgaFile.data[mode*(i*xsize + j)+(mode-1)] / 255.0;
			map[i*xsize + j] = pointHeight;
/* if mode = RGBA then fill the colors array as well */
			if (mode==4) {
				terrainColors[3*(i*xsize + j)]   = (tgaFile.data[mode*(i*xsize + j)])/255.0;
				terrainColors[3*(i*xsize + j)+1] = (tgaFile.data[mode*(i*xsize + j)+1])/255.0;
				terrainColors[3*(i*xsize + j)+2] = (tgaFile.data[mode*(i*xsize + j)+2])/255.0;
			}
		}
/* if we want normals then compute them		*/
	//if (normals)
	//	terrainComputeNormals();
	
	return TERRAIN_OK; 
}


void AltitudeMap::showProgressbar(void){
	dialog = new wxProgressDialog(wxT("Fortschritt der Operation"),
		wxT("Fortschritt der Operation"),100,NULL, wxPD_AUTO_HIDE );
}

void AltitudeMap::showProgressbar(wxString Text){
	dialog = new wxProgressDialog(Text,
		Text,100,NULL, wxPD_AUTO_HIDE );
}




void AltitudeMap::Scale_Terrain(double factor){
	wxImage original = Make_Preview_Image();
	delete_diffMap(); //wichtig,dass vor neuem xsize
	xsize = int(xsize * factor);
	ysize = int(ysize * factor);
	if (xsize < 4 || ysize < 4){
		return; // kleiner als 4*4 px gibt ein schlechtes image
	}
	original.Rescale(xsize+1,ysize+1); //rescale operiert in-place
	delete [] map;
	map = new double [xsize*ysize];
	sediment_map = new double [xsize*ysize];

	DiffMap = new double** [xsize]; //dreidimensionales array
	
	for (int i = 0; i < xsize; i++)
	{
		DiffMap[i] = new double* [ysize]; // speicher 2 dimension
		for (int j = 0; j < ysize; j++)
		{
			DiffMap[i][j] = new double [4]; // speicher 3 dimension anfordern
		}
	}

	
	for (int x = 0; x < xsize; x++){
		for ( int y = 0; y < ysize; y++){
			ALT(x,y) = (double(original.GetRed(x,y)) + double(original.GetBlue(x,y)) + double(original.GetGreen(x,y))) / (255.0 * 3.0); // greyscale wert im intervall zwischen 0 und 1
		}
	}
	original.Destroy();
	reset_Flag(); //die flagge in die mitte, damit keine zugriffsverletzung
	//glPlane->m_totalVertices = xsize * ysize;
}

void AltitudeMap::reset_Flag(void){
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
        n.x = getaltitude(x - 1, z) - getaltitude(x + 1, z);
    else if (x > 0)
        n.x = 2.0f * (getaltitude(x - 1, z) - getaltitude(x, z));
    else
        n.x = 2.0f * (getaltitude(x, z) - getaltitude(x + 1, z));

    if (z > 0 && z < ysize - 1)
        n.z = getaltitude(x, z - 1) - getaltitude(x, z + 1);
    else if (z > 0)
        n.z = 2.0f * (getaltitude(x, z - 1) - getaltitude(x, z));
    else
        n.z = 2.0f * (getaltitude(x, z) - getaltitude(x, z + 1));

    n.y = 2.0f * 16.0f;
    n.normalize();
}


// Die Höhenunterschiede zu den umgebenden Zellen errechnen
// 0 = (x, y+1), 1 = (x-1, y), 2 = (x+1, y), 3 = (x, y-1) 
void AltitudeMap::Diff()
{

	for (int y = 1; y < ysize-1; y++)
	{
		for (int x = 1; x < xsize-1; x++)
		{

			DiffMap[x][y][0] = 0;
			DiffMap[x][y][1] = 0;
			DiffMap[x][y][2] = 0;
			DiffMap[x][y][3] = 0;

			if (ALT(x,y) > ALT(x,y+1))
			{
            	 DiffMap[x][y][0] = ALT(x,y) - ALT(x,y+1);
		    }
			if (ALT(x,y) > ALT(x-1,y))
			{            
				DiffMap[x][y][1] = ALT(x,y) - ALT(x-1,y);
		    }
			if (ALT(x,y) > ALT(x+1,y))
			{
            	 DiffMap[x][y][2] = ALT(x,y) - ALT(x+1,y);
		    }
			if (ALT(x,y) > ALT(x,y-1))
			{
            	 DiffMap[x][y][3] = ALT(x,y) - ALT(x,y-1);
		    }

		}
	}
}

//Run thermalerosion
// MaterialMap is generated using perlinnoise
void AltitudeMap::ThermalErosion(int iter, double Talus)
{
	float Difftotal;
	float Diffmax;
	float c = 0.5; //konstante mit relativ guten auswirkungen
	float T = Talus;///*T = Talus angle*/
	int duration = iter; // test
	showProgressbar();
	float progress;


	for (int m = 0; m < duration; m++) //Gesamtdurchläufe
	{
		
		Diff();

		for (int y = 1; y < ysize-1; ++y)
		{
			for (int x = 1; x < xsize-1; ++x)
			{
				Difftotal = 0.0f;
				Diffmax = T;

				for (int n = 0; n < 4; n++)
				{
					if (DiffMap[x][y][n] > T)
					{
						Difftotal += DiffMap[x][y][n];
						
						if (DiffMap[x][y][n] >  Diffmax)
						{
							Diffmax = DiffMap[x][y][n];
						
						}

					}
				}   		
	
				if (Difftotal != 0) // Wenn unterschied vorhanden, verschiebe material
				{
				ALT(x,y+1) = ALT(x,y+1) +  c * ( Diffmax - T ) * (DiffMap[x][y][0] / Difftotal); //Material zu den tieferen Punkten verschieben
				ALT(x-1,y) = ALT(x-1,y) +  c * ( Diffmax - T ) * (DiffMap[x][y][1] / Difftotal);
				ALT(x+1,y) = ALT(x+1,y) +  c * ( Diffmax - T ) * (DiffMap[x][y][2] / Difftotal);
				ALT(x,y-1) = ALT(x,y-1) +  c * ( Diffmax - T ) * (DiffMap[x][y][3] / Difftotal);

				ALT(x,y) -=  c * ( Diffmax - T ); // Material von Indexzelle entfernen
				}
				


				
			}
		}
		progress = double(m) / double(duration) * 100.0;
		dialog->Update(int(progress));
		

        
	}

	dialog->Destroy();
}



