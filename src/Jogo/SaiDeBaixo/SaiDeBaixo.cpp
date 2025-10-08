/*
 * Sai de Baixo!
 *
 * Desenvolvido por: Cássio Ferreira Braga
 * 					 Gabriel Cezar Walber
 * 					 Patrícia Nagel
 *
 * Disciplina:
 *   - Processamento Gráfico: Fundamentos (Ciência da Computação - Presencial)
 *
 * Professora: Rossana Baptista Queiroz
 * 
 * Descrição:
 *   "Sai de Baixo!" é um jogo de queda de plataformas, em que alguém
 * 		muito incompetente derruba plataformas continuamente e
 * 		coincidentemente acima de sua cabeça. Como um 
 * 		ser vivo que preza pela própria existência, seus instintos dizem que
 * 		você não deve ficar abaixo das plataformas que estão em queda, caso
 * 		queira sobreviver ao máximo de tempo possível.
 * 	 Desvie das plataformas para sobreviver ao máximo.
 *
 * Histórico:
 *   - Versão inicial: 08/10/2025
 *
 */

#include <iostream>
#include <string>
#include <assert.h>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#define STEP_SIZE 16
#define PLAYER_WIDTH (WIDTH/20)
#define PLATFORMS 5
#define PLATFORM_HEIGHT (HEIGHT/20)

#define COLOR_RANGE(a) (a>1?2-a:a)

using namespace glm;

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

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 800, HEIGHT = 600;

// Código fonte do Vertex Shader (em GLSL): ainda hardcoded
const GLchar *vertexShaderSource = R"(
 #version 400
 layout (location = 0) in vec3 position;
 layout (location = 1) in vec3 color;
 out vec3 vColor; 
 uniform mat4 projection;
 void main()
 {
	 gl_Position = projection * vec4(position.x, position.y, position.z, 1.0);
	 vColor = color;
 }
 )";

// Código fonte do Fragment Shader (em GLSL): ainda hardcoded
const GLchar *fragmentShaderSource = R"(
 #version 400
 in vec3 vColor;
 out vec4 color;
 void main()
 {
	 color = vec4(vColor,1.0);
 }
 )";

PointXY last_mouse_pos;
GLfloat redTriangle = 0.0f, greenTriangle = 0.5f, blueTriangle = 0.2f;
GLfloat bgBrightness = 0.0f;
int platormToFall = -1;
game_state estado_de_jogo = GAME_STOPPED;

GameObject player;
GameObject walls[2];
GameObject platforms[PLATFORMS];
GLboolean isPlatFalling[PLATFORMS];

GLuint max_points = 0;
GLuint points = 0;

bool moveLeft = false, moveRight = false;

void gameSetup(GameObject *player, GameObject *walls, GLuint quantWalls, GameObject *platforms, GLuint quantPlatforms)
{
	player->sizeTopLeft = {0, 0};
	player->sizeBottomRight = {WIDTH, HEIGHT};
	player->topLeft = {WIDTH/2, HEIGHT/40};
	player->bottomRight = {player->topLeft.x + PLAYER_WIDTH, HEIGHT/10};
	player->colors = {0, 0, 0};

	walls[0].sizeTopLeft = {100, 0};
	walls[0].sizeBottomRight = {120, HEIGHT};
	walls[0].topLeft = walls[0].sizeTopLeft;
	walls[0].bottomRight = walls[0].sizeBottomRight;
	walls[0].colors = {0.5, 0.5, 0.5};

	walls[1].sizeTopLeft = {WIDTH-120, 0};
	walls[1].sizeBottomRight = {WIDTH-100, HEIGHT};
	walls[1].topLeft = walls[1].sizeTopLeft;
	walls[1].bottomRight = walls[1].sizeBottomRight;
	walls[1].colors = {0.5, 0.5, 0.5};

	GLuint platformWidth = ((walls[1].topLeft.x-walls[0].bottomRight.x)/PLATFORMS)-1;
	for(int i = 0; i < PLATFORMS; i++)
	{
		platforms[i].sizeTopLeft = {0, 0};
		platforms[i].sizeBottomRight = {WIDTH, HEIGHT};

		if(i == 0)
			platforms[i].topLeft = {walls[0].bottomRight.x, -(GLfloat)PLATFORM_HEIGHT};
		else
			platforms[i].topLeft = {platforms[i-1].bottomRight.x+1, -(GLfloat)PLATFORM_HEIGHT};

		platforms[i].bottomRight = {platforms[i].topLeft.x + platformWidth, 0};
		platforms[i].colors = {1, 0, 0};

		isPlatFalling[i] = false;
	}

	platormToFall = randomFall();

	points = 0;

	cout << "Iniciando partida..." << endl;
	cout << "Pontuacao Maxima Atual: " << max_points << endl;
}

// Função MAIN
int main()
{
	// Inicialização da GLFW
	glfwInit();

	srand(time(0));

	// Muita atenção aqui: alguns ambientes não aceitam essas configurações
	// Você deve adaptar para a versão do OpenGL suportada por sua placa
	// Sugestão: comente essas linhas de código para desobrir a versão e
	// depois atualize (por exemplo: 4.5 com 4 e 5)
	 //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	 //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	 //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	 //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Ativa a suavização de serrilhado (MSAA) com 8 amostras por pixel
	// glfwWindowHint(GLFW_SAMPLES, 8);

	// Criação da janela GLFW
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Sai de Baixo!", nullptr, nullptr);
	if (!window)
	{
		std::cerr << "Falha ao criar a janela GLFW" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);

	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Falha ao inicializar GLAD" << std::endl;
		return -1;
	}

	// Obtendo as informações de versão
	const GLubyte *renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte *version = glGetString(GL_VERSION);	/* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "Versao do OpenGL suportada: " << version << endl;

	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// Compilando e buildando o programa de shader
	GLuint shaderID = setupShader();

	GameObject *tmpObj;

	GLuint platformFallSpeed = 3;

	gameSetup(&player, walls, 2, platforms, PLATFORMS);

	// Gerando um buffer simples, com a geometria de um triângulo
	GLuint VAO[1];

	//GLuint texID = loadTexture("../assets/tex/background.png");
	
	//Habilitação do teste de profundidade
	//glEnable(GL_DEPTH_TEST);

	glUseProgram(shaderID); // Reseta o estado do shader para evitar problemas futuros

	double prev_s = glfwGetTime();	// Define o "tempo anterior" inicial.
	double title_countdown_s = 0.1; // Intervalo para atualizar o título da janela com o FPS.

	// Criação da matriz de projeção
	mat4 projection = ortho(0.0, (double)WIDTH, (double)HEIGHT, 0.0, -1.0, 1.0);

	glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"),1,GL_FALSE,value_ptr(projection));

	//mat4 model = mat4(1); //matriz identidade

	// Ativar o primeiro buffer de textura do OpenGL
	//glActiveTexture(GL_TEXTURE0);

	VAO[0] = objectDrawn(&player, walls, 2, platforms, PLATFORMS);

	double lastUpdateTime;
	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		{
			double curr_s = glfwGetTime();		// Obtém o tempo atual.
			double elapsed_s = curr_s - prev_s; // Calcula o tempo decorrido desde o último frame.
			prev_s = curr_s;					// Atualiza o "tempo anterior" para o próximo frame.

			// Exibe o FPS, mas não a cada frame, para evitar oscilações excessivas.
			title_countdown_s -= elapsed_s;
			if (title_countdown_s <= 0.0 && elapsed_s > 0.0)
			{
				double fps = 1.0 / elapsed_s; // Calcula o FPS com base no tempo decorrido.
				// Cria uma string e define o FPS como título da janela.
				char tmp[256];
				sprintf(tmp, "Sai de Baixo!\tFPS %.2lf", fps);
				glfwSetWindowTitle(window, tmp);

				title_countdown_s = 0.1; // Reinicia o temporizador para atualizar o título periodicamente.
			}

			if(curr_s - lastUpdateTime > 0.016)
			{
				lastUpdateTime = curr_s;
				if(estado_de_jogo == GAME_RUNNING)
				{
					if(moveLeft)
					{
						moveLeft = false;
						player.topLeft.x -= STEP_SIZE;
						player.bottomRight.x -= STEP_SIZE;
						if(checkCollision(&player, &walls[0]))
						{
							player.topLeft.x = walls[0].bottomRight.x;
							player.bottomRight.x = player.topLeft.x + PLAYER_WIDTH;
						}
					}

					if(moveRight)
					{
						moveRight = false;
						player.topLeft.x += STEP_SIZE;
						player.bottomRight.x += STEP_SIZE;
						if(checkCollision(&player, &walls[1]))
						{
							player.topLeft.x = walls[1].topLeft.x - PLAYER_WIDTH;
							player.bottomRight.x = player.topLeft.x + PLAYER_WIDTH;
						}
					}

					for(int i = 0; i < PLATFORMS; i++)
					{
						if(checkCollision(&player, &platforms[i]))
						{
							estado_de_jogo = GAME_FINISHED;
							cout << "FIM DE JOGO" << endl;
							cout << "Pontuacao: " << points << endl;
							cout << "Maximo: " << max_points << endl;
							break;
						}
						else if(platforms[i].bottomRight.y > 0) {
							platforms[i].bottomRight.y -= (platformFallSpeed + ((int)(points/5)));
							platforms[i].topLeft.y = platforms[i].bottomRight.y - PLATFORM_HEIGHT;
						}
						else if(isPlatFalling[i])
						{
							isPlatFalling[i] = false;
							points++;
							if(points > max_points) max_points = points;

							platormToFall = randomFall();
						}
						else if(platormToFall == i) //Começa a derrubar proxima plataforma
						{
							platforms[i].topLeft.y = HEIGHT;
							platforms[i].bottomRight.y = platforms[i].topLeft.y + PLATFORM_HEIGHT;
							platormToFall = -1;
							isPlatFalling[i] = true;
						}
					}

				}

			}
		}

		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // cor de fundo
		glClear(GL_COLOR_BUFFER_BIT);

		glLineWidth(5);
		glPointSize(10);

		// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		glBindVertexArray(VAO[0]); // Conectando ao buffer de geometria
		
		//Paredes
		glViewport(0, 0, width, height);
		glDrawArrays(GL_QUADS, 4, 8);

		//Plataformas
		for(int i = 0; i < PLATFORMS; i++)
		{
			tmpObj = &platforms[i];
			glViewport(tmpObj->topLeft.x, tmpObj->topLeft.y, tmpObj->bottomRight.x-tmpObj->topLeft.x, tmpObj->bottomRight.y-tmpObj->topLeft.y);
			glDrawArrays(GL_QUADS, 12 + (i*4), 4);
		}

		//Player
		tmpObj = &player;
		glViewport(tmpObj->topLeft.x, tmpObj->topLeft.y, tmpObj->bottomRight.x-tmpObj->topLeft.x, tmpObj->bottomRight.y-tmpObj->topLeft.y);
		glDrawArrays(GL_QUADS, 0, 4);

		glBindVertexArray(0); // Desnecessário aqui, pois não há múltiplos VAOs

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO[0]);
	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
        double x, y;
		glfwGetCursorPos(window, &x, &y);

		last_mouse_pos.x = (GLfloat)x;
		last_mouse_pos.y = (GLfloat)y;

		if(estado_de_jogo != GAME_RUNNING)
		{
			gameSetup(&player, walls, 2, platforms, PLATFORMS);
			estado_de_jogo = GAME_RUNNING;
		}
	}
}

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	else if(key == GLFW_KEY_LEFT && action == GLFW_PRESS && estado_de_jogo == GAME_RUNNING)
		moveLeft = true;
	else if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS && estado_de_jogo == GAME_RUNNING)
		moveRight = true;
	else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		platormToFall = randomFall();
	}
}

int randomFall()
{
	int min_val = 0;
	int max_val = PLATFORMS-1;
	return min_val + (rand() % (max_val - min_val + 1));
}

bool checkCollision(GameObject *one, GameObject *two)
{
	// collision x-axis?
	bool collisionX = one->bottomRight.x >= 
	two->topLeft.x &&
	two->bottomRight.x >= one->topLeft.x;
	// collision y-axis?
	bool collisionY = one->bottomRight.y >= 
	two->topLeft.y &&
	two->bottomRight.y >= one->topLeft.y;

	// collision only if on both axes
	return collisionX && collisionY;
}

// Esta função está bastante hardcoded - objetivo é compilar e "buildar" um programa de
//  shader simples e único neste exemplo de código
//  O código fonte do vertex e fragment shader está nos arrays vertexShaderSource e
//  fragmentShader source no iniçio deste arquivo
//  A função retorna o identificador do programa de shader
int setupShader()
{
	// Vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// Checando erros de compilação (exibição via log no terminal)
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
				  << infoLog << std::endl;
	}
	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// Checando erros de compilação (exibição via log no terminal)
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
				  << infoLog << std::endl;
	}
	// Linkando os shaders e criando o identificador do programa de shader
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Checando por erros de linkagem
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
				  << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

int objectDrawn(GameObject *player, GameObject *walls, GLuint quantWalls, GameObject *platforms, GLuint quantPlatforms)
{
	vector <GLfloat> vertices;
	GameObject *tmp;
	tmp = player;
	vertices.insert(vertices.end(), {tmp->sizeTopLeft.x, tmp->sizeTopLeft.y, 0, tmp->colors.red, tmp->colors.green, tmp->colors.blue});
	vertices.insert(vertices.end(), {tmp->sizeBottomRight.x, tmp->sizeTopLeft.y, 0, tmp->colors.red, tmp->colors.green, tmp->colors.blue});
	vertices.insert(vertices.end(), {tmp->sizeBottomRight.x, tmp->sizeBottomRight.y, 0, tmp->colors.red, tmp->colors.green, tmp->colors.blue});
	vertices.insert(vertices.end(), {tmp->sizeTopLeft.x, tmp->sizeBottomRight.y, 0, tmp->colors.red, tmp->colors.green, tmp->colors.blue});
	
	//Paredes
	for(int i = 0; i < quantWalls; i++)
	{
		tmp = &walls[i];
		vertices.insert(vertices.end(), {tmp->sizeTopLeft.x, tmp->sizeTopLeft.y, 0, tmp->colors.red, tmp->colors.green, tmp->colors.blue});
		vertices.insert(vertices.end(), {tmp->sizeBottomRight.x, tmp->sizeTopLeft.y, 0, tmp->colors.red, tmp->colors.green, tmp->colors.blue});
		vertices.insert(vertices.end(), {tmp->sizeBottomRight.x, tmp->sizeBottomRight.y, 0, tmp->colors.red, tmp->colors.green, tmp->colors.blue});
		vertices.insert(vertices.end(), {tmp->sizeTopLeft.x, tmp->sizeBottomRight.y, 0, tmp->colors.red, tmp->colors.green, tmp->colors.blue});
	}

	//Plataformas
	for(int i = 0; i < quantPlatforms; i++)
	{
		tmp = &platforms[i];
		vertices.insert(vertices.end(), {tmp->sizeTopLeft.x, tmp->sizeTopLeft.y, 0, tmp->colors.red, tmp->colors.green, tmp->colors.blue});
		vertices.insert(vertices.end(), {tmp->sizeBottomRight.x, tmp->sizeTopLeft.y, 0, tmp->colors.red, tmp->colors.green, tmp->colors.blue});
		vertices.insert(vertices.end(), {tmp->sizeBottomRight.x, tmp->sizeBottomRight.y, 0, tmp->colors.red, tmp->colors.green, tmp->colors.blue});
		vertices.insert(vertices.end(), {tmp->sizeTopLeft.x, tmp->sizeBottomRight.y, 0, tmp->colors.red, tmp->colors.green, tmp->colors.blue});
	}

	GLuint VBO, VAO;
	// Geração do identificador do VBO
	glGenBuffers(1, &VBO);
	// Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

	// Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos
	glBindVertexArray(VAO);
	// Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando:
	//  Localização no shader * (a localização dos atributos devem ser correspondentes no layout especificado no vertex shader)
	//  Numero de valores que o atributo tem (por ex, 3 coordenadas xyz)
	//  Tipo do dado
	//  Se está normalizado (entre zero e um)
	//  Tamanho em bytes
	//  Deslocamento a partir do byte zero

	// Atributo posição - x, y, z
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	// Atributo cor - r, g b
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}

/*GLuint loadTexture(string filePath)
{
	GLuint texID;

	// Gera o identificador da textura na memória
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	// Ajusta os parâmetros de wrapping e filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Carregamento dos pixels da imagem
	int width, height, nrChannels;
	unsigned char *data = stbi_load(filePath.c_str(), &width, &height,
	&nrChannels, 0);

	if (data)
	{
		if (nrChannels == 3) //jpg, bmp
 		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
			data);
 		}
 		else //png
 		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
			data);
 		}
 		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	// Liberar o data e desconectar a textura
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);

    return texID;
}*/