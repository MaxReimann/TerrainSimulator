#pragma once
#define	TGA_ERROR_FILE_OPEN				-5
#define TGA_ERROR_READING_FILE			-4
#define TGA_ERROR_INDEXED_COLOR			-3
#define TGA_ERROR_MEMORY				-2
#define TGA_ERROR_COMPRESSED_FILE		-1
#define TGA_OK							 0


typedef struct {
	int status;
	unsigned char type, pixelDepth;
	short int width, height;
	unsigned char *imageData;
}tgaInfo;

tgaInfo* tgaLoad(char *filename);

int tgaSave(char			*filename, 
			 short int		width, 
			 short int		height, 
			 unsigned char	pixelDepth, 
			 unsigned char	*imageData);

int tgaSaveSeries(char			*filename, 
			 short int		width, 
			 short int		height, 
			 unsigned char	pixelDepth, 
			 unsigned char	*imageData);

void tgaRGBtoGreyscale(tgaInfo *info);

int tgaGrabScreenSeries(char *filename, int x,int y, int w, int h);

void tgaDestroy(tgaInfo *info);



/// alternative
struct STGA
{
        STGA()
        {
                data = (unsigned char*)0;
                width = 0;
                height = 0;
                byteCount = 0;
				type = 0;
        }

	~STGA() { delete[] data; data = 0; }

	void destroy() { delete[] data; data = 0; }

        int width;
	int height;
        unsigned char byteCount;
        unsigned char* data;
		unsigned char* type;
};

int loadTGA(const char *filename, STGA& tgaFile);

void tgaRGBtoGreyscale(STGA& tgaFile);
