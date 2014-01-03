#include "imports.h"
#include "main.h"

//#include "myopengl.h"
#include "gl_object.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int       TERRAIN_REGIONS_COUNT = 4;

const float     HEIGHTMAP_SCALE = 0.005f;
const float     HEIGHTMAP_ROUGHNESS = 1.2f;
//const float     HEIGHTMAP_SCALE = 2.0f;
const float     HEIGHTMAP_TILING_FACTOR = 2.0f;
const int       HEIGHTMAP_SIZE = 128;
const int       HEIGHTMAP_GRID_SPACING = 16;









GLuint BasicGLPane::LinkShaders(GLuint vertShader, GLuint fragShader)
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

GLuint BasicGLPane::LoadShaderProgram(const char *pszFilename, std::string &infoLog)
{
    infoLog.clear();

    GLuint program = 0;
    std::string buffer;

    // Read the text file containing the GLSL shader program.
    // This file contains 1 vertex shader and 1 fragment shader.
    ReadTextFile(pszFilename, buffer);

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
                vertShader = CompileShader(GL_VERTEX_SHADER, pSource, length);
            }

            // Get the fragment shader source and compile it.
            // The source is between the [frag] tag and the end of the file.
            if (fragOffset != std::string::npos)
            {
                fragOffset += 6;        // skip over the [frag] tag
                pSource = static_cast<char *>(&buffer[fragOffset]);
                length = static_cast<GLint>(buffer.length() - fragOffset - 1);
                fragShader = CompileShader(GL_FRAGMENT_SHADER, pSource, length);
            }

            // Now link the vertex and fragment shaders into a shader program.
            program = LinkShaders(vertShader, fragShader);
        }
        catch (const std::string &errors)
        {
            infoLog = errors;
        }
    }

    return program;
}

GLuint BasicGLPane::LoadTexture(const char *pszFilename)
{
    return LoadTexture(pszFilename, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR,
        GL_REPEAT, GL_REPEAT);
}

GLuint BasicGLPane::LoadTexture(const char *pszFilename, GLint magFilter, GLint minFilter,
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

void BasicGLPane::BindTexture(GLuint texture, GLuint unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
}

GLuint BasicGLPane::CompileShader(GLenum type, char *pszSource, GLint length)
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

GLuint BasicGLPane::CreateNullTexture(int width, int height)
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



void BasicGLPane::create_textures() {
	if (!(g_nullTexture = CreateNullTexture(2, 2)))
        throw std::runtime_error("failed to create null texture.");

    for (int i = 0; i < TERRAIN_REGIONS_COUNT; ++i)
    {
        if (!(g_regions[i].texture = LoadTexture(g_regions[i].filename.c_str())))
            throw std::runtime_error("Failed to load texture: " + g_regions[i].filename);
    }

    // Setup shaders.

    std::string infoLog;

    if (!(g_terrainShader = LoadShaderProgram("shaders/terrain.glsl", infoLog)))
        throw std::runtime_error("Failed to load shader: terrain.glsl.\n" + infoLog);

}

void BasicGLPane::ReadTextFile(const char *pszFilename, std::string &buffer)
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

void BasicGLPane::initstruct(void) {


	g_regions[0].min = -10.0f;//alle werte 10 runter, da bei 0.0 nur schwarz (nichts) angezigt wird
	g_regions[0].max = 50.0f * HEIGHTMAP_SCALE;
	g_regions[0].texture = 0;
	g_regions[0].filename = string( "other_resources/browndirt.jpg"); //wahlweise browndirt oder dirt

	g_regions[1].min =  41.0f * HEIGHTMAP_SCALE;
	g_regions[1].max = 91.0f * HEIGHTMAP_SCALE;
	g_regions[1].texture = 0;
	g_regions[1].filename = string("other_resources/grass.jpg");

	g_regions[2].min =  92.0f * HEIGHTMAP_SCALE;
	g_regions[2].max = 193.0f * HEIGHTMAP_SCALE;
	g_regions[2].texture = 0;
	g_regions[2].filename = string("other_resources/rock.jpg");

	g_regions[3].min =  194.0f * HEIGHTMAP_SCALE;
	g_regions[3].max =  300.0f * HEIGHTMAP_SCALE;
	g_regions[3].texture = 0;
	g_regions[3].filename = string("other_resources/snow.jpg");


}


void BasicGLPane::UpdateTerrainShaderParameters()
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
    int size = Alti.getxsize();
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

    for (int y = 0; y < Alti.getysize(); ++y)
    {
        for (int x = 0; x < Alti.getxsize(); ++x)
        {
            currVertex = y * size + x;
            pVertex = &pVertices[currVertex];

            pVertex->x = static_cast<float>(x * 0.025);
			pVertex->y = static_cast<float>(y * 0.025);
            pVertex->z = Alti.getaltitude(x, y) * Terrain_height_scale_fac; 
			

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

	m_totalVertices = Alti.getxsize() * Alti.getysize();
    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_totalVertices,0, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Initialize the index buffer object.

    m_totalIndices = (Alti.getxsize() - 1) * (Alti.getysize() * 2 + 1);
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
	int size = Alti.getxsize();

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


void BasicGLPane::CleanupShader()
{
	CleanupStructs();

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


void BasicGLPane::CleanupStructs(){
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

