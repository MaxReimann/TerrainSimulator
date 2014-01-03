#ifndef  _SIMULATION_H
#define _SIMULATION_H
#define TERRAIN_ERROR_LOADING_IMAGE		-4
#define TERRAIN_ERROR_INVALID_PARAM		-5
#define TERRAIN_ERROR_MEMORY_PROBLEM	-3
#define	TERRAIN_ERROR_NOT_SAVED			-2
#define TERRAIN_ERROR_NOT_INITIALISED	-1
#define TERRAIN_OK						0
#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
#endif


#include "imports.h"
#include "Perturbation.h"
//#include "gl_object.h" //<-dummer fehler! 
// darf hier auf keinen fall importiert werden, da sonst objekte erzeugt werden sollen,
// deren deklaration (also folgende) noch nicht vollzogen ist
using namespace std;






class AltitudeMap {
private:
	wxImage *preview_image;
	double sealevel;
	int xsize, ysize;
	void subdiv_private(double coeff,int x1, int y1, int x2, int y2);
	//unsigned TT800(void);
	void init_random(void);
	std::fstream errorstream;
	double change ;
	wxProgressDialog* dialog;
	double *terrainNormals;
	double *terrainColors;
public:
	double *map;
	double *sediment_map;
	double ***DiffMap;
	AltitudeMap(int _xsize,int _ysize);
	AltitudeMap();
	~AltitudeMap(void);
	void delete_diffMap();
	double getaltitude(int x,int y);
	void Alt(int x,int y, double z);
	void add(int x,int y, double z);
	void multiply(int x,int y, double z);
	double getsealevel(void);
	void getsealevel(double sl);
	int getysize(void);
	int getxsize(void);
	void subdivision(double coeff,double lt,double rt, double lb, double rb);
	double random(double a, double b);
	void randomize(double r);
	void erosion(int r,int iter);
	void plateau(int x,int y ,int r );
	void normalize();
	void normalize2();
	double slope_x(int x, int y);
	double slope_y(int x, int y);
	double angle_slope_x(int x, int y);
	double angle_slope_y(int x, int y);
	int terrainLoadFromImage(char *filename, int normals);
	int terrainLoadFromImage(char *filename, int normals, int ungebraucht);
	bool load_image(char path[], int normals);
	void error_output(void);
	void randomize2(double r );
	void Progressbar(void);
	void Progressbar(wxString Text);
	bool Load_obj(char  path []);
	double split_text_doubles(std::string str,int& pos);
	pair<double,double> find_range(vector<double> heightvec);
	bool save_obj(char path[]);
	bool save_image(char path[],string format);
	wxImage Make_Preview_Image(void);
	wxImage Make_Sediment_Image(void);
	void Scale_Terrain(double factor);
	void reset_Flag(void);
	void normalAtPoint(int x, int z, Vector3 &n);
	//void Voronoi(int x_int, int y_int, float *da);
	void Voronoi_Tex(int fValue, int blockSize,/* int _seed=0, */
							  bool negative, float clip,int distance,
							  float coefficients[]  = NULL,int elements = 0);

	void Inp_Voronoi_Tex(int fValue, int blockSize,/* int _seed=0, */
							  bool negative, float clip,int distance,double inp_map[],float coefficients[]  = NULL,int elements = 0);
	int FlagX; //der positionsmarker
	int FlagY; 
	void setflag(int x, int y);
	bool showflag;




	void Perlin(double Persistence, int Octaves,double zoom);
	void Inp_Perlin(double inp_map[],double Persistence = 0.6,int Octaves = 7, double zoom=0.67 /*werte, um ein noisiges perlin zu erzeugen*/);
	double Noise(int x, int y);
	double Smooth_Noise(int x,int y);
	double cosin_interpolate(double a,double b,double x);
	double Interpolated_Noise(float x,float y);
	double PerlinNoise_2D(float x,float y);
	double persistence;
	int octaves;
	int frequency;
	double amplitude;
	double total;
	int rand;
	int iterat;
	int rand2;

	void Inp_subdivision(double coeff,double _inp_map[]);
	void subdiv_inp_map(double coeff,int x1, int y1, int x2, int y2);
	double *inp_map;
	void delete_hashs();
	
	

	void Diff();
	void ThermalErosion(int iter, double Talus);




};
#endif