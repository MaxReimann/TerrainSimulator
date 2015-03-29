#ifndef IMPORTS_H
#define IMPORTS_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream> 
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include <cstddef>
#include <time.h>


#include <list>
#include <utility>
#include <vector>
#include <algorithm>
#include <cmath>
#include "util/mathlib.h"
#include "util/random.h"



#include <GL/glew.h> //gl und glu werden mimt glew import
#include <GL/glu.h>
#include <GL/gl.h>
#include <GL/glut.h>


#include "drawable.h"
#include "image.h"


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



#ifdef _DEBUG
#include <crtdbg.h>
// include information in allocation, useful for finding memory leaks
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__) 
#else
#define DEBUG_NEW new
#endif


#endif
