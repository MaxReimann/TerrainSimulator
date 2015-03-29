#include "imports.h"
#include "globals.h"
#include "3dwindow.h"
#include "..\resources\rotate_icon_png.cpp"
#include "main.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int       TERRAIN_REGIONS_COUNT = 4;
const float     HEIGHTMAP_SCALE = 0.005f;
const float     HEIGHTMAP_ROUGHNESS = 1.2f;
const float     HEIGHTMAP_TILING_FACTOR = 2.0f;
const int       HEIGHTMAP_SIZE = 128;
const int       HEIGHTMAP_GRID_SPACING = 16;

#define SIMSIZEX 256
#define SIMSIZEY 256
#define terrainStepWidth 0.0625
#define terrainStepLength 0.0625

float BasicGLPane::zoom = -5.0;


using namespace std;


void BasicGLPane::mouseMoved(wxMouseEvent& event) {
	if (event.m_middleDown == true) //wenn gleichzeitg die mittelmaustaste gedrueckt
	{
		if (event.m_shiftDown == true) //modifier shift zum translieren
			mouseshift(event.GetX(), event.GetY());
		else 
			mouseMovement(event.GetX(), event.GetY());

		m_rotateIcon->setHotspot(event.GetX()*0.01, event.GetY()*0.01);
		Refresh();
	}

	if (Alti.showflag == true && event.m_leftDown &&  event.m_middleDown == false && event.m_shiftDown == false) {
		int mousex = event.GetX();
		int mousey = getHeight() - event.GetY();

		if (unproject(mousex, mousey, -5) == GL_FALSE)
			return;  //error

		int flagx = static_cast<int>(floor(obj_cox[0] / terrainStepWidth));
		int flagy = static_cast<int>(floor(obj_coy[0] / terrainStepLength));

		setFlag(abs(flagx), abs(flagy));
	}

}
GLint BasicGLPane::project(int x, int y, int z){
	//results are written to win_co
	GLdouble model_mat[16];
	GLdouble project_mat[16];
	GLint viewp_mat[4];

	glGetDoublev(GL_MODELVIEW_MATRIX, model_mat);
	glGetDoublev(GL_PROJECTION_MATRIX, project_mat);
	glGetIntegerv(GL_VIEWPORT, viewp_mat);

	GLint result = gluProject(-x, -y, z, 
		model_mat, project_mat, viewp_mat,
		win_cox, win_coy, win_coz);

	return result;
}

GLint  BasicGLPane::unproject(int winx, int winy, int winz){
	GLdouble model_mat[16];
	GLdouble project_mat[16];
	GLint viewp_mat[4];

	glPushMatrix(); 
	defineFlag(); //position pointer

	float ratio_w_h = (float)(getWidth()) / (float)(getHeight());
	gluPerspective(45 /*view angle*/, ratio_w_h, 0.1 /*clip close*/, 200 /*clip far*/);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glGetDoublev(GL_MODELVIEW_MATRIX, model_mat);
	glViewport(0, 0, getWidth(), getHeight());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glTranslatef(m_xShift, m_yShift, -5);
	glGetDoublev(GL_PROJECTION_MATRIX, project_mat);
	glGetIntegerv(GL_VIEWPORT, viewp_mat);

	GLint result = gluUnProject(winx, winy, winz, 
		model_mat, project_mat, viewp_mat,
		obj_cox, obj_coy, obj_coz);

	return result;
}

void BasicGLPane::mouseWheelMoved(wxMouseEvent& event) {
	int fac = event.GetWheelRotation();
	zoom += fac / 120.0;
	Refresh();
}


void BasicGLPane::rightClick(wxMouseEvent& event) {}
void BasicGLPane::mouseLeftWindow(wxMouseEvent& event) {}
void BasicGLPane::keyPressed(wxKeyEvent& event) {}
void BasicGLPane::keyReleased(wxKeyEvent& event) {}

void BasicGLPane::middleMouseDown(wxMouseEvent& event) {
	m_lastX = event.GetX(); //lastX prevents model jumping at second click
	m_lastY = event.GetY();
	m_shiftLastX = event.GetX();
	m_shiftLastY = event.GetY();
	m_rotating = true;
}

void BasicGLPane::mouseReleased(wxMouseEvent& event) {
	if (event.m_middleDown == false)
		m_rotating = false;
}



BasicGLPane::BasicGLPane(wxFrame* parent, int* args) :
wxGLCanvas(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, wxT("GLCanvas"), args),
m_alpha(0), 
m_xPos(0), m_yPos(0), m_zPos(0), 
m_xRot(0), m_yRot(0), m_angle(0.0),
m_xShift(0), m_yShift(0),
m_terrainHeightScale(2.0),
m_runs(0), 
m_vertexBuffer(0), m_indexBuffer(0), m_totalIndices(0), 
m_update(0)
{
	m_errorStream.open("gl_error.txt");

	g_lightDir[0] = 0.5f, g_lightDir[2] = 0.0f, g_lightDir[3] = 1.0f, g_lightDir[4] = 1.0f;
	lightIntesity[0] = 1.0f, lightIntesity[1] = 1.0f, lightIntesity[2] = 1.0f, lightIntesity[3] = 1.0f;
	yellowAmbientDiffuse[0] = 1.0f, yellowAmbientDiffuse[1] = 1.0f, yellowAmbientDiffuse[2] = 1.0f, yellowAmbientDiffuse[3] = 1.0f;
	
	int argc = 1;
	char* argv[1] = { wxString((wxTheApp->argv)[0]).char_str() };
	glutInit(&argc, argv);

	obj_cox = new GLdouble[2];
	obj_coy = new GLdouble[2];
	obj_coz = new GLdouble[2];
	win_cox = new GLdouble[2];
	win_coy = new GLdouble[2];
	win_coz = new GLdouble[2];

	wxMemoryInputStream istream(rotate_icon_png, sizeof(rotate_icon_png));
	wxImage im_rotate_png(istream, wxBITMAP_TYPE_PNG);
	m_rotatingImage = new Image(im_rotate_png);
	m_rotateIcon = new Drawable(m_rotatingImage);
	m_rotateIcon->setZ(0.1);
	m_rotateIcon->scale(0.001);
	m_rotateIcon->rotate(90);
	m_rotating = true;
}

BasicGLPane::~BasicGLPane() {
	m_errorStream.close();
	delete[] win_cox, delete[]  win_coy, delete[]  win_coz;
	delete[] obj_cox, delete[] obj_coy, delete[] obj_coz;
	delete m_rotatingImage;
	delete m_rotateIcon;
	cleanupShader();
}

void BasicGLPane::resized(wxSizeEvent& evt)
{
	wxGLCanvas::OnSize(evt);
	Refresh();
}


void  BasicGLPane::processMouse(int button, int state, int x, int y)
{
	// Used for wheels, has to be up
	if (state == GLUT_UP)
	{
		if (button == 3) // freeglut Up event
			zoom++;
		else if (button == 4)
			zoom--;
	}
}



void BasicGLPane::prepare3DViewport(int topleftX, int topleftY, int bottomrigthX, int bottomrigth_y)
{
	/*
	*  Inits the OpenGL viewport for drawing in 3D.
	*/

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black Background
	glClearDepth(1.0f);	// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LEQUAL); // The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_COLOR_MATERIAL);

	glViewport(topleftX, -topleftY, bottomrigthX - topleftX, bottomrigth_y - topleftY);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	float ratio_w_h = (float)(bottomrigthX - topleftX) / (float)(bottomrigth_y - topleftY);
	gluPerspective(60/*view angle*/, ratio_w_h, 0.1 /*clip close*/, 200 /*clip far*/);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void BasicGLPane::camera(void) {
	glRotatef(m_xRot, 1.0, 0.0, 0.0);  //rotate our camera on the x-axis (left and right)
	glRotatef(m_yRot, 0.0, -1.0, 0.0);  //rotate our camera on the y-axis (up and down)
	glTranslated(-m_xPos, m_yPos, -m_zPos); //translate the screen to the position of our camera
}

void BasicGLPane::mouseMovement(int x, int y) {
	int diffX = x - m_lastX; //check the difference between the current x and the last x position
	int diffY = y - m_lastY; //check the difference between the current y and the last y position
	m_lastX = x; //set lastx to the current x position
	m_lastY = y; //set lasty to the current y position
	m_xRot += (float)diffY; //set the xrot to xrot with the addition of the difference in the y position
	m_yRot += (float)diffX;    //set the xrot to yrot with the addition of the difference in the x position
}
void BasicGLPane::mouseshift(int x, int y) {
	float diffX = ((float)x - m_shiftLastX) / 50.0; //check the difference between the current x and the last x position
	float diffY = -((float)y - m_shiftLastY) / 50.0; //check the difference between the current y and the last y position
	m_shiftLastX = x; //set lastx to the current x position
	m_shiftLastY = y; //set lasty to the current y position
	m_xShift += diffX;
	m_yShift += diffY;
}
void BasicGLPane::setFlag(int x, int y) {
	if (x >= 0 && x < Alti.getXSize())
		Alti.FlagX = x;
	if (y >= 0 && y < Alti.getYSize())
		Alti.FlagY = Alti.getYSize() - y;
	Refresh();
}

int BasicGLPane::getWidth()
{
	return GetSize().x;
}

int BasicGLPane::getHeight()
{
	return GetSize().y;
}

GLuint BasicGLPane::makeTerrain(){

	m_terrainDL = glGenLists(1); // generate list on GPU

	glNewList(m_terrainDL, GL_COMPILE);
	glColor4f(0.8, 0.5, 0.5, 1);

	for (double y = 0; y < Alti.getYSize(); ++y){
		glBegin(GL_TRIANGLE_STRIP);
		for (double x = 0; x < Alti.getXSize(); ++x) {
			double color = 1.0 / (Alti.getAltitude(x, y) * m_terrainHeightScale  * 3.5); // coloring by height
			color = ((color) > 1.0) ? (1.0) : color;
			
			if (y == 0)
			{
				glColor4f(color, 0.7, 0.4, 1);
				glTexCoord2f(0, 0);
				glVertex3f(x * terrainStepWidth, y * terrainStepLength, -Alti.getAltitude(x, y) * m_terrainHeightScale);
			}
			else
			{
				glColor4f(color, 0.7, 0.4, 1);
				glTexCoord2f(0, (int(x) % 2) ? 0 : 1);
				glVertex3f(x * terrainStepWidth, y * terrainStepLength, -Alti.getAltitude(x, y - 1)  * m_terrainHeightScale);
			}

			glTexCoord2f((int(x) % 2) ? 0 : 1, 1);
			glVertex3f(x * terrainStepWidth, (y + 1) * terrainStepLength, -Alti.getAltitude(x, y)  * m_terrainHeightScale);
		}

		glEnd();

	}
	glEndList();


	return(m_terrainDL);
}

void BasicGLPane::refreshTerrain() {
	m_update = true;

}
void BasicGLPane::defineFlag() {
	glColor4f(0.8, 0.7, 1, 1);
	glTranslatef(Alti.FlagX * 0.025, (Alti.FlagY)  * 0.025, Alti.getAltitude(Alti.FlagX, (Alti.FlagY))  * m_terrainHeightScale);
	m_cylinder = gluNewQuadric();
	gluCylinder(m_cylinder, 0.05, 0.05, 0.5, 20, 20);
}

void BasicGLPane::zoomFunc(){
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPickMatrix(
		viewport[2] * (.5), viewport[3] * (.5),
		viewport[2] * zoom, viewport[3] * zoom,
		viewport);

	gluPerspective(45, double(viewport[2]) / viewport[3], 0.1, 100);
}


void BasicGLPane::render(wxPaintEvent& evt)
{
	if (!IsShown()) return;

	wxGLCanvas::SetCurrent();

	if (m_runs == 0) { //initialize
		initTextures();
		terrainCreate();
		if (!generateVertices())
		{
			string msg;
			msg += "Application initialization failed! could not create vertices :";
			createOkDialog(msg, string("Error"));
		}
	}

	wxPaintDC(this); // only to be used in paint events.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// ------------- draw some 3D ----------------
	prepare3DViewport(0, 0, getWidth(), getHeight());

	glLoadIdentity();

	glUseProgram(g_terrainShader);
	updateTerrainShaderParameters();

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, yellowAmbientDiffuse);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, yellowAmbientDiffuse);

	glEnable(GL_COLOR_MATERIAL);


	glColor4f(0.6, 0.7, 1, 1);
	//	gluLookAt(camera[0], camera[1], camera[2], 
	//		/* look from camera XYZ */ 0, 0, 0, 
	//		/* look at the origin */ 0, 1, 0);

	glTranslatef(m_xShift, m_yShift, zoom);
	glRotatef(m_yRot, 0, 1, 0);
	glRotatef(m_xRot, 1, 0, 0);

	if (m_update == true){
		updateTerrain();
		m_update = false;
	}

	glEnable(GL_TEXTURE_2D);
	bindTexture(g_regions[0].texture, 0);
	bindTexture(g_regions[1].texture, 1);
	bindTexture(g_regions[2].texture, 2);
	bindTexture(g_regions[3].texture, 3);

	terrainDraw();
	if (Alti.showflag == true)
	{
		glPushMatrix();
		defineFlag(); 
		glPopMatrix();
	}


	for (int i = 3; i >= 0; --i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
	}

	glUseProgram(0);

	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);

	glFlush();
	SwapBuffers();
	m_runs += 1;
}

void BasicGLPane::createOkDialog(string msg, string title)
{
	okDialog = new wxMessageDialog(NULL, wxString(msg), wxString(title), wxOK | wxICON_HAND, wxDefaultPosition);
	okDialog->ShowModal();
	okDialog->Destroy();
}

void BasicGLPane::initTextures(void) {
	GLenum err = glewInit();

	if (GLEW_OK != err){
		string msg;
		msg += "Application initialization failed! wrong opengl version ? :";
		msg += (char*)glGetString(GL_VERSION);
		/* Problem: glewInit failed, something is seriously wrong. */
		createOkDialog(msg, string("Error"));
	}

	try {
		initStruct();
		createTextures();
	}
	catch (const std::exception &e){
		string msg;
		msg += "Application initialization failed! /n";
		msg += e.what();
		createOkDialog(msg, string("Error"));
	}
}

void BasicGLPane::updateTerrain(void) {
	terrainCreate();
	if (!generateVertices())
	{
		string msg;
		msg += "Application initialization failed! could not create vertices :";
		createOkDialog(msg, string("Error"));
	}

}

void BasicGLPane::startWaterSimulation(int method, WaterConstants *constants)
{
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	WaterSimulation watersim = WaterSimulation(constants, method);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}



GLuint BasicGLPane::linkShaders(GLuint vertShader, GLuint fragShader)
{
    // Links the compiled vertex and/or fragment shaders into an executable
    // shader program. Returns the executable shader object. If the shaders
    // failed to link into an executable shader program, then a std::string
    // object is thrown containing the info log.

    GLuint program = glCreateProgram();

    if (program)
    {
        GLint linked = 0;

        if (vertShader)
            glAttachShader(program, vertShader);

        if (fragShader)
            glAttachShader(program, fragShader);

        glLinkProgram(program);
        glGetProgramiv(program, GL_LINK_STATUS, &linked);

        if (!linked)
        {
            GLsizei infoLogSize = 0;
            std::string infoLog;

            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogSize);
            infoLog.resize(infoLogSize);
            glGetProgramInfoLog(program, infoLogSize, &infoLogSize, &infoLog[0]);

            throw infoLog;
        }

        // Mark the two attached shaders for deletion. These two shaders aren't
        // deleted right now because both are already attached to a shader
        // program. When the shader program is deleted these two shaders will
        // be automatically detached and deleted.

        if (vertShader)
            glDeleteShader(vertShader);

        if (fragShader)
            glDeleteShader(fragShader);
    }

    return program;
}

GLuint BasicGLPane::loadShaderProgram(const char *pszFilename, std::string &infoLog)
{
    infoLog.clear();

    GLuint program = 0;
    std::string buffer;

    // Read the text file containing the GLSL shader program.
    // This file contains 1 vertex shader and 1 fragment shader.
    readTextFile(pszFilename, buffer);

    // Compile and link the vertex and fragment shaders.
    if (buffer.length() > 0)
    {
        char *pSource = 0;
        GLint length = 0;
        GLuint vertShader = 0;
        GLuint fragShader = 0;

        std::string::size_type vertOffset = buffer.find("[vert]");
        std::string::size_type fragOffset = buffer.find("[frag]");

        try
        {
            // Get the vertex shader source and compile it.
            // The source is between the [vert] and [frag] tags.
            if (vertOffset != std::string::npos)
            {
                vertOffset += 6;        // skip over the [vert] tag
                pSource = static_cast<char *>(&buffer[vertOffset]);
                length = static_cast<GLint>(fragOffset - vertOffset);
                vertShader = compileShader(GL_VERTEX_SHADER, pSource, length);
            }

            // Get the fragment shader source and compile it.
            // The source is between the [frag] tag and the end of the file.
            if (fragOffset != std::string::npos)
            {
                fragOffset += 6;        // skip over the [frag] tag
                pSource = static_cast<char *>(&buffer[fragOffset]);
                length = static_cast<GLint>(buffer.length() - fragOffset - 1);
                fragShader = compileShader(GL_FRAGMENT_SHADER, pSource, length);
            }

            // Now link the vertex and fragment shaders into a shader program.
            program = linkShaders(vertShader, fragShader);
        }
        catch (const std::string &errors)
        {
            infoLog = errors;
        }
    }

    return program;
}

GLuint BasicGLPane::loadTexture(const char *pszFilename)
{
    return loadTexture(pszFilename, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT);
}

GLuint BasicGLPane::loadTexture(const char *pszFilename, GLint magFilter, GLint minFilter,
                   GLint wrapS, GLint wrapT)
{
    GLuint id = 0;
    wxImage image;

	image = wxImage(pszFilename);
        // The Bitmap class loads images and orients them top-down.
        // OpenGL expects bitmap images to be oriented bottom-up.
        image = (image.Rotate90(true)).Rotate90(true); //image wird vertical geflippt

        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

        /*if (g_maxAnisotrophy > 1)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT,
                g_maxAnisotrophy);
        }
		*/

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.GetWidth(), image.GetHeight(),
            0, GL_RGB,GL_UNSIGNED_BYTE,image.GetData() );

        glBindTexture(GL_TEXTURE_2D, 0);
    return id;
}

void BasicGLPane::bindTexture(GLuint texture, GLuint unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
}

GLuint BasicGLPane::compileShader(GLenum type, char *pszSource, GLint length)
{
    // Compiles the shader given it's source code. Returns the shader object.
    // A std::string object containing the shader's info log is thrown if the
    // shader failed to compile.
    //
    // 'type' is either GL_VERTEX_SHADER or GL_FRAGMENT_SHADER.
    // 'pszSource' is a C style string containing the shader's source code.
    // 'length' is the length of 'pszSource'.

    GLuint shader = glCreateShader(type);

    if (shader)
    {
        GLint compiled = 0;
		const GLchar *temp = reinterpret_cast<const GLchar*>(pszSource);
        glShaderSource(shader, 1,&temp, &length);
        glCompileShader(shader);
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

        if (!compiled)
        {
            GLsizei infoLogSize = 0;
            std::string infoLog;

            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogSize);
            infoLog.resize(infoLogSize);
            glGetShaderInfoLog(shader, infoLogSize, &infoLogSize, &infoLog[0]);

            throw infoLog;
        }
    }

    return shader;
}

GLuint BasicGLPane::createNullTexture(int width, int height)
{
    // Create an empty white texture. This texture is applied to models
    // that don't have any texture maps. This trick allows the same shader to
    // be used to draw the model with and without textures applied.

    int pitch = ((width * 32 + 31) & ~31) >> 3; // align to 4-byte boundaries
    std::vector<GLubyte> pixels(pitch * height, 255);
    GLuint texture = 0;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA,
        GL_UNSIGNED_BYTE, &pixels[0]);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}



void BasicGLPane::createTextures() {
	if (!(g_nullTexture = createNullTexture(2, 2)))
        throw std::runtime_error("failed to create null texture.");

    for (int i = 0; i < TERRAIN_REGIONS_COUNT; ++i)
    {
        if (!(g_regions[i].texture = loadTexture(g_regions[i].filename.c_str())))
            throw std::runtime_error("Failed to load texture: " + g_regions[i].filename);
    }

    // Setup shaders.

    std::string infoLog;

    if (!(g_terrainShader = loadShaderProgram("shaders/terrain.glsl", infoLog)))
        throw std::runtime_error("Failed to load shader: terrain.glsl.\n" + infoLog);

}

void BasicGLPane::readTextFile(const char *pszFilename, std::string &buffer)
{
    std::ifstream file(pszFilename, std::ios::binary);

    if (file.is_open())
    {
        file.seekg(0, std::ios::end);

        std::ifstream::pos_type fileSize = file.tellg();

        buffer.resize(fileSize);
        file.seekg(0, std::ios::beg);
        file.read(&buffer[0], fileSize);
    }
}

void BasicGLPane::initStruct(void) {


	g_regions[0].min = -10.0f;//alle werte 10 runter, da bei 0.0 nur schwarz (nichts) angezigt wird
	g_regions[0].max = 50.0f * HEIGHTMAP_SCALE;
	g_regions[0].texture = 0;
	g_regions[0].filename = string( "resources/textures/browndirt.jpg"); //wahlweise browndirt oder dirt

	g_regions[1].min =  41.0f * HEIGHTMAP_SCALE;
	g_regions[1].max = 91.0f * HEIGHTMAP_SCALE;
	g_regions[1].texture = 0;
	g_regions[1].filename = string("resources/textures/grass.jpg");

	g_regions[2].min =  92.0f * HEIGHTMAP_SCALE;
	g_regions[2].max = 193.0f * HEIGHTMAP_SCALE;
	g_regions[2].texture = 0;
	g_regions[2].filename = string("resources/textures/rock.jpg");

	g_regions[3].min =  194.0f * HEIGHTMAP_SCALE;
	g_regions[3].max =  300.0f * HEIGHTMAP_SCALE;
	g_regions[3].texture = 0;
	g_regions[3].filename = string("resources/textures/snow.jpg");


}


void BasicGLPane::updateTerrainShaderParameters()
{
    GLint handle = -1;

    // Update the terrain tiling factor.

    handle = glGetUniformLocation(g_terrainShader, "tilingFactor");
    glUniform1f(handle, HEIGHTMAP_TILING_FACTOR);

    // Update terrain region 1.

    handle = glGetUniformLocation(g_terrainShader, "region1.max");
    glUniform1f(handle, g_regions[0].max);

    handle = glGetUniformLocation(g_terrainShader, "region1.min");
    glUniform1f(handle, g_regions[0].min);    

    // Update terrain region 2.

    handle = glGetUniformLocation(g_terrainShader, "region2.max");
    glUniform1f(handle, g_regions[1].max);

    handle = glGetUniformLocation(g_terrainShader, "region2.min");
    glUniform1f(handle, g_regions[1].min);

    // Update terrain region 3.

    handle = glGetUniformLocation(g_terrainShader, "region3.max");
    glUniform1f(handle, g_regions[2].max);

    handle = glGetUniformLocation(g_terrainShader, "region3.min");
    glUniform1f(handle, g_regions[2].min);

    // Update terrain region 4.

    handle = glGetUniformLocation(g_terrainShader, "region4.max");
    glUniform1f(handle, g_regions[3].max);

    handle = glGetUniformLocation(g_terrainShader, "region4.min");
    glUniform1f(handle, g_regions[3].min);

    // Bind textures.

    glUniform1i(glGetUniformLocation(g_terrainShader, "region1ColorMap"), 0);
    glUniform1i(glGetUniformLocation(g_terrainShader, "region2ColorMap"), 1);
    glUniform1i(glGetUniformLocation(g_terrainShader, "region3ColorMap"), 2);
    glUniform1i(glGetUniformLocation(g_terrainShader, "region4ColorMap"), 3);
}



bool BasicGLPane::generateVertices()
{
    //*pVertices = 0;
    Vertex *pVertex = 0;
    int currVertex = 0;
    int size = Alti.getXSize();
    int gridSpacing = 16;
    float heightScale = HEIGHTMAP_SCALE;
    Vector3 normal;

    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer );
    pVertices = static_cast<Vertex *>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));

    if (!pVertices)
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        return false;
    }

    for (int y = 0; y < Alti.getYSize(); ++y)
    {
        for (int x = 0; x < Alti.getXSize(); ++x)
        {
            currVertex = y * size + x;
            pVertex = &pVertices[currVertex];

            pVertex->x = static_cast<float>(x * 0.025);
			pVertex->y = static_cast<float>(y * 0.025);
            pVertex->z = Alti.getAltitude(x, y) * m_terrainHeightScale; 
			

            Alti.normalAtPoint(x, y, normal);
            pVertex->nx = normal.x;
            pVertex->ny = normal.y;
			pVertex->nz = normal.z;
            
            pVertex->s = static_cast<float>(x) / static_cast<float>(size);
            pVertex->t = static_cast<float>(y) / static_cast<float>(size);
        }
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return true;
}

bool BasicGLPane::terrainCreate(void)
{
    // Initialize the vertex buffer object.

	m_totalVertices = Alti.getXSize() * Alti.getYSize();
    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_totalVertices,0, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Initialize the index buffer object.

    m_totalIndices = (Alti.getXSize() - 1) * (Alti.getYSize() * 2 + 1);
    glGenBuffers(1, &m_indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    
    int indexSize = use16BitIndices() ? sizeof(unsigned short) : sizeof(unsigned int);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize * m_totalIndices, 0, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return generateIndices();
}

bool BasicGLPane::generateIndices()
{
    void *pBuffer = 0;
	int size = Alti.getXSize();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    
    if (!(pBuffer = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY)))
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        return false;
    }

    if (use16BitIndices())
    {
        unsigned short *pIndex = static_cast<unsigned short *>(pBuffer);

        for (int z = 0; z < size - 1; ++z)
        {
            if (z % 2 == 0)
            {
                for (int x = 0; x < size; ++x)
                {
                    *pIndex++ = static_cast<unsigned short>(x + z * size);
                    *pIndex++ = static_cast<unsigned short>(x + (z + 1) * size);
                }

                // Add degenerate triangles to stitch strips together.
                *pIndex++ = static_cast<unsigned short>((size - 1) + (z + 1) * size);
            }
            else
            {
                for (int x = size - 1; x >= 0; --x)
                {
                    *pIndex++ = static_cast<unsigned short>(x + z * size);
                    *pIndex++ = static_cast<unsigned short>(x + (z + 1) * size);
                }

                // Add degenerate triangles to stitch strips together.
                *pIndex++ = static_cast<unsigned short>((z + 1) * size);
            }
        }
    }
    else
    {
        unsigned int *pIndex = static_cast<unsigned int *>(pBuffer);

        for (int z = 0; z < size - 1; ++z)
        {
            if (z % 2 == 0)
            {
                for (int x = 0; x < size; ++x)
                {
                    *pIndex++ = x + z * size;
                    *pIndex++ = x + (z + 1) * size;
                }

                // Add degenerate triangles to stitch strips together.
                *pIndex++ = (size - 1) + (z + 1) * size;
            }
            else
            {
                for (int x = size - 1; x >= 0; --x)
                {
                    *pIndex++ = x + z * size;
                    *pIndex++ = x + (z + 1) * size;
                }

                // Add degenerate triangles to stitch strips together.
                *pIndex++ = (z + 1) * size;
            }
        }
    }   

    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return true;
}


void BasicGLPane::terrainDraw()
{


    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(6 * sizeof(float)));

    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(3 * sizeof(float)));

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(0));

    if (use16BitIndices())
        glDrawElements(GL_TRIANGLE_STRIP, m_totalIndices, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));
    else
        glDrawElements(GL_TRIANGLE_STRIP, m_totalIndices, GL_UNSIGNED_INT, BUFFER_OFFSET(0));

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void BasicGLPane::cleanupShader()
{
	cleanupStructs();

    for (int i = 0; i < TERRAIN_REGIONS_COUNT; ++i)
    {
        if (g_regions[i].texture)
        {
            glDeleteTextures(1, &g_regions[i].texture);
            g_regions[i].texture = 0;
        }
    }

    if (g_nullTexture)
    {
        glDeleteTextures(1, &g_nullTexture);
        g_nullTexture = 0;
    }

    if (g_terrainShader)
    {
        glUseProgram(0);
        glDeleteProgram(g_terrainShader);
        g_terrainShader = 0;
    }

}


void BasicGLPane::cleanupStructs(){
	if (m_vertexBuffer)
    {
        glDeleteBuffers(1, &m_vertexBuffer);
        m_vertexBuffer = 0;
        m_totalVertices = 0;
    }

    if (m_indexBuffer)
    {
        glDeleteBuffers(1, &m_indexBuffer);
        m_indexBuffer = 0;
        m_totalIndices = 0;
    }
}