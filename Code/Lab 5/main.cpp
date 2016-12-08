
//Some Windows Headers (For Time, IO, etc.)
#include <windows.h>
#include <string>
#include <mmsystem.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <SOIL.h>
#include <iostream>
#include "maths_funcs.h"
#include "text.h"

// Assimp includes

#include <assimp/cimport.h> // C importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations
#include <stdio.h>
#include <math.h>
#include <vector> // STL dynamic memory.


/*----------------------------------------------------------------------------
                   MESH TO LOAD
  ----------------------------------------------------------------------------*/
// this mesh is a dae file format but you should be able to use any other format too, obj is typically what is used
// put the mesh in your project directory, or provide a filepath for it here
#define CITY_MESH "../Meshes/baseCity.obj"
#define PLANE_MESH "../Meshes/plane2.obj"
#define BANANAMAN_MESH "../Meshes/bananaMan4.obj"
#define MONKEY_MESH "../Meshes/monkeyhead.dae"
#define BULLET_MESH "../Meshes/banana.obj"
/*----------------------------------------------------------------------------
  ----------------------------------------------------------------------------*/
// Mesh Variables
std::vector<float> g_vp[5], g_vn[5], g_vt[5];
int g_point_count[5];
unsigned int vao[5];
unsigned int vp_vbo[5];
unsigned int vn_vbo[5];
unsigned int vt_vbo[5];

int text_id;
int score = 0;
bool leftButtonDown = false;

// Macro for indexing vertex buffer
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

#define M_PI 3.1415926535897932384626433832795

using namespace std;
GLuint shaderProgramID;

int width = 1920;
int height = 1080;

GLuint loc1, loc2, loc3;

static DWORD  last_time = 0;

// Mouse field of view variables
float horizontalAngle = M_PI;
float verticalAngle = 0.0f;
float initialFoV = 90.0f;
float speed = 0.11f;
float mouseSpeed = 0.0007f;

// Keep track of how far the mouse has deviated from the origin
int totalX = 0, totalY = 0;

vec3 direction = vec3(0.0, 1.0, 1.0), rightVec, up, position = vec3(0.0, 1.0, 0.0f);
vec3 newPosVector = vec3(0, 1.5, 1.5);
bool z_axis;

// Banana man variables
mat4 bananamanModel;
vec3 bananaPosition = vec3(0.0, 0.0, 0.0);
double bananaDirection = -90.0;
float bananaScale = 0.25;

GLuint planeTex, bananaTex, cityTex, bulletTex;


// Track camera rotations with character
int rotations = 0;

// Define a class for the bullet
class Bullet {
	private:
		vec3 pos;
		vec3 dir;
		float movement_unit = 1.0;
		vec3 x_movement = vec3(movement_unit, 0.0, 0.0);
		vec3 z_movement = vec3(0.0, 0.0, movement_unit);
		float speed = 0.4f;
		float distance_travelled = 0.0f;
		float max_distance = 10.0f;
		float scaleFactor;
		float rotation = 0.0;
		vec3 scaleVector;
		float damage = 1.0;
		bool visible;
		bool bazooka;

		void setStartPoint() {
			//print(bananaPosition);
			pos = position + (direction);
		}

		void setDirection() {
			dir = vec3(direction.v[0], direction.v[1], direction.v[2]);
			//print(dir);
		}

		void setRotation() {
			print(dir);
			float x = dir.v[0], z = dir.v[2];
			if ((x <= 0 && x >= -1) && (z <= 1 && z >= 0)) {
				rotation = (90.0 * abs(x)) * -1.0;
			}
			else if ((x >= -1 && x <= 0) && (z <= 0 && z >= -1)) {
				rotation = ((90.0 * abs(z)) + 90.0) * -1.0;
			}
			else if ((x <= 1 && x >= 0) && (z >= -1 && z <= 0)) {
				rotation = ((90.0 * abs(x)) + 180.0) * -1.0;
			}
			else if ((x <= 1 && x >= 0) && (z >= 0 && z <= 1)) {
				rotation = ((90.0 * abs(z)) + 270.0) * -1.0;
			}
			printf("Angle: %f\n", rotation);
			/*if (dir.v[2] == -1.0) {
				rotation = 90.0;
			}
			else if (dir.v[2] == 1.0) {
				rotation = -90.0;
			}
			else if (dir.v[0] == -1.0) {
				rotation = 180.0;
			}
			else {
				rotation = 0.0;
			}
			rotation = 0.0;*/
		}

		void setVisible() {
			visible = true;
		}

		void setBazooka(bool val) {
			bazooka = val;
		}

		void setScale() {
			if (bazooka) {
				scaleFactor = 1.0f;
			}
			else {
				scaleFactor = 0.33f;
			}
			scaleVector = vec3(scaleFactor, scaleFactor, scaleFactor);
		}

	public:
		void initBullet(bool val) {
			setStartPoint();
			setVisible();
			setDirection();
			setRotation();
			setBazooka(val);
			setScale();
		}

		vec3 getPosition() {
			return pos;
		}

		float getScaleFactor() {
			return scaleFactor;
		}

		vec3 getScaleVector() {
			return scaleVector;
		}

		void updatePosition() {
			pos += dir * speed;
			distance_travelled += movement_unit * speed;
		}

		bool inBounds() {
			return distance_travelled < max_distance;
		}

		double getRotation() {
			return rotation;
		}

		float getDamage() {
			return damage;
		}

		void setVisibility(bool val) {
			visible = val;
		}

		bool getVisibility() {
			return visible;
		}

		bool isBazooka() {
			return bazooka;
		}
};

DWORD lastBazooka = timeGetTime();
DWORD bazookaInterval = 3000;

const int max_bullets = 30;
Bullet bullets[max_bullets];

DWORD lastBullet = timeGetTime();
DWORD minInterval = 100;

int bulletCount = 0, bulletIndex = 0;

// Define a class for the Monkey heads
class Banana {
	private:
		int index;
		vec3 position;
		double rotation;
		float movement_unit = 1.0;
		vec3 x_movement = vec3(movement_unit, 0.0, 0.0);
		vec3 z_movement = vec3(0.0, 0.0, movement_unit);
		float speed = 0.4f;
		double distance_travelled = 0.0;
		double max_distance = 40.0f;
		float scaleFactor = 0.75f;
		vec3 scaleVector = vec3(0.25, 0.25, 0.25);
		float health = 1.0;

		void generateIndex() {
			index = rand() % 4;
			//printf("INDEX %d\n", index);
		}

		void setStartPoint() {
			vec3 positions[4] = { vec3(0.0, 0.5, -20.0), vec3(-20.0, 0.5, 0.0), vec3(0.0, 0.5, 20.0), vec3(20.0, 0.5, 0.0) };
			if (index == 0 || index == 2) {
				float randOffset = (float) (rand() % 3000) / 1500.0;
				randOffset -= 1.0;
				position = positions[index] + vec3(randOffset, 0.0, 0.0);
			}
			else {
				float randOffset = (float)(rand() % 3000) / 1500.0;
				randOffset -= 1.0;
				position = positions[index] + vec3(0.0, 0.0, randOffset);
			}
		}

		void setRotation() {
			double rotations[4] = { 90.0, 180.0, 270.0, 0.0 };
			rotation = rotations[index];
		}

		void setSpeed() {
			int rando = rand() % 500 + 500;
			//printf("RANDO: %i\n", rando);
			speed = (float)rando / 10000.0;
			//printf("SPEED: %f\n", speed);
		}

	public:
		void initBanana() {
			generateIndex();
			setStartPoint();
			setRotation();
			setSpeed();
			distance_travelled = 0.0f;
		}

		vec3 getPosition() {
			return position;
		}

		double getRotation() {
			return rotation;
		}

		void updatePosition() {
			switch (index) {
				case 0:
					position = position + (z_movement * speed);
					distance_travelled += (movement_unit * speed);
					break;
				case 1:
					position = position + (x_movement * speed);
					distance_travelled += (movement_unit * speed);
					break;
				case 2:
					position = position - (z_movement * speed);
					distance_travelled += (movement_unit * speed);
					break;
				case 3:
					position = position - (x_movement * speed);
					distance_travelled += (movement_unit * speed);
					break;
			}
		}

		bool inBounds() {
			return distance_travelled < max_distance;
		}

		float getScaleFactor() {
			return scaleFactor;
		}

		vec3 getScaleVector() {
			return scaleVector;
		}

		float getHealth() {
			return health;
		}

		void updateHealth(float damage) {
			health -= damage;
		}

		bool stillAlive() {
			return health > 0.0;
		}
};

const int max_bananas = 30;
Banana banana[max_bananas];
int bananaCount = 0;
DWORD lastBanana;
DWORD bananaInterval = 5000;


bool collision(vec3 objectPos1, double objScale1, vec3 objectPos2, double objScale2) {
	float x1 = objectPos1.v[0];
	float x2 = objectPos2.v[0];
	float z1 = objectPos1.v[2];
	float z2 = objectPos2.v[2];

	float objEdge1 = objScale1 / 2;
	float objEdge2 = objScale2 / 2;

	float minDistance = objEdge1 + objEdge2;

	float absX = abs(x2 - x1);
	float absZ = abs(z2 - z1);

	if (absX < minDistance && absZ < minDistance) {
		return true;
	}
	return false;
}

#pragma region MESH LOADING
/*----------------------------------------------------------------------------
                   MESH LOADING FUNCTION
  ----------------------------------------------------------------------------*/

bool load_mesh (const char* file_name, std::vector<float> *g_vp, std::vector<float> *g_vn, std::vector<float> *g_vt, int *g_point_count, bool loadTextures) {
  const aiScene* scene = aiImportFile (file_name, aiProcess_Triangulate); // TRIANGLES!
  if (!scene) {
    fprintf (stderr, "ERROR: reading mesh %s\n", file_name);
    return false;
  }
  printf ("  %i animations\n", scene->mNumAnimations);
  printf ("  %i cameras\n", scene->mNumCameras);
  printf ("  %i lights\n", scene->mNumLights);
  printf ("  %i materials\n", scene->mNumMaterials);
  printf ("  %i meshes\n", scene->mNumMeshes);
  printf ("  %i textures\n", scene->mNumTextures);
  
  *g_point_count = 0;
  for (unsigned int m_i = 0; m_i < scene->mNumMeshes; m_i++) {
    const aiMesh* mesh = scene->mMeshes[m_i];
    //printf ("    %i vertices in mesh\n", mesh->mNumVertices);
    *g_point_count += mesh->mNumVertices;
    for (unsigned int v_i = 0; v_i < mesh->mNumVertices; v_i++) {
      if (mesh->HasPositions ()) {
        const aiVector3D* vp = &(mesh->mVertices[v_i]);
        //printf ("      vp %i (%f,%f,%f)\n", v_i, vp->x, vp->y, vp->z);
        g_vp->push_back (vp->x);
        g_vp->push_back (vp->y);
        g_vp->push_back (vp->z);
      }
      if (mesh->HasNormals ()) {
        const aiVector3D* vn = &(mesh->mNormals[v_i]);
        //printf ("      vn %i (%f,%f,%f)\n", v_i, vn->x, vn->y, vn->z);
        g_vn->push_back (vn->x);
        g_vn->push_back (vn->y);
        g_vn->push_back (vn->z);
      }
      if (mesh->HasTextureCoords (0)) {
        const aiVector3D* vt = &(mesh->mTextureCoords[0][v_i]);
        //printf ("      vt %i (%f,%f)\n", v_i, vt->x, vt->y);
		if (loadTextures) {
			g_vt->push_back(vt->x);
			g_vt->push_back(vt->y);
		}
      }
      if (mesh->HasTangentsAndBitangents ()) {
        // NB: could store/print tangents here
      }
    }
  }
  
  aiReleaseImport (scene);
  return true;
}

#pragma endregion MESH LOADING

// Shader Functions- click on + to expand
#pragma region SHADER_FUNCTIONS

// Create a NULL-terminated string by reading the provided file
char* readShaderSource(const char* shaderFile) {   
    FILE* fp = fopen(shaderFile, "rb"); //!->Why does binary flag "RB" work and not "R"... wierd msvc thing?

    if ( fp == NULL ) { return NULL; }

    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);

    fseek(fp, 0L, SEEK_SET);
    char* buf = new char[size + 1];
    fread(buf, 1, size, fp);
    buf[size] = '\0';

    fclose(fp);

    return buf;
}


static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	// create a shader object
    GLuint ShaderObj = glCreateShader(ShaderType);

    if (ShaderObj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(0);
    }
	const char* pShaderSource = readShaderSource( pShaderText);

	// Bind the source code to the shader, this happens before compilation
	glShaderSource(ShaderObj, 1, (const GLchar**)&pShaderSource, NULL);
	// compile the shader and check for errors
    glCompileShader(ShaderObj);
    GLint success;
	// check for shader related errors using glGetShaderiv
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        exit(1);
    }
	// Attach the compiled shader object to the program object
    glAttachShader(ShaderProgram, ShaderObj);
}

GLuint CompileShaders()
{
	//Start the process of setting up our shaders by creating a program ID
	//Note: we will link all the shaders together into this ID
    shaderProgramID = glCreateProgram();
    if (shaderProgramID == 0) {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

	// Create two shader objects, one for the vertex, and one for the fragment shader
    AddShader(shaderProgramID, "../Shaders/simpleVertexShader.txt", GL_VERTEX_SHADER);
    AddShader(shaderProgramID, "../Shaders/simpleFragmentShader.txt", GL_FRAGMENT_SHADER);

    GLint Success = 0;
    GLchar ErrorLog[1024] = { 0 };
	// After compiling all shader objects and attaching them to the program, we can finally link it
    glLinkProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
    glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
	}

	// program has been successfully linked but needs to be validated to check whether the program can execute given the current pipeline state
    glValidateProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
    glGetProgramiv(shaderProgramID, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }
	// Finally, use the linked shader program
	// Note: this program will stay in effect for all draw calls until you replace it with another or explicitly disable its use
    glUseProgram(shaderProgramID);
	return shaderProgramID;
}
#pragma endregion SHADER_FUNCTIONS

// VBO Functions - click on + to expand
#pragma region VBO_FUNCTIONS

// Bind the buffers for the mesh specified by index
void bindBuffers(int index) {
	loc1 = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc2 = glGetAttribLocation(shaderProgramID, "vertex_normal");
	loc3 = glGetAttribLocation(shaderProgramID, "vertex_texture");

	vao[index] = 0;
	glBindVertexArray(vao[index]);

	glEnableVertexAttribArray(loc1);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo[index]);
	glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc2);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo[index]);
	glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc3);
	glBindBuffer(GL_ARRAY_BUFFER, vt_vbo[index]);
	glVertexAttribPointer(loc3, 2, GL_FLOAT, GL_FALSE, 0, NULL);
}

void generateObjectBufferMesh(int index) {
/*----------------------------------------------------------------------------
                   LOAD MESH HERE AND COPY INTO BUFFERS
  ----------------------------------------------------------------------------*/

	//Note: you may get an error "vector subscript out of range" if you are using this code for a mesh that doesnt have positions and normals
	//Might be an idea to do a check for that before generating and binding the buffer.

	vp_vbo[index] = 0;

	glGenBuffers(1, &vp_vbo[index]);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo[index]);
	glBufferData(GL_ARRAY_BUFFER, g_point_count[index] * 3 * sizeof(float), &g_vp[index][0], GL_STATIC_DRAW);
	vn_vbo[index] = 0;
	glGenBuffers(1, &vn_vbo[index]);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo[index]);
	glBufferData(GL_ARRAY_BUFFER, g_point_count[index] * 3 * sizeof(float), &g_vn[index][0], GL_STATIC_DRAW);


	//	This is for texture coordinates which you don't currently need, so I have commented it out
	//vt_vbo[index] = 0;
	glGenBuffers (1, &vt_vbo[index]);
	glBindBuffer (GL_ARRAY_BUFFER, vt_vbo[index]);
	glBufferData (GL_ARRAY_BUFFER, g_point_count[index] * 2 * sizeof (float), &g_vt[index][0], GL_STATIC_DRAW);

	bindBuffers(index);
}


#pragma endregion VBO_FUNCTIONS
// Function for updating camera angles and direction
void getInputData() {
	horizontalAngle += mouseSpeed *  float(totalX) * -1.0;
	//verticalAngle += mouseSpeed * float(totalY) * -1.0;
	//horizontalAngle = M_PI;
	//verticalAngle = -0.5;
	totalX = 0, totalY = 0;

	// Calculate vector for camera direction and other vectors
	direction = vec3(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	rightVec = vec3(
		sin(horizontalAngle - M_PI / 2.0f),
		0,
		cos(horizontalAngle - M_PI / 2.0f)
	);

	up = cross(rightVec, direction);
}

void shoot() {
	DWORD curr_time = timeGetTime();
	DWORD interval = curr_time - lastBullet;
	if (minInterval < interval) {
		bullets[bulletIndex].initBullet(false);
		bulletIndex = (bulletIndex + 1) % max_bullets;
		bulletCount++;
		lastBullet = curr_time;
	}
}

void display(){

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable (GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClearColor (0.5f, 0.5f, 0.5f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram (shaderProgramID);

	//Declare your uniform variables that will be used in your shader
	int matrix_location = glGetUniformLocation(shaderProgramID, "model");
	int view_mat_location = glGetUniformLocation(shaderProgramID, "view");
	int proj_mat_location = glGetUniformLocation(shaderProgramID, "proj");

	// Root of the Hierarchy
	mat4 view = identity_mat4();
	mat4 persp_proj = perspective(initialFoV, (float)width / (float)height, 0.1, 100.0);
	mat4 model = identity_mat4();
	//model = rotate_x_deg(model, -90.0);
	model = scale(model, vec3(2.0, 2.0, 2.0));
	// Get mouse data to update camera position
	getInputData();
	vec3 newPosition = position + newPosVector;
	vec3 newDirection = direction + vec3(0.0, -0.5, 0.0);
	//view = look_at(newPosition, newPosition + newDirection, up);
	view = look_at(position, position + direction, up);

	// Bind buffer for city mesh
	bindBuffers(0);
	glBindTexture(GL_TEXTURE_2D, cityTex);
	
	// update uniforms & draw
	glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv (view_mat_location, 1, GL_FALSE, view.m);
	glUniformMatrix4fv (matrix_location, 1, GL_FALSE, model.m);

	glDrawArrays (GL_TRIANGLES, 0, g_point_count[0]);

	// Plane stuff
	bindBuffers(1);

	// Bind texture buffers
	glBindTexture(GL_TEXTURE_2D, planeTex);


	// Hierarchical placement of plane above the city
	mat4 plane = identity_mat4();
	plane = translate(plane, vec3(0.0, 10.0, 0.0));
	mat4 plane_location = model * plane;
	model = scale(model, vec3(0.5, 0.5, 0.5));

	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, plane_location.m);
	glDrawArrays(GL_TRIANGLES, 0, g_point_count[1]);

	// Banana man stuff
	//bindBuffers(2);
	//glBindTexture(GL_TEXTURE_2D, bananaTex);

	// Create the objects model, scale it and orient it properly at the origin
	//bananamanModel = identity_mat4();
	//bananamanModel = scale(bananamanModel, vec3(bananaScale, bananaScale, bananaScale));
	//bananamanModel = rotate_x_deg(bananamanModel, -90.0);
	//bananamanModel = rotate_y_deg(bananamanModel, bananaDirection);

	// Translate it in front of the camera
	//bananaPosition= position + vec3(0.0, -0.5, 0.0);
	//bananamanModel = translate(bananamanModel, bananaPosition);


	//glUniformMatrix4fv(matrix_location, 1, GL_FALSE, bananamanModel.m);
	//glDrawArrays(GL_TRIANGLES, 0, g_point_count[2]);

	// Draw in the monkey
	bindBuffers(2);
	glBindTexture(GL_TEXTURE_2D, bananaTex);
	for (int i = 0; i < bananaCount; i++) {
		mat4 bananaModel = identity_mat4();
		bananaModel = scale(bananaModel, banana[i].getScaleVector());
		bananaModel = rotate_y_deg(bananaModel, banana[i].getRotation());
		bananaModel = translate(bananaModel, banana[i].getPosition());

		if (collision(position, 0.25, banana[i].getPosition(), banana[i].getScaleFactor())) {
			banana[i].initBanana();
		}

		banana[i].updatePosition();

		if (!banana[i].inBounds()) {
			score++;
			banana[i].initBanana();
		}

		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, bananaModel.m);
		glDrawArrays(GL_TRIANGLES, 0, g_point_count[2]);
	}

	DWORD curr_time = timeGetTime();

	if ((curr_time - lastBanana) > bananaInterval && bananaCount < max_bananas) {
		banana[bananaCount].initBanana();
		bananaCount++;
		lastBanana = timeGetTime();
		//printf("ADDED Monkey %d! \n", monkeyCount);
	}

	if (leftButtonDown) {
		shoot();
	}

	// Now draw the bullet objects
	bindBuffers(3);
	for (int i = 0; i < min(bulletCount, max_bullets); i++) {
		if (bullets[i].getVisibility()) {
			mat4 bulletModel = identity_mat4();
			bulletModel = rotate_y_deg(bulletModel, bullets[i].getRotation());
			bulletModel = scale(bulletModel, bullets[i].getScaleVector());
			bulletModel = translate(bulletModel, bullets[i].getPosition());
			bullets[i].updatePosition();

			// Check if banana collided with a monkey
			for (int j = 0; j < bananaCount; j++) {
				if (collision(bullets[i].getPosition(), bullets[i].getScaleFactor(), banana[j].getPosition(), banana[j].getScaleFactor())) {
					banana[j].updateHealth(bullets[i].getDamage());
					if (!banana[j].stillAlive()) {
						banana[j].initBanana();
						score++;
					}
					bullets[i].setVisibility(false);
					if (!bullets[i].isBazooka()) {
						break;
					}
				}
			}

			glUniformMatrix4fv(matrix_location, 1, GL_FALSE, bulletModel.m);
			glDrawArrays(GL_TRIANGLES, 0, g_point_count[3]);
		}
	}


	char tmp[256];
	sprintf(tmp, "Score: %i\n", score);
	update_text(text_id, tmp);

	draw_texts();

    glutSwapBuffers();
}



void updateScene() {	

	// Placeholder code, if you want to work with framerate
	// Wait until at least 16ms passed since start of last frame (Effectively caps framerate at ~60fps)
	DWORD  curr_time = timeGetTime();
	float  delta = (curr_time - last_time) * 0.001f;
	if (delta > 0.03f)
		delta = 0.03f;
	last_time = curr_time;

	// Draw the next frame
	glutPostRedisplay();
}


void init()
{
	// Set up the shaders
	GLuint shaderProgramID = CompileShaders();
	// load mesh into a vertex buffer array
	load_mesh(CITY_MESH, &g_vp[0], &g_vn[0], &g_vt[0], &g_point_count[0], true);
	generateObjectBufferMesh(0);
	load_mesh(PLANE_MESH, &g_vp[1], &g_vn[1], &g_vt[1], &g_point_count[1], true);
	generateObjectBufferMesh(1);
	load_mesh(BANANAMAN_MESH, &g_vp[2], &g_vn[2], &g_vt[2], &g_point_count[2], true);
	generateObjectBufferMesh(2);
	load_mesh(MONKEY_MESH, &g_vp[3], &g_vn[3], &g_vt[3], &g_point_count[3], false);
	generateObjectBufferMesh(3);
	load_mesh(BULLET_MESH, &g_vp[4], &g_vn[4], &g_vt[4], &g_point_count[4], true);
	generateObjectBufferMesh(4);
	banana[0].initBanana();
	banana[1].initBanana();
	banana[2].initBanana();
	bananaCount = 3;
	lastBanana = timeGetTime();

	// Get the plane texture
	glGenTextures(1, &planeTex);
	glBindTexture(GL_TEXTURE_2D, planeTex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glGenerateMipmap(GL_TEXTURE_2D);
	int wid, hei;
	unsigned char* image = SOIL_load_image("../Meshes/plane.JPG", &wid, &hei, 0, SOIL_LOAD_RGBA);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wid, hei, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Get bananaman texture
	glGenTextures(1, &bananaTex);
	glBindTexture(GL_TEXTURE_2D, bananaTex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	wid = 0, hei = 0;
	//glGenerateMipmap(GL_TEXTURE_2D);
	image = SOIL_load_image("../Meshes/bananaTex.png", &wid, &hei, 0, SOIL_LOAD_RGBA);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wid, hei, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Get city texture
	glGenTextures(1, &cityTex);
	glBindTexture(GL_TEXTURE_2D, cityTex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	wid = 0, hei = 0;
	//glGenerateMipmap(GL_TEXTURE_2D);
	image = SOIL_load_image("../Meshes/cityscaper.png", &wid, &hei, 0, SOIL_LOAD_RGBA);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wid, hei, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Get city texture
	glGenTextures(1, &bulletTex);
	glBindTexture(GL_TEXTURE_2D, bulletTex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	wid = 0, hei = 0;
	//glGenerateMipmap(GL_TEXTURE_2D);
	image = SOIL_load_image("../Meshes/Banana.png", &wid, &hei, 0, SOIL_LOAD_RGBA);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wid, hei, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	init_text_rendering("../Lab 5/freemono.png", "../Lab 5/freemono.meta", width, height);
	// x and y are -1 to 1
	// size_px is the maximum glyph size in pixels (try 100.0f)
	// r,g,b,a are red,blue,green,opacity values between 0.0 and 1.0
	// if you want to change the text later you will use the returned integer as a parameter
	text_id = add_text("Score: 0", -0.9 , -0.75, 60.0f, 1.0, 0.0, 0.0, 1.0);
}

// Placeholder code for the keypress
void keypress(unsigned char key, int x, int y) {
	double currentTime = timeGetTime();
	float deltaTime = float(currentTime - last_time);
	float xpos[4] = { 0.0, -1.5, 0, 1.5 };
	float zpos[4] = { 1.5, 0.0, -1.5, 0.0 };
	//printf("KEY %c\n", key);
	switch (key) {
		// Move forward
		case 'w':
			position += direction * speed;
			break;
		// Move backwards
		case 's':
			position -= direction * speed;
			break;
		// Move Right
		case 'a':
			if (rotations == 0) {
				position += vec3(-2.5, 0.0, 0.0) * speed;
			}
			else if (rotations == 1){
				position += vec3(0.0, 0.0, -2.5) * speed;
			}
			else if (rotations == 2) {
				position += vec3(2.5, 0.0, 0.0) * speed;
			}
			else {
				position += vec3(0.0, 0.0, 2.5) * speed;
			}
			break;
		case 'd':
			if (rotations == 0) {
				position += vec3(2.5, 0.0, 0.0) * speed;
			}
			else if (rotations == 1) {
				position += vec3(0.0, 0.0, 2.5) * speed;
			}
			else if (rotations == 2) {
				position += vec3(-2.5, 0.0, 0.0) * speed;
			}
			else {
				position += vec3(0.0, 0.0, -2.5) * speed;
			}
			break;
		// Exit program
		case 'x':
			exit(0);
			break;
	}

}

void updateMouse(int x, int y) {
	// Keep a tally of how far mouse has moved from origin
	totalX += x - (width / 2);
	totalY += y - (height / 2);

	// If mouse has strayed more than 10% height or width from origin, reset it
	int absX = abs(x - width / 2);
	int absY = abs(y - height / 2);
	if (absX > 20 || absY > 20) {
		glutWarpPointer(width / 2, height / 2);
	}
}

void handleMouse(int button, int state, int x, int y) {
	if ((button == GLUT_LEFT_BUTTON)) {
		leftButtonDown = (state == GLUT_DOWN);
	}
	else if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN)) {
		DWORD newTime = timeGetTime();
		DWORD timeDiff = newTime - lastBazooka;
		if (bazookaInterval < timeDiff) {
			bullets[bulletIndex].initBullet(true);
			bulletIndex = (bulletIndex + 1) % max_bullets;
			bulletCount++;
			lastBazooka = newTime;
		}
	}
}

int main(int argc, char** argv){
	// Set up the window
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(width, height);
    glutCreateWindow("Hello Metropolis");

	// Tell glut where the display function is
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);
	glutKeyboardFunc(keypress);
	glutMouseFunc(handleMouse);
	glutMotionFunc(updateMouse);
	glutPassiveMotionFunc(updateMouse);
	glutSetCursor(GLUT_CURSOR_NONE);
	// Set mouse in the beginning of the screen
	glutWarpPointer(width / 2, height / 2);

	 // A call to glewInit() must be done after glut is initialized!
    GLenum res = glewInit();
	// Check for any errors
    if (res != GLEW_OK) {
      fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
      return 1;
    }
	// Set up your objects and shaders
	init();
	// Begin infinite event loop
	glutMainLoop();
    return 0;
}
