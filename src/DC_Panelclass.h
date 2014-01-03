#ifndef DC_PANELCLASS_H
#define DC_PANELCLASS_H

#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
#endif

#include "imports.h"
class Bitmap_Panel : public wxPanel
{
 

public:
	Bitmap_Panel(wxWindow* parent);
	Bitmap_Panel(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size, long style, const wxString& name);
	~Bitmap_Panel();
	wxBitmap* crosshair_bmp;
	wxBitmap temp_preview_bitmap;
	int x,y;
	int pic_width,pic_height;
	bool flag_on;
	bool flag_changed; //wenn die flag veraendert aber noch nicht von glplane abgefragt wurde, ist das true
	std::fstream errorstream;
	wxFont dcfont;
    
	void render(void);
	void ToggleFlag(bool flagstatus);
	bool GetFlagStatus(void);
	void init_flag(void);
    
	// events
	void mouseMoved(wxMouseEvent& event);
	void mouseDown(wxMouseEvent& event);
	void mouseWheelMoved(wxMouseEvent& event);
	void mouseReleased(wxMouseEvent& event);
	void rightClick(wxMouseEvent& event);
	void mouseLeftWindow(wxMouseEvent& event);
	void keyPressed(wxKeyEvent& event);
	void keyReleased(wxKeyEvent& event);
    
	DECLARE_EVENT_TABLE()
};
#endif
 