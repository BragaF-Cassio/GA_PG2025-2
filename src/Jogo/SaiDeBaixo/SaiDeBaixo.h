#include <iostream>
#include <string>
#include <assert.h>
#include <vector>
#include <cstdlib>
#include <ctime>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

using namespace std;

using namespace glm;

#define STEP_SIZE 16
#define PLAYER_WIDTH (WIDTH/20)
#define PLATFORMS 5
#define PLATFORM_HEIGHT (HEIGHT/20)

#define COLOR_RANGE(a) (a>1?2-a:a)

typedef struct coordinates {
	GLfloat x;
	GLfloat y;
} PointXY;

typedef struct colors {
	GLfloat red;
	GLfloat green;
	GLfloat blue;
} GameColor;

typedef struct object_game {
	PointXY topLeft;
	PointXY bottomRight;
	PointXY sizeTopLeft;
	PointXY sizeBottomRight;
	string name;
	GameColor colors;
} GameObject;

enum game_state {
	GAME_STOPPED,
	GAME_RUNNING,
	GAME_FINISHED
};

// Protótipo da função de callback de teclado
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// Protótipos das funções
int setupShader();
int randomFall();
bool checkCollision(GameObject *one, GameObject *two);
int objectDrawn(GameObject *player, GameObject *walls, GLuint quantWalls, GameObject *platforms, GLuint quantPlatforms);
void gameSetup(GameObject *player, GameObject *walls, GLuint quantWalls, GameObject *platforms, GLuint quantPlatforms);
GLuint loadTexture(string filePath);