#ifndef _image_loader
#define _image_loader
 
#ifdef __WXMAC__
#include "OpenGL/gl.h"
#else
#include <GL/glew.h> //gl und glu werden mimt glew import
#include <GL/glu.h>
#include <GL/gl.h>
#endif
#include "wx/wx.h"
	
GLuint* loadImage(wxString path, int* imageWidth, int* imageHeight, int* textureWidth, int* textureHeight);
GLuint* loadImage(wxImage in_img, int* imageWidth, int* imageHeight, int* textureWidth, int* textureHeight);


#endif 