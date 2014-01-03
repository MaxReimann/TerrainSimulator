#ifndef MAIN_H
#define MAIN_H
#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
#endif


#include "imports.h"
#include "simulation.h"
#include "DC_Panelclass.h"




wxGauge* get_gauge(void);



struct TerrainRegion
	{
		float min;
		float max;
		GLuint texture;
		std::string filename;
	};

struct Water_Constants
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


  struct Vertex
    {
        float x, y, z;
        float nx, ny, nz;
        float s, t;
		//double Waterdistribution;
		double d;
		//float sediment_amount;
		/*double d1;
		double d2;
		double fluxLeft, fluxRight, fluxBottom, fluxTop;
		double Volume;
		double Vx, Vy;*/
		
		//double sediment_amount1;
    };

  struct Water_Vertex
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
		double w; // w= d, nur wegen zwei versch simulationen
  };




class BasicFrame : public wxFrame {
   public:
   BasicFrame( const wxChar *title,
               int xpos, int ypos,
               int width, int height);
   ~BasicFrame();
   void Onthebutton(wxCommandEvent &event);
   void Subdivide_Button(wxCommandEvent &event);
   void Open_image_button(wxCommandEvent &event);
   void Erosion_Button(wxCommandEvent &event);
   void Thermal_Erosion_Button(wxCommandEvent &event);
   void Randomize_Button(wxCommandEvent &event);
   void Plateau_Button(wxCommandEvent &event);
   void PositionFlagButton(wxCommandEvent &event);
   void Open_obj_button(wxCommandEvent &event);
   void Save_obj(wxCommandEvent &event);
   void Save_image(wxCommandEvent &event);
   void Open_close_pic(wxCommandEvent &event);
   void Scale_Terrain_Real_Button(wxCommandEvent &event);
   void Simulate_Water(wxCommandEvent &event);
   void Perlin_Noise(wxCommandEvent &event);
   void Button_Voronoi(wxCommandEvent &event);
   void Combined_Generate(wxCommandEvent &event);
   void Button_Save_Preset(wxCommandEvent &event);
   void Button_Load_Preset(wxCommandEvent &event);
   void Perturbation_Button(wxCommandEvent &event);

   void Spinctrl1(wxSpinEvent &event);
   void Spinctrl2(wxSpinEvent &event);
   void Spinctrl3(wxSpinEvent &event);
   void Choice_combined1(wxCommandEvent &event);
   void Choice_combined2(wxCommandEvent &event);
   void Choice_combined3(wxCommandEvent &event);

   void Slider_Erosion_iterationen(wxCommandEvent &event);
   void Slider_Subdivision(wxCommandEvent &event);
   void Slider_Erosion_radius(wxCommandEvent &event);
   void Randomize_Slider(wxCommandEvent &event);
   void Slider_Scale_Terrain(wxCommandEvent &event);
   void Slider_Plateau_Radius(wxCommandEvent &event);
   void Slider_Octaves(wxSpinEvent &event);
   void Slider_Perlin_Zoom(wxCommandEvent &event);
   void Slider_Persistence(wxCommandEvent &event);

   void Toggle_Watersource(wxCommandEvent &event);
   void Toggle_Rain(wxCommandEvent &event);
   void Toggle_Sediment_Preview(wxCommandEvent &event);
   void Toggle_Water_Preview(wxCommandEvent &event);
   void ToggleAuto(wxCommandEvent &event);

   void Slider_Rain_Amount(wxCommandEvent &event);
   void Slider_Water_Radius(wxCommandEvent &event);
   void Slider_Water_Amount(wxCommandEvent &event);
   void Slider_Sediment_Capacity(wxCommandEvent &event);
   void Slider_Deposition_Constant(wxCommandEvent &event);
   void Slider_Dissolving_Constant(wxCommandEvent &event);
   void Slider_Evaporation_Constant(wxCommandEvent &event);
   void Slider_Timestep(wxCommandEvent &event);
   void Slider_Minimum_Alpha(wxCommandEvent &event);
   void Slider_Iterations(wxCommandEvent &event);
	
   void Voronoi(double inp_map[] = NULL);
   bool Execute_Choice(int choice,double inp_map[]);
   void Error_dialog(string text);
   void Save_Preset_Text(string path);
   string Get_Values(int choice);
   void Load_Preset_Text(string path);
  
   template <class T>
   string num_to_string(T input);

   template <class T>
   T string_to_num(string input,T wished_type);

   string Readline(ifstream& file,char buffer[]);
   void Change_wxChoice(int choice,int num);
   double Get_Talus_Threshold();




   void PaintEvent(wxPaintEvent & event);
   void render_it(wxDC&  dc);
   void UpdateImage(void);
   void ManualRender();
   void HandleSash_position(wxSplitterEvent &event);
   void DrawPoint(wxMouseEvent &event);
   void ToggleCrosshair(wxCommandEvent &event);
   void UpdateChain(void);
   

   wxSplitterWindow* m_splitter3;
   wxPanel* m_panel12;
   wxImageList *imagelist;

   wxPanel *panel;
   wxButton *button;
   //wxPanel *Subdivide_panel;
   wxButton *Loadfromimage_but;
   //wxPanel *glPanel;
   wxButton *but_subdivide;
   wxSlider *subdivide_coeff_slider;
   //AltitudeMap Alti;



	wxNotebook *m_notebook2;
	wxPanel* Subdivision;
	wxButton* m_button3;
	wxNotebook* m_notebook3;
	wxPanel* m_panel14;
	wxStaticText* m_staticText3;
	wxSlider* subdiv_slider_coeff;
	wxSlider* slider_fine_subdivision;
	
	//--voronoi--
	wxPanel* m_voronoi;
	wxButton* m_button_voro_gen;
	wxCheckBox* m_checkBox_negative;
	wxStaticText* m_staticText_voronoi_coeffs;
	wxTextCtrl* m_textCtrl2_vor_coeff;
	wxStaticText* m_staticText_distances;
	wxChoice* m_choice_distance;
	wxStaticText* m_staticText_clip;
	wxStaticText* m_staticText35;
	wxTextCtrl* m_textCtrl1;
	wxStaticText* m_staticText_blocksize;
	wxSlider* m_slider_blocksize;
	wxStaticLine* m_staticline5;
	wxStaticText* m_staticText_fValue;
	wxSlider* m_slider_fValue;
	
	//-----Combined Panel--//
	wxPanel* Combined_panel;
	wxChoice* m_choice21;
	wxStaticBitmap* m_bitmap3;
	wxSpinCtrl* m_spinCtrl2_add_factor_1;
	wxChoice* m_choice2;
	wxStaticBitmap* m_bitmap4;
	wxSpinCtrl* m_spinCtrl2_add_factor_2;
	wxChoice* m_choice22;
	wxSpinCtrl* m_spinCtrl2_add_factor_3;
	wxStaticLine* m_staticline8;
	wxButton* m_button_comb_generate;
	wxStaticLine* m_staticline6;
	wxButton* m_button_perturb;
	wxStaticText* m_staticText37;
	wxStaticText* m_staticText39;
	wxBitmapButton* m_bpButton_load_preset;
	wxBitmapButton* m_bpButton9_save_preset;
	wxStaticText* m_staticText_load_path;
	wxStaticLine* m_staticline7;
	wxRadioButton* m_radioBtn1_midpoint_displ;
	wxRadioButton* m_radioBtn2;

	//---------------



	wxStaticText* m_staticText271;
	wxPanel* m_panel15;
	wxButton* m_button11;
	wxStaticText* m_staticText301;
	wxSlider* m_slider211;
	wxStaticText* m_staticText_persistence;
	wxSlider* m_slider201;
	wxStaticLine* m_staticline4;
	wxStaticText* m_staticText29;
	wxSpinCtrl* m_spinCtrl1;
	wxBitmapButton* positionflag;
	wxBitmap positionflagbitmap;
	wxBitmap positionflagbitmap_selected;
	wxIcon icon;
	wxIcon icon_big;
	wxIconBundle icon_bundle;
	wxStaticText* flag_pos_text;
	wxPanel* save_files;
	wxButton* save_obj_but;
	wxStaticLine* m_staticline2;
	wxButton* save_tga_but;

	//erosion
	wxSlider* erosion_slider_iterationen;
	wxPanel* m_panel5;
	wxButton* m_button4;
	wxStaticText* m_staticText4;
	wxStaticLine* m_staticline9;
	wxButton* m_button_thermal_erosion;
	wxStaticText* m_staticText40;
	wxStaticText* m_staticText41;
	wxTextCtrl* m_textCtrl3;
	wxToggleButton* m_toggleBtn5;
	wxStaticLine* m_staticline10;
	wxStaticText* m_staticText5;
	wxSlider* erosion_slider_radius;
	//--

	wxPanel* m_panel6;
	wxButton* m_button7;
	wxStaticText* m_staticText6;
	wxSlider* m_slider6;
	wxPanel* m_panel7;
	wxButton* m_button8;
	wxButton* Randomize_but;
	wxStaticText* randomcoeff;
	wxSlider* random_fac_slider;
	wxStaticText* m_staticText7;
	wxSlider* scale_image_terrain;





	wxPanel* Scale;
	wxButton* m_scale_terrain_but;
	wxStaticText* m_staticText_scale;
	wxSlider* m_scale_terrain;
	wxStaticText* slider_labels;

	//water_sim buttons
		wxPanel* Water_sim_panel;
		wxRadioBox* m_radioBox1_water_methods;
		wxNotebook* m_notebook21;
		wxPanel* water_pan_1;
		wxButton* Simulate_button;
		wxStaticLine* m_staticline3;
		wxStaticText* m_staticText17;
		wxToggleButton* m_togglerain;
		
		wxSlider* m_slider12;
		wxStaticLine* m_staticline21;
		wxStaticText* m_staticText18;
		wxStaticText* m_staticText20;
		wxToggleButton* m_toggleBtn2;
		wxSlider* m_slider13;
		wxSlider* m_slider14;
		wxPanel* pan_coefficients;
		wxStaticText* m_staticText10;
		wxSlider* m_slider8_KC;
		wxStaticText* m_staticText12;
		wxStaticText* m_staticText11;
		wxSlider* m_slider9;
		wxStaticText* m_staticText15;
		wxStaticText* m_staticText13;
		wxSlider* m_slider10;
		wxStaticText* m_staticText16;
		wxStaticText* m_staticText14;
		wxSlider* m_slider11;
		wxPanel* m_panel13;
		wxStaticText* m_staticText28;
		
		wxStaticText* m_staticText27;
		wxStaticText* m_staticText26;
		wxSlider* m_slider20;
		
		wxStaticText* m_staticText261;
		wxSlider* m_slider18;
		
		wxStaticText* m_staticText32;
		
		wxStaticText* m_staticText33;
		wxStaticText* m_staticText30;
		wxSlider* m_slider22;
		wxStaticText* m_staticText31;
		wxSlider* m_slider21;
		wxPanel* m_panel131;
		wxToggleButton* m_toggleBtn3;
		wxToggleButton* m_toggleBtn4;

	//wxPanel* load_obj_panel;
	wxButton* load_obj;

	Bitmap_Panel* panel_preview_picture;
	wxBitmapButton* open_close_pic_but;
	wxStaticBitmap* m_bitmap2;
	wxBitmap* crosshair_bmp;
	wxBitmap* crosshair_bmp_deact;
	wxBitmapButton* m_bitmap_button_setflag;
	wxBitmap* Pfeil_RU;
	wxBitmap* Pfeil_LO;
	wxMask *Save_mask;
	wxMask  *pfeil_mask;
	wxMask *Open_mask;

	
	
	
   wxString clip_variable; 
   wxString voronoi_coeff_validator; 
   wxString combination_listbox1; 
   wxString combination_listbox2; 
   wxString combination_listbox3; 
   wxString threshold_var; 

   std::fstream errorstream;
   std::ofstream save_preset_stream;
   std::ifstream read_preset_stream;
   bool sediment_preview_on;
   bool water_preview_on;

   

   
   
   
   DECLARE_EVENT_TABLE()
private :
	enum {
		BUTTON_SUBDIVIDE,
		BUTTON_OPEN_IMAGE,
		SLIDER_SUBDIVISION_COEFF,
		SLIDER_FINE_SUBDIV,
		BUTTON_GENERATE,
		SLIDER_PERLIN_ZOOM,
		SLIDER_PERSISTENCE,
		SLIDER_OCTAVES,
		//--voronoi--
		SLIDER_INTESITY,
		SLIDER_BLOCKSIZE,
		BUTTON_VORONOI,
		CHECKBOX_NEGATIVE,
		TEXT_CTRL_CLIP,
		SPIN_CTRL_DISTANCES,
		//---combined
		SAVE_BUT_PRESET,
		OPEN_BUT_PRESET,
		SPINCTRL1,
		SPINCTRL2,
		SPINCTRL3,
		CHOICEBOX1,
		CHOICEBOX2,
		CHOICEBOX3,
		COMBINED_GENERATE,
		BUTTON_PERTURB,
		



		BUTTON_EROSION,
		BUTTON_THERMAL_EROSION,
		TOGGLE_AUTO,
		EROSION_RADIUS_SLIDER,
		EROSION_ITER_SLIDER,
		BUTTON_PLATEAU,
		PLATEAU_RADIUS_SLIDER,
		BUTTON_RANDOMIZE,
		RANDOMIZE_SLIDER,
		SLIDER_SCALE_IMAGE_TERRAIN,
		BITMAP_BUTTON_FLAGPOS,
		BUTTON_OPEN_OBJ,
		SAVE_IMAGE,
		SAVE_OBJ,
		SPLITTER_WINDOW,
		ON_OFF_PIC_BUT,
		PANEL_PREVIEW_PICTURE,
		BUTTON_TOGGLE_CROSSHAIR,
		SLIDER_SCALE_TERRAIN_REAL,
		BUTTON_SCALE_TERRAIN_REAL,
		
		
		BUTTON_WATER_SIMULATE,		
		SLIDER_RAIN_WATER_AMOUNT,
		SLIDER_WATER_RADIUS,
		SLIDER_WATER_AMOUNT,
		SLIDER_SEDIMENT_CAPACITY,
		SLIDER_DEPOSITION_CONSTANT,
		SLIDER_DISSOLVING_CONSTANT,
		SLIDER_EVAPORATION_CONSTANT,
		SLIDER_TIMESTEP,
		TOGGLE_WATERSOURCE,
		TOGGLE_RAIN,
		MINIMUM_ALPHA,
		SLIDER_ITERATIONS,
		TOGGLE_SEDIMENT_PREVIEW,
		TOGGLE_WATER_PREVIEW

	};
	double Coeff; //der faltigketi coeff fuer subdivision
	int Erosion_radius;
	int Erosion_iterationen;
	double Randomize_radius;
	int Plateau_radius;
	void m_splitter3OnIdle( wxIdleEvent& );
	bool side_window;
	bool crosshair;
	double Perlin_Zoom;
	double Perlin_Persistence;
	bool toggle_talus_auto;
	int Octaves;
	int factor_combination1;
	int factor_combination2;
	int factor_combination3;
	int Choice_combination1;
	int Choice_combination2;
	int Choice_combination3;

	double *Copy_map1; //copy_maps fuer combined
	double *Copy_map2;
	double *Copy_map3;




   
};


class HalloWeltApp : public wxApp {
   public:
	//BasicFrame *frame;
   virtual bool OnInit();
   ~HalloWeltApp();
   wxBoxSizer* sizer;
   
};




class BasicGLPane : public wxGLCanvas
{
private:
	float alpha; // test zur rotaton
	//angle of rotation
	float xpos, ypos, zpos, xrot, yrot, angle;
	float xshift, yshift;
	float lastx, lasty,shiftlastx,shiftlasty;
	GLuint   g_nullTexture;
	GLuint   g_terrainShader;
	GLuint terrainDL; //die terrain liste im gpu
	//GLuint terrainDL_id;
	GLUquadric* cylinder;
	GLUquadric* sphere;
	std::fstream errorstream;
	//fuer gluproject und n
	GLdouble* win_cox;
	GLdouble* win_coy;
	GLdouble* win_coz;
	GLdouble* obj_cox;
	GLdouble* obj_coy;
	GLdouble* obj_coz;
	
	
	//float hoehenmap[2500];
	float g_lightDir[4];
	float light_intesity[4];
	float yellowAmbientDiffuse[4];



    
public:
	BasicGLPane(wxFrame* parent, int* args);
	~BasicGLPane();
	void resized(wxSizeEvent& evt);
	//static, damit es in der satic funktion 
	// process mouse genutzt werden kann
    static float zoom;
	int getWidth();
	int getHeight();
    
	void render(wxPaintEvent& evt);
	void prepare3DViewport(int topleft_x, int topleft_y, int bottomrigth_x, int bottomrigth_y);
	void prepare2DViewport(int topleft_x, int topleft_y, int bottomrigth_x, int bottomrigth_y);

	void camera(void);
	void mouseMovement(int x, int y);
	void mouseshift(int x, int y);
	void zoom_func();
	GLint  project(int x , int y ,int z );
	GLint unproject(int winx , int winy ,int winz );
    
	// events
	void mouseMoved(wxMouseEvent& event);
	void mouseDown(wxMouseEvent& event);
	void mouseWheelMoved(wxMouseEvent& event);
	void mouseReleased(wxMouseEvent& event);
	void rightClick(wxMouseEvent& event);
	void mouseLeftWindow(wxMouseEvent& event);
	void keyPressed(wxKeyEvent& event);
	void keyReleased(wxKeyEvent& event);
	void middleMouseDown(wxMouseEvent& event);
	GLuint Maketerrain(void);
	void refresh_terrain();
	void defineflag(void);
	void setFlag(int x, int y);
	// static, damit glutmousefunc diese nutzen kann
	// sonst gibts argumenten listen error
	static void processMouse(int button, int state, int x, int y);
	GLuint LinkShaders(GLuint vertShader, GLuint fragShader);
	GLuint LoadShaderProgram(const char *pszFilename, std::string &infoLog);
	GLuint LoadTexture(const char *pszFilename);
	GLuint LoadTexture(const char *pszFilename, GLint magFilter, GLint minFilter,GLint wrapS, GLint wrapT);


	void BindTexture(GLuint texture, GLuint unit);
	GLuint CompileShader(GLenum type , char* pszSource , GLint length);
	GLuint CreateNullTexture(int width, int height);
	void create_textures();
	void ReadTextFile(const char *pszFilename, std::string &buffer);
	void initstruct(void);
	void init_textures(void);
	void Create_Ok_Dialog(std::string msg,std::string title);
	void UpdateTerrainShaderParameters(void);
	bool generateVertices(void);
	bool generateIndices(void);
	bool terrainCreate(void);
	void terrainDraw(void);
	void updateTerrain(void);
	void CleanupShader(void);
	void CleanupStructs(void);
	void Start_WaterSimulation(int method);

	
	
	bool use16BitIndices() const
    { return m_totalVertices <= 65536; }

	const GLubyte *BUFFER_OFFSET(size_t bytes)
	{ return reinterpret_cast<const GLubyte *>(0) + bytes; }


	wxMessageDialog *Yes_no_dialog;
	wxMessageDialog *Ok_dialog;



	struct TerrainRegion g_regions[4];	
	GLuint m_vertexBuffer;
    unsigned int m_indexBuffer;
    int m_totalVertices;
    int m_totalIndices;
	bool Update;
	double Terrain_height_scale_fac;
	//wxBitmap* rotate_bmp;
	Image* rotating_image;
	bool rotating;
	Drawable* rotate_icon;//spezielle klasse, um auf gl mit wxwidgets zu malen
	Vertex *pVertices;
	//std::vector<TerrainRegion> g_regions;



	
    
	DECLARE_EVENT_TABLE()
};

class Water_Simulation
{
public:
Water_Simulation(const struct Water_Constants *constants,int _method);
~Water_Simulation();
void Distribution(double Water_Amount);
void Rain_Distribution();
void Increase_Water();
void Sim_Loop();
void Flux_Sim();
void Surface_and_Velocity();
void Erosion_Deposition();
void Sediment_Transport();
void Evaporation();
void normalize_height();
void visualize_Water2D();
void visualize_Sediment2D();


void Water_Sim2();
double Average_a(int x,int y);
double d_total(int x, int y);
void Water_Sim3();

void Water_Sim4();


void Progressbar();

private:
	Water_Vertex *vert_array;
	
	std::fstream errorstream;
	double A; //Querschnittflaeche einer pipe
	double l; //laenge einer Pipe
	int ysize;
	int xsize;
	double dif_height_l, dif_height_r, dif_height_t, dif_height_b;//delta height: left right top bottom
	double d_avg;
	double Waterpass_x, Waterpass_y;
	double inflow_sum, outflow_sum;
	wxProgressDialog* dialog;
	float RADIUS;
	float KC, KD, KS, KE, KR;
	float WATER_AMOUNT;
	float PIPE_LENGTH;
	float DT;
	int Iterations;
	wxImage temp_img;
	int normalized;
	bool Rain_on;
	bool Water_source_on;
	time_t seconds;
	int method;



};


//BasicFrame *p_objekt;


#endif
  
