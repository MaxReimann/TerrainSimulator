
#include "simulation.h"
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include "wx/image.h"
#define ALT(x,y) (map[(x)*ysize+(y)])
#define SEDIMENT(x,y) (sedimentMap[(x)*ysize+(y)])

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool AltitudeMap::save_image(char path[],string format){
	
	wxImage image = wxImage(xsize,ysize,true);
	for (int x = 0; x < xsize; x++){
		for (int y = 0; y < ysize;y++){
			//der wert y=[0,255] und dann als int
			int value = static_cast<int>(ALT(x,y) * 255);
			//dadurch,dass alle R,G,B gleich gross sind wird das
			//Bild Greyscale
			image.SetRGB(x,y, value,value,value);
		}
	}

	if (format.compare(".png") == 0) {
			image.SetOption(wxT("wxIMAGE_OPTION_PNG_FORMAT"),wxT("wxPNG_TYPE_COLOUR"));
			image.SetOption(wxT("wxIMAGE_OPTION_PNG_BITDEPTH"),8);
			image.SaveFile(wxString(path),wxBITMAP_TYPE_PNG);
		}
	else if (format.compare(".jpg") == 0){
			image.SetOption(wxT("quality"),100);
			image.SaveFile(wxString(path), wxBITMAP_TYPE_JPEG);
		}
	if (format.compare(".bmp") == 0) {
		image.SaveFile(wxString(path), wxBITMAP_TYPE_BMP);
		}
	bool t = image.IsOk();
	image.Destroy();
	return  t;


	//dialog->Update(30);
}

wxImage  AltitudeMap::makePreviewImage(void){
	if (xsize != previewImage->GetWidth() || ysize != previewImage->GetHeight() ){ //wenn groesse geaendert wurde
		previewImage->Destroy();
		previewImage = new wxImage(xsize,ysize,true);
	}
	for (int x = 0; x < xsize; x++){
		for (int y = 0; y < ysize ;y++){
			//der wert y=[0,255] und dann als int
			int value = static_cast<int>(ALT(x,y) * 255);
			//dadurch,dass alle R,G,B gleich gross sind wird das
			//Bild Greyscale
			previewImage->SetRGB(x,y, value,value,value);
			
		}
	}
	//wxImage* preview_image_2 = new wxImage(); //vertikal spiegeln
	return (previewImage->Mirror(false));
}

wxImage  AltitudeMap::Make_Sediment_Image(void){
	if (xsize != previewImage->GetWidth() || ysize != previewImage->GetHeight() ){ //wenn groesse geaendert wurde
		previewImage->Destroy();
		previewImage = new wxImage(xsize,ysize,true);
	}
	int normalized;
	for (int x = 0; x < xsize; x++){
		for (int y = 0; y < ysize ;y++){
			//der wert y=[0,255] und dann als int
			if (SEDIMENT(x,y) > 0.0001){
			normalized = int(1.0 / 2.0 * ( 1.0 - SEDIMENT(x,y) / (sqrt( SEDIMENT(x,y) * SEDIMENT(x,y) + 0.1)) * 255)); // 1/2 * (1-d/(sqrt(d² + a)) * 255 
			}
			else {
				normalized = previewImage->GetBlue(x,y); //normales greysacle bild erzugen wenn werte zu klein
			}
			//int value = static_cast<int>(SEDIMENT(x,y) * 255);
			//dadurch,dass alle R,G,B gleich gross sind wird das
			previewImage->SetRGB(x,y, previewImage->GetGreen(x,y),(unsigned char) normalized,previewImage->GetRed(x,y));
			
		}
	}
	//wxImage* preview_image_2 = new wxImage(); //vertikal spiegeln
	return (previewImage->Mirror(false));
}


bool AltitudeMap::loadImage(char path[],int normals){
	//Loader fuer alle formate ausser .tga
	wxImage image;
	//#### format extrahieren ### ///
	string format;
	string wildcard(path);
	string::size_type pos = wildcard.find_last_of(".",wildcard.length());//trennpunkt der format angabe
	format = wildcard.substr(pos,wildcard.length());
	
	if (format.at(format.length()-1) == ' '){//format.length() -1 = last
		format.erase(format.length()-1,1); //loescht letzes whitespace wenn gefunden
	}
	
	if (format.compare(".png") == 0) {
		image = wxImage(wxString(path), wxBITMAP_TYPE_PNG);
		}
	else if (format.compare(".jpg") == 0){
		image = wxImage(wxString(path), wxBITMAP_TYPE_JPEG);
		}
	else if (format.compare(".bmp") == 0) {
		image = wxImage(wxString(path), wxBITMAP_TYPE_JPEG);
		}
	else {
		return false;
	}

	delete [] map;
	unsigned char *data;
	data = image.GetData();
	xsize = image.GetWidth();
	ysize = image.GetHeight();

	/* alocate memory for the terrain, and check for errors */
	map = new double [ xsize * ysize ];
	if (map == NULL)
		return false;

/* allocate memory for the normals, and check for errors */
	if (normals) {
		delete terrainNormals;
		terrainNormals = new double [xsize * ysize  * 3];
		if (terrainNormals == NULL)
			return false;
	}
	else {
			terrainNormals = NULL;
	}

/* if mode = RGBA then allocate memory for colors, and check for errors */
	if ( image.HasAlpha() ) {
		delete terrainColors;
		terrainColors = new double [xsize * ysize  * 3];
		if (terrainColors == NULL)
			return false;
	}
	else {
		terrainColors = NULL;
	}
/* fill arrays */
	double pointHeight;
	for (int y = 0 ; y < ysize; y++)
		for (int x = 0;x < xsize; x++) {

/* compute the height as a value between 0.0 and 1.0 */
			pointHeight = double(image.GetBlue(x,y) + image.GetRed(x,y) + image.GetGreen(x,y)) / (255.0*3.0);  //(double(data[x * xsize  + y]) / 255.0 + double(data[x * xsize  + y+1]) / 255.0 + double(data[x * xsize  + y+2]) / 255.0) / 3.0 ;//[mode*(i*xsize + j)+(mode-1)] / 255.0;
			map[y*xsize + x] = 1.0 - pointHeight;
/* if mode = RGBA then fill the colors array as well */
			if (image.HasAlpha()) {
				terrainColors[3*(y*xsize + x)]   = double(data[(y*xsize + x)])/255.0;
				terrainColors[3*(y*xsize + x)+1] = double(data[(y*xsize + x)+1])/255.0;
				terrainColors[3*(y*xsize + x)+2] = double(data[(y*xsize + x)+2])/255.0;
			}
		}
	bool res = image.IsOk();
	image.Destroy();
	return  res;


	//dialog->Update(30);
}



