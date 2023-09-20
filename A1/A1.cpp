// Termm--Fall 2023

#include "A1.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>

#include <sys/types.h>
#include <unistd.h>

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

static const size_t DIM = 16;
int numBlocks = 0;
int up_times = 0;
int down_times = 0;
int left_times = 0;
int right_times = 0;
float lastX = 0.0f;
float releaseX = -2.0f;
float height = 1.0f;
float fov = 30.0f;
float sphere_positionf[3] = {0.5f, 0.5f, 0.5f};
float sphere_positioni[3] = {0, 0, 0};
float rotation = 0.0f;
glm::vec3 lightPos{0.0f, 10.0f, 0.0f};

//----------------------------------------------------------------------------------------
// Constructor
A1::A1()
	: current_col( 0 )
{
	floor_colour[0] = 0.23f;
	floor_colour[1] = 0.38f;
	floor_colour[2] = 0.53f;
	sphere_colour[0] = 1.0f;
	sphere_colour[1] = 1.0f;
	sphere_colour[2] = 1.0f;
	maze_colour[0] = 0.75f;
	maze_colour[1] = 0.75f;
	maze_colour[2] = 0.75f;
	m = new Maze(DIM);
}

//----------------------------------------------------------------------------------------
// Destructor
A1::~A1()
{
	delete m;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A1::init()
{
	// Initialize random number generator
	int rseed=getpid();
	srandom(rseed);
	// Print random number seed in case we want to rerun with
	// same random numbers
	cout << "Random number seed = " << rseed << endl;
	
	// Set the background colour.
	glClearColor( 0.3, 0.5, 0.7, 1.0 );

	// Build the shader
	m_shader.generateProgramObject();
	m_shader.attachVertexShader(
		getAssetFilePath( "VertexShader.vs" ).c_str() );
	m_shader.attachFragmentShader(
		getAssetFilePath( "FragmentShader.fs" ).c_str() );

	m_light_shader.generateProgramObject();
	m_light_shader.attachVertexShader(
		getAssetFilePath( "VertexShaderLight.vs" ).c_str() );
	m_light_shader.attachFragmentShader(
		getAssetFilePath( "FragmentShaderLight.fs" ).c_str() );

	m_shader.link();
	m_light_shader.link();

	// Set up the uniforms
	P_uni = m_shader.getUniformLocation( "P" );
	V_uni = m_shader.getUniformLocation( "V" );
	M_uni = m_shader.getUniformLocation( "M" );
	col_uni = m_shader.getUniformLocation( "colour" );
	light_uni = m_shader.getUniformLocation( "lightPos" );
	light_col_uni = m_shader.getUniformLocation( "lightColor" );
	view_uni = m_shader.getUniformLocation("viewPos");

	P_light_uni = m_light_shader.getUniformLocation( "P" );
	V_light_uni = m_light_shader.getUniformLocation( "V" );
	M_light_uni = m_light_shader.getUniformLocation( "M" );

	initGrid();
	initFloor();
	initSphere();
	initLight();

	// Set up initial view and projection matrices (need to do this here,
	// since it depends on the GLFW window being set up correctly).
	view = glm::lookAt( 
		glm::vec3( 0.0f, 2.*float(DIM)*2.0*M_SQRT1_2, float(DIM)*2.0*M_SQRT1_2 ),
		glm::vec3( 0.0f, 0.0f, 0.0f ),
		glm::vec3( 0.0f, 1.0f, 0.0f ) );

	proj = glm::perspective( 
		glm::radians( 30.0f ),
		float( m_framebufferWidth ) / float( m_framebufferHeight ),
		1.0f, 1000.0f );
}

void A1::initGrid()
{
	size_t sz = 6 * 2 * 2 * (DIM+3);

	float *verts = new float[ sz ];
	size_t ct = 0;
	for( int idx = 0; idx < DIM+3; ++idx ) {
		verts[ ct++ ] = -1;
		verts[ ct++ ] = 0;
		verts[ ct++ ] = idx-1;
		verts[ ct++ ] = 0.0f;
		verts[ ct++ ] = 1.0f;
		verts[ ct++ ] = 0.0f;

		verts[ ct++ ] = DIM+1;
		verts[ ct++ ] = 0;
		verts[ ct++ ] = idx-1;
		verts[ ct++ ] = 0.0f;
		verts[ ct++ ] = 1.0f;
		verts[ ct++ ] = 0.0f;

		verts[ ct++ ] = idx-1;
		verts[ ct++ ] = 0;
		verts[ ct++ ] = -1;
		verts[ ct++ ] = 0.0f;
		verts[ ct++ ] = 1.0f;
		verts[ ct++ ] = 0.0f;

		verts[ ct++ ] = idx-1;
		verts[ ct++ ] = 0;
		verts[ ct++ ] = DIM+1;
		verts[ ct++ ] = 0.0f;
		verts[ ct++ ] = 1.0f;
		verts[ ct++ ] = 0.0f;
	}

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_grid_vao );
	glBindVertexArray( m_grid_vao );

	// Create the cube vertex buffer
	glGenBuffers( 1, &m_grid_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_grid_vbo );
	glBufferData( GL_ARRAY_BUFFER, sz*sizeof(float),
		verts, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

	GLint norAttrib = m_shader.getAttribLocation( "normal" );
	glVertexAttribPointer(norAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(norAttrib);

	// glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0 );

	// Reset state to prevent rogue code from messing with *my* 
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete [] verts;

	CHECK_GL_ERRORS;
}

void A1::initFloor() {
	size_t sz1 = 6 * 4;
	size_t sz2 = 6;
	float *verts = new float[ sz1 ];
	GLuint indices[] = {
		0, 1, 2,
		1, 2, 3
	};
	std::vector<std::pair<int, int>> dirs{{0,0}, {DIM, 0}, {0, DIM}, {DIM, DIM}};

	size_t ct = 0;
	for( auto&[x, z]: dirs ) {
		verts[ ct++ ] = static_cast<float>(x);
		verts[ ct++ ] = 0.0;
		verts[ ct++ ] = static_cast<float>(z);
		verts[ ct++ ] = 0.0f;
		verts[ ct++ ] = 1.0f;
		verts[ ct++ ] = 0.0f;
	}

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_floor_vao );
	glBindVertexArray( m_floor_vao );

	// Create the cube vertex buffer
	glGenBuffers( 1, &m_floor_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_floor_vbo );
	glBufferData( GL_ARRAY_BUFFER, sz1*sizeof(float),
		verts, GL_STATIC_DRAW );

	glGenBuffers( 1, &m_sphere_ebo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_sphere_ebo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sz2 * sizeof(GLuint), indices, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

	GLint norAttrib = m_shader.getAttribLocation( "normal" );
	glVertexAttribPointer(norAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(norAttrib);

	// Reset state to prevent rogue code from messing with *my* 
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete [] verts;

	CHECK_GL_ERRORS;
}

void A1::drawCube(float* verts, int i, int j, size_t& ct) {
	size_t sz = 6 * 6 * 6;
	float vertices[] = {
		0.0f, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f, 
		1.0f, 1.0f, 0.0f,  0.0f,  0.0f, -1.0f, 
		1.0f, 1.0f, 0.0f,  0.0f,  0.0f, -1.0f, 
		0.0f, 1.0f, 0.0f,  0.0f,  0.0f, -1.0f, 
		0.0f, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f, 

		0.0f, 0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		1.0f, 0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		1.0f, 1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		1.0f, 1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		0.0f, 1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		0.0f, 0.0f,  1.0f,  0.0f,  0.0f, 1.0f,

		0.0f, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
		0.0f, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
		0.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
		0.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
		0.0f, 0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
		0.0f, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f,

		1.0f, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
		1.0f, 1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
		1.0f, 0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
		1.0f, 0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
		1.0f, 0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
		1.0f, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,

		0.0f, 0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
		1.0f, 0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
		1.0f, 0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
		1.0f, 0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
		0.0f, 0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
		0.0f, 0.0f, 0.0f,  0.0f, -1.0f,  0.0f,

		0.0f, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
		1.0f, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
		1.0f, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
		1.0f, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
		0.0f, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
		0.0f, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f
	};

	for (int k = 0; k < sz; k+=6) {
		verts[ ct++ ] = vertices[k] + j;
		verts[ ct++ ] = vertices[k+1];
		verts[ ct++ ] = vertices[k+2] + i;
		verts[ ct++ ] = vertices[k+3];
		verts[ ct++ ] = vertices[k+4];
		verts[ ct++ ] = vertices[k+5];
	}
}

void A1::initMaze()
{	
	numBlocks = m->getBlockNumber();
	size_t sz = 6 * 6 * 6 * numBlocks;
	float *verts = new float[ sz ];
	size_t ct = 0;

	for (size_t i = 0; i < DIM; i++) {
		for (size_t j = 0; j < DIM; j++) {
			if (m->getValue(i, j)) {
				drawCube(verts, i, j, ct);
			}
		}
	}

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_maze_vao );
	glBindVertexArray( m_maze_vao );

	// Create the cube vertex buffer
	glGenBuffers( 1, &m_maze_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_maze_vbo );
	glBufferData( GL_ARRAY_BUFFER, sz * sizeof(float), verts, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

	GLint norAttrib = m_shader.getAttribLocation( "normal" );
	glVertexAttribPointer(norAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(norAttrib);

	// Reset state to prevent rogue code from messing with *my* 
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete [] verts;

	CHECK_GL_ERRORS;
}

void A1::initSphere() {
	// clear memory of prev arrays
	const int sectorCount = 30;
	const int stackCount = 30;
	size_t sz1 = (stackCount + 1) * (sectorCount + 1) * 6;
	size_t sz2 = (stackCount - 1) * sectorCount * 6;
	size_t idx = 0;
	size_t ct = 0;
	float* verts = new float[sz1];
	GLuint* indices = new GLuint[sz2];

	const float radius = 0.4f;
	const float PI = 3.14159265359f;

	float x, y, z, xy;                              // vertex position
	float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal

	float sectorStep = 2 * PI / sectorCount;
	float stackStep = PI / stackCount;
	float sectorAngle, stackAngle;

	for(int i = 0; i <= stackCount; ++i)
	{
		stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		xy = radius * cosf(stackAngle);             // r * cos(u)
		z = radius * sinf(stackAngle);              // r * sin(u)

		// add (sectorCount+1) vertices per stack
		// first and last vertices have same position and normal, but different tex coords
		for(int j = 0; j <= sectorCount; ++j)
		{
			sectorAngle = j * sectorStep;           // starting from 0 to 2pi

			// vertex position (x, y, z)
			x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
			y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
			verts[ct++] = x;
			verts[ct++] = y;
			verts[ct++] = z;

			// normalized vertex normal (nx, ny, nz)
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;
			verts[ct++] = nx;
			verts[ct++] = ny;
			verts[ct++] = nz;
		}
	}

	int k1, k2;
	for(int i = 0; i < stackCount; ++i)
	{
		k1 = i * (sectorCount + 1);     // beginning of current stack
		k2 = k1 + sectorCount + 1;      // beginning of next stack

		for(int j = 0; j < sectorCount; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding first and last stacks
			// k1 => k2 => k1+1
			if(i != 0)
			{
				indices[idx++] = k1;
				indices[idx++] = k2;
				indices[idx++] = k1 + 1;
			}

			// k1+1 => k2 => k2+1
			if(i != (stackCount-1))
			{
				indices[idx++] = k1 + 1;
				indices[idx++] = k2;
				indices[idx++] = k2 + 1;
			}
		}
	}
	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_sphere_vao );
	glBindVertexArray( m_sphere_vao );

	// Create the cube vertex buffer
	glGenBuffers( 1, &m_sphere_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_sphere_vbo );
	glBufferData( GL_ARRAY_BUFFER, sz1 * sizeof(float), verts, GL_STATIC_DRAW );

	glGenBuffers( 1, &m_sphere_ebo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_sphere_ebo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sz2 * sizeof(GLuint), indices, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

	GLint norAttrib = m_shader.getAttribLocation( "normal" );
	glVertexAttribPointer(norAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(norAttrib);

	// Reset state to prevent rogue code from messing with *my* 
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete [] verts;
	delete [] indices;

	CHECK_GL_ERRORS;
}

void A1::initLight() {
	size_t sz = 6 * 6 * 6;
	size_t ct = 0;
	float *verts = new float[ sz ];

	drawCube(verts, 0, 0, ct);

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_light_vao );
	glBindVertexArray( m_light_vao );

	// Create the cube vertex buffer
	glGenBuffers( 1, &m_light_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_light_vbo );
	glBufferData( GL_ARRAY_BUFFER, sz * sizeof(float), verts, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_light_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

	// Reset state to prevent rogue code from messing with *my* 
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete [] verts;

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A1::appLogic()
{
	if (left_times > 0) {
		left_times--;
		sphere_positionf[0] -= 0.025f;
	}
	if (right_times > 0) {
		right_times--;
		sphere_positionf[0] += 0.025f;
	}
	if (up_times > 0) {
		up_times--;
		sphere_positionf[2] -= 0.025f;
	}
	if (down_times > 0) {
		down_times--;
		sphere_positionf[2] += 0.025f;
	}
	int x = static_cast<int>(sphere_positionf[2]);
	int y = static_cast<int>(sphere_positionf[0]);
	if (m->getValue(x, y) == 1) {
		m->setValue(x, y, 0);
		initMaze();
	}
	// Place per frame, application logic here ...
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A1::guiLogic()
{
	// We already know there's only going to be one window, so for 
	// simplicity we'll store button states in static local variables.
	// If there was ever a possibility of having multiple instances of
	// A1 running simultaneously, this would break; you'd want to make
	// this into instance fields of A1.
	static bool showTestWindow(false);
	static bool showDebugWindow(true);

	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Debug Window", &showDebugWindow, ImVec2(100,100), opacity, windowFlags);
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}
		if( ImGui::Button( "Reset" ) ) {
			reset();
		}
		ImGui::SameLine();
		if( ImGui::Button( "Dig" ) ) {
			dig();
		}
		// Eventually you'll create multiple colour widgets with
		// radio buttons.  If you use PushID/PopID to give them all
		// unique IDs, then ImGui will be able to keep them separate.
		// This is unnecessary with a single colour selector and
		// radio button, but I'm leaving it in as an example.

		// Prefixing a widget name with "##" keeps it from being
		// displayed.

		ImGui::PushID( 0 );
		float colour[3];
		ImGui::RadioButton( "Floor", &current_col, 0 );
		ImGui::SameLine();
		ImGui::RadioButton( "Maze", &current_col, 1 );
		ImGui::SameLine();
		ImGui::RadioButton( "Sphere", &current_col, 2 );
		if (current_col == 0) {
			ImGui::ColorEdit3( "##FloorColour", floor_colour );
		} else if (current_col == 1) {
			ImGui::ColorEdit3( "##MazeColour", maze_colour );
		} else {
			ImGui::ColorEdit3( "##SphereColour", sphere_colour );
		}
		ImGui::PopID();


		// For convenience, you can uncomment this to show ImGui's massive
		// demonstration window right in your application.  Very handy for
		// browsing around to get the widget you want.  Then look in 
		// shared/imgui/imgui_demo.cpp to see how it's done.
		if( ImGui::Button( "Test Window" ) ) {
			showTestWindow = !showTestWindow;
		}

		ImGui::Text( "X: %.1f, Y: %.1f, Z: %.1f", sphere_positioni[0], sphere_positioni[1], sphere_positioni[2] );
		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();

	if( showTestWindow ) {
		ImGui::ShowTestWindow( &showTestWindow );
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A1::draw()
{
	// Create a global transformation for the model (centre it).
	mat4 W, Ws, Wc, Wl;
	W = glm::translate( W, vec3( -float(DIM)/2.0f, 0, -float(DIM)/2.0f ) );

	m_shader.enable();
		glEnable( GL_DEPTH_TEST );
		
		rotation *= 0.9995;

		view = glm::rotate(view, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));

		glm::vec3 cameraPosition(glm::inverse(view)[3]);
		// glm::vec3 cameraPosition = {0,0,0};
	
		glUniformMatrix4fv( P_uni, 1, GL_FALSE, value_ptr( proj ) );
		glUniformMatrix4fv( V_uni, 1, GL_FALSE, value_ptr( view ) );
		glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( W ) );
		glUniform3fv( light_uni, 1, value_ptr(lightPos));
		glUniform3fv( view_uni, 1, value_ptr(cameraPosition) );
		glUniform3f( light_col_uni, 1.0f, 1.0f, 1.0f );

		// Just draw the grid for now.
		glBindVertexArray( m_grid_vao );
		glUniform3f( col_uni, 1, 1, 1 );
		glDrawArrays( GL_LINES, 0, (3+DIM)*4 );

		// Draw the floor
		glBindVertexArray( m_floor_vao );
		glUniform3f( col_uni, floor_colour[0], floor_colour[1], floor_colour[2] );
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// Draw the sphere
		glBindVertexArray( m_sphere_vao );
		Ws = glm::translate( W, vec3( sphere_positionf[0], sphere_positionf[1], sphere_positionf[2]) );
		glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( Ws ) );
		glUniform3f( col_uni, sphere_colour[0], sphere_colour[1], sphere_colour[2] );
		glDrawElements(GL_TRIANGLES, 29 * 30 * 6, GL_UNSIGNED_INT, 0);

		// Draw the cubes
		if (numBlocks > 0) {
			glBindVertexArray( m_maze_vao );
			Wc = glm::scale( W, vec3( 1.0f, height ,1.0f) );
			glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( Wc ) );
			glUniform3f( col_uni, maze_colour[0], maze_colour[1], maze_colour[2] );
			glDrawArrays( GL_TRIANGLES, 0, 36 * numBlocks );
		}
		// Highlight the active square.
	m_shader.disable();

	m_light_shader.enable();
		glUniformMatrix4fv( P_light_uni, 1, GL_FALSE, value_ptr( proj ) );
		glUniformMatrix4fv( V_light_uni, 1, GL_FALSE, value_ptr( view ) );

        Wl = glm::translate(Wl, lightPos);
		Wl = glm::translate(Wl, glm::vec3(-0.5f, -0.5f, -0.5f));
        Wl = glm::scale(Wl, glm::vec3(0.2f)); // a smaller cube

		glUniformMatrix4fv( M_light_uni, 1, GL_FALSE, value_ptr( Wl ) );

		// Draw the light
		glBindVertexArray( m_light_vao );
		glDrawArrays( GL_TRIANGLES, 0, 36 );
	m_light_shader.disable();

	// Restore defaults
	glBindVertexArray( 0 );

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A1::cleanup()
{}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A1::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A1::mouseMoveEvent(double xPos, double yPos) 
{
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		// Put some code here to handle rotations.  Probably need to
		// check whether we're *dragging*, not just moving the mouse.
		// Probably need some instance variables to track the current
		// rotation amount, and maybe the previous X position (so 
		// that you can rotate relative to the *change* in X.
		if (ImGui::IsMouseDragging()) {
			rotation = (xPos - lastX) * 0.05;
		}
		if (releaseX != -2.0f) {
			rotation = (xPos - releaseX) * 0.05;
			releaseX = -2.0f;
		}
		lastX = xPos;
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A1::mouseButtonInputEvent(int button, int actions, int mods) {
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		// The user clicked in the window.  If it's the left
		// mouse button, initiate a rotation.
		if (button == GLFW_MOUSE_BUTTON_LEFT && actions == GLFW_RELEASE) {
			releaseX = lastX;
		} else {
			rotation = 0.0f;
		}
		
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A1::mouseScrollEvent(double xOffSet, double yOffSet) {

	// Zoom in or out.

	bool eventHandled(false);
	fov -= (float)yOffSet;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f; 
	proj = glm::perspective(glm::radians(fov), float( m_framebufferWidth ) / float( m_framebufferHeight ), 1.0f, 1000.0f);
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A1::windowResizeEvent(int width, int height) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A1::keyInputEvent(int key, int action, int mods) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if( action == GLFW_PRESS ) {
		// Respond to some key events.
		if (key == GLFW_KEY_Q) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}
		if (key == GLFW_KEY_R) {
			reset();
		}
		if (key == GLFW_KEY_D) {
			dig();
		}
		if (key == GLFW_KEY_SPACE) {
			height = std::min(9.0f, height + 1.0f);
		}
		if (key == GLFW_KEY_BACKSPACE) {
			height = std::max(1.0f, height - 1.0f);
		}
		if (key == GLFW_KEY_LEFT && sphere_positioni[0] > 0) {
			if (m->getValue(sphere_positioni[2], sphere_positioni[0] - 1) == 1) {
				if ((mods & GLFW_MOD_SHIFT) == GLFW_MOD_SHIFT) {
					left_times += 40;
					sphere_positioni[0] -= 1;
				}
			} else {
				sphere_positioni[0] -= 1;
				left_times += 40;
			}
		}
		if (key == GLFW_KEY_RIGHT && sphere_positioni[0] < DIM - 1) {
			if (m->getValue(sphere_positioni[2], sphere_positioni[0] + 1) == 1) {
				if ((mods & GLFW_MOD_SHIFT) == GLFW_MOD_SHIFT) {
					right_times += 40;
					sphere_positioni[0] += 1;
				}
			} else {
				sphere_positioni[0] += 1;
				right_times += 40;
			}
		}
		if (key == GLFW_KEY_UP && sphere_positioni[2] > 0) {
			if (m->getValue(sphere_positioni[2] - 1, sphere_positioni[0]) == 1) {
				if ((mods & GLFW_MOD_SHIFT) == GLFW_MOD_SHIFT) {
					up_times += 40;
					sphere_positioni[2] -= 1;
				}		
			} else {
				sphere_positioni[2] -= 1;
				up_times += 40;
			}
		}
		if (key == GLFW_KEY_DOWN && sphere_positioni[2] < DIM - 1) {
			if (m->getValue(sphere_positioni[2] + 1, sphere_positioni[0]) == 1) {
				if ((mods & GLFW_MOD_SHIFT) == GLFW_MOD_SHIFT) {
					down_times += 40;
					sphere_positioni[2] += 1;
				}
			} else {
				sphere_positioni[2] += 1;
				down_times += 40;
			}
		}
	}
		
	return eventHandled;
}

void A1::reset() {
	numBlocks = 0;
	m->reset();
	floor_colour[0] = 0.23f;
	floor_colour[1] = 0.38f;
	floor_colour[2] = 0.53f;
	sphere_colour[0] = 1.0f;
	sphere_colour[1] = 1.0f;
	sphere_colour[2] = 1.0f;
	maze_colour[0] = 0.75f;
	maze_colour[1] = 0.75f;
	maze_colour[2] = 0.75f;
	sphere_positionf[0] = 0.5f;
	sphere_positionf[1] = 0.5f;
	sphere_positionf[2] = 0.5f;
	sphere_positioni[0] = 0;
	sphere_positioni[1] = 0;
	sphere_positioni[2] = 0;
	lastX = 0.0f;
	releaseX = -2.0f;
	rotation = 0.0f;
	left_times = 0;
	right_times = 0;
	up_times = 0;
	down_times = 0;

	view = glm::lookAt( 
	glm::vec3( 0.0f, 2.*float(DIM)*2.0*M_SQRT1_2, float(DIM)*2.0*M_SQRT1_2 ),
	glm::vec3( 0.0f, 0.0f, 0.0f ),
	glm::vec3( 0.0f, 1.0f, 0.0f ) );

	proj = glm::perspective( 
	glm::radians( 30.0f ),
	float( m_framebufferWidth ) / float( m_framebufferHeight ),
	1.0f, 1000.0f );
}

void A1::dig() {
	m->digMaze();
	m->printMaze();
	initMaze();
	sphere_positioni[0] = m->getStartCell();
	sphere_positioni[1] = 0;
	sphere_positioni[2] = 0;
	sphere_positionf[0] = static_cast<float>(sphere_positioni[0]) + 0.5f;
	sphere_positionf[1] = 0.5f;
	sphere_positionf[2] = 0.5f;
}
