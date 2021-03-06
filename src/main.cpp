#include "stdafx.h"
#include "main.h"
#include "simulation.h"
#include "globals.h"

#include "3dwindow.h"
#include "watersimulation.h"
#include <crtdbg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define coeff_sizer  15.0

using namespace std;
int f;

AltitudeMap Alti(256, 256);
GLPane *glPlane;

wxBitmap global_preview_bitmap = wxBitmap(Alti.getXSize(), Alti.getYSize(), -1);
wxBitmap g_sedimentBitmap = wxBitmap(Alti.getXSize(), Alti.getYSize(), -1);
SimGUI *frame;



struct WaterConstants Water_Kcs = {
	/*Water_Kcs.KC*/ 0.01,
	/*Water_Kcs.KD*/ 0.5,
	/*Water_Kcs.KE*/0.5,
	/*Water_Kcs.KS*/ 0.01,
	/*Water_Kcs.KR*/0.01,
	/*Water_Kcs.Pipe_length_x*/ 1.0,
	/*Water_Kcs.Pipe_length_y*/1.0,
	/*Water_Kcs.Radius*/ 50.0,
	/*Water_Kcs.Water_Amount*/  100.0,
	/*Water_Kcs.DT*/ 0.1,
	/*Water_Kcs.Min_alpha  */0.01,
	/*Water_Kcs.Rain_on */ true,
	/*Water_Kcs.Watersource_on*/ true,
	/*Iterations*/50
};



IMPLEMENT_APP(BaseApp)


bool BaseApp::OnInit() {
	sizer = new wxBoxSizer(wxHORIZONTAL);
	frame = new SimGUI(wxT("Terrain Generator"), 50, 50, 1024, 800);

	frame->SetIcon(frame->icon_big);
	frame->Show(TRUE);
	SetTopWindow(frame);
	frame->manualRender();
	return TRUE;
}

BaseApp::~BaseApp()
{
	sizer->Clear();
	delete sizer;
}

SimGUI::SimGUI(
	const wxChar *title,
	int xpos, int ypos,
	int width, int height)
	: wxFrame((wxFrame *)NULL,
	-1, title,
	wxPoint(xpos, ypos),
	wxSize(width, height)){


	foldingCoeff = 3.0 / coeff_sizer;
	erosionRadius = 3;
	erosionIterations = 3;
	randomizeRadius = 50;
	sideWindow = true; 
	crosshair = true;
	perlinPersistence = 0.25;
	perlinZoom = 0.25;
	octaves = 5;
	factorCombination1 = 1;
	factorCombination2 = 1;
	factorCombination3 = 1;
	choiceCombination1 = 0;//0 = subdivision
	choiceCombination2 = 0;
	choiceCombination3 = 3;//3= None
	updateImage();

	wxInitAllImageHandlers();


	imagelist = new wxImageList(20, 20, true, 3);
	wxBitmap saveIcon = wxBitmap(wxT("BITMAP_SAVE"), wxBITMAP_TYPE_RESOURCE);
	wxBitmap saveIconMask = wxBitmap(wxT("BITMAP_SAVE_MASK"), wxBITMAP_TYPE_RESOURCE);
	wxBitmap erosionIcon = wxBitmap(wxT("BITMAP_EROSION"), wxBITMAP_TYPE_RESOURCE);
	wxBitmap subdivisionIcon = wxBitmap(wxT("BITMAP_SUBDIVISION"), wxBITMAP_TYPE_RESOURCE);
	wxBitmap plateauIcon = wxBitmap(wxT("BITMAP_PLATEAU"), wxBITMAP_TYPE_RESOURCE);
	wxBitmap openIcon = wxBitmap(wxT("BITMAP_OPEN"), wxBITMAP_TYPE_RESOURCE);
	wxBitmap openIconMask = wxBitmap(wxT("BITMAP_OPEN_MASK"), wxBITMAP_TYPE_RESOURCE);
	wxBitmap scaleIcon = wxBitmap(wxT("BITMAP_SCALE"), wxBITMAP_TYPE_RESOURCE);
	wxBitmap waterIcon = wxBitmap(wxT("BITMAP_WATER"), wxBITMAP_TYPE_RESOURCE);
	wxBitmap arrowIcon = wxBitmap(wxT("BITMAP_ARROW"), wxBITMAP_TYPE_RESOURCE);
	wxBitmap arrowIconMask = wxBitmap(wxT("BITMAP_ARROW_MASK"), wxBITMAP_TYPE_RESOURCE);

	imagelist->Add(subdivisionIcon, wxColour(char(255), char(255), char(255))); //weiss ist transparent
	imagelist->Add(erosionIcon, wxColour(char(255), char(255), char(255)));
	imagelist->Add(saveIcon, wxColour(char(255), char(255), char(255)));
	imagelist->Add(plateauIcon, wxColour(char(255), char(255), char(255)));
	imagelist->Add(openIcon, wxColour(char(255), char(255), char(255)));
	imagelist->Add(scaleIcon, wxColour(char(255), char(255), char(255)));
	imagelist->Add(waterIcon, wxColour(char(255), char(255), char(255)));


	arrowMask = new wxMask(arrowIconMask);
	arrowIcon.SetMask(arrowMask);
	Open_mask = new wxMask(openIconMask);
	openIcon.SetMask(Open_mask);
	Save_mask = new wxMask(saveIconMask);
	saveIcon.SetMask(Save_mask);

	wxBoxSizer* superSizer;
	superSizer = new wxBoxSizer(wxHORIZONTAL);

	m_splitter3 = new wxSplitterWindow(this, SPLITTER_WINDOW, wxDefaultPosition, wxDefaultSize, wxSP_3D | wxSP_3DBORDER | wxSP_3DSASH);

	m_panel12 = new wxPanel(m_splitter3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer *generalsizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer *panelsizer = new wxBoxSizer(wxVERTICAL);
	//panel = new wxPanel(this, wxID_ANY,wxPoint(0,0),wxSize(width,80));


	wxBoxSizer* bSizer5;
	m_notebook2 = new wxNotebook(m_panel12, wxID_ANY, wxDefaultPosition, wxSize(-1, 50), 0);
	Subdivision = new wxPanel(m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	bSizer5 = new wxBoxSizer(wxHORIZONTAL);

	m_notebook3 = new wxNotebook(Subdivision, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
	m_panel14 = new wxPanel(m_notebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer60;
	bSizer60 = new wxBoxSizer(wxHORIZONTAL);

	m_button3 = new wxButton(m_panel14, BUTTON_SUBDIVIDE, wxT("Subdivide"), wxPoint(100, 40), wxSize(100, -1), 0);
	m_button3->SetToolTip(wxT("Generate terrain"));
	wxBoxSizer* bSizer101;
	bSizer101 = new wxBoxSizer(wxVERTICAL);

	bSizer101->Add(m_button3, 0, wxALL, 5);
	Randomize_but = new wxButton(m_panel14, BUTTON_RANDOMIZE, wxT("Randomize"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer101->Add(Randomize_but, 0, wxALL, 5);

	bSizer60->Add(bSizer101, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* subdivide_coeff_sizer;
	subdivide_coeff_sizer = new wxBoxSizer(wxHORIZONTAL);

	m_staticText3 = new wxStaticText(m_panel14, wxID_ANY, wxT("Coeff :"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText3->Wrap(-1);
	m_staticText3->SetFont(wxFont(-1, 70, 90, 90, false, wxEmptyString));
	m_staticText3->SetToolTip(wxT("coeff sets terrain folding strength"));

	subdivide_coeff_sizer->Add(m_staticText3, 0, wxALIGN_LEFT | wxALL, 5);

	//bSizer6->Add( m_staticText3, 0, wxALIGN_LEFT|wxALL, 5 );

	subdiv_slider_coeff = new wxSlider(m_panel14, SLIDER_SUBDIVISION_COEFF, 5, 5, 20, wxPoint(-1, -1), wxDefaultSize, wxSL_AUTOTICKS | wxSL_LABELS | wxSL_RIGHT | wxSL_TOP);
	subdivide_coeff_sizer->Add(subdiv_slider_coeff, 0, wxALIGN_CENTER | wxALIGN_TOP | wxALL | wxSHAPED, 5);



	m_staticText271 = new wxStaticText(m_panel14, wxID_ANY, wxT("Fine_coeff:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText271->Wrap(-1);

	slider_fine_subdivision = new wxSlider(m_panel14, SLIDER_FINE_SUBDIV, 10, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS | wxSL_HORIZONTAL | wxSL_LABELS | wxSL_RIGHT | wxSL_TOP);
	slider_fine_subdivision->SetToolTip(wxT("finetuning for coeffcient"));

	subdivide_coeff_sizer->Add(m_staticText271, 0, wxALL, 5);
	subdivide_coeff_sizer->Add(slider_fine_subdivision, 0, wxALL, 5);


	bSizer6->Add(subdivide_coeff_sizer, 1, wxEXPAND, 5);


	wxBoxSizer* random_slider_sizer;
	random_slider_sizer = new wxBoxSizer(wxHORIZONTAL);

	randomcoeff = new wxStaticText(m_panel14, wxID_ANY, wxT("Random factor"), wxDefaultPosition, wxDefaultSize, 0);
	randomcoeff->Wrap(-1);
	random_slider_sizer->Add(randomcoeff, 0, wxALL, 5);

	random_fac_slider = new wxSlider(m_panel14, RANDOMIZE_SLIDER, (int)randomizeRadius, 10, 100, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS | wxSL_HORIZONTAL | wxSL_LABELS);
	random_slider_sizer->Add(random_fac_slider, 0, wxALIGN_TOP | wxALL, 5);

	bSizer6->Add(random_slider_sizer, 1, wxEXPAND, 5);

	bSizer60->Add(bSizer6, 1, wxEXPAND, 5);

	m_panel14->SetSizer(bSizer60);
	m_panel14->Layout();
	bSizer60->Fit(m_panel14);
	m_notebook3->AddPage(m_panel14, wxT("MidPoint"), false);

	m_panel15 = new wxPanel(m_notebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer62;
	bSizer62 = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* bSizer63;
	bSizer63 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer66;
	bSizer66 = new wxBoxSizer(wxVERTICAL);
	m_button11 = new wxButton(m_panel15, BUTTON_GENERATE, wxT("Generate "), wxDefaultPosition, wxDefaultSize, 0);
	bSizer66->Add(m_button11, 0, wxALIGN_CENTER | wxALL, 5);
	bSizer63->Add(bSizer66, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer67;
	bSizer67 = new wxBoxSizer(wxVERTICAL);

	m_staticText301 = new wxStaticText(m_panel15, wxID_ANY, wxT("Zoom"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText301->Wrap(-1);
	bSizer67->Add(m_staticText301, 0, wxALL, 5);
	m_slider211 = new wxSlider(m_panel15, SLIDER_PERLIN_ZOOM, 25, 1, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_LABELS);
	bSizer67->Add(m_slider211, 0, wxALL | wxEXPAND, 5);

	bSizer63->Add(bSizer67, 1, wxEXPAND, 5);
	bSizer62->Add(bSizer63, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer64;
	bSizer64 = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* bSizer65;
	bSizer65 = new wxBoxSizer(wxVERTICAL);

	m_staticText_persistence = new wxStaticText(m_panel15, wxID_ANY, wxT("Persistence"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText_persistence->Wrap(-1);
	m_staticText_persistence->SetToolTip(wxT("High values for rough, low for smooth terrain"));

	bSizer65->Add(m_staticText_persistence, 0, wxALL, 5);
	m_slider201 = new wxSlider(m_panel15, SLIDER_PERSISTENCE, 25, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS | wxSL_HORIZONTAL | wxSL_LABELS);
	bSizer65->Add(m_slider201, 0, wxALL | wxEXPAND, 5);

	m_staticline4 = new wxStaticLine(m_panel15, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	bSizer65->Add(m_staticline4, 0, wxEXPAND | wxALL, 5);

	m_staticText29 = new wxStaticText(m_panel15, wxID_ANY, wxT("Octaves"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText29->Wrap(-1);
	bSizer65->Add(m_staticText29, 0, wxALL, 5);

	m_spinCtrl1 = new wxSpinCtrl(m_panel15, SLIDER_OCTAVES, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 12, 5);
	bSizer65->Add(m_spinCtrl1, 0, wxALL, 5);

	bSizer64->Add(bSizer65, 1, wxEXPAND, 5);
	bSizer62->Add(bSizer64, 1, wxEXPAND, 5);
	m_panel15->SetSizer(bSizer62);
	m_panel15->Layout();
	bSizer62->Fit(m_panel15);
	m_notebook3->AddPage(m_panel15, wxT("Perlin"), true);

	//--------------Voronoi-----------------
	m_voronoi = new wxPanel(m_notebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer651;
	bSizer651 = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* bSizer661;
	bSizer661 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer70;
	bSizer70 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer75;
	bSizer75 = new wxBoxSizer(wxHORIZONTAL);

	m_button_voro_gen = new wxButton(m_voronoi, BUTTON_VORONOI, wxT("Generate"), wxDefaultPosition, wxDefaultSize, 0);
	m_button_voro_gen->SetToolTip(wxT("Create cell-like structure"));

	bSizer75->Add(m_button_voro_gen, 0, wxALL, 5);

	m_checkBox_negative = new wxCheckBox(m_voronoi, wxID_ANY, wxT("Negative"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer75->Add(m_checkBox_negative, 0, wxALL, 10);

	bSizer70->Add(bSizer75, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer76;
	bSizer76 = new wxBoxSizer(wxHORIZONTAL);

	m_staticText_voronoi_coeffs = new wxStaticText(m_voronoi, wxID_ANY, wxT("Coefficients"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText_voronoi_coeffs->Wrap(-1);
	bSizer76->Add(m_staticText_voronoi_coeffs, 0, wxALL, 5);

	m_textCtrl2_vor_coeff = new wxTextCtrl(m_voronoi, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	wxTextValidator Coeff_validator = wxTextValidator(wxFILTER_INCLUDE_CHAR_LIST, &voronoi_coeff_validator);
	wxString includes[] = { wxT("1"), wxT("2"), wxT("3"), wxT("4"), wxT("5"), wxT("6"), wxT("7"), wxT("8"), wxT("9"), wxT("0"), wxT("."), wxT(";"), wxT("-") };
	Coeff_validator.SetIncludes(wxArrayString(13, includes));
	m_textCtrl2_vor_coeff->SetValidator(Coeff_validator);
	m_textCtrl2_vor_coeff->SetToolTip(wxT("Coefficients which have influence on form';'"));

	bSizer76->Add(m_textCtrl2_vor_coeff, 0, wxALL, 5);

	bSizer70->Add(bSizer76, 1, wxEXPAND, 5);

	bSizer661->Add(bSizer70, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer71;
	bSizer71 = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* bSizer72;
	bSizer72 = new wxBoxSizer(wxVERTICAL);

	m_staticText_distances = new wxStaticText(m_voronoi, wxID_ANY, wxT("Distance Methods"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText_distances->Wrap(-1);
	bSizer72->Add(m_staticText_distances, 0, wxALL, 5);

	wxString m_choice_distanceChoices[] = { wxT("Linear"), wxT("SquareLength"), wxT("Manhattan"), wxT("Chebyshev"), wxT("Quadratic"), wxT("My Method") };
	int m_choice_distanceNChoices = sizeof(m_choice_distanceChoices) / sizeof(wxString);
	m_choice_distance = new wxChoice(m_voronoi, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice_distanceNChoices, m_choice_distanceChoices, 0);
	m_choice_distance->SetSelection(0);
	m_choice_distance->SetToolTip(wxT("distance measure for algorithms"));

	bSizer72->Add(m_choice_distance, 0, wxALL, 5);

	bSizer71->Add(bSizer72, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer73;
	bSizer73 = new wxBoxSizer(wxVERTICAL);

	m_staticText_clip = new wxStaticText(m_voronoi, wxID_ANY, wxT("Clip"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText_clip->Wrap(-1);
	m_staticText_clip->SetToolTip(wxT("contrast, range 0-1"));

	bSizer73->Add(m_staticText_clip, 0, wxALL, 5);

	wxBoxSizer* bSizer74;
	bSizer74 = new wxBoxSizer(wxHORIZONTAL);

	m_staticText35 = new wxStaticText(m_voronoi, wxID_ANY, wxT("0."), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText35->Wrap(-1);
	bSizer74->Add(m_staticText35, 0, wxLEFT | wxTOP, 10);

	m_textCtrl1 = new wxTextCtrl(m_voronoi, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, wxTE_NO_VSCROLL | wxSIMPLE_BORDER);
	m_textCtrl1->SetMaxLength(3);
	m_textCtrl1->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &clip_variable));
	m_textCtrl1->SetFont(wxFont(8, 70, 90, 90, false, wxEmptyString));
	m_textCtrl1->SetToolTip(wxT("contrast, range 0-1"));

	bSizer74->Add(m_textCtrl1, 0, wxTOP, 8);
	bSizer73->Add(bSizer74, 1, wxEXPAND, 5);
	bSizer71->Add(bSizer73, 1, wxEXPAND, 5);
	bSizer661->Add(bSizer71, 1, wxEXPAND, 5);
	bSizer651->Add(bSizer661, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer671;
	bSizer671 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer68;
	bSizer68 = new wxBoxSizer(wxVERTICAL);

	m_staticText_blocksize = new wxStaticText(m_voronoi, wxID_ANY, wxT("BlockSize"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText_blocksize->Wrap(-1);
	bSizer68->Add(m_staticText_blocksize, 0, wxALL, 5);

	m_slider_blocksize = new wxSlider(m_voronoi, wxID_ANY, 20, 1, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_LABELS);
	m_slider_blocksize->SetToolTip(wxT("Size of each cell, bigger size leads to more border cells beeing calculated"));
	bSizer68->Add(m_slider_blocksize, 0, wxEXPAND | wxRIGHT, 10);

	m_staticline5 = new wxStaticLine(m_voronoi, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	bSizer68->Add(m_staticline5, 0, wxEXPAND | wxALL, 2);
	bSizer671->Add(bSizer68, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer69;
	bSizer69 = new wxBoxSizer(wxVERTICAL);

	m_staticText_fValue = new wxStaticText(m_voronoi, wxID_ANY, wxT("Intensity"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText_fValue->Wrap(-1);
	bSizer69->Add(m_staticText_fValue, 0, wxALL, 1);

	m_slider_fValue = new wxSlider(m_voronoi, wxID_ANY, 1, 1, 7, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS | wxSL_HORIZONTAL | wxSL_LABELS);
	m_slider_fValue->SetToolTip(wxT("Intensity of cell-borders"));

	bSizer69->Add(m_slider_fValue, 0, wxEXPAND | wxRIGHT, 10);
	bSizer671->Add(bSizer69, 1, wxEXPAND, 5);
	bSizer651->Add(bSizer671, 1, wxEXPAND, 5);

	m_voronoi->SetSizer(bSizer651);
	m_voronoi->Layout();
	bSizer651->Fit(m_voronoi);
	m_notebook3->AddPage(m_voronoi, wxT("Voronoi"), true);

	//#-----------------Combined Panel -------------------------------------------#//
	Combined_panel = new wxPanel(m_notebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	Combined_panel->SetToolTip(wxT("Factor of mixing for noise function"));

	wxBoxSizer* bSizer77;
	bSizer77 = new wxBoxSizer(wxHORIZONTAL);
	
	wxBoxSizer* bSizer80;
	bSizer80 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer81;
	bSizer81 = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* bSizer98;
	bSizer98 = new wxBoxSizer(wxVERTICAL);

	wxString m_choice21Choices[] = { wxT("Midpoint Displacement"), wxT("Perlin"), wxT("Voronoi") };
	int m_choice21NChoices = sizeof(m_choice21Choices) / sizeof(wxString);
	m_choice21 = new wxChoice(Combined_panel, CHOICEBOX1, wxDefaultPosition, wxDefaultSize, m_choice21NChoices, m_choice21Choices, 0);
	m_choice21->SetSelection(0);
	bSizer98->Add(m_choice21, 0, wxALL, 5);

	m_bitmap3 = new wxStaticBitmap(Combined_panel, wxID_ANY, arrowIcon, wxDefaultPosition, wxDefaultSize, 0);
	bSizer98->Add(m_bitmap3, 0, wxALIGN_CENTER_HORIZONTAL, 5);
	bSizer81->Add(bSizer98, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer99;
	bSizer99 = new wxBoxSizer(wxVERTICAL);

	m_spinCtrl2_add_factor_1 = new wxSpinCtrl(Combined_panel, SPINCTRL1, wxEmptyString, wxDefaultPosition, wxSize(100, -1), wxSP_ARROW_KEYS | wxSP_WRAP, 1, 10, 1);
	m_spinCtrl2_add_factor_1->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &combination_listbox1));
	m_spinCtrl2_add_factor_1->SetToolTip(wxT("Factor of mixing for noise function"));

	bSizer99->Add(m_spinCtrl2_add_factor_1, 0, wxTOP | wxRIGHT | wxLEFT, 5);
	bSizer81->Add(bSizer99, 0, wxEXPAND, 5);
	bSizer80->Add(bSizer81, 0, 0, 5);

	wxBoxSizer* bSizer82;
	bSizer82 = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* bSizer85;
	bSizer85 = new wxBoxSizer(wxVERTICAL);

	wxString m_choice2Choices[] = { wxT("Midpoint Displacement"), wxT("Perlin"), wxT("Voronoi"), wxT("None") };
	int m_choice2NChoices = sizeof(m_choice2Choices) / sizeof(wxString);
	m_choice2 = new wxChoice(Combined_panel, CHOICEBOX2, wxDefaultPosition, wxDefaultSize, m_choice2NChoices, m_choice2Choices, 0);
	m_choice2->SetSelection(0);
	bSizer85->Add(m_choice2, 0, wxALL, 5);

	m_bitmap4 = new wxStaticBitmap(Combined_panel, wxID_ANY, arrowIcon, wxDefaultPosition, wxDefaultSize, 0);
	bSizer85->Add(m_bitmap4, 0, wxALIGN_CENTER, 5);
	bSizer82->Add(bSizer85, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer97;
	bSizer97 = new wxBoxSizer(wxVERTICAL);

	m_spinCtrl2_add_factor_2 = new wxSpinCtrl(Combined_panel, SPINCTRL2, wxEmptyString, wxDefaultPosition, wxSize(100, -1), wxSP_ARROW_KEYS | wxSP_WRAP, 0, 10, 1);
	m_spinCtrl2_add_factor_2->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &combination_listbox2));
	m_spinCtrl2_add_factor_2->SetToolTip(wxT("Factor of mixing for noise function"));

	bSizer97->Add(m_spinCtrl2_add_factor_2, 0, wxALL | wxEXPAND, 5);
	bSizer82->Add(bSizer97, 0, wxEXPAND, 0);
	bSizer80->Add(bSizer82, 0, 0, 5);

	wxBoxSizer* bSizer83;
	bSizer83 = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* bSizer1011;
	bSizer1011 = new wxBoxSizer(wxVERTICAL);

	wxString m_choice22Choices[] = { wxT("Midpoint Displacement"), wxT("Perlin"), wxT("Voronoi"), wxT("None") };
	int m_choice22NChoices = sizeof(m_choice22Choices) / sizeof(wxString);
	m_choice22 = new wxChoice(Combined_panel, CHOICEBOX3, wxDefaultPosition, wxDefaultSize, m_choice22NChoices, m_choice22Choices, 0);
	m_choice22->SetSelection(3);
	bSizer1011->Add(m_choice22, 0, wxALL, 5);
	bSizer83->Add(bSizer1011, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer100;
	bSizer100 = new wxBoxSizer(wxVERTICAL);

	m_spinCtrl2_add_factor_3 = new wxSpinCtrl(Combined_panel, SPINCTRL3, wxEmptyString, wxDefaultPosition, wxSize(100, -1), wxSP_ARROW_KEYS | wxSP_WRAP, 0, 10, 0);
	m_spinCtrl2_add_factor_3->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &combination_listbox3));

	bSizer100->Add(m_spinCtrl2_add_factor_3, 0, wxALL, 5);
	bSizer83->Add(bSizer100, 0, wxEXPAND, 5);
	bSizer80->Add(bSizer83, 0, wxEXPAND, 5);
	bSizer77->Add(bSizer80, 1, wxEXPAND, 5);

	m_staticline8 = new wxStaticLine(Combined_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
	bSizer77->Add(m_staticline8, 0, wxEXPAND | wxALL, 5);

	wxBoxSizer* bSizer84;
	bSizer84 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer86;
	bSizer86 = new wxBoxSizer(wxHORIZONTAL);

	m_button_comb_generate = new wxButton(Combined_panel, COMBINED_GENERATE, wxT("Generate"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer86->Add(m_button_comb_generate, 1, wxALL | wxEXPAND, 5);

	m_staticline6 = new wxStaticLine(Combined_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
	bSizer86->Add(m_staticline6, 0, wxEXPAND | wxALL, 5);

	m_button_perturb = new wxButton(Combined_panel, BUTTON_PERTURB, wxT("Perturb"), wxDefaultPosition, wxDefaultSize, 0);
	m_button_perturb->SetToolTip(wxT("Apply perturbation on existing map"));

	bSizer86->Add(m_button_perturb, 1, wxALL | wxEXPAND, 5);
	bSizer84->Add(bSizer86, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer87;
	bSizer87 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer92;
	bSizer92 = new wxBoxSizer(wxHORIZONTAL);

	m_staticText37 = new wxStaticText(Combined_panel, wxID_ANY, wxT("Load / Save Presets:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText37->Wrap(-1);
	bSizer92->Add(m_staticText37, 1, wxALL, 5);
	bSizer92->Add(0, 0, 0, wxEXPAND, 5);

	m_staticText39 = new wxStaticText(Combined_panel, wxID_ANY, wxT("Noise Map:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText39->Wrap(-1);
	m_staticText39->SetToolTip(wxT("Noise Function for perturbation filter"));

	bSizer92->Add(m_staticText39, 1, wxALL, 5);
	bSizer87->Add(bSizer92, 0, wxEXPAND, 5);

	wxBoxSizer* bSizer93;
	bSizer93 = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* bSizer88;
	bSizer88 = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* bSizer90;
	bSizer90 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer911;
	bSizer911 = new wxBoxSizer(wxHORIZONTAL);

	m_bpButton_load_preset = new wxBitmapButton(Combined_panel, OPEN_BUT_PRESET, openIcon, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
	bSizer911->Add(m_bpButton_load_preset, 0, wxALL, 5);

	m_bpButton9_save_preset = new wxBitmapButton(Combined_panel, SAVE_BUT_PRESET, saveIcon, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
	bSizer911->Add(m_bpButton9_save_preset, 0, wxALL, 5);
	bSizer90->Add(bSizer911, 0, wxEXPAND, 5);

	wxBoxSizer* bSizer89;
	bSizer89 = new wxBoxSizer(wxVERTICAL);

	m_staticText_load_path = new wxStaticText(Combined_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_staticText_load_path->Wrap(-1);
	bSizer89->Add(m_staticText_load_path, 0, wxALL, 9);
	bSizer90->Add(bSizer89, 0, wxEXPAND, 5);
	bSizer88->Add(bSizer90, 19, wxEXPAND, 0);

	wxBoxSizer* bSizer91;
	bSizer91 = new wxBoxSizer(wxHORIZONTAL);

	m_staticline7 = new wxStaticLine(Combined_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
	bSizer91->Add(m_staticline7, 0, wxEXPAND | wxALL, 5);

	wxBoxSizer* bSizer901;
	bSizer901 = new wxBoxSizer(wxVERTICAL);

	m_radioBtn1_midpoint_displ = new wxRadioButton(Combined_panel, wxID_ANY, wxT("Midpoint displ."), wxDefaultPosition, wxDefaultSize, 0);
	bSizer901->Add(m_radioBtn1_midpoint_displ, 0, wxALL, 5);

	m_radioBtn2 = new wxRadioButton(Combined_panel, wxID_ANY, wxT("Perlin Noise"), wxDefaultPosition, wxDefaultSize, 0);
	m_radioBtn2->SetValue(true);
	bSizer901->Add(m_radioBtn2, 0, wxALL, 5);
	bSizer91->Add(bSizer901, 4, wxEXPAND, 0);
	bSizer88->Add(bSizer91, 20, wxEXPAND, 0);
	bSizer93->Add(bSizer88, 1, wxEXPAND, 5);
	bSizer87->Add(bSizer93, 1, wxEXPAND, 5);
	bSizer84->Add(bSizer87, 2, wxEXPAND, 5);
	bSizer77->Add(bSizer84, 1, wxEXPAND, 5);

	Combined_panel->SetSizer(bSizer77);
	Combined_panel->Layout();
	bSizer77->Fit(Combined_panel);
	m_notebook3->AddPage(Combined_panel, wxT("Combined"), true);

	bSizer5->Add(m_notebook3, 1, wxEXPAND | wxALL, 5);

	Subdivision->SetSizer(bSizer5);
	Subdivision->Layout();
	bSizer5->Fit(Subdivision);
	m_notebook2->AddPage(Subdivision, wxT("Subdivsion"), true);

	m_panel5 = new wxPanel(m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	//######### Erosion ################### //

	wxBoxSizer* bSizer991;
	bSizer991 = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* bSizer105;
	bSizer105 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer(wxHORIZONTAL);

	m_button4 = new wxButton(m_panel5, BUTTON_EROSION, wxT("Erosion"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer7->Add(m_button4, 0, wxALL, 5);

	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer(wxHORIZONTAL);

	m_staticText4 = new wxStaticText(m_panel5, wxID_ANY, wxT("Radius:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText4->Wrap(-1);
	m_staticText4->SetToolTip(wxT("Radius of effect"));
	bSizer8->Add(m_staticText4, 0, wxALL, 5);

	erosion_slider_radius = new wxSlider(m_panel5, EROSION_RADIUS_SLIDER, 3, 0, 5, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS | wxSL_HORIZONTAL | wxSL_LABELS);
	bSizer8->Add(erosion_slider_radius, 0, wxALL, 5);
	bSizer8->Add(0, 0, 1, wxEXPAND, 5);
	bSizer7->Add(bSizer8, 1, wxEXPAND, 5);
	bSizer105->Add(bSizer7, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer102;
	bSizer102 = new wxBoxSizer(wxVERTICAL);

	m_staticline9 = new wxStaticLine(m_panel5, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	bSizer102->Add(m_staticline9, 0, wxALL | wxEXPAND, 5);

	wxBoxSizer* bSizer103;
	bSizer103 = new wxBoxSizer(wxHORIZONTAL);

	m_button_thermal_erosion = new wxButton(m_panel5, BUTTON_THERMAL_EROSION, wxT("Thermal Erosion"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer103->Add(m_button_thermal_erosion, 0, wxALL, 5);

	m_staticText40 = new wxStaticText(m_panel5, wxID_ANY, wxT("Talus threshold:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText40->Wrap(-1);
	m_staticText40->SetToolTip(wxT("Threshold above which material is moved"));

	bSizer103->Add(m_staticText40, 0, wxALL, 5);

	m_staticText41 = new wxStaticText(m_panel5, wxID_ANY, wxT("0."), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText41->Wrap(-1);
	bSizer103->Add(m_staticText41, 0, wxLEFT | wxTOP, 5);

	m_textCtrl3 = new wxTextCtrl(m_panel5, TOGGLE_AUTO, wxT("015625"), wxDefaultPosition, wxDefaultSize, 0);
	m_textCtrl3->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &threshold_var));
	bSizer103->Add(m_textCtrl3, 0, wxRIGHT | wxSHAPED | wxTOP, 3);

	m_toggleBtn5 = new wxToggleButton(m_panel5, TOGGLE_AUTO, wxT("Auto"), wxDefaultPosition, wxDefaultSize, 0);
	m_toggleBtn5->SetValue(false);
	m_toggleBtn5->SetToolTip(wxT("Take 4/xsize as talus threshold"));
	m_toggleBtn5->SetMinSize(wxSize(40, -1));

	bSizer103->Add(m_toggleBtn5, 0, 0, 0);
	bSizer102->Add(bSizer103, 1, wxEXPAND, 5);
	bSizer105->Add(bSizer102, 1, wxLEFT, 5);
	bSizer991->Add(bSizer105, 1, wxEXPAND, 5);
	bSizer991->Add(0, 0, 1, wxEXPAND, 5);

	m_staticline10 = new wxStaticLine(m_panel5, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
	bSizer991->Add(m_staticline10, 0, wxALL | wxEXPAND, 5);

	wxBoxSizer* bSizer106;
	bSizer106 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer104;
	bSizer104 = new wxBoxSizer(wxVERTICAL);

	m_staticText5 = new wxStaticText(m_panel5, wxID_ANY, wxT("Iterationen"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText5->Wrap(-1);
	m_staticText5->SetToolTip(wxT("Number of iterations"));

	bSizer104->Add(m_staticText5, 0, wxALL, 5);

	erosion_slider_iterationen = new wxSlider(m_panel5, EROSION_ITER_SLIDER, 3, 0, 50, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS | wxSL_HORIZONTAL | wxSL_LABELS);
	bSizer104->Add(erosion_slider_iterationen, 0, wxALL, 5);
	bSizer106->Add(bSizer104, 1, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 5);
	bSizer991->Add(bSizer106, 1, wxALIGN_CENTER, 5);

	m_panel5->SetSizer(bSizer991);
	m_panel5->Layout();
	bSizer991->Fit(m_panel5);
	m_notebook2->AddPage(m_panel5, wxT("Erosion"), true);

	//####### Plateau ###### //

	m_panel6 = new wxPanel(m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer(wxHORIZONTAL);

	m_button7 = new wxButton(m_panel6, BUTTON_PLATEAU, wxT("Plateau"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer9->Add(m_button7, 0, wxALL, 5);

	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer(wxHORIZONTAL);

	m_staticText6 = new wxStaticText(m_panel6, wxID_ANY, wxT("Radius:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText6->Wrap(-1);
	bSizer10->Add(m_staticText6, 0, wxALL, 10);

	m_slider6 = new wxSlider(m_panel6, PLATEAU_RADIUS_SLIDER, 5, 1, (Alti.getXSize() >= Alti.getYSize()) ? (Alti.getXSize() - 1) : (Alti.getYSize() - 1), wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS | wxSL_HORIZONTAL | wxSL_LABELS);
	bSizer10->Add(m_slider6, 0, wxALL, 5);

	positionflagbitmap.LoadFile(wxT("BITMAP_POSITION_FLAG"), wxBITMAP_TYPE_BMP_RESOURCE);
	positionflagbitmap_selected.LoadFile(wxT("BITMAP_POSITION_FLAG_SELECTED"), wxBITMAP_TYPE_BMP_RESOURCE);

	positionflag = new wxBitmapButton(m_panel6, BITMAP_BUTTON_FLAGPOS, positionflagbitmap_selected, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
	positionflag->SetToolTip(wxT("Position of plateau"));
	bSizer10->Add(positionflag, 0, wxALIGN_TOP | wxALIGN_RIGHT | wxALL, 5);

	flag_pos_text = new wxStaticText(m_panel6, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1, -1), 0);
	flag_pos_text->Wrap(-1);
	bSizer10->Add(flag_pos_text, 0, wxALL, 5);
	bSizer9->Add(bSizer10, 1, wxEXPAND, 5);

	m_panel6->SetSizer(bSizer9);
	m_panel6->Layout();
	bSizer9->Fit(m_panel6);
	m_notebook2->AddPage(m_panel6, wxT("Plateau"), false);

	// ######## Load Image ####### //
	m_panel7 = new wxPanel(m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer(wxVERTICAL);

	m_button8 = new wxButton(m_panel7, BUTTON_OPEN_IMAGE, wxT("Load from Image"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer11->Add(m_button8, 0, wxALIGN_CENTER | wxALL | wxEXPAND, 5);

	//load_obj_panel = new wxPanel( m_notebook2,wxID_ANY , wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer(wxVERTICAL);

	load_obj = new wxButton(m_panel7, BUTTON_OPEN_OBJ, wxT("load .obj Data"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer17->Add(load_obj, 0, wxALIGN_CENTER_HORIZONTAL | wxALL | wxEXPAND, 5);
	bSizer11->Add(bSizer17, 1, wxEXPAND, 5);

	m_panel7->SetSizer(bSizer11);
	m_panel7->Layout();
	bSizer11->Fit(m_panel7);
	m_notebook2->AddPage(m_panel7, wxT("Load Image"), false);




	//##### .obj .tga save #####///
	save_files = new wxPanel(m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer(wxVERTICAL);

	save_obj_but = new wxButton(save_files, SAVE_OBJ, wxT("Save to .obj"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer19->Add(save_obj_but, 0, wxALL, 5);
	bSizer18->Add(bSizer19, 1, wxEXPAND, 5);

	m_staticline2 = new wxStaticLine(save_files, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	bSizer18->Add(m_staticline2, 0, wxEXPAND | wxALL, 5);

	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer(wxVERTICAL);

	save_tga_but = new wxButton(save_files, SAVE_IMAGE, wxT("Save as Image"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer20->Add(save_tga_but, 0, wxALL, 5);
	bSizer18->Add(bSizer20, 1, wxEXPAND, 5);

	save_files->SetSizer(bSizer18);
	save_files->Layout();
	bSizer18->Fit(save_files);
	m_notebook2->AddPage(save_files, wxT("save file"), false);

	//### scale ##///
	scale = new wxPanel(m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* Sizer_load_image_scale;
	Sizer_load_image_scale = new wxBoxSizer(wxHORIZONTAL);

	m_staticText7 = new wxStaticText(scale, wxID_ANY, wxT("Scale Terrainheight"), wxDefaultPosition, wxDefaultSize, 0);

	m_staticText7->SetToolTip(wxT("Only affects the visual GL window, not the actual hight"));
	Sizer_load_image_scale->Add(m_staticText7, 1, wxALL, 5);
	scale_image_terrain = new wxSlider(scale, SLIDER_SCALE_IMAGE_TERRAIN, 5, 1, 10, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS | wxSL_HORIZONTAL | wxSL_LABELS);
	Sizer_load_image_scale->Add(scale_image_terrain, 2, wxLEFT | wxRIGHT, 20);

	bSizer23->Add(Sizer_load_image_scale, 1, wxEXPAND | wxTOP, 5);

	wxBoxSizer* bSizer27;
	bSizer27 = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* bSizer28;
	bSizer28 = new wxBoxSizer(wxHORIZONTAL);

	m_scale_terrain_but = new wxButton(scale, BUTTON_SCALE_TERRAIN_REAL, wxT("Scale Terrain"), wxDefaultPosition, wxDefaultSize, 0);
	m_scale_terrain_but->SetToolTip(wxT("Scale the actual Terrain"));

	bSizer28->Add(m_scale_terrain_but, 0, wxALL, 5);
	bSizer28->Add(30, 0, 0, 0, 3); //scaler

	m_staticText_scale = new wxStaticText(scale, wxID_ANY, wxT("Size: 256 x 256"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer28->Add(m_staticText_scale, 0, wxALIGN_CENTER | wxALL, 5);
	bSizer27->Add(bSizer28, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer24;
	bSizer24 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer25;
	bSizer25 = new wxBoxSizer(wxVERTICAL);

	m_scale_terrain = new wxSlider(scale, SLIDER_SCALE_TERRAIN_REAL, 3, 0, 7, wxDefaultPosition, wxSize(210, -1), wxSL_AUTOTICKS | wxSL_HORIZONTAL);
	bSizer25->Add(m_scale_terrain, 0, wxALL, 5);
	bSizer24->Add(bSizer25, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer261;
	bSizer261 = new wxBoxSizer(wxVERTICAL);

	slider_labels = new wxStaticText(scale, wxID_ANY, wxT("0.25 0.5 0.75  1.0  1.5  2.0  3.0  4.0"), wxDefaultPosition, wxDefaultSize, 0);
	slider_labels->Wrap(-1);
	bSizer261->Add(slider_labels, 0, wxALL, 5);
	bSizer24->Add(bSizer261, 1, wxEXPAND, 5);
	bSizer27->Add(bSizer24, 1, wxEXPAND, 5);
	bSizer23->Add(bSizer27, 1, wxEXPAND, 5);

	scale->SetSizer(bSizer23);
	scale->Layout();
	bSizer23->Fit(scale);
	m_notebook2->AddPage(scale, wxT("Scale"), false);


	//########## Water_sim panel ########## //

	Water_sim_panel = new wxPanel(m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer271;
	bSizer271 = new wxBoxSizer(wxHORIZONTAL);

	m_notebook21 = new wxNotebook(Water_sim_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
	water_pan_1 = new wxPanel(m_notebook21, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer43;
	bSizer43 = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* bSizer44;
	bSizer44 = new wxBoxSizer(wxVERTICAL);

	Simulate_button = new wxButton(water_pan_1, BUTTON_WATER_SIMULATE, wxT("Simulate!"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer44->Add(Simulate_button, 0, wxALL, 5);


	wxString m_radioBox1_water_methodsChoices[] = { wxT("shallow water"), wxT("strict solver"), wxT("schnell "), wxT("hydraulic erosion") };
	int m_radioBox1_water_methodsNChoices = sizeof(m_radioBox1_water_methodsChoices) / sizeof(wxString);
	m_radioBox1_water_methods = new wxRadioBox(water_pan_1, wxID_ANY, wxT("Methods"), wxDefaultPosition, wxDefaultSize, m_radioBox1_water_methodsNChoices, m_radioBox1_water_methodsChoices, 1, wxRA_SPECIFY_COLS);
	m_radioBox1_water_methods->SetSelection(3);
	m_radioBox1_water_methods->SetItemToolTip(0, wxT("shallow water: Pipe reflux model, derivation of shallow water euqations"));
	m_radioBox1_water_methods->SetItemToolTip(1, wxT("strict solver: strict equations from procedural terrain generation"));
	m_radioBox1_water_methods->SetItemToolTip(2, wxT("fast hydraulic erosion: currently does not work!"));
	m_radioBox1_water_methods->SetItemToolTip(3, wxT("Hydraulische Erosion: erosion using simplified hydraulic equations, usually achieves best results"));

	bSizer44->Add(m_radioBox1_water_methods, 0, 0, 5);
	bSizer43->Add(bSizer44, 0, wxEXPAND, 5);

	wxBoxSizer* bSizer45;
	bSizer45 = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* bSizer46;
	bSizer46 = new wxBoxSizer(wxHORIZONTAL);

	m_staticline3 = new wxStaticLine(water_pan_1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
	bSizer46->Add(m_staticline3, 0, wxEXPAND | wxALL, 5);

	wxBoxSizer* bSizer51;
	bSizer51 = new wxBoxSizer(wxVERTICAL);

	m_staticText17 = new wxStaticText(water_pan_1, wxID_ANY, wxT("Amount of rain:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText17->Wrap(-1);
	bSizer51->Add(m_staticText17, 0, wxALL, 5);

	m_togglerain = new wxToggleButton(water_pan_1, TOGGLE_RAIN, wxT("Rain"), wxDefaultPosition, wxDefaultSize, 0);
	m_togglerain->SetValue(true);
	bSizer51->Add(m_togglerain, 2, wxEXPAND | wxLEFT | wxRIGHT, 4);
	bSizer51->Add(0, 0, 1, wxEXPAND, 2);
	bSizer46->Add(bSizer51, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer52;
	bSizer52 = new wxBoxSizer(wxHORIZONTAL);

	//rain amount
	m_slider12 = new wxSlider(water_pan_1, SLIDER_RAIN_WATER_AMOUNT, 10, 1, 10000, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_LABELS);
	m_slider12->SetToolTip(wxT("scales the amount of rain per time t"));
	bSizer52->Add(m_slider12, 0, wxALL, 5);

	m_staticline21 = new wxStaticLine(water_pan_1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
	bSizer52->Add(m_staticline21, 0, wxEXPAND | wxALL, 2);
	bSizer46->Add(bSizer52, 1, wxEXPAND, 5);
	bSizer45->Add(bSizer46, 0, wxEXPAND, 5);

	wxBoxSizer* bSizer47;
	bSizer47 = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* bSizer49;
	bSizer49 = new wxBoxSizer(wxVERTICAL);

	m_staticText18 = new wxStaticText(water_pan_1, wxID_ANY, wxT("Water amount:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText18->Wrap(-1);
	bSizer49->Add(m_staticText18, 0, wxALL, 5);

	m_staticText20 = new wxStaticText(water_pan_1, wxID_ANY, wxT("\n \n Source radius:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText20->Wrap(-1);
	bSizer49->Add(m_staticText20, 0, wxALL, 5);

	m_toggleBtn2 = new wxToggleButton(water_pan_1, TOGGLE_WATERSOURCE, wxT("Water source"), wxDefaultPosition, wxDefaultSize, 0);
	m_toggleBtn2->SetValue(true);
	bSizer49->Add(m_toggleBtn2, 1, wxALL, 0);
	bSizer47->Add(bSizer49, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer50;
	bSizer50 = new wxBoxSizer(wxVERTICAL);

	//Water amount
	m_slider13 = new wxSlider(water_pan_1, SLIDER_WATER_AMOUNT, 100, 1, 5000, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_LABELS | wxSL_TOP);
	m_slider13->SetToolTip(wxT("water amount in l"));
	bSizer50->Add(m_slider13, 0, wxALL, 5);

	//Water radius
	m_slider14 = new wxSlider(water_pan_1, SLIDER_WATER_RADIUS, 50, 0, 100, wxPoint(-1, -1), wxDefaultSize, wxSL_HORIZONTAL | wxSL_LABELS | wxSL_TOP);
	m_slider14->SetToolTip(wxT("radius of water source in cells"));
	bSizer50->Add(m_slider14, 0, wxALL, 5);

	bSizer47->Add(bSizer50, 1, wxEXPAND, 5);
	bSizer45->Add(bSizer47, 1, wxEXPAND, 5);
	bSizer43->Add(bSizer45, 1, wxEXPAND, 5);

	water_pan_1->SetSizer(bSizer43);
	water_pan_1->Layout();
	bSizer43->Fit(water_pan_1);
	m_notebook21->AddPage(water_pan_1, wxT("main"), false);
	pan_coefficients = new wxPanel(m_notebook21, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer41;
	bSizer41 = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* bSizer281;
	bSizer281 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer29;
	bSizer29 = new wxBoxSizer(wxHORIZONTAL);

	//sediment capacity
	m_staticText10 = new wxStaticText(pan_coefficients, wxID_ANY, wxT("sediment capacity"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText10->Wrap(-1);
	bSizer29->Add(m_staticText10, 0, wxALL, 5);


	m_slider8_KC = new wxSlider(pan_coefficients, SLIDER_SEDIMENT_CAPACITY, 15, 1, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_LABELS);
	m_slider8_KC->SetToolTip(wxT("How much sediment the water can carry"));
	bSizer29->Add(m_slider8_KC, 0, wxALL, 5);

	bSizer281->Add(bSizer29, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer30;
	bSizer30 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer32;
	bSizer32 = new wxBoxSizer(wxVERTICAL);

	m_staticText12 = new wxStaticText(pan_coefficients, wxID_ANY, wxT("                               0.01               1.0"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText12->Wrap(-1);
	bSizer32->Add(m_staticText12, 1, wxALL | wxEXPAND, 5);

	bSizer30->Add(bSizer32, 0, wxEXPAND, 5);

	wxBoxSizer* bSizer31;
	bSizer31 = new wxBoxSizer(wxHORIZONTAL);


	m_staticText11 = new wxStaticText(pan_coefficients, wxID_ANY, wxT("deposition constant"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText11->Wrap(-1);
	bSizer31->Add(m_staticText11, 0, wxALL, 5);

	//deposition constant
	m_slider9 = new wxSlider(pan_coefficients, SLIDER_DEPOSITION_CONSTANT, 50, 1, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
	bSizer31->Add(m_slider9, 0, wxALL, 5);

	bSizer30->Add(bSizer31, 1, wxEXPAND, 5);
	bSizer281->Add(bSizer30, 1, wxEXPAND, 5);
	bSizer41->Add(bSizer281, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer33;
	bSizer33 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer34;
	bSizer34 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer35;
	bSizer35 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer38;
	bSizer38 = new wxBoxSizer(wxVERTICAL);

	m_staticText15 = new wxStaticText(pan_coefficients, wxID_ANY, wxT("                           0.01                 1.0"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText15->Wrap(-1);
	bSizer38->Add(m_staticText15, 0, wxALL, 5);
	bSizer35->Add(bSizer38, 0, wxEXPAND, 5);

	wxBoxSizer* bSizer37;
	bSizer37 = new wxBoxSizer(wxHORIZONTAL);

	m_staticText13 = new wxStaticText(pan_coefficients, wxID_ANY, wxT("dissolving constant"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText13->Wrap(-1);
	bSizer37->Add(m_staticText13, 0, wxALL, 5);

	//dissolving constant
	m_slider10 = new wxSlider(pan_coefficients, SLIDER_DISSOLVING_CONSTANT, 30, 1, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
	bSizer37->Add(m_slider10, 0, wxALL, 5);
	bSizer35->Add(bSizer37, 1, wxEXPAND, 5);
	bSizer34->Add(bSizer35, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer36;
	bSizer36 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer40;
	bSizer40 = new wxBoxSizer(wxVERTICAL);

	m_staticText16 = new wxStaticText(pan_coefficients, wxID_ANY, wxT("                           0.01                 1.0"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText16->Wrap(-1);
	bSizer40->Add(m_staticText16, 0, wxALL, 5);
	bSizer36->Add(bSizer40, 0, wxEXPAND, 5);

	wxBoxSizer* bSizer39;
	bSizer39 = new wxBoxSizer(wxHORIZONTAL);

	m_staticText14 = new wxStaticText(pan_coefficients, wxID_ANY, wxT("evaporation const"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText14->Wrap(-1);
	bSizer39->Add(m_staticText14, 0, wxALL, 5);

	//evaporation constant
	m_slider11 = new wxSlider(pan_coefficients, SLIDER_EVAPORATION_CONSTANT, 50, 20, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
	bSizer39->Add(m_slider11, 0, wxALL, 5);

	bSizer36->Add(bSizer39, 1, wxEXPAND, 5);
	bSizer34->Add(bSizer36, 1, wxEXPAND, 5);
	bSizer33->Add(bSizer34, 1, wxEXPAND, 5);
	bSizer41->Add(bSizer33, 1, wxEXPAND, 5);

	pan_coefficients->SetSizer(bSizer41);
	pan_coefficients->Layout();
	bSizer41->Fit(pan_coefficients);
	m_notebook21->AddPage(pan_coefficients, wxT("coefficients"), false);
	m_panel13 = new wxPanel(m_notebook21, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer53;
	bSizer53 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer55;
	bSizer55 = new wxBoxSizer(wxHORIZONTAL);

	m_staticText28 = new wxStaticText(m_panel13, wxID_ANY, wxT("               0.01                 1.0"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText28->Wrap(-1);
	bSizer55->Add(m_staticText28, 0, wxALL, 5);
	bSizer55->Add(0, 0, 1, wxEXPAND, 5);

	m_staticText27 = new wxStaticText(m_panel13, wxID_ANY, wxT("0.01                  1"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText27->Wrap(-1);
	bSizer55->Add(m_staticText27, 0, wxALL, 5);

	bSizer53->Add(bSizer55, 0, wxEXPAND, 5);

	wxBoxSizer* bSizer54;
	bSizer54 = new wxBoxSizer(wxHORIZONTAL);

	m_staticText26 = new wxStaticText(m_panel13, wxID_ANY, wxT("Timestep:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText26->Wrap(-1);
	bSizer54->Add(m_staticText26, 0, wxALL, 5);

	m_slider20 = new wxSlider(m_panel13, SLIDER_TIMESTEP, 10, 1, 1000, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
	bSizer54->Add(m_slider20, 0, wxALL, 5);
	bSizer54->Add(0, 0, 1, wxEXPAND, 5);

	m_staticText261 = new wxStaticText(m_panel13, wxID_ANY, wxT("Minimum alpha:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText261->Wrap(-1);
	m_staticText261->SetToolTip(wxT("The minimum angle between two cells for sediment computation"));

	bSizer54->Add(m_staticText261, 0, wxALL, 5);

	m_slider18 = new wxSlider(m_panel13, MINIMUM_ALPHA, 50, 1, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
	bSizer54->Add(m_slider18, 0, wxALL, 5);
	bSizer53->Add(bSizer54, 0, wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer(new wxStaticBox(m_panel13, wxID_ANY, wxT("Pipe")), wxHORIZONTAL);

	m_staticText30 = new wxStaticText(m_panel13, wxID_ANY, wxT("length_x:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText30->Wrap(-1);
	sbSizer2->Add(m_staticText30, 0, wxALL, 5);

	wxBoxSizer* bSizer541;
	bSizer541 = new wxBoxSizer(wxVERTICAL);

	m_staticText32 = new wxStaticText(m_panel13, wxID_ANY, wxT("0.2                  20"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText32->Wrap(-1);
	bSizer541->Add(m_staticText32, 0, wxALL, 5);

	m_slider22 = new wxSlider(m_panel13, wxID_ANY, 10, 2, 200, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
	bSizer541->Add(m_slider22, 0, wxALL, 5);

	sbSizer2->Add(bSizer541, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer551;
	bSizer551 = new wxBoxSizer(wxHORIZONTAL);

	m_staticText31 = new wxStaticText(m_panel13, wxID_ANY, wxT("Iterations: "), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText31->Wrap(-1);
	bSizer551->Add(m_staticText31, 0, wxALL, 5);

	m_slider21 = new wxSlider(m_panel13, SLIDER_ITERATIONS, 10, 2, 200, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_LABELS);
	bSizer551->Add(m_slider21, 0, wxALL, 5);

	sbSizer2->Add(bSizer551, 1, wxEXPAND, 5);

	bSizer53->Add(sbSizer2, 1, wxEXPAND, 5);

	m_panel13->SetSizer(bSizer53);
	m_panel13->Layout();
	bSizer53->Fit(m_panel13);
	m_notebook21->AddPage(m_panel13, wxT("expert"), false);

	//Visualisation constrols
	m_panel131 = new wxPanel(m_notebook21, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer56;
	bSizer56 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer57;
	bSizer57 = new wxBoxSizer(wxHORIZONTAL);

	m_toggleBtn3 = new wxToggleButton(m_panel131, TOGGLE_SEDIMENT_PREVIEW, wxT("Sediment_preview"), wxDefaultPosition, wxDefaultSize, 0);
	m_toggleBtn3->SetValue(true);
	bSizer57->Add(m_toggleBtn3, 0, wxALL, 5);

	m_toggleBtn4 = new wxToggleButton(m_panel131, TOGGLE_WATER_PREVIEW, wxT("Water_preview"), wxDefaultPosition, wxDefaultSize, 0);
	m_toggleBtn3->SetValue(true);
	bSizer57->Add(m_toggleBtn4, 0, wxALL, 5);

	bSizer56->Add(bSizer57, 1, wxEXPAND, 5);

	m_panel131->SetSizer(bSizer56);
	m_panel131->Layout();
	bSizer56->Fit(m_panel131);
	m_notebook21->AddPage(m_panel131, wxT("visualization"), false);

	//add water simpanel
	bSizer271->Add(m_notebook21, 1, wxEXPAND | wxALL, 5);

	Water_sim_panel->SetSizer(bSizer271);
	Water_sim_panel->Layout();
	bSizer271->Fit(Water_sim_panel);
	m_notebook2->AddPage(Water_sim_panel, wxT("Water_sim"), false);

	//#### Put everything into Panel1 ####//	

	panelsizer->Add(m_notebook2, 1, wxGROW, 20);

	// OpenGL Window
	wxBoxSizer *glPlanesizer = new wxBoxSizer(wxHORIZONTAL);
	//glPanel = new wxPanel(this, wxID_ANY,wxDefaultPosition,wxSize(width,500));
	int args[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0 };
	glPlane = new GLPane((wxFrame*)m_panel12, args); //das OPENGL Fenster
	glPlanesizer->Add(glPlane, 1, wxGROW, 20);

	//add both GUI areas into one sizer
	generalsizer->Add(panelsizer, 4, wxEXPAND | wxFIXED_MINSIZE | wxALIGN_TOP);
	generalsizer->Add(glPlanesizer, 8, wxGROW); //proportion 4:8:0

	m_panel12->SetSizer(generalsizer);
	m_panel12->Layout();
	generalsizer->Fit(m_panel12);

	//### Panel 2 with image #### ///
	panelPreviewPicture = new BitmapPanel(m_splitter3, PANEL_PREVIEW_PICTURE, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, "panel");
	wxBoxSizer* bSizer22;
	bSizer22 = new wxBoxSizer(wxVERTICAL);

	arrowRB = new wxBitmap(wxT("BITMAP_ARROW_RB"), wxBITMAP_TYPE_RESOURCE);
	arrowLT = new wxBitmap(wxT("BITMAP_ARROW_LT"), wxBITMAP_TYPE_RESOURCE);
	open_close_pic_but = new wxBitmapButton(panelPreviewPicture, ON_OFF_PIC_BUT, *arrowRB, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
	bSizer22->Add(open_close_pic_but, 0, wxALL, 5);

	crosshair_bmp = new wxBitmap(wxT("BITMAP_CROSSHAIR"), wxBITMAP_TYPE_RESOURCE);
	crosshair_bmp_deact = new wxBitmap(wxT("BITMAP_CROSSHAIR_DEACTIVATED"), wxBITMAP_TYPE_RESOURCE);
	m_bitmap_button_setflag = new wxBitmapButton(panelPreviewPicture, BUTTON_TOGGLE_CROSSHAIR, *crosshair_bmp, wxPoint(100, 100), wxDefaultSize, wxBU_AUTODRAW | wxSUNKEN_BORDER);
	m_bitmap_button_setflag->SetToolTip(wxT("Switch on to set position flag on the preview picture"));


	bSizer22->Add(m_bitmap_button_setflag, 0, wxALIGN_RIGHT | wxALL, 5);

	panelPreviewPicture->SetSizer(bSizer22);
	m_splitter3->SplitVertically(m_panel12, panelPreviewPicture, 700);
	m_splitter3->SetMinimumPaneSize(55); //prevents unsplit
	superSizer->Add(m_splitter3, 1, wxEXPAND, 5);


	m_notebook2->SetImageList(imagelist);
	m_notebook2->SetPageImage(0, 0);
	m_notebook2->SetPageImage(1, 1);
	m_notebook2->SetPageImage(4, 2);
	m_notebook2->SetPageImage(2, 3);
	m_notebook2->SetPageImage(3, 4);
	m_notebook2->SetPageImage(5, 5);
	m_notebook2->SetPageImage(6, 6);
	
	//## Icons  ## //
	icon.LoadFile(wxT("ICON_TERRAIN"), wxBITMAP_TYPE_ICO_RESOURCE);
	icon_big.LoadFile(wxT("ICON_TERRAIN_BIG"), wxBITMAP_TYPE_ICO_RESOURCE);
	icon_bundle.AddIcon(icon);
	icon_bundle.AddIcon(icon_big);

	errorstream.open("error.txt");
	sediment_preview_on = true;
	water_preview_on = true;


	SetSizer(superSizer);
	SetAutoLayout(TRUE);
	Layout();
}

SimGUI::~SimGUI() {
	delete glPlane;
	//delete preview_bitmap;
	delete crosshair_bmp, delete arrowRB, delete arrowLT, delete crosshair_bmp_deact;
	//delete Save_mask;
	//delete pfeil_mask;
	//delete Open_mask;
	errorstream.close();
	delete[] imagelist;



}


void SimGUI::openClosePicEvent(wxCommandEvent &event){
	if (sideWindow == true) {
		sideWindow = false;
		m_splitter3->GetWindow2()->ClearBackground();
		m_splitter3->GetWindow2()->Update(); 
		m_splitter3->SetSashPosition(int((this->GetSize()).GetWidth()) - 70, true);
		open_close_pic_but->SetBitmapLabel(*arrowLT);
	}
	else {
		sideWindow = true;
		m_splitter3->SetSashPosition(int((this->GetSize()).GetWidth()) - Alti.getXSize() - 40, true);
		updateImage();
		manualRender();
		open_close_pic_but->SetBitmapLabel(*arrowRB);
		//panel_preview_picture->init_flag();
	}
}

void SimGUI::updateImage(void){
	wxImage temp_pic = Alti.makePreviewImage();
	global_preview_bitmap = wxBitmap(temp_pic, -1);

	if (sediment_preview_on){
		g_sedimentBitmap = wxBitmap(Alti.makeSedimentImage());
	}

	temp_pic.Destroy(); //prevents memory leaks
}


void SimGUI::manualRender(){ //single frame render
	if (sideWindow == true) {
		m_splitter3->UpdateSize();
		panelPreviewPicture->render();
	}
}

void SimGUI::handleSashPosition(wxSplitterEvent &event){
	manualRender();
}


void SimGUI::drawPoint(wxMouseEvent &event){
	wxPen def_pen = wxPen("wxRED_PEN", 1, 1);
	wxClientDC dc(m_splitter3->GetWindow2());
	dc.DrawPoint(event.GetX(), event.GetY());
	m_splitter3->SetSashPosition(int((this->GetSize()).GetWidth()) - 70, true);
	event.Skip();

}

void SimGUI::toggleCrosshair(wxCommandEvent &event){
	if (crosshair == true){
		crosshair = false;
		m_bitmap_button_setflag->SetBitmapLabel(*crosshair_bmp_deact);
	}
	else{
		crosshair = true;
		m_bitmap_button_setflag->SetBitmapLabel(*crosshair_bmp);
	}
	panelPreviewPicture->ToggleFlag(crosshair);
}


void SimGUI::updateChain(void){
	updateImage();
	manualRender();
	glPlane->refreshTerrain();
	glPlane->Refresh();
}



void SimGUI::buttonClick(wxCommandEvent &event){
	Close();
}
void SimGUI::erosionButtonEvent(wxCommandEvent &event){
	Alti.erosion(erosionRadius, erosionIterations);
	updateChain();
}

void SimGUI::thermalErosionButtonEvent(wxCommandEvent &event){
	double Talus;
	if (toggleTalusAuto == true)
	{
		Talus = 4.0 / Alti.getXSize(); 
	}
	else
	{
		Talus = getTalusThreshold();
	}
	int iterations = erosion_slider_iterationen->GetValue();
	Alti.thermalErosion(iterations * 10, Talus);
	updateChain();
}

void SimGUI::toggleAutoEvent(wxCommandEvent &event){
	toggleTalusAuto = (bool)m_toggleBtn5->GetValue();
	if (toggleTalusAuto == true)
	{
		m_textCtrl3->Clear();
		string num = numToString(double(4.0 / Alti.getXSize()));
		num = num.erase(0, 2);
		*m_textCtrl3 << num;
		m_textCtrl3->Enable(false);

	}
	else
		m_textCtrl3->Enable(true);
}

double SimGUI::getTalusThreshold()
{
	wxString inp = m_textCtrl3->GetLineText(0);

	stringstream NumberString;
	NumberString << "0." << inp;
	string Number = NumberString.str();
	char* str = (char*)Number.data();
	double num = atof(str);

	return num;
}

void SimGUI::plateauButtonEvent(wxCommandEvent &event){
	if (Alti.showflag == true){
		Alti.plateau(Alti.FlagX, Alti.FlagY, plateauRadius);
		updateChain();
		flag_pos_text->SetLabel(wxT(""));
	}
	else {
		flag_pos_text->SetLabel(wxT("<-positionflag must be activated!"));
	}
}

void SimGUI::randomizeButtonEvent(wxCommandEvent &event){
	Alti.randomize(randomizeRadius);
	updateImage();
	manualRender();
	glPlane->refreshTerrain();
	glPlane->Refresh();
}

void SimGUI::subdivideButtonEvent(wxCommandEvent &event){
	Alti.subdivision(foldingCoeff, 0.0, 0.0, 0.0, 0.0);
	updateImage();
	manualRender();
	glPlane->refreshTerrain(); 
	glPlane->Refresh();
}

void SimGUI::saveObjEvent(wxCommandEvent &event){
	//Open file dialog
	wxFileDialog *dlg = new wxFileDialog(
		this, wxT("Save .obj file"), wxT(""), wxT(""),
		wxT("Wavefront files (.obj)|*.obj"),
		wxFD_SAVE | wxFD_CHANGE_DIR | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);
	if (dlg->ShowModal() == wxID_OK) {
		bool res = Alti.saveOBJ((char *)dlg->GetFilename().ToStdString().c_str());
		if (res == false){
			errorstream << "error while saving .obj" << endl;
		}
	}
}

void SimGUI::saveImageEvent(wxCommandEvent &event){

	wxFileDialog *dlg = new wxFileDialog(
		this, wxT("Save as Image"), wxT(""), wxT(""),
		wxT("Portable Network Graphic (.png)|*.png | JPEG Image (.jpg) |*.jpg | Bitmap (.bmp) | *.bmp"),
		wxFD_SAVE | wxFD_CHANGE_DIR | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);
	if (dlg->ShowModal() == wxID_OK) {
		//#### format extraction### ///
		string format;
		string wildcard(dlg->GetPath().c_str());
		string::size_type pos = wildcard.find_last_of(".", wildcard.length());
		format = wildcard.substr(pos, wildcard.length());
		if (format.at(format.length() - 1) == ' '){
			format.erase(format.length() - 1, 1); 
		}


		bool res = Alti.saveImage((char *)dlg->GetFilename().ToStdString().c_str(), format);
		if (res == false){
			errorstream << "error while saving image file " << endl;
		}
	}
}







void SimGUI::openImageButtonEvent(wxCommandEvent &event){
	//Open file dialog
	wxFileDialog *dlg = new wxFileDialog(
		this, wxT("Öfnnen der .tga Image Datei"), wxT(""), wxT(""),
		wxT("Image Dateien (png,jpg,bmp,tga)|*.tga;*.png;*.jpg;*.bmp |Targa Images (.tga)|*.tga|PNG Images (.png)|*.png|JPEG Images (.jpg)|*.jpg|Bitmaps (.bmp)|*.bmp "),
		wxFD_OPEN | wxFD_CHANGE_DIR, wxDefaultPosition);

	if (dlg->ShowModal() == wxID_OK) {
		int stat = Alti.terrainLoadFromImage((char *)dlg->GetFilename().ToStdString().c_str(), 0); //normals  off
		errorstream << "Loadimage " << stat << dlg->GetFilename().ToStdString().c_str() << endl;
		if (stat != 0) { 
			int stat2 = Alti.terrainLoadFromImage((char *)dlg->GetFilename().ToStdString().c_str(), 0, 5); 
			errorstream << "Loadimage2 " << stat2 << endl;
		}

	}
	dlg->Destroy();
	updateChain();
}


void SimGUI::openObjButtonEvent(wxCommandEvent &event){
	//Open file dialog
	wxFileDialog *dlg = new wxFileDialog(
		this, wxT("Open .obj Wavefront 3D File"), wxT(""), wxT(""),
		wxT("Wavefront Files (.obj)|*.obj|All Files(*.*)|*.*"),
		wxFD_OPEN | wxFD_CHANGE_DIR, wxDefaultPosition);

	if (dlg->ShowModal() == wxID_OK) {
		int stat = Alti.loadOBJ((char *)dlg->GetFilename().ToStdString().c_str()); //normals  off
		Alti.FlagX = (int)Alti.getXSize() / 2;
		Alti.FlagY = (int)Alti.getYSize() / 2;

	}
	dlg->Destroy();
	updateImage();
	manualRender();
	glPlane->refreshTerrain();
	glPlane->Refresh();
}



void SimGUI::positionFlagButtonEvent(wxCommandEvent &event){
	if (Alti.showflag == true) {
		positionflag->SetBitmapLabel(positionflagbitmap); //unselected state
		Alti.showflag = false;

	}
	else{
		positionflag->SetBitmapLabel(positionflagbitmap_selected); //selected state
		Alti.showflag = true;
	}
	glPlane->Refresh();
}

//-----------Perlin----------------
void SimGUI::sliderPersistenceEvent(wxCommandEvent &event){
	perlinPersistence = double(m_slider201->GetValue()) / 100.0;
}

void SimGUI::sliderPerlinZoomEvent(wxCommandEvent &event){
	perlinZoom = double(m_slider211->GetValue()) / 100.0;
}

void SimGUI::sliderOctavesEvent(wxSpinEvent  &event){
	octaves = int(m_spinCtrl1->GetValue());

}
void  SimGUI::perlinNoiseEvent(wxCommandEvent &event){
	Alti.perlin(nullptr, perlinPersistence, octaves, perlinZoom);
	updateImage();
	manualRender();
	glPlane->refreshTerrain(); 
	glPlane->Refresh();
}



//----------Voronoi----------------
void SimGUI::voronoi(double inputMap[]) 
{
	wxString clip_num = m_textCtrl1->GetLineText(0);
	clip_num.Replace(wxT("."), wxT(""), true);
	clip_num.Replace(wxT("e"), wxT(""), true);
	clip_num.Replace(wxT("-"), wxT(""), true);
	clip_num.Replace(wxT("+"), wxT(""), true);
	clip_num.insert(0, wxT("0."));
	double clip = 0.0;
	clip_num.ToDouble(&clip);

	int blockSize = int(m_slider_blocksize->GetValue());
	int intesity = int(m_slider_fValue->GetValue());
	bool negative = m_checkBox_negative->GetValue();
	int distanceMethod = m_choice_distance->GetCurrentSelection() + 1;

	wxString coeffs_str = m_textCtrl2_vor_coeff->GetLineText(0);
	if (coeffs_str.size() != 0)
	{

		coeffs_str += ";"; 
		wxString word = wxT("");
		float  tmp[20];
		double coeff;
		int words = 0;

		for (size_t i = 0; i < coeffs_str.size(); i++){
			if (coeffs_str[i] != ';')
				word += coeffs_str[i];
			else
			{
				word.ToDouble(&coeff);
				tmp[words] = float(coeff);
				word = wxT("");
				words++;
			}
		}
		if (inputMap == NULL)
			Alti.voronoiTex(intesity, blockSize, negative, float(clip), distanceMethod, tmp, words);
		else
			Alti.voronoiTex(intesity, blockSize, negative, float(clip), distanceMethod, inputMap, tmp, words);

	}

	else{
		if (inputMap == NULL)
			Alti.voronoiTex(intesity, blockSize, negative, float(clip), distanceMethod);
		else
			Alti.voronoiTex(intesity, blockSize, negative, float(clip), distanceMethod, inputMap);
	}
}

void SimGUI::buttonVoronoiEvent(wxCommandEvent &event){
	voronoi();

	updateImage();
	manualRender();
	glPlane->refreshTerrain();
	glPlane->Refresh();
}

//-----------------------Combined-----------------------------
void SimGUI::spinctrl1Event(wxSpinEvent &event){
	factorCombination1 = m_spinCtrl2_add_factor_1->GetValue();
	if (factorCombination1 == 0)
	{
		factorCombination1 = 1;
		m_spinCtrl2_add_factor_1->SetValue(1); //first listbox should never be 0
	}
}

void SimGUI::spinctrl2Event(wxSpinEvent &event){
	factorCombination2 = m_spinCtrl2_add_factor_2->GetValue();
}

void SimGUI::spinctrl3Event(wxSpinEvent &event){
	factorCombination3 = m_spinCtrl2_add_factor_3->GetValue();
}

void SimGUI::choiceCombined1Event(wxCommandEvent &event){
	choiceCombination1 = m_choice21->GetCurrentSelection();
}
void SimGUI::choiceCombined2Event(wxCommandEvent &event){
	choiceCombination2 = m_choice2->GetCurrentSelection();
	if (choiceCombination2 == 3) // 3 = None
	{
		factorCombination2 = 0;
		m_spinCtrl2_add_factor_2->SetValue(0);
	}

}
void SimGUI::choiceCombined3Event(wxCommandEvent &event){
	choiceCombination3 = m_choice22->GetCurrentSelection();
	if (choiceCombination3 == 3) // 3 = None
	{
		factorCombination3 = 0;
		m_spinCtrl2_add_factor_3->SetValue(0);
	}
	else if (choiceCombination3 != 3 && factorCombination3 == 0){
		factorCombination3 = 1;
	}
}


bool SimGUI::executeChoice(int choice, double inp_map[])
{
	if (choice != 3) // 3 = None
	{
		if (choice == 0) //0 = Midpoint Displacement
		{
			Alti.subdivision(foldingCoeff, inp_map);
		}
		else if (choice == 1) //1 = Perlin
		{
			perlinPersistence = double(m_slider201->GetValue()) / 100.0;
			perlinZoom = double(m_slider211->GetValue()) / 100.0;
			octaves = int(m_spinCtrl1->GetValue());
			Alti.perlin(inp_map, perlinPersistence, octaves, perlinZoom);
		}
		else if (choice == 2) // 2 = Voronoi
		{
			this->voronoi(inp_map);
		}
		return true;
	}
	else {
		return false;
	}
}

void SimGUI::combinedGenerateEvent(wxCommandEvent &event){
	int xsize = Alti.getXSize();
	int ysize = Alti.getYSize();
	copyMap1 = new double[xsize * ysize + 1];
	copyMap2 = new double[xsize * ysize + 1];
	copyMap3 = new double[xsize * ysize + 1];
	executeChoice(choiceCombination1, copyMap1);
	bool two = executeChoice(choiceCombination2, copyMap2);
	bool three = executeChoice(choiceCombination3, copyMap3);
	int whole = factorCombination1 + factorCombination2 + factorCombination3;
	float fac_one = factorCombination1 / float(whole);
	float fac_two = factorCombination2 / float(whole);
	float fac_three = factorCombination3 / float(whole);
	double val;


	for (int x = 0; x < xsize; ++x){
		for (int y = 0; y < ysize; ++y){
			Alti.writeAltitude(x, y, copyMap1[x*ysize + y] * fac_one);
			if (two)
			{
				val = (copyMap2[x*ysize + y])*fac_two;
				Alti.add(x, y, val);
			}
			if (three)
				Alti.multiply(x, y, copyMap3[x*ysize + y] * fac_three);
		}
	}

	delete[] copyMap1;
	delete[] copyMap2;
	delete[] copyMap3;

	updateImage();
	manualRender();
	glPlane->refreshTerrain();
	glPlane->Refresh();
}

void SimGUI::buttonSavePresetEvent(wxCommandEvent &event){
	wxFileDialog *dlg = new wxFileDialog(
		this, wxT("Save Preset"), wxT(""), wxT(""),
		wxT("Text (.txt) | *.txt"),
		wxFD_SAVE | wxFD_CHANGE_DIR | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);
	if (dlg->ShowModal() == wxID_OK) {
		string path(dlg->GetPath().c_str());
		savePresetText(path);
	}
}

void SimGUI::buttonLoadPresetEvent(wxCommandEvent &event){
	wxFileDialog *dlg = new wxFileDialog(
		this, wxT("Load Preset"), wxT(""), wxT(""),
		wxT("Text (.txt) | *.txt"),
		wxFD_OPEN | wxFD_CHANGE_DIR, wxDefaultPosition);
	if (dlg->ShowModal() == wxID_OK) {
		string path(dlg->GetPath().c_str());
		loadPresetText(path);
	}
	dlg->Destroy();
}

void SimGUI::savePresetText(string path)
{
	save_preset_stream.open(path.c_str());
	if (save_preset_stream.fail() == true)
	{
		errorDialog(std::string("Error creating File"));
	}
	save_preset_stream << "#Terrain Generator Preset File";
	save_preset_stream << getValues(m_choice21->GetCurrentSelection());
	save_preset_stream << "Fac:" << numToString(factorCombination1);

	save_preset_stream << getValues(m_choice2->GetCurrentSelection());
	save_preset_stream << "Fac:" << numToString(factorCombination2);

	save_preset_stream << getValues(m_choice22->GetCurrentSelection());
	save_preset_stream << "Fac:" << numToString(factorCombination3);

	save_preset_stream.close();
}

void SimGUI::loadPresetText(string path)
{
	char buffer[512];
	read_preset_stream.open(path.c_str());

	read_preset_stream.seekg(0, ios::end);
	int endpos = read_preset_stream.tellg();

	read_preset_stream.seekg(0, ios::beg); 
	if (read_preset_stream.fail() == true)
	{
		errorDialog(string("Error reading File"));
		read_preset_stream.close();
		return;
	}
	string first = readline(read_preset_stream, buffer);
	if (first.compare("#Terrain Generator Preset File") != 0)
	{
		errorDialog(string("Not a preset File"));
		read_preset_stream.close();
		return;
	}

	int choice = 0; 
	while (int(read_preset_stream.tellg()) != endpos)
	{
		string line = readline(read_preset_stream, buffer);

		if (line.empty() == true)
		{
			continue;
		}
		string::size_type tp = line.find("#");//new noise method
		if (tp != string::npos)//if noise method exists
		{
			string Noise_method = line.substr(tp + 1);
			if (Noise_method.compare("Midpoint") == 0)
			{
				changeWxChoice(0, choice);//0=midpoint
			}
			else if (Noise_method.compare("Perlin") == 0)
			{
				changeWxChoice(1, choice);//1=Perlin
			}
			else if (Noise_method.compare("Voronoi") == 0)
			{
				changeWxChoice(2, choice);//2=Voronoi
			}
			else if (Noise_method.compare("None") == 0)
			{
				changeWxChoice(3, choice);//3=None
			}
			else
			{
				errorDialog(string("Error in Preset File"));
			}
			choice++;
		}
		else if (line.find(":") != string::npos)
		{
			string prop = line.substr(0, line.find(":"));
			string value = line.substr(line.find(":") + 1);
			int type = 1;
			float ftype = 1.0;
			int iValue = stringToNum(value, type); 
			float flValue = stringToNum(value, ftype);

			if (prop.find("Persistence") != string::npos)
			{
				m_slider201->SetValue(int(flValue * 100.0));
				perlinPersistence = double(m_slider201->GetValue()) / 100.0;
			}
			else if (prop.find("Zoom") != string::npos)
			{
				m_slider211->SetValue(int(flValue * 100.0));
				perlinZoom = double(m_slider211->GetValue()) / 100.0;
			}
			else if (prop.find("Octaves") != string::npos)
			{
				m_spinCtrl1->SetValue(iValue);
				octaves = int(m_spinCtrl1->GetValue());
			}
			else if (prop.find("Fac") != string::npos)
			{
				switch (choice)
				{
				case 1://if num has be increased at least once
					m_spinCtrl2_add_factor_1->SetValue(iValue);
					factorCombination1 = iValue;
					break;
				case 2:
					m_spinCtrl2_add_factor_2->SetValue(iValue);
					factorCombination2 = iValue;
					break;
				case 3:
					m_spinCtrl2_add_factor_3->SetValue(iValue);
					factorCombination3 = iValue;
					break;
				}
			}
			else if (prop.find("BlockSize") != string::npos)
			{
				m_slider_blocksize->SetValue(iValue);
			}
			else  if (prop.find("Intesity") != string::npos)
			{
				m_slider_fValue->SetValue(iValue);
			}
			else if (prop.find("Negative") != string::npos)
			{
				m_checkBox_negative->SetValue(iValue);
			}
			else if (prop.find("Distance") != string::npos)
			{
				m_choice_distance->SetSelection(iValue);
			}
			else if (prop.find("Coefficients") != string::npos)
			{
				m_textCtrl2_vor_coeff->Clear();//remove current selection
				m_textCtrl2_vor_coeff->AppendText(value);
			}
			else if (prop.find("Clip") != string::npos)
			{
				m_textCtrl1->Clear();
				m_textCtrl1->AppendText(value);
			}
			else if (prop.find("Coeff") != string::npos)
			{
				subdiv_slider_coeff->SetValue(iValue);
				foldingCoeff = double(subdiv_slider_coeff->GetValue()) / float(coeff_sizer);
			}
		}
	}

	read_preset_stream.close();
}


void SimGUI::changeWxChoice(int choice, int num /*choicebox number*/)
{
	switch (num)
	{
	case 0:
		m_choice21->SetSelection(choice);
		choiceCombination1 = choice;
		break;
	case 1:
		m_choice2->SetSelection(choice);
		choiceCombination2 = choice;
		break;
	case 2:
		m_choice22->SetSelection(choice);
		choiceCombination3 = choice;
		break;
	}
}

string  SimGUI::readline(std::ifstream& file, char buffer[])
{
	file.getline(buffer, 512);
	stringstream Line;
	Line << buffer;
	return Line.str();
}

template <class T>
T SimGUI::stringToNum(string input, T wished_type)
{
	double num = atof(input.c_str());
	T out = static_cast<T>(num);
	return out;
}


template <class T>
string SimGUI::numToString(T input)
{
	stringstream NumberString;
	NumberString << input;
	string Number = NumberString.str();
	return Number;
}

string SimGUI::getValues(int choice)
{
	string out("\n#");
	if (choice == 0) //0 = Midpoint Displacement
	{
		out.append("Midpoint");
		out.append("\n");
		out.append("Coeff:").append(numToString(foldingCoeff)).append("\n");
	}
	else if (choice == 1) //1 = Perlin
	{
		perlinPersistence = double(m_slider201->GetValue()) / 100.0;
		perlinZoom = double(m_slider211->GetValue()) / 100.0;
		octaves = int(m_spinCtrl1->GetValue());

		out.append("Perlin").append("\n");
		out.append("Persistence:").append(numToString(perlinPersistence)).append("\n");
		out.append("Zoom:").append(numToString(perlinZoom)).append("\n");
		out.append("Octaves:").append(numToString(octaves)).append("\n");
	}
	else if (choice == 2) // 2 = Voronoi
	{
		int blockSize = int(m_slider_blocksize->GetValue());
		int intesity = int(m_slider_fValue->GetValue());
		bool negative = m_checkBox_negative->GetValue();
		int distance = m_choice_distance->GetCurrentSelection() + 1;//+1, da distance_methods nummer 1-5 haben
		wxString coeffs_str = m_textCtrl2_vor_coeff->GetLineText(0);
		wxString clip_num = m_textCtrl1->GetLineText(0);

		out.append("Voronoi").append("\n");
		out.append("BlockSize:").append(numToString(blockSize)).append("\n");
		out.append("Intesity:").append(numToString(intesity)).append("\n");
		out.append("Negative: ").append(numToString(int(negative))).append("\n");
		out.append("Distance:").append(numToString(distance - 1)).append("\n");
		out.append("Coefficients:").append(coeffs_str.c_str()).append("\n");
		out.append("Clip:").append(clip_num.c_str()).append("\n");
	}
	else if (choice == 3)
	{
		out.append("None").append("\n");
	}
	return out;

}


void SimGUI::perturbationButtonEvent(wxCommandEvent &event){
	Perturbation perturb = Perturbation(0.15);
	if (m_radioBtn1_midpoint_displ->GetValue() == true)
	{
		perturb.apply(0); // 0 = midpoint displacement
	}
	else if (m_radioBtn2->GetValue() == true)
	{
		perturb.apply(1); // 1 = Perlin
	}
	updateImage();
	manualRender();
	glPlane->refreshTerrain(); 
	glPlane->Refresh();
}


//#----------------------------------------------------------------------------


void SimGUI::errorDialog(string text){
	wxMessageDialog *error = new wxMessageDialog(this, text.c_str(), wxT("Error"), wxOK | wxICON_ERROR);
	error->ShowModal();
	error->Destroy();
}

void SimGUI::sliderSubdivisionEvent(wxCommandEvent &event){
	foldingCoeff = double(subdiv_slider_coeff->GetValue()) / float(coeff_sizer);// + (double(slider_fine_subdivision->GetValue())   / 100.0); //herunterskalieren, slider nimmt nur int
}

void SimGUI::sliderErosionRadiusEvent(wxCommandEvent &event){
	erosionRadius = (int)erosion_slider_radius->GetValue();
}
void SimGUI::sliderErosionIterationenEvent(wxCommandEvent &event){
	erosionIterations = (int)erosion_slider_iterationen->GetValue();
}
void SimGUI::randomizeSliderEvent(wxCommandEvent &event){
	randomizeRadius = static_cast<double>(random_fac_slider->GetValue());
}

void SimGUI::sliderScaleTerrainEvent(wxCommandEvent &event){
	glPlane->m_terrainHeightScale = static_cast<double>(scale_image_terrain->GetValue());
	glPlane->refreshTerrain();


	glPlane->Refresh();
}

void SimGUI::sliderPlateauRadiusEvent(wxCommandEvent &event){
	plateauRadius = (int)m_slider6->GetValue(); //m_slider6 = slider plateau
}

void SimGUI::scaleTerrainRealButtonEvent(wxCommandEvent &event){
	int value = m_scale_terrain->GetValue();
	double realfactor;
	if (value == 0)
		realfactor = 0.25;
	else if (value == 1)
		realfactor = 0.5;
	else if (value == 2)
		realfactor = 0.75;
	else if (value == 3)
		realfactor = 1.0;
	else if (value == 4)
		realfactor = 1.5;
	else if (value == 5)
		realfactor = 2.0;
	else if (value == 6)
		realfactor = 3.0;
	else if (value == 7){
		realfactor = 4.0;
	}

	if (realfactor == 1.0) { return; }
	Alti.scaleTerrain(realfactor);

	wxString str1;
	str1.Printf(wxT("Size: %d x %d"), Alti.getXSize(), Alti.getXSize());
	m_staticText_scale->SetLabel(str1);
	updateChain();
}

void SimGUI::simulateWaterEvent(wxCommandEvent &event){
	int sel = (int)m_radioBox1_water_methods->GetSelection();
	glPlane->startWaterSimulation(sel + 1, &Water_Kcs); //methods sind 1-4

	updateImage();
	manualRender();
	glPlane->refreshTerrain();
	glPlane->Refresh();
}

void SimGUI::toggleWatersourceEvent(wxCommandEvent &event){
	if (m_toggleBtn2->GetValue() == false){
		m_staticText18->Enable(false);
		m_slider13->Enable(false);
		m_staticText20->Enable(false);
		m_slider14->Enable(false);

		Water_Kcs.Watersource_on = false;
	}
	else{
		m_staticText18->Enable(true);
		m_slider13->Enable(true);
		m_staticText20->Enable(true);
		m_slider14->Enable(true);

		Water_Kcs.Watersource_on = true;
	}
}

void SimGUI::toggleRainEvent(wxCommandEvent &event){
	if (m_togglerain->GetValue() == false){
		m_staticText17->Enable(false);
		m_togglerain->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INACTIVEBORDER));
		m_slider12->Enable(false);
		m_slider12->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INACTIVECAPTIONTEXT));
		Water_Kcs.Rain_on = false;
	}
	else{
		m_staticText17->Enable(true);
		m_slider12->Enable(true);
		Water_Kcs.Rain_on = true;
	}
}

void SimGUI::toggleSedimentPreviewEvent(wxCommandEvent &event){
	if (m_toggleBtn3->GetValue() == false){
		sediment_preview_on = false;
		manualRender();
	}
	else {
		sediment_preview_on = true;
		manualRender();
	}
}

void SimGUI::toggleWaterPreviewEvent(wxCommandEvent &event){
	if (m_toggleBtn4->GetValue() == false){
		water_preview_on = false;
		manualRender();
	}
	else {
		water_preview_on = true;
		manualRender();
	}
}

void SimGUI::sliderRainAmountEvent(wxCommandEvent &event){
	Water_Kcs.KR = float(m_slider12->GetValue()) / 10000.0;
}

void SimGUI::sliderWaterRadiusEvent(wxCommandEvent &event){
	Water_Kcs.Radius = float(m_slider14->GetValue());
}

void SimGUI::sliderWaterAmountEvent(wxCommandEvent &event){
	Water_Kcs.Water_Amount = float(m_slider13->GetValue());
}
void SimGUI::sliderSedimentCapacityEvent(wxCommandEvent &event){
	Water_Kcs.KC = float(m_slider8_KC->GetValue()) / 1000.0;
}

void SimGUI::sliderDepositionConstantEvent(wxCommandEvent &event){
	Water_Kcs.KD = float(m_slider9->GetValue()) / 100.0;
}
void SimGUI::sliderDissolvingConstantEvent(wxCommandEvent &event){
	Water_Kcs.KS = float(m_slider10->GetValue()) / 100.0;
}
void SimGUI::sliderEvaporationConstantEvent(wxCommandEvent &event){
	Water_Kcs.KE = float(m_slider11->GetValue()) / 100.0;
}
void SimGUI::sliderTimestepEvent(wxCommandEvent &event){
	Water_Kcs.DT = float(m_slider20->GetValue()) / 1000.0;
}
void SimGUI::sliderMinimumAlphaEvent(wxCommandEvent &event){
	Water_Kcs.Min_alpha = float(m_slider18->GetValue()) / 100.0;
}
void SimGUI::sliderIterationsEvent(wxCommandEvent &event){
	Water_Kcs.Iterations = int(m_slider21->GetValue());
}

//events for basic frame
BEGIN_EVENT_TABLE(SimGUI, wxFrame)
EVT_BUTTON(BUTTON_SUBDIVIDE, SimGUI::subdivideButtonEvent)
EVT_BUTTON(BUTTON_OPEN_IMAGE, SimGUI::openImageButtonEvent)
EVT_SLIDER(SLIDER_SUBDIVISION_COEFF, SimGUI::sliderSubdivisionEvent)
EVT_BUTTON(BUTTON_EROSION, SimGUI::erosionButtonEvent)
EVT_BUTTON(BUTTON_RANDOMIZE, SimGUI::randomizeButtonEvent)
EVT_BUTTON(BUTTON_PLATEAU, SimGUI::plateauButtonEvent)
EVT_BUTTON(BUTTON_OPEN_OBJ, SimGUI::openObjButtonEvent)
EVT_BUTTON(BITMAP_BUTTON_FLAGPOS, SimGUI::positionFlagButtonEvent)
EVT_BUTTON(SAVE_OBJ, SimGUI::saveObjEvent)
EVT_BUTTON(SAVE_IMAGE, SimGUI::saveImageEvent)
EVT_SLIDER(RANDOMIZE_SLIDER, SimGUI::randomizeSliderEvent)
EVT_SLIDER(SLIDER_SCALE_IMAGE_TERRAIN, SimGUI::sliderScaleTerrainEvent)
EVT_SLIDER(PLATEAU_RADIUS_SLIDER, SimGUI::sliderPlateauRadiusEvent)
EVT_SLIDER(SLIDER_PERLIN_ZOOM, SimGUI::sliderPerlinZoomEvent)
EVT_SLIDER(SLIDER_PERSISTENCE, SimGUI::sliderPersistenceEvent)
EVT_SPINCTRL(SLIDER_OCTAVES, SimGUI::sliderOctavesEvent)
EVT_BUTTON(ON_OFF_PIC_BUT, SimGUI::openClosePicEvent)
//EVT_SPLITTER_SASH_POS_CHANGING(SPLITTER_WINDOW,BasicFrame::HandleSash_position)
EVT_SPLITTER_SASH_POS_CHANGED(SPLITTER_WINDOW, SimGUI::handleSashPosition)
EVT_SPLITTER_DCLICK(SPLITTER_WINDOW, SimGUI::handleSashPosition)
EVT_BUTTON(BUTTON_TOGGLE_CROSSHAIR, SimGUI::toggleCrosshair)
EVT_BUTTON(BUTTON_SCALE_TERRAIN_REAL, SimGUI::scaleTerrainRealButtonEvent)
EVT_BUTTON(BUTTON_GENERATE, SimGUI::perlinNoiseEvent)
EVT_BUTTON(BUTTON_VORONOI, SimGUI::buttonVoronoiEvent)
EVT_BUTTON(COMBINED_GENERATE, SimGUI::combinedGenerateEvent)
EVT_SPINCTRL(SPINCTRL1, SimGUI::spinctrl1Event)
EVT_SPINCTRL(SPINCTRL2, SimGUI::spinctrl2Event)
EVT_SPINCTRL(SPINCTRL3, SimGUI::spinctrl3Event)
EVT_CHOICE(CHOICEBOX1, SimGUI::choiceCombined1Event)
EVT_CHOICE(CHOICEBOX2, SimGUI::choiceCombined2Event)
EVT_CHOICE(CHOICEBOX3, SimGUI::choiceCombined3Event)
EVT_BUTTON(SAVE_BUT_PRESET, SimGUI::buttonSavePresetEvent)
EVT_BUTTON(OPEN_BUT_PRESET, SimGUI::buttonLoadPresetEvent)
EVT_BUTTON(BUTTON_PERTURB, SimGUI::perturbationButtonEvent)
EVT_BUTTON(BUTTON_THERMAL_EROSION, SimGUI::thermalErosionButtonEvent)
EVT_TOGGLEBUTTON(TOGGLE_AUTO, SimGUI::toggleAutoEvent)



EVT_BUTTON(BUTTON_WATER_SIMULATE, SimGUI::simulateWaterEvent)
EVT_TOGGLEBUTTON(TOGGLE_WATERSOURCE, SimGUI::toggleWatersourceEvent)
EVT_TOGGLEBUTTON(TOGGLE_RAIN, SimGUI::toggleRainEvent)
EVT_TOGGLEBUTTON(TOGGLE_SEDIMENT_PREVIEW, SimGUI::toggleSedimentPreviewEvent)
EVT_TOGGLEBUTTON(TOGGLE_WATER_PREVIEW, SimGUI::toggleWaterPreviewEvent)
EVT_SLIDER(SLIDER_RAIN_WATER_AMOUNT, SimGUI::sliderRainAmountEvent)
EVT_SLIDER(SLIDER_WATER_RADIUS, SimGUI::sliderWaterRadiusEvent)
EVT_SLIDER(SLIDER_WATER_AMOUNT, SimGUI::sliderWaterAmountEvent)
EVT_SLIDER(SLIDER_SEDIMENT_CAPACITY, SimGUI::sliderSedimentCapacityEvent)
EVT_SLIDER(SLIDER_DEPOSITION_CONSTANT, SimGUI::sliderDepositionConstantEvent)
EVT_SLIDER(SLIDER_DISSOLVING_CONSTANT, SimGUI::sliderDissolvingConstantEvent)
EVT_SLIDER(SLIDER_EVAPORATION_CONSTANT, SimGUI::sliderEvaporationConstantEvent)
EVT_SLIDER(SLIDER_TIMESTEP, SimGUI::sliderTimestepEvent)
EVT_SLIDER(MINIMUM_ALPHA, SimGUI::sliderMinimumAlphaEvent)
EVT_SLIDER(SLIDER_ITERATIONS, SimGUI::sliderIterationsEvent)
END_EVENT_TABLE()

//event table for gl events
BEGIN_EVENT_TABLE(GLPane, wxGLCanvas)
EVT_MOTION(GLPane::mouseMoved)
//EVT_LEFT_DOWN(BasicGLPane::mouseDown)
EVT_LEFT_UP(GLPane::mouseReleased)
EVT_RIGHT_DOWN(GLPane::rightClick)
EVT_LEAVE_WINDOW(GLPane::mouseLeftWindow)
EVT_SIZE(GLPane::resized)
EVT_KEY_DOWN(GLPane::keyPressed)
EVT_KEY_UP(GLPane::keyReleased)
EVT_MOUSEWHEEL(GLPane::mouseWheelMoved)
EVT_MIDDLE_DOWN(GLPane::middleMouseDown)
EVT_PAINT(GLPane::render)
END_EVENT_TABLE()

