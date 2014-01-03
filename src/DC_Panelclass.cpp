#include "DC_Panelclass.h"
#include "Button_Bitmap_rcs\crosshair_png.cpp" //png bild in c vector
#include "gl_object.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_EVENT_TABLE(Bitmap_Panel, wxPanel)
//EVT_MOTION(Bitmap_Panel::mouseDown)
EVT_LEFT_DOWN(Bitmap_Panel::mouseDown)
EVT_LEFT_UP(Bitmap_Panel::mouseDown)
EVT_RIGHT_DOWN(Bitmap_Panel::rightClick)
EVT_LEAVE_WINDOW(Bitmap_Panel::mouseLeftWindow)
EVT_KEY_DOWN(Bitmap_Panel::keyPressed)
EVT_KEY_UP(Bitmap_Panel::keyReleased)
EVT_MOUSEWHEEL(Bitmap_Panel::mouseWheelMoved)
//EVT_PAINT(Bitmap_Panel::render)
END_EVENT_TABLE()
 

void Bitmap_Panel::mouseMoved(wxMouseEvent& event) {}

void Bitmap_Panel::mouseDown(wxMouseEvent& event) {
	
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



void Bitmap_Panel::mouseWheelMoved(wxMouseEvent& event) {}
void Bitmap_Panel::mouseReleased(wxMouseEvent& event) {}
void Bitmap_Panel::rightClick(wxMouseEvent& event) {}
void Bitmap_Panel::mouseLeftWindow(wxMouseEvent& event) {}
void Bitmap_Panel::keyPressed(wxKeyEvent& event) {}
void Bitmap_Panel::keyReleased(wxKeyEvent& event) {}
 
Bitmap_Panel::Bitmap_Panel(wxWindow* parent) :
wxPanel(parent)
{
}

Bitmap_Panel::Bitmap_Panel(wxWindow* parent,wxWindowID id = wxID_ANY,const wxPoint& pos = wxDefaultPosition,const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = "panel") :
wxPanel(parent,id,pos,size,style,name)
{
	/*Das Png Image als header importieren und in stream leiten*/
	wxMemoryInputStream istream(crosshair_png, sizeof(crosshair_png));
	wxImage im_crosshair_png(istream, wxBITMAP_TYPE_PNG); 

	crosshair_bmp = new wxBitmap(im_crosshair_png);
	x = int(double(Alti.getxsize()) / 2.0);
	y = int(double(Alti.getysize()) / 2.0) ;
	flag_on = true;
	flag_changed = false;
	dcfont = wxFont(8,wxFONTFAMILY_DEFAULT,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL);
	//im_crosshair_png.Destroy();
}
 
Bitmap_Panel::~Bitmap_Panel()
{
	delete crosshair_bmp;
	
}
 
void Bitmap_Panel::render( void )
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
		dc.DrawText(wxT("Sediment amount:"),0,101 + Alti.getysize());
		dc.DrawBitmap(global_sediment_bitmap,0,120 + Alti.getysize(),false);
	}
	pic_width = global_preview_bitmap.GetWidth();
	pic_height = global_preview_bitmap.GetHeight();
			
	glPlane->Refresh();
	//delete temp_preview_bitmap;
	
}

void Bitmap_Panel::ToggleFlag(bool flagstatus){
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
void Bitmap_Panel::init_flag(void){
	wxClientDC dc(this);
	dc.DrawBitmap(*crosshair_bmp,int(double(temp_preview_bitmap.GetWidth()) / 2.0),int(double(temp_preview_bitmap.GetHeight()) / 2.0) + 100,true);
}

bool Bitmap_Panel::GetFlagStatus(void){
	if (flag_changed == true){
		flag_changed = false;
		return true; //flag wurde veraendert
	}
	else{
		return false;
	}
}