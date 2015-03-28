#pragma once
#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
#endif

#include "imports.h"
#include "DC_Panelclass.h"
#include "main.h"

wxGauge* get_gauge(void);

struct TerrainRegion
	{
		float min;
		float max;
		GLuint texture;
		std::string filename;
	};

  struct Vertex
    {
        float x, y, z;
        float nx, ny, nz;
        float s, t;
		double d;
    };



class BasicGLPane : public wxGLCanvas
{
   
public:
	BasicGLPane(wxFrame* parent, int* args);
	~BasicGLPane();
	void resized(wxSizeEvent& evt);
    static float zoom;
	int getWidth();
	int getHeight();
    
	void render(wxPaintEvent& evt);
	void prepare3DViewport(int topleft_x, int topleft_y, int bottomrigth_x, int bottomrigth_y);

	void camera(void);
	void mouseMovement(int x, int y);
	void mouseshift(int x, int y);
	void zoomFunc();
	GLint  project(int x , int y ,int z );
	GLint unproject(int winx , int winy ,int winz );
    
	// events
	void mouseMoved(wxMouseEvent& event);
	void mouseDown(wxMouseEvent& event);
	void mouseWheelMoved(wxMouseEvent& event);
	void mouseReleased(wxMouseEvent& event);
	void rightClick(wxMouseEvent& event);
	void mouseLeftWindow(wxMouseEvent& event);
	void keyPressed(wxKeyEvent& event);
	void keyReleased(wxKeyEvent& event);
	void middleMouseDown(wxMouseEvent& event);
	GLuint makeTerrain(void);
	void refreshTerrain();
	void defineFlag(void);
	void setFlag(int x, int y);

	static void processMouse(int button, int state, int x, int y);
	GLuint linkShaders(GLuint vertShader, GLuint fragShader);
	GLuint loadShaderProgram(const char *pszFilename, std::string &infoLog);
	GLuint loadTexture(const char *pszFilename);
	GLuint loadTexture(const char *pszFilename, GLint magFilter, GLint minFilter,GLint wrapS, GLint wrapT);


	void bindTexture(GLuint texture, GLuint unit);
	GLuint compileShader(GLenum type , char* pszSource , GLint length);
	GLuint createNullTexture(int width, int height);
	void createTextures();
	void readTextFile(const char *pszFilename, std::string &buffer);
	void initStruct(void);
	void initTextures(void);
	void createOkDialog(std::string msg,std::string title);
	void updateTerrainShaderParameters(void);
	bool generateVertices(void);
	bool generateIndices(void);
	bool terrainCreate(void);
	void terrainDraw(void);
	void updateTerrain(void);
	void cleanupShader(void);
	void cleanupStructs(void);
	void startWaterSimulation(int method, WaterConstants *constants);

	
	
	bool use16BitIndices() const
    { return m_totalVertices <= 65536; }

	const GLubyte *BUFFER_OFFSET(size_t bytes)
	{ return reinterpret_cast<const GLubyte *>(0) + bytes; }


	double m_terrainHeightScale;


	DECLARE_EVENT_TABLE()

private:
	float m_alpha;
	float m_xPos, m_yPos, m_zPos, m_xRot, m_yRot, m_angle;
	float m_xShift, m_yShift;
	float m_lastX, m_lastY, m_shiftLastX, m_shiftLastY;
	std::fstream m_errorStream;
	
	GLuint   g_nullTexture;
	GLuint   g_terrainShader;
	GLuint m_terrainDL; //die terrain liste im gpu
	GLUquadric* m_cylinder;
	GLUquadric* m_sphere;
	GLuint m_vertexBuffer;
	GLuint m_indexBuffer;
	int m_totalVertices;
	int m_totalIndices;
	Vertex *pVertices;

	GLdouble* win_cox;
	GLdouble* win_coy;
	GLdouble* win_coz;
	GLdouble* obj_cox;
	GLdouble* obj_coy;
	GLdouble* obj_coz;


	//float hoehenmap[2500];
	float g_lightDir[4];
	float lightIntesity[4];
	float yellowAmbientDiffuse[4];
	

	wxMessageDialog *yesNoDialog;
	wxMessageDialog *okDialog;


	struct TerrainRegion g_regions[4];
	Image* m_rotatingImage;
	Drawable* m_rotateIcon;

	int m_runs;
	bool m_update;
	bool m_rotating;
};

  
