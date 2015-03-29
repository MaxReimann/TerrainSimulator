#include "stdafx.h"
#include "image.h"


Image::Image()
{
}

Image::Image(wxString path)
{
	load(path);
}

Image::Image(wxImage img)
{
	load(img);
}

void Image::load(wxString path)
{
	ID = loadImage(path, &width, &height, &textureWidth, &textureHeight);

	tex_coord_x = (float)width / (float)textureWidth;
	tex_coord_y = (float)height / (float)textureHeight;
}

void Image::load(wxImage img)
{
	ID = loadImage(img, &width, &height, &textureWidth, &textureHeight);

	tex_coord_x = (float)width / (float)textureWidth;
	tex_coord_y = (float)height / (float)textureHeight;
}

GLuint* Image::getID()
{
	return ID;
}

Image::~Image()
{
	glDeleteTextures(1, ID);
}

#include <cmath>

#include "wx/image.h"
#include "wx/wx.h"

GLuint* Image::loadImage(wxImage in_img, int* imageWidth, int* imageHeight, int* textureWidth, int* textureHeight)
{

	GLuint* ID = new GLuint[1];
	glGenTextures(1, &ID[0]);
	glBindTexture(GL_TEXTURE_2D, *ID);

	wxImage* img = new wxImage(in_img);

	(*imageWidth) = img->GetWidth();
	(*imageHeight) = img->GetHeight();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	/*
	 * Many graphics card require that textures be power of two.
	 * Below is a simple implementation, probably not optimal but working.
	 */

	float power_of_two_that_gives_correct_width = std::log((float)(*imageWidth)) / std::log(2.0);
	float power_of_two_that_gives_correct_height = std::log((float)(*imageHeight)) / std::log(2.0);

	// check if image dimensions are a power of two
	if ((int)power_of_two_that_gives_correct_width == power_of_two_that_gives_correct_width &&
		(int)power_of_two_that_gives_correct_height == power_of_two_that_gives_correct_height)
	{
		// note: must make a local copy before passing the data to OpenGL, as GetData() returns RGB 
		// and we want the Alpha channel if it's present. Additionally OpenGL seems to interpret the 
		// data upside-down so we need to compensate for that.
		GLubyte *bitmapData = img->GetData();
		GLubyte *alphaData = img->GetAlpha();
		GLubyte *imageData;

		int bytesPerPixel = img->HasAlpha() ? 4 : 3;

		int imageSize = (*imageWidth) * (*imageHeight) * bytesPerPixel;
		imageData = (GLubyte *)malloc(imageSize);

		int rev_val = (*imageHeight) - 1;

		for (int y = 0; y < (*imageHeight); y++)
		{
			for (int x = 0; x < (*imageWidth); x++)
			{
				imageData[(x + y*(*imageWidth))*bytesPerPixel + 0] =
					bitmapData[(x + (rev_val - y)*(*imageWidth)) * 3];

				imageData[(x + y*(*imageWidth))*bytesPerPixel + 1] =
					bitmapData[(x + (rev_val - y)*(*imageWidth)) * 3 + 1];

				imageData[(x + y*(*imageWidth))*bytesPerPixel + 2] =
					bitmapData[(x + (rev_val - y)*(*imageWidth)) * 3 + 2];

				if (bytesPerPixel == 4) imageData[(x + y*(*imageWidth))*bytesPerPixel + 3] =
					alphaData[x + (rev_val - y)*(*imageWidth)];
			}//next
		}//next

		// if yes, everything is fine
		glTexImage2D(GL_TEXTURE_2D,
			0,
			bytesPerPixel,
			*imageWidth,
			*imageHeight,
			0,
			img->HasAlpha() ? GL_RGBA : GL_RGB,
			GL_UNSIGNED_BYTE,
			imageData);

		(*textureWidth) = (*imageWidth);
		(*textureHeight) = (*imageHeight);

		free(imageData);
	}
	else // texture is not a power of two. We need to resize it
	{

		int newWidth = (int)std::pow(2.0, (int)(std::ceil(power_of_two_that_gives_correct_width)));
		int newHeight = (int)std::pow(2.0, (int)(std::ceil(power_of_two_that_gives_correct_height)));

		//printf("Unsupported image size. Recommand values: %i %i\n",newWidth,newHeight);   

		GLubyte *bitmapData = img->GetData();
		GLubyte        *alphaData = img->GetAlpha();
		GLubyte *imageData;

		int old_bytesPerPixel = 3;
		int bytesPerPixel = img->HasAlpha() ? 4 : 3;

		int imageSize = newWidth * newHeight * bytesPerPixel;
		imageData = (GLubyte *)malloc(imageSize);

		int rev_val = (*imageHeight) - 1;

		for (int y = 0; y < newHeight; y++)
		{
			for (int x = 0; x < newWidth; x++)
			{

				if (x < (*imageWidth) && y < (*imageHeight)){
					imageData[(x + y*newWidth)*bytesPerPixel + 0] =
						bitmapData[(x + (rev_val - y)*(*imageWidth))*old_bytesPerPixel + 0];

					imageData[(x + y*newWidth)*bytesPerPixel + 1] =
						bitmapData[(x + (rev_val - y)*(*imageWidth))*old_bytesPerPixel + 1];

					imageData[(x + y*newWidth)*bytesPerPixel + 2] =
						bitmapData[(x + (rev_val - y)*(*imageWidth))*old_bytesPerPixel + 2];

					if (bytesPerPixel == 4) imageData[(x + y*newWidth)*bytesPerPixel + 3] =
						alphaData[x + (rev_val - y)*(*imageWidth)];

				}
				else
				{

					imageData[(x + y*newWidth)*bytesPerPixel + 0] = 0;
					imageData[(x + y*newWidth)*bytesPerPixel + 1] = 0;
					imageData[(x + y*newWidth)*bytesPerPixel + 2] = 0;
					if (bytesPerPixel == 4) imageData[(x + y*newWidth)*bytesPerPixel + 3] = 0;
				}

			}//next
		}//next


		glTexImage2D(GL_TEXTURE_2D,
			0,
			img->HasAlpha() ? 4 : 3,
			newWidth,
			newHeight,
			0,
			img->HasAlpha() ? GL_RGBA : GL_RGB,
			GL_UNSIGNED_BYTE,
			imageData);

		(*textureWidth) = newWidth;
		(*textureHeight) = newHeight;

		free(imageData);
	}

	// set texture parameters as you wish
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // GL_LINEAR

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return ID;

}



GLuint* Image::loadImage(wxString path, int* imageWidth, int* imageHeight, int* textureWidth, int* textureHeight)
{

	GLuint* ID = new GLuint[1];
	glGenTextures(1, &ID[0]);

	glBindTexture(GL_TEXTURE_2D, *ID);

	// check the file exists
	if (!wxFileExists(path))
	{
		wxMessageBox(_("Failed to load resource image"));
		exit(1);
	}

	wxImage* img = new wxImage(path);

	(*imageWidth) = img->GetWidth();
	(*imageHeight) = img->GetHeight();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	/*
	 * Many graphics card require that textures be power of two.
	 * Below is a simple implementation, probably not optimal but working.
	 * If your texture sizes are not restricted to power of 2s, you can
	 * of course adapt the bit below as needed.
	 */

	float power_of_two_that_gives_correct_width = std::log((float)(*imageWidth)) / std::log(2.0);
	float power_of_two_that_gives_correct_height = std::log((float)(*imageHeight)) / std::log(2.0);

	// check if image dimensions are a power of two
	if ((int)power_of_two_that_gives_correct_width == power_of_two_that_gives_correct_width &&
		(int)power_of_two_that_gives_correct_height == power_of_two_that_gives_correct_height)
	{
		// note: must make a local copy before passing the data to OpenGL, as GetData() returns RGB 
		// and we want the Alpha channel if it's present. Additionally OpenGL seems to interpret the 
		// data upside-down so we need to compensate for that.
		GLubyte *bitmapData = img->GetData();
		GLubyte *alphaData = img->GetAlpha();
		GLubyte *imageData;

		int bytesPerPixel = img->HasAlpha() ? 4 : 3;

		int imageSize = (*imageWidth) * (*imageHeight) * bytesPerPixel;
		imageData = (GLubyte *)malloc(imageSize);

		int rev_val = (*imageHeight) - 1;

		for (int y = 0; y < (*imageHeight); y++)
		{
			for (int x = 0; x < (*imageWidth); x++)
			{
				imageData[(x + y*(*imageWidth))*bytesPerPixel + 0] =
					bitmapData[(x + (rev_val - y)*(*imageWidth)) * 3];

				imageData[(x + y*(*imageWidth))*bytesPerPixel + 1] =
					bitmapData[(x + (rev_val - y)*(*imageWidth)) * 3 + 1];

				imageData[(x + y*(*imageWidth))*bytesPerPixel + 2] =
					bitmapData[(x + (rev_val - y)*(*imageWidth)) * 3 + 2];

				if (bytesPerPixel == 4) imageData[(x + y*(*imageWidth))*bytesPerPixel + 3] =
					alphaData[x + (rev_val - y)*(*imageWidth)];
			}//next
		}//next

		// if yes, everything is fine
		glTexImage2D(GL_TEXTURE_2D,
			0,
			bytesPerPixel,
			*imageWidth,
			*imageHeight,
			0,
			img->HasAlpha() ? GL_RGBA : GL_RGB,
			GL_UNSIGNED_BYTE,
			imageData);

		(*textureWidth) = (*imageWidth);
		(*textureHeight) = (*imageHeight);

		free(imageData);
	}
	else // texture is not a power of two. We need to resize it
	{

		int newWidth = (int)std::pow(2.0, (int)(std::ceil(power_of_two_that_gives_correct_width)));
		int newHeight = (int)std::pow(2.0, (int)(std::ceil(power_of_two_that_gives_correct_height)));

		//printf("Unsupported image size. Recommand values: %i %i\n",newWidth,newHeight);   

		GLubyte *bitmapData = img->GetData();
		GLubyte        *alphaData = img->GetAlpha();
		GLubyte *imageData;

		int old_bytesPerPixel = 3;
		int bytesPerPixel = img->HasAlpha() ? 4 : 3;

		int imageSize = newWidth * newHeight * bytesPerPixel;
		imageData = (GLubyte *)malloc(imageSize);

		int rev_val = (*imageHeight) - 1;

		for (int y = 0; y < newHeight; y++)
		{
			for (int x = 0; x < newWidth; x++)
			{

				if (x < (*imageWidth) && y < (*imageHeight)){
					imageData[(x + y*newWidth)*bytesPerPixel + 0] =
						bitmapData[(x + (rev_val - y)*(*imageWidth))*old_bytesPerPixel + 0];

					imageData[(x + y*newWidth)*bytesPerPixel + 1] =
						bitmapData[(x + (rev_val - y)*(*imageWidth))*old_bytesPerPixel + 1];

					imageData[(x + y*newWidth)*bytesPerPixel + 2] =
						bitmapData[(x + (rev_val - y)*(*imageWidth))*old_bytesPerPixel + 2];

					if (bytesPerPixel == 4) imageData[(x + y*newWidth)*bytesPerPixel + 3] =
						alphaData[x + (rev_val - y)*(*imageWidth)];

				}
				else
				{

					imageData[(x + y*newWidth)*bytesPerPixel + 0] = 0;
					imageData[(x + y*newWidth)*bytesPerPixel + 1] = 0;
					imageData[(x + y*newWidth)*bytesPerPixel + 2] = 0;
					if (bytesPerPixel == 4) imageData[(x + y*newWidth)*bytesPerPixel + 3] = 0;
				}

			}//next
		}//next


		glTexImage2D(GL_TEXTURE_2D,
			0,
			img->HasAlpha() ? 4 : 3,
			newWidth,
			newHeight,
			0,
			img->HasAlpha() ? GL_RGBA : GL_RGB,
			GL_UNSIGNED_BYTE,
			imageData);

		(*textureWidth) = newWidth;
		(*textureHeight) = newHeight;

		free(imageData);
	}

	// set texture parameters as you wish
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // GL_LINEAR

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return ID;

}