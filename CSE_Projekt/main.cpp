/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/

//Using SDL, SDL OpenGL, GLEW, standard IO, and strings

#include <SDL.h>
#include "GL/glew.h"
#include <SDL_opengl.h>
#include "GL/glu.h"
#include <stdio.h>
#include "MyVAO.h"
#include "Mesh.h"
#include "Texture.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "vertexdata.h"
#include <map>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int lod = LOW;

//Starts up SDL, creates window, and initializes OpenGL
bool init();

//Initializes rendering program and clear color
bool initGL();

//Per frame update
void update();

//Renders quad to the screen
void render();

//Frees media and shuts down SDL
void close();

std::vector<MyVAO> vaos;
std::vector<Texture> textures;
std::vector<Mesh> meshes;
std::vector<Shader> shaders;

//Shader loading utility programs
void printProgramLog(GLuint program);
void printShaderLog(GLuint shader);

//The window we'll be rendering to
SDL_Window* gWindow = nullptr;

//OpenGL context
SDL_GLContext gContext;

//Render flag
bool gRenderQuad = true;

GLint gVertexPos2DLocation = -1;
GLuint gVBO = 0;
GLuint gIBO = 0;

glm::mat4 transformation;

glm::vec3 cameraRight;
glm::vec3 cameraUp;
glm::vec3 cameraFront;
glm::vec3 cameraPos;

GLfloat yaw = -90.0f; // Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
GLfloat pitch = 0.0f;

int lastX = SCREEN_WIDTH / 2;
int lastY = SCREEN_HEIGHT / 2;

glm::mat4 view;
glm::mat4 projection;


bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Use OpenGL 3.1 core
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
		if (gWindow == nullptr)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create context
			gContext = SDL_GL_CreateContext(gWindow);
			if (gContext == nullptr)
			{
				printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize GLEW
				glewExperimental = GL_TRUE;
				GLenum glewError = glewInit();
				if (glewError != GLEW_OK)
				{
					printf("Error initializing GLEW! %p\n", glewGetErrorString(glewError));
				}

				//Use Vsync
				if (SDL_GL_SetSwapInterval(1) < 0)
				{
					printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
				}

				//Initialize OpenGL
				if (!initGL())
				{
					printf("Unable to initialize OpenGL!\n");
					success = false;
				}
			}
		}
	}


	textures.push_back(Texture("brick.dds"));
	textures.push_back(Texture("test.dds"));
	shaders.push_back(Shader("VertexShader.glsl", "FragmentShader.glsl"));

	//Data container in vertexdata.h
	VertexData v;
	vaos.push_back(MyVAO(shaders[0], &v.triangle[0], v.plane.size()));
	vaos.push_back(MyVAO(shaders[0], &v.plane[0], v.plane.size()));
	vaos.push_back(MyVAO(shaders[0], &v.cube[0], v.cube.size()));

	Mesh mesh(vaos[0], vaos[1], vaos[2]);
	mesh.setTexture(textures[0]);
	mesh.setAlpha(0.5f);
	mesh.Translate(0.5, 0.5, 0.5);
	meshes.push_back(mesh);

	Mesh mesh2(vaos[0], vaos[1], vaos[2]);
	mesh2.setTexture(textures[1]);
	mesh2.setAlpha(1.0f);
	mesh2.Translate(-0.5, -0.5, -0.5);
	meshes.push_back(mesh2);

	Mesh mesh3(vaos[0], vaos[1], vaos[2]);
	mesh3.setTexture(textures[1]);
	mesh3.setAlpha(0.5);
	mesh3.Translate(1.5f, 1.5f, 1.5f);
	meshes.push_back(mesh3);

	Mesh light(vaos[2], vaos[2], vaos[2]);
	light.setAlpha(1.0f);
	light.Translate(-1.f, 3.f, -2.f);
	light.Scale(0.2f, 0.2f, 0.2f);
	meshes.push_back(light);
	shaders[0].Use();

	GLint lightColorLoc = glGetUniformLocation(shaders[0].m_program, "lightColor");
	glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);

	GLint lightPosLoc = glGetUniformLocation(shaders[0].m_program, "lightPos");
	glUniform3f(lightPosLoc, light.getPosition().x, light.getPosition().y, light.getPosition().z);

	return success;
}

bool initGL()
{
	//Success flag
	bool success = true;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Initialize clear color
	glClearColor(0.1f, 0.1f, 0.1f, 1.f);

	cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	cameraUp = glm::cross(cameraDirection, cameraRight);
	cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);

	projection = glm::perspective(45.0f, GLfloat(SCREEN_WIDTH) / GLfloat(SCREEN_HEIGHT), 0.1f, 100.0f);
	return success;
}

bool firstMouse = true;

void mouse_callback(SDL_Window* gWindow, int xpos, int ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	GLfloat sensitivity = 0.5f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
}


int keys[1024];

void handleInput(SDL_Event* event, int x, int y)
{
	mouse_callback(gWindow, x, y);
	switch (event->type)
	{
	case SDL_KEYDOWN:
		keys[event->key.keysym.scancode] = true;
		break;
	case SDL_KEYUP:
		keys[event->key.keysym.scancode] = false;
		break;
	default: break;
	}
}


void moveCamera()
{
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);

	// Camera controls
	GLfloat cameraSpeed = 0.01f;
	if (keys[SDL_SCANCODE_W])
		cameraPos += cameraSpeed * cameraFront;
	if (keys[SDL_SCANCODE_S])
		cameraPos -= cameraSpeed * cameraFront;
	if (keys[SDL_SCANCODE_A])
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keys[SDL_SCANCODE_D])
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void moveCube()
{
	if (keys[SDL_SCANCODE_RIGHT])
		meshes[0].Rotate(2.f, z);
	if (keys[SDL_SCANCODE_LEFT])
		meshes[0].Rotate(-2.f, z);
	if (keys[SDL_SCANCODE_UP])
		meshes[0].Rotate(-2.f, x);
	if (keys[SDL_SCANCODE_DOWN])
		meshes[0].Rotate(-2.f, x);
};

void update()
{
	moveCamera();
	moveCube();
	for (size_t i = 0; i < meshes.size(); i++)
	{
		meshes[i].calculateLOD(cameraPos);
	}
}

void render()
{
	//Clear color buffer
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	// Sort meshes
	std::map<GLfloat, Mesh> sorted;
	for (GLuint i = 0; i < meshes.size(); i++)
	{
		GLfloat distance = glm::distance(cameraPos, glm::vec3(meshes[i].getPosition()));
		sorted[distance] = meshes[i];
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	// Get their uniform location
	GLint viewLoc = glGetUniformLocation(shaders[0].m_program, "view");
	GLint projLoc = glGetUniformLocation(shaders[0].m_program, "projection");
	// Pass them to the shaders
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	// Note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	for (std::map<float, Mesh>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
	{
		it->second.draw();
	}
}

void close()
{
	//Destroy window	
	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;

	//Quit SDL subsystems
	SDL_Quit();
}

void printProgramLog(GLuint program)
{
	//Make sure name is shader
	if (glIsProgram(program))
	{
		//Program log length
		int infoLogLength = 0;
		int maxLength = infoLogLength;

		//Get info string length
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		//Allocate string
		char* infoLog = new char[ maxLength ];

		//Get info log
		glGetProgramInfoLog(program, maxLength, &infoLogLength, infoLog);
		if (infoLogLength > 0)
		{
			//Print Log
			printf("%s\n", infoLog);
		}

		//Deallocate string
		delete[] infoLog;
	}
	else
	{
		printf("Name %d is not a program\n", program);
	}
}

void printShaderLog(GLuint shader)
{
	//Make sure name is shader
	if (glIsShader(shader))
	{
		//Shader log length
		int infoLogLength = 0;
		int maxLength = infoLogLength;

		//Get info string length
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		//Allocate string
		char* infoLog = new char[ maxLength ];

		//Get info log
		glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);
		if (infoLogLength > 0)
		{
			//Print Log
			printf("%s\n", infoLog);
		}

		//Deallocate string
		delete[] infoLog;
	}
	else
	{
		printf("Name %d is not a shader\n", shader);
	}
}

int main(int argc, char* args[])
{
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Main loop flag
		bool quit = false;

		//Event handler
		SDL_Event e;

		//Enable text input
		SDL_StartTextInput();

		//While application is running
		while (!quit)
		{
			//Handle events on queue
			while (SDL_PollEvent(&e) != 0)
			{
				//User requests quit
				if (e.type == SDL_QUIT)
				{
					quit = true;
				}
				//Handle keypress with current mouse position
				else
				{
					int x = 0, y = 0;
					SDL_GetMouseState(&x, &y);
					handleInput(&e, x, y);
				}
			}

			update();
			//Render quad
			render();

			//Update screen
			SDL_GL_SwapWindow(gWindow);
		}

		//Disable text input
		SDL_StopTextInput();
	}

	//Free resources and close SDL
	close();

	return 0;
}
