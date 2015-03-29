#ifndef MAIN_H
#define MAIN_H
#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
#endif

#include "imports.h"
#include "simulation.h"
#include "dcpanel.h"




wxGauge* get_gauge(void);





class BasicFrame : public wxFrame {
   public:
   BasicFrame( const wxChar *title,
               int xpos, int ypos,
               int width, int height);
   ~BasicFrame();
   void Onthebutton(wxCommandEvent &event);
   void subdivideButtonEvent(wxCommandEvent &event);
   void openImageButtonEvent(wxCommandEvent &event);
   void erosionButtonEvent(wxCommandEvent &event);
   void thermalErosionButtonEvent(wxCommandEvent &event);
   void randomizeButtonEvent(wxCommandEvent &event);
   void plateauButtonEvent(wxCommandEvent &event);
   void positionFlagButtonEvent(wxCommandEvent &event);
   void openObjButtonEvent(wxCommandEvent &event);
   void saveObjEvent(wxCommandEvent &event);
   void saveImageEvent(wxCommandEvent &event);
   void openClosePicEvent(wxCommandEvent &event);
   void scaleTerrainRealButtonEvent(wxCommandEvent &event);
   void simulateWaterEvent(wxCommandEvent &event);
   void perlinNoiseEvent(wxCommandEvent &event);
   void buttonVoronoiEvent(wxCommandEvent &event);
   void combinedGenerateEvent(wxCommandEvent &event);
   void buttonSavePresetEvent(wxCommandEvent &event);
   void buttonLoadPresetEvent(wxCommandEvent &event);
   void perturbationButtonEvent(wxCommandEvent &event);

   void spinctrl1Event(wxSpinEvent &event);
   void spinctrl2Event(wxSpinEvent &event);
   void spinctrl3Event(wxSpinEvent &event);
   void choiceCombined1Event(wxCommandEvent &event);
   void choiceCombined2Event(wxCommandEvent &event);
   void choiceCombined3Event(wxCommandEvent &event);

   void sliderErosionIterationenEvent(wxCommandEvent &event);
   void sliderSubdivisionEvent(wxCommandEvent &event);
   void sliderErosionRadiusEvent(wxCommandEvent &event);
   void randomizeSliderEvent(wxCommandEvent &event);
   void sliderScaleTerrainEvent(wxCommandEvent &event);
   void sliderPlateauRadiusEvent(wxCommandEvent &event);
   void sliderOctavesEvent(wxSpinEvent &event);
   void sliderPerlinZoomEvent(wxCommandEvent &event);
   void sliderPersistenceEvent(wxCommandEvent &event);

   void toggleWatersourceEvent(wxCommandEvent &event);
   void toggleRainEvent(wxCommandEvent &event);
   void toggleSedimentPreviewEvent(wxCommandEvent &event);
   void toggleWaterPreviewEvent(wxCommandEvent &event);
   void toggleAutoEvent(wxCommandEvent &event);

   void sliderRainAmountEvent(wxCommandEvent &event);
   void sliderWaterRadiusEvent(wxCommandEvent &event);
   void sliderWaterAmountEvent(wxCommandEvent &event);
   void sliderSedimentCapacityEvent(wxCommandEvent &event);
   void sliderDepositionConstantEvent(wxCommandEvent &event);
   void sliderDissolvingConstantEvent(wxCommandEvent &event);
   void sliderEvaporationConstantEvent(wxCommandEvent &event);
   void sliderTimestepEvent(wxCommandEvent &event);
   void sliderMinimumAlphaEvent(wxCommandEvent &event);
   void sliderIterationsEvent(wxCommandEvent &event);
	
   void voronoi(double inp_map[] = NULL);
   bool executeChoice(int choice,double inp_map[]);
   void errorDialog(string text);
   void savePresetText(string path);
   string getValues(int choice);
   void loadPresetText(string path);
  
   template <class T>
   string numToString(T input);

   template <class T>
   T stringToNum(string input,T wished_type);

   string readline(ifstream& file,char buffer[]);
   void changeWxChoice(int choice,int num);
   double getTalusThreshold();

   void paintEvent(wxPaintEvent & event);
   void render(wxDC&  dc);
   void updateImage(void);
   void manualRender();
   void handleSashPosition(wxSplitterEvent &event);
   void drawPoint(wxMouseEvent &event);
   void toggleCrosshair(wxCommandEvent &event);
   void updateChain(void);
   void m_splitter3OnIdle(wxIdleEvent&);
   

   wxSplitterWindow* m_splitter3;
   wxPanel* m_panel12;
   wxImageList *imagelist;

   wxPanel *panel;
   wxButton *button;
   wxButton *Loadfromimage_but;
   wxButton *but_subdivide;
   wxSlider *subdivide_coeff_slider;


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





	wxPanel* scale;
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

	BitmapPanel* panel_preview_picture;
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
	double foldingCoeff; 
	int erosionRadius;
	int erosionIterations;
	double randomizeRadius;
	int plateauRadius;

	bool sideWindow;
	bool crosshair;
	double perlinZoom;
	double perlinPersistence;
	bool toggleTalusAuto;
	int octaves;
	int factorCombination1;
	int factorCombination2;
	int factorCombination3;
	int choiceCombination1;
	int choiceCombination2;
	int choiceCombination3;

	double *copyMap1;
	double *copyMap2;
	double *copyMap3; 
};


class BaseApp : public wxApp {
   public:
	//BasicFrame *frame;
   virtual bool OnInit();
   ~BaseApp();
   wxBoxSizer* sizer;
   
};


#endif
  
