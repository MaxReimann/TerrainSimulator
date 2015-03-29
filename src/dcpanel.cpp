#include "stdafx.h"
#include "dcpanel.h"
#include "..\resources\crosshair_png.cpp" //png bild in c vector
#include "globals.h"
#include "wx\event.h"


BEGIN_EVENT_TABLE(BitmapPanel, wxPanel)
//EVT_MOTION(BitmapPanel::mouseDown)
EVT_LEFT_DOWN(BitmapPanel::mouseDown)
EVT_LEFT_UP(BitmapPanel::mouseDown)
EVT_RIGHT_DOWN(BitmapPanel::rightClick)
EVT_LEAVE_WINDOW(BitmapPanel::mouseLeftWindow)
EVT_KEY_DOWN(BitmapPanel::keyPressed)
EVT_KEY_UP(BitmapPanel::keyReleased)
EVT_MOUSEWHEEL(BitmapPanel::mouseWheelMoved)
//EVT_PAINT(BitmapPanel::render)
END_EVENT_TABLE()
 

void BitmapPanel::mouseMoved(wxMouseEvent& event) {}

void BitmapPanel::mouseDown(wxMouseEvent& event) {
	
	if (flag_on == true  && event.GetX() < pic_width && (int(event.GetY()) <= (pic_height+100) && int(event.GetY()) > 100)){
		//wxClientDC dc(this);
		x = int( event.GetX() );
		y = int( event.GetY() ) - 100 ;
		render();
		//dc.DrawBitmap(*crosshair_bmp,event.GetX() - int(double(crosshair_bmp->GetWidth()) / 2.0),event.GetY() - int(double(crosshair_bmp->GetHeight()) / 2.0),true);
		flag_changed = true;
	}

	event.Skip();
	}



void BitmapPanel::mouseWheelMoved(wxMouseEvent& event) {}
void BitmapPanel::mouseReleased(wxMouseEvent& event) {}
void BitmapPanel::rightClick(wxMouseEvent& event) {}
void BitmapPanel::mouseLeftWindow(wxMouseEvent& event) {}
void BitmapPanel::keyPressed(wxKeyEvent& event) {}
void BitmapPanel::keyReleased(wxKeyEvent& event) {}
 
BitmapPanel::BitmapPanel(wxWindow* parent) :
wxPanel(parent)
{
}

BitmapPanel::BitmapPanel(wxWindow* parent,wxWindowID id = wxID_ANY,const wxPoint& pos = wxDefaultPosition,const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = "panel") :
wxPanel(parent,id,pos,size,style,name)
{
	/*Das Png Image als header importieren und in stream leiten*/
	wxMemoryInputStream istream(crosshair_png, sizeof(crosshair_png));
	wxImage im_crosshair_png(istream, wxBITMAP_TYPE_PNG); 

	crosshair_bmp = new wxBitmap(im_crosshair_png);
	x = int(double(Alti.getXSize()) / 2.0);
	y = int(double(Alti.getYSize()) / 2.0) ;
	flag_on = true;
	flag_changed = false;
	dcfont = wxFont(8,wxFONTFAMILY_DEFAULT,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL);
	//im_crosshair_png.Destroy();
}
 
BitmapPanel::~BitmapPanel()
{
	delete crosshair_bmp;
	
}
 
void BitmapPanel::render( void )
{
	temp_preview_bitmap = wxBitmap(global_preview_bitmap);
    wxClientDC dc(this); // nur innerhalb von paint eventen benutzen
	dc.Clear();
	dc.DrawBitmap(global_preview_bitmap,0,100,false);
	if (flag_on == true){
		dc.DrawBitmap(*crosshair_bmp,x - (double(crosshair_bmp->GetWidth())/2.0),y + 100 - (double(crosshair_bmp->GetHeight())/2.0),true);
		glPlane->setFlag(x,y);
	}

	if ( frame->sediment_preview_on ){
		dc.SetFont(dcfont);
		dc.DrawText(wxT("Sediment amount:"),0,101 + Alti.getYSize());
		dc.DrawBitmap(g_sedimentBitmap,0,120 + Alti.getYSize(),false);
	}
	pic_width = global_preview_bitmap.GetWidth();
	pic_height = global_preview_bitmap.GetHeight();
			
	glPlane->Refresh();
	//delete temp_preview_bitmap;
	
}

void BitmapPanel::ToggleFlag(bool flagstatus){
	flag_on = flagstatus;
	if (flag_on == true){
		x = int(double(temp_preview_bitmap.GetWidth()) / 2.0);
		y = int(double(temp_preview_bitmap.GetHeight()) / 2.0);
		render();
		//wxClientDC dc(this);
		//dc.DrawBitmap(*crosshair_bmp,int(double(temp_preview_bitmap->GetWidth()) / 2.0),int(double(temp_preview_bitmap->GetHeight()) / 2.0) + 100,true);
	}
	else {
		render(); //bild ueberschreiben
	}
}
void BitmapPanel::init_flag(void){
	wxClientDC dc(this);
	dc.DrawBitmap(*crosshair_bmp,int(double(temp_preview_bitmap.GetWidth()) / 2.0),int(double(temp_preview_bitmap.GetHeight()) / 2.0) + 100,true);
}

bool BitmapPanel::GetFlagStatus(void){
	if (flag_changed == true){
		flag_changed = false;
		return true; //flag wurde veraendert
	}
	else{
		return false;
	}
}