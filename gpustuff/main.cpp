#define WIN32_LEAN_AND_MEAN

#define STRINGIFY(s) #s

#include <GL/glew.h>
#include <GL/wglew.h>
#include <GL/gl.h>
#include "../glm-0.9.8.2/glm/glm.hpp"
#include "../glm-0.9.8.2/glm/gtx/transform.hpp"
#include "../glm-0.9.8.2/glm/gtc/type_ptr.hpp"
#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <iomanip>

using namespace glm;

const static PIXELFORMATDESCRIPTOR pfd =
{
	sizeof( PIXELFORMATDESCRIPTOR ),
	1,
	PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    
	PFD_TYPE_RGBA,           
	32,                      
	0, 0, 0, 0, 0, 0,
	0,
	0,
	0,
	0, 0, 0, 0,
	24,                        
	8,                        
	0,                        
	PFD_MAIN_PLANE,
	0,
	0, 0, 0
};

GLuint loadComputeProgram( std::string computeName )
{
	std::ifstream computeIfstream( "shaders/" + computeName );
	std::string computeStr( ( std::istreambuf_iterator<char>( computeIfstream ) ), 
		( std::istreambuf_iterator<char>() ) );

	char const * ComputeSourcePointer = computeStr.c_str();

	GLint Result = GL_FALSE;
	int InfoLogLength;

	GLuint computeShader = glCreateShader( GL_COMPUTE_SHADER );
	glShaderSource( computeShader, 1, &ComputeSourcePointer, NULL );
	glCompileShader( computeShader );

	glGetShaderiv( computeShader, GL_COMPILE_STATUS, &Result );
	glGetShaderiv( computeShader, GL_INFO_LOG_LENGTH, &InfoLogLength );
	if ( InfoLogLength > 0 ) {
		std::vector<char> ComputeShaderErrorMessage( InfoLogLength + 1 );
		glGetShaderInfoLog( computeShader, InfoLogLength, NULL, &ComputeShaderErrorMessage[ 0 ] );
		std::cout << &ComputeShaderErrorMessage[ 0 ] << std::endl;
	}

	GLuint computeProgram = glCreateProgram();
	glAttachShader( computeProgram, computeShader );
	glLinkProgram( computeProgram );

	glGetProgramiv( computeProgram, GL_LINK_STATUS, &Result );
	glGetProgramiv( computeProgram, GL_INFO_LOG_LENGTH, &InfoLogLength );
	if ( InfoLogLength > 0 ) {
		std::vector<char> ProgramErrorMessage( InfoLogLength + 1 );
		glGetProgramInfoLog( computeProgram, InfoLogLength, NULL, &ProgramErrorMessage[ 0 ] );
		std::cout << &ProgramErrorMessage[ 0 ] << std::endl;
	}

	return computeProgram;
}

GLuint loadPassThruProgram( std::string vertName, std::string fragName )
{
	std::ifstream vert( "shaders/" + vertName );
	std::ifstream frag( "shaders/" + fragName );

	std::string vertStr( ( std::istreambuf_iterator<char>( vert ) ),
		( std::istreambuf_iterator<char>() ) );
	std::string fragStr( ( std::istreambuf_iterator<char>( frag ) ),
		( std::istreambuf_iterator<char>() ) );

	char const * VertexSourcePointer = vertStr.c_str();
	char const * FragmentSourcePointer = fragStr.c_str();

	GLint Result = GL_FALSE;
	int InfoLogLength;

	GLuint VertexShaderID = glCreateShader( GL_VERTEX_SHADER );
	glShaderSource( VertexShaderID, 1, &VertexSourcePointer, NULL );
	glCompileShader( VertexShaderID );

	// Check Vertex Shader
	glGetShaderiv( VertexShaderID, GL_COMPILE_STATUS, &Result );
	glGetShaderiv( VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength );
	if ( InfoLogLength > 0 ) {
		std::vector<char> VertexShaderErrorMessage( InfoLogLength + 1 );
		glGetShaderInfoLog( VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[ 0 ] );
		std::cout << &VertexShaderErrorMessage[ 0 ] << std::endl;
	}

	GLuint FragmentShaderID = glCreateShader( GL_FRAGMENT_SHADER );
	// Compile Fragment Shader
	glShaderSource( FragmentShaderID, 1, &FragmentSourcePointer, NULL );
	glCompileShader( FragmentShaderID );

	// Check Fragment Shader
	glGetShaderiv( FragmentShaderID, GL_COMPILE_STATUS, &Result );
	glGetShaderiv( FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength );
	if ( InfoLogLength > 0 ) {
		std::vector<char> FragmentShaderErrorMessage( InfoLogLength + 1 );
		glGetShaderInfoLog( FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[ 0 ] );
		std::cout << &FragmentShaderErrorMessage[ 0 ] << std::endl;
	}

	GLuint ProgramID = glCreateProgram();
	glAttachShader( ProgramID, VertexShaderID );
	glAttachShader( ProgramID, FragmentShaderID );
	glLinkProgram( ProgramID );

	// Check the program
	glGetProgramiv( ProgramID, GL_LINK_STATUS, &Result );
	glGetProgramiv( ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength );
	if ( InfoLogLength > 0 ) {
		std::vector<char> ProgramErrorMessage( InfoLogLength + 1 );
		glGetProgramInfoLog( ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[ 0 ] );
		std::cout << &ProgramErrorMessage[ 0 ] << std::endl;
	}

	glDetachShader( ProgramID, VertexShaderID );
	glDetachShader( ProgramID, FragmentShaderID );

	glDeleteShader( VertexShaderID );
	glDeleteShader( FragmentShaderID );

	return ProgramID;
}

int main( void )
{
	ShowCursor( 0 );
#ifdef _DEBUG	
	HWND hWnd = CreateWindowExA( WS_EX_APPWINDOW, "static", 0, WS_BORDER | WS_VISIBLE | WS_POPUP , CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, 0, 0, 0, 0 );
	HWND consoleWindow = GetConsoleWindow();
	SetWindowPos( consoleWindow, 0, 0, 720, 1280, 300, SWP_NOZORDER );
#else
	/* FULLSCREEN */
	
	HWND hWnd = CreateWindowExA( WS_EX_APPWINDOW, "static", 0, WS_VISIBLE | WS_POPUP | WS_MAXIMIZE, 0, 0, 0, 0, 0, 0, 0, 0 );
	HWND consoleWindow = GetConsoleWindow();
	SetWindowPos( consoleWindow, 0, -1280, 300, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
#endif	
	SetFocus( hWnd );
	HDC hDC = GetDC( hWnd );
	SetPixelFormat( hDC, ChoosePixelFormat( hDC, &pfd ), &pfd );
	HGLRC hGLRC = wglCreateContext( hDC );
	wglMakeCurrent( hDC, hGLRC );

	RECT wRect;
	GetWindowRect( hWnd, &wRect );
	int width = abs(wRect.right - wRect.left);
	int height = abs(wRect.top - wRect.bottom);

	glewInit();
	wglSwapIntervalEXT( 0 ); //disable vsync
	glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f,-1.0f, 0.0f,
		1.0f,-1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
	};

	GLuint vertexbuffer;
	glGenBuffers( 1, &vertexbuffer );
	glBindBuffer( GL_ARRAY_BUFFER, vertexbuffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof( g_vertex_buffer_data ), g_vertex_buffer_data, GL_STATIC_DRAW );
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE, 0,( void* )0);

	GLuint tex;
	glGenTextures( 1, &tex );
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, tex );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA32F, width,height, 0, GL_RGBA, GL_FLOAT,
		NULL );
	glBindImageTexture( 0, tex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F );

	GLuint computeProgram = loadComputeProgram( "compute.glsl" );
	GLuint texDrawProgram = loadPassThruProgram( "passThru.vert", "texPassThru.frag" );

	GLint CLoc = glGetUniformLocation( computeProgram, "C" );
	GLint timeLoc = glGetUniformLocation( computeProgram, "time" );
	GLint frameLoc = glGetUniformLocation( computeProgram, "frame" );
	GLint sampleLoc = glGetUniformLocation( computeProgram, "samplesPerFrame" );

	mat4 C = mat4( 1.0f );
	C[ 0 ] = vec4( normalize( cross( vec3( C[ 2 ] ), vec3( 0.0f, 1.0f, 0.0f ) ) ), 0.0f );
	C[ 3 ] = vec4( 0.0f, 2.0f, -4.0f, 1.0f );

	/****************************** START TIMER *******************************/
	LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
	LARGE_INTEGER Frequency;
	GLfloat time = 0.0f;
	GLfloat prevTime = 0.0f;
	GLfloat frameTime = 0.0f;
	unsigned int fps = 0;
	unsigned int sec = 0;
	unsigned int frame = 1;
	QueryPerformanceFrequency( &Frequency );
	QueryPerformanceCounter( &StartingTime );
	/******/
	int samplesPerFrame = 4;

	for (;;)
	{
		static MSG dummyMessage;
		PeekMessageA( &dummyMessage, 0, 0, 0, 1 );
		

		if ( GetAsyncKeyState( VK_ESCAPE ) )
			break;

		if ( GetFocus() == hWnd ) {
			static bool first = true;
			if ( GetAsyncKeyState( VK_LBUTTON ) && !first) {
				
				POINT mPos;
				GetCursorPos( &mPos );
				ScreenToClient( hWnd, &mPos );
				int dx = mPos.x - ( width / 2 );
				int dy = mPos.y - ( height / 2 );
				if ( dx != 0 || dy != 0 )
					frame = 1u;
				POINT center;
				center.x = width / 2;
				center.y = height / 2;
				ClientToScreen( hWnd, &center );
				SetCursorPos( center.x, center.y );

				C[ 2 ] = C[ 2 ] * rotate( ( float )dx*frameTime*0.2f, vec3( 0.0f, 1.0f, 0.0f ) )*rotate( ( float )dy*frameTime*0.2f, vec3( C[ 0 ] ) );
				vec4 old = C[ 0 ];
				C[ 0 ] = vec4( normalize( cross( vec3( C[ 2 ] ), vec3( 0.0f, 1.0f, 0.0f ) ) ), 0.0f );
				if ( dot( old, C[ 0 ] ) < 0.0f ) C[ 0 ] *= -1.0f;
				C[ 1 ] = vec4( normalize( cross( vec3( C[ 0 ] ), vec3( C[ 2 ] ) ) ), 0.0f );	
			}else if ( GetAsyncKeyState( VK_LBUTTON ) ) { //PREVENT SNAPPING ON THE FIRST CAMERA MOVEMENT
				POINT center;
				center.x = width / 2;
				center.y = height / 2;
				ClientToScreen( hWnd, &center );
				SetCursorPos( center.x, center.y );
				first = false;
			}

			if ( GetAsyncKeyState( 0x57 ) ) { //W
				C[ 3 ] += C[ 2 ] * frameTime* 2.0f;
				frame = 1u;
			}
			if ( GetAsyncKeyState( 0x41 ) ) { //A
				C[ 3 ] -= C[ 0 ] * frameTime* 2.0f;
				frame = 1u;
			}
			if ( GetAsyncKeyState( 0x53 ) ) { //S
				C[ 3 ] -= C[ 2 ] * frameTime* 2.0f;
				frame = 1u;
			}
			if ( GetAsyncKeyState( 0x44 ) ) { //D
				C[ 3 ] += C[ 0 ] * frameTime * 2.0f;
				frame = 1u;
			}
			if ( GetAsyncKeyState( 0x51 ) ) { //Q
				C[ 3 ] -= vec4( 0.0f, 1.0f, 0.0f, 0.0f ) * frameTime* 2.0f;
				frame = 1u;
			}
			if ( GetAsyncKeyState( 0x45 ) ) { //E
				C[ 3 ] += vec4( 0.0f, 1.0f, 0.0f, 0.0f ) * frameTime* 2.0f;
				frame = 1u;
			}
			if ( GetAsyncKeyState( 0x31 ) ) { //1
				samplesPerFrame = 1;
			}
			if ( GetAsyncKeyState( 0x32 ) ) { //2
				samplesPerFrame = 2;
			}
			if ( GetAsyncKeyState( 0x33 ) ) { //3
				samplesPerFrame = 4;
			}
			if ( GetAsyncKeyState( 0x34 ) ) { //4
				samplesPerFrame = 8;
			}
			if ( GetAsyncKeyState( 0x35 ) ) { //5
				samplesPerFrame = 16;
			}
			if ( GetAsyncKeyState( 0x36 ) ) { //6
				samplesPerFrame = 32;
			}
			if ( GetAsyncKeyState( 0x37 ) ) { //7
				samplesPerFrame = 64;
			}
			if ( GetAsyncKeyState( 0x38 ) ) { //8
				samplesPerFrame = 128;
			}
		}
		//run compute shader
		glUseProgram( computeProgram );
		glUniformMatrix4fv( CLoc, 1, GL_FALSE, glm::value_ptr(C) );
		glUniform1f( timeLoc, time );
		glUniform1ui( frameLoc, frame );
		glUniform1i( sampleLoc, samplesPerFrame );
		glDispatchCompute( ( GLuint )((width-1)/16 + 1), ( GLuint )((height-1)/16 + 1), 1 ); 

		//wait for compute to finish
		glMemoryBarrier( GL_ALL_BARRIER_BITS );

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
		glUseProgram( texDrawProgram ); 
		glBindBuffer( GL_ARRAY_BUFFER, vertexbuffer );
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, tex );
		glEnableVertexAttribArray( 0 );
		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 ); 
		glDisableVertexAttribArray( 0 );
		
		/*********************************** TIMER *************************************/
		QueryPerformanceCounter( &EndingTime );
		ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;
		ElapsedMicroseconds.QuadPart *= 1000000;
		ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;
		time = ( float )ElapsedMicroseconds.QuadPart / 1000000.0f;
		/*********************************** TIMER *************************************/

		/*********************************** FPS ***************************************/
		frameTime = time - prevTime;
		prevTime = time;
		fps = ( unsigned )( 1.0f / frameTime );
		std::cout << "\r" << "Frames: " << frame << " | Samples/Frame: " << samplesPerFrame << " | FPS: " << fps << " | Frametime: " << frameTime << "      ";
		
		/*********************************** FPS ****************************************/
		frame++;

		SwapBuffers( hDC );
	}

	return EXIT_SUCCESS;
}