#include "stdafx.h"
#define ALT(x,y) (map[(x)*ysize+(y)])
/* needed for voronoi */
#define HASHPNT(x,y) hashpntf+2*hash[ (hash[(y) & 255]+(x)) & 255]


#include "globals.h"
#include "simulation.h"
#include "primenumbers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void AltitudeMap::perlin(double *inputMap, double persistence, int octaves, double zoom){
	showProgressbar(wxString("Perlin noise"));
	m_persistence = persistence;
	m_octaves = octaves;
	rand = int(random(1,30));
	rand2 = int(random(1,30));

	//use standard or custom map
	double* usedMap = (inputMap == nullptr) ? map : inputMap;
#define MAP(x,y) usedMap[(x)*ysize+(y)]
	
	float f = 0.0;
	for ( int x = 0;x < xsize; ++x){
		for (int y=0; y < ysize; ++y)
			MAP(x, y) = perlinNoise2D(float(x) / (xsize / (zoom * 14)), float(y) / (ysize / (zoom * 14)));

		if (x % 10 == 0){
			f += 100.0/(xsize * 0.1 + 1);
			dialog->Update(int(f));
		}
	}

	normalize();
	dialog->Destroy();
}



double AltitudeMap::perlinNoise2D(float x, float y){
	total = 0;
	iteration = 0;
	for (int i = 0; i < m_octaves; i++){
		iteration += 1;
		m_frequency = pow(2.0, i);
		m_amplitude = pow(m_persistence, i);

		total = total + interpolatedNoise(x * m_frequency, y * m_frequency) * m_amplitude;
	}

	return total;
}

double AltitudeMap::noise(int x, int y) {
	//noise function based on (deterministic) prime number juggling
    int n = x+y * 57;
    n = (n<<13) ^ n;
    return 1.0 - ((n * (n * n * primes[sizeof(primes)/sizeof(int)%int(iteration*(rand2/2.0 + 1)) + iteration*rand ] + 789221) 
		+ Prims2[sizeof(Prims2)/sizeof(int)%iteration + iteration* rand2] )  & 2147483647) / 1073741824.0;
}

double AltitudeMap::smoothNoise(int x,int y){
    double corners = (noise(x-1,y-1) + noise(x+1, y-1) + noise(x-1,y+1) + noise(x+1,y+1)) / 16;
    double edges = ( noise(x-1,y) + noise(x+1,y) + noise(x,y-1) + noise(x,y+1)) / 8;
    double middle = noise(x,y) / 4;
    return corners + edges + middle;
	}



double AltitudeMap::cosinInterpolate(double a, double b, double x){
	double ft = x * 3.1415927;
	double f = (1 - cos(ft)) * 0.5;
	return a*(1 - f) + b*f;
}
    

double AltitudeMap::interpolatedNoise(float x, float y){
	int intX = int(x);
	float  fractX = x - intX;

	int intY = int(y);
	float fractY = y - intY;

	double v1 = smoothNoise(intX, intY);
	double v2 = smoothNoise(intX + 1, intY);
	double v3 = smoothNoise(intX, intY + 1);
	double v4 = smoothNoise(intX + 1, intY + 1);


	double i1 = cosinInterpolate(v1, v2, fractX);
	double i2 = cosinInterpolate(v3, v4, fractX);

	return cosinInterpolate(i1, i2, fractY);
}



void AltitudeMap::delete_hashs()
{
	//delete  Prims;
	//delete  Prims2;
	//delete  hashpntf;
}

