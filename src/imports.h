#ifndef IMPORTS_H
#define IMPORTS_H
/* Alle benoetigten imports, damits man nicht alle s 3mal importieren muss*/


#include <iostream>
#include <fstream>
#include <string>
#include <sstream> //stringstream
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include <cstddef>
#include <time.h>
//#include <stdafx.h>

#include <list>
#include <utility> //fuer pair
#include <vector>
#include <algorithm>
#include <cmath>
#include "mathlib.h"
#include "random.h"



#include <GL/glew.h> //gl und glu werden mimt glew import
#include <GL/glu.h>
#include <GL/gl.h>
#include <GL/glut.h>


#include "wx/wx.h"
#include "wx/sizer.h"
#include "wx/glcanvas.h"
#include "wx/stockitem.h"
#include "wx/spinctrl.h"
#include <wx/string.h>
#include <wx/button.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/stattext.h>
#include <wx/tglbtn.h>
#include <wx/slider.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/notebook.h>
#include <wx/event.h>
#include <wx/statline.h>
#include <wx/splitter.h>
#include <wx/mstream.h>
#include "wx/progdlg.h"

#include "Drawable.h"
#include "Image.h"


#ifdef _DEBUG
#include <crtdbg.h>
//Bei der Debug Version werden news direkt in das File allokiert-> mann kann memory leaks finden
//dieses Vorgehen ist speziell auf die Verwendung mit wxwidgets ausgerichtet
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__) 
#else
#define DEBUG_NEW new
#endif


#endif
