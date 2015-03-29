#ifndef _image_
#define _image_
 
// include OpenGL
#ifdef __WXMAC__
#include "OpenGL/gl.h"
#else
#include <GL/glew.h> 
#include <GL/glu.h>
#include <GL/gl.h>
#endif
 
 #include <cmath>
 
#include "wx/image.h"
#include "wx/wx.h"
 
class Image
{
    GLuint* ID;
    
public:
 
	/*
	 * it is preferable to use textures that are a power of two. this loader will automatically
	 * resize texture to be a power of two, filling the remaining areas with black.
	 * width/height are the width of the actual loaded image.
	 * textureWidth/Height are the total width of the texture, including black filling.
	 * tex_coord_x/y are the texture coord parameter you must give OpenGL when rendering
	 * to get only the image, without the black filling.
	 */
    int width, height, textureWidth, textureHeight;
    float tex_coord_x, tex_coord_y;
    
    GLuint* getID();
    Image();
    Image(wxString path);
	Image(wxImage img);
    ~Image();
    void load(wxString path);
	void load(wxImage img);
	GLuint* loadImage(wxString path, int* imageWidth, int* imageHeight, int* textureWidth, int* textureHeight);
	GLuint* loadImage(wxImage in_img, int* imageWidth, int* imageHeight, int* textureWidth, int* textureHeight);
        
};
 
 
#endif 