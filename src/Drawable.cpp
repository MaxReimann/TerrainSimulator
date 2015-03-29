#include "stdafx.h"
#include "drawable.h"
#include <iostream>
 
#ifdef __WXMAC__
#include "OpenGL/gl.h"
#else
#include <GL/glew.h> //gl und glu werden mimt glew import
#include <GL/glu.h>
#include <GL/gl.h>
#endif
 
#include "wx/wx.h"
 
/*
 * This is a simple class built on top of OpenGL that manages drawing images in a higher-level and quicker way.
 */
 
Drawable::Drawable(Image* image_arg)
{
 
    x=0;
    y=0;
    hotspotX=0;
    hotspotY=0;
    angle=0;
	z= 0.0;
    
    xscale=1;
    yscale=1;
 
    xflip=false;
    yflip=false;
    
    if(image_arg!=NULL) setImage(image_arg);
    else image=NULL;
}
 
void Drawable::setFlip(bool x, bool y)
{
    xflip=x;
    yflip=y;
}

void Drawable::setZ(float zett){
	z = zett;
}

 
void Drawable::setHotspot(int x, int y)
{
    hotspotX=x;
    hotspotY=y;
}
 
void Drawable::move(int x, int y)
{
    Drawable::x=x;
    Drawable::y=y;
}
 
void Drawable::scale(float x, float y)
{
    Drawable::xscale=x;
    Drawable::yscale=y;
}
 
void Drawable::scale(float k)
{
    Drawable::xscale=k;
    Drawable::yscale=k;
}
 
void Drawable::setImage(Image* image)
{
    Drawable::image=image;
}
 
void Drawable::rotate(int angle)
{
    Drawable::angle=angle;
}
 
void Drawable::render()
{
    assert(image!=NULL);
    
    glLoadIdentity();
 
    glTranslatef(x,y,0);
    
    if(xscale!=1 || yscale!=1)
	{
        glScalef(xscale, yscale, 1);
    }
    
    if(angle!=0)
	{
        glRotatef(angle, 0,0,1);   
    }
	glEnable(GL_LIGHTING);
	//glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHT0);
    GLuint temp = image->getID()[0];
    glBindTexture(GL_TEXTURE_2D,temp  );
    
    glBegin(GL_QUADS);
    
    glTexCoord2f( image->tex_coord_x,  0 );
    glVertex3f( -hotspotX, -hotspotY, -z);
	//glVertex2f( -hotspotX, -hotspotY);
    
    glTexCoord2f(0,1);
    glVertex3f( image->width-hotspotX,-hotspotY, -z );
	//glVertex2f( image->width-hotspotX,-hotspotY );
    
    glTexCoord2f(1,image->tex_coord_y);
    glVertex3f( image->width-hotspotX, image->height-hotspotY,-z);
	//glVertex2f( image->width-hotspotX, image->height-hotspotY);
    
    glTexCoord2f(image->tex_coord_x,image->tex_coord_y);
    glVertex3f( -hotspotX, image->height-hotspotY, -z );
	//glVertex2f( -hotspotX, image->height-hotspotY );
    
    glEnd();
    
}