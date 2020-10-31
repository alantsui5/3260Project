#include "Dependencies/glew/glew.h"
#include "Dependencies/glm/glm.hpp"
#include "Dependencies/GLFW/glfw3.h"
#include "Dependencies/glm/gtc/matrix_transform.hpp"

#include "Shader.h"
#include "Texture.h"

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <time.h>

using namespace std;
clock_t now_t;
int st_time = 0;


// Shader and light brightness
const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 600;
Shader myShader;
int isRotate = 1;
int lightbrightness = 3;


// Viewport Controller Properties
int buttonPressed = 0;
float radius = 10.0f;
float camX;
float camY;
float camZ;
float yaw = -90.0f;
float pitch = 17.0f;
double oldX;
double oldY;

// Dolphin and Sea Properties
int dolphinColor = 0;
int seaColor = 0;
float dolphinPosX;
float dolphinPosY;
float dolphinRotate = 0;
float dolphinDelta = 0.1f;
float mouseSensitivity = 0.03f;

// Penguin Properties
float penguinPosX = -6.0f;
float penguinPosY = 6.0f;
float penguinRotate = 0;
float penguinDelta = 0.1f;
float penguinSwim = 0;

glm::vec3 lightPosition = glm::vec3(8, 3, 0);

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT){
		if (action == GLFW_PRESS) {
			buttonPressed = 1;
			glfwGetCursorPos(window, &oldX, &oldY);
		}
		if (action == GLFW_RELEASE) {
			buttonPressed = 0;
		}
	}
}

void cursor_position_callback(GLFWwindow* window, double x, double y)
{
	// Sets the cursor position callback for the current window
	if (buttonPressed == 1) {
		//getting cursor position
		double xOffset = oldX - x;
		double yOffset = oldY - y;
		yaw += mouseSensitivity * xOffset;
		pitch += mouseSensitivity * yOffset;
		pitch = glm::clamp(pitch, -89.0f, 89.0f);
		cout << "Cursor Position at (" << oldX << " : " << oldY << ")" << endl;
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Sets the Keyboard callback for the current window.
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		dolphinColor = 0;
	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		dolphinColor = 1;
	}
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
	{
		seaColor = 0;
	}
	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
	{
		seaColor = 1;
	}
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {

		if (isRotate == 1) {
			isRotate = 0;
		}
		else {
			isRotate = 1;
		}
	}
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		lightbrightness += 1;
		if (lightbrightness > 10)lightbrightness = 10;
	}
	if (key == GLFW_KEY_K && action == GLFW_PRESS)
	{
		lightbrightness -= 1;
		if (lightbrightness < 0)lightbrightness = 0;
	}
	if (key == GLFW_KEY_C && action == GLFW_PRESS)
	{
		lightbrightness = -10;
	}
	if (key == GLFW_KEY_V && action == GLFW_PRESS)
	{
		lightbrightness = 3;
	}
	if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
		penguinPosY += 0.1f * cos(penguinDelta * penguinRotate);
		penguinPosX += 0.1f * sin(penguinDelta * penguinRotate);
	}

	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
		penguinPosY -= 0.1f * cos(penguinDelta * penguinRotate);
		penguinPosX -= 0.1f * sin(penguinDelta * penguinRotate);
	}

	if (key == GLFW_KEY_N && action == GLFW_PRESS) {
		dolphinRotate += 1;
	}

	if (key == GLFW_KEY_M && action == GLFW_PRESS) {
		dolphinRotate -= 1;
	}

	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
		penguinRotate += 1;
	}

	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
		penguinRotate -= 1;
	}

	if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
		if (penguinSwim == 1) {
			penguinSwim = 0;
		}
		else {
			penguinSwim = 1;
		}
		
	}
}

// struct for storing the obj file
struct Vertex {
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
};

struct Model {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};

Model loadOBJ(const char* objPath)
{
	// function to load the obj file
	// Note: this simple function cannot load all obj files.

	struct V {
		// struct for identify if a vertex has showed up
		unsigned int index_position, index_uv, index_normal;
		bool operator == (const V& v) const {
			return index_position == v.index_position && index_uv == v.index_uv && index_normal == v.index_normal;
		}
		bool operator < (const V& v) const {
			return (index_position < v.index_position) ||
				(index_position == v.index_position && index_uv < v.index_uv) ||
				(index_position == v.index_position && index_uv == v.index_uv && index_normal < v.index_normal);
		}
	};

	std::vector<glm::vec3> temp_positions;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;

	std::map<V, unsigned int> temp_vertices;

	Model model;
	unsigned int num_vertices = 0;

	std::cout << "\nLoading OBJ file " << objPath << "..." << std::endl;

	std::ifstream file;
	file.open(objPath);

	// Check for Error
	if (file.fail()) {
		std::cerr << "Impossible to open the file! Do you use the right path? See Tutorial 6 for details" << std::endl;
		exit(1);
	}

	while (!file.eof()) {
		// process the object file
		char lineHeader[128];
		file >> lineHeader;

		if (strcmp(lineHeader, "v") == 0) {
			// geometric vertices
			glm::vec3 position;
			file >> position.x >> position.y >> position.z;
			temp_positions.push_back(position);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			// texture coordinates
			glm::vec2 uv;
			file >> uv.x >> uv.y;
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			// vertex normals
			glm::vec3 normal;
			file >> normal.x >> normal.y >> normal.z;
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			// Face elements
			V vertices[3];
			for (int i = 0; i < 3; i++) {
				char ch;
				file >> vertices[i].index_position >> ch >> vertices[i].index_uv >> ch >> vertices[i].index_normal;
			}

			// Check if there are more than three vertices in one face.
			std::string redundency;
			std::getline(file, redundency);
			if (redundency.length() >= 5) {
				std::cerr << "There may exist some errors while load the obj file. Error content: [" << redundency << " ]" << std::endl;
				std::cerr << "Please note that we only support the faces drawing with triangles. There are more than three vertices in one face." << std::endl;
				std::cerr << "Your obj file can't be read properly by our simple parser :-( Try exporting with other options." << std::endl;
				exit(1);
			}

			for (int i = 0; i < 3; i++) {
				if (temp_vertices.find(vertices[i]) == temp_vertices.end()) {
					// the vertex never shows before
					Vertex vertex;
					vertex.position = temp_positions[vertices[i].index_position - 1];
					vertex.uv = temp_uvs[vertices[i].index_uv - 1];
					vertex.normal = temp_normals[vertices[i].index_normal - 1];

					model.vertices.push_back(vertex);
					model.indices.push_back(num_vertices);
					temp_vertices[vertices[i]] = num_vertices;
					num_vertices += 1;
				}
				else {
					// reuse the existing vertex
					unsigned int index = temp_vertices[vertices[i]];
					model.indices.push_back(index);
				}
			} // for
		} // else if
		else {
			// it's not a vertex, texture coordinate, normal or face
			char stupidBuffer[1024];
			file.getline(stupidBuffer, 1024);
		}
	}
	file.close();

	std::cout << "There are " << num_vertices << " vertices in the obj file.\n" << std::endl;
	return model;
}



//seaObject Attribute
GLuint seaVAO, seaVBO, seaEBO;
Model seaObj;
Texture seaTexture0, seaTexture1;

// dolphinObject Attribute
GLuint dolphinVAO, dolphinVBO, dolphinEBO;
Model dolphinObj;
Texture dolphinTexture0, dolphinTexture1;

//penguin
GLuint penguinVAO, penguinVBO, penguinEBO;
Model penguinObj;
Texture penguinTexture0;

GLuint grassVAO, grassVBO, grassEBO;
Model grassobj;
GLuint grassTexture0;

GLuint ccVAO, ccVBO, ccEBO;
Model ccobj;
GLuint ccTexture0;

GLuint catVAO, catVBO, catEBO;
Model catobj;
GLuint catTexture0, catTexture1;
void sendDataToOpenGL()
{
	//TODO:
	//Load objects and bind to VAO & VBO
	//Load texture
	
	seaObj = loadOBJ("resources/sea/sea.obj");
	glGenVertexArrays(1, &seaVAO);
	glBindVertexArray(seaVAO);
	//Create Vertex Buffer Objects
	glGenBuffers(1, &seaVBO);
	glBindBuffer(GL_ARRAY_BUFFER, seaVBO);
	glBufferData(GL_ARRAY_BUFFER, seaObj.vertices.size() * sizeof(Vertex), &seaObj.vertices[0], GL_STATIC_DRAW);
	//Create Element array Buffer Objects
	glGenBuffers(1, &seaEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, seaEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, seaObj.indices.size() * sizeof(unsigned int), &seaObj.indices[0], GL_STATIC_DRAW);
	// 1st attribute buffer : position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0, // attribute
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		sizeof(Vertex), // stride
		(void*)offsetof(Vertex, position) // array buffer offset
	);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1, // attribute
		2, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		sizeof(Vertex), // stride
		(void*)offsetof(Vertex, uv) // array buffer offset
	);
	seaTexture0.setupTexture("resources/sea/sea_01.jpg");
	seaTexture1.setupTexture("resources/sea/sea_02.jpg");
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2, // attribute
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		sizeof(Vertex), // stride
		(void*)offsetof(Vertex, normal) // array buffer offset
	);

	dolphinObj = loadOBJ("resources/dolphin/dolphin.obj");
	glGenVertexArrays(1, &dolphinVAO);
	glBindVertexArray(dolphinVAO);
	//Create Vertex Buffer Objects
	glGenBuffers(1, &dolphinVBO);
	glBindBuffer(GL_ARRAY_BUFFER, dolphinVBO);
	glBufferData(GL_ARRAY_BUFFER, dolphinObj.vertices.size() * sizeof(Vertex), &dolphinObj.vertices[0], GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, dolphinObj.vertices.size() * sizeof(Vertex), &dolphinObj.vertices[0], GL_STATIC_DRAW);
	//Create Element array Buffer Objects
	glGenBuffers(1, &dolphinEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dolphinEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, dolphinObj.indices.size() * sizeof(unsigned int), &dolphinObj.indices[0], GL_STATIC_DRAW);
	// 1st attribute buffer : position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0, // attribute
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		sizeof(Vertex), // stride
		(void*)offsetof(Vertex, position) // array buffer offset
	);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1, // attribute
		2, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		sizeof(Vertex), // stride
		(void*)offsetof(Vertex, uv) // array buffer offset
	);

	dolphinTexture0.setupTexture("resources/dolphin/dolphin_01.jpg");
	dolphinTexture1.setupTexture("resources/dolphin/dolphin_02.jpg");
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2, // attribute
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		sizeof(Vertex), // stride
		(void*)offsetof(Vertex, normal) // array buffer offset
	);

	
	penguinObj = loadOBJ("resources/penguin/PenguinBaseMesh.obj");
	glGenVertexArrays(1, &penguinVAO);
	glBindVertexArray(penguinVAO);
	//Create Vertex Buffer Objects
	glGenBuffers(1, &penguinVBO);
	glBindBuffer(GL_ARRAY_BUFFER, penguinVBO);
	glBufferData(GL_ARRAY_BUFFER, penguinObj.vertices.size() * sizeof(Vertex), &penguinObj.vertices[0], GL_STATIC_DRAW);
	//Create Element array Buffer Objects
	glGenBuffers(1, &penguinEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, penguinEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, penguinObj.indices.size() * sizeof(unsigned int), &penguinObj.indices[0], GL_STATIC_DRAW);
	// 1st attribute buffer : position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0, // attribute
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		sizeof(Vertex), // stride
		(void*)offsetof(Vertex, position) // array buffer offset
	);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1, // attribute
		2, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		sizeof(Vertex), // stride
		(void*)offsetof(Vertex, uv) // array buffer offset
	);
	penguinTexture0.setupTexture("resources/penguin/penguintexture.png");
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2, // attribute
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		sizeof(Vertex), // stride
		(void*)offsetof(Vertex, normal) // array buffer offset
	);
}

void matrix(string obj) {
	glm::mat4 modelTransformMatrix = glm::mat4(1.0f);
	glm::mat4 modelRotationMatrix = glm::mat4(1.0f);
	glm::mat4 modelScalingMatrix = glm::mat4(1.0f);
	unsigned int slot = 0;

	if (obj == "dolphin") {
		modelTransformMatrix = glm::translate(glm::mat4(), glm::vec3(dolphinPosX, -1.0f, dolphinPosY));
		modelScalingMatrix = glm::scale(glm::mat4(), glm::vec3(0.05f, 0.05f, 0.05f));
		modelRotationMatrix = glm::rotate(glm::mat4(), -14.0f, glm::vec3(1, 0, 0));
		modelRotationMatrix *= glm::rotate(glm::mat4(), dolphinRotate*dolphinDelta, glm::vec3(1, 0, 0));
	}
	else if (obj == "penguin") {
		modelTransformMatrix = glm::translate(glm::mat4(), glm::vec3(penguinPosX, -1.0f, penguinPosY));
		modelScalingMatrix = glm::scale(glm::mat4(), glm::vec3(3.05f, 3.05f, 3.05f));
		if (penguinSwim == 1)
			modelRotationMatrix = glm::rotate(glm::mat4(), -14.0f, glm::vec3(1, 0, 0));
		modelRotationMatrix *= glm::rotate(glm::mat4(), penguinRotate * penguinDelta, glm::vec3(0, 0, 1));
	}
	
	myShader.setMat4("modelTransformMatrix", modelTransformMatrix);
	myShader.setMat4("modelRotationMatrix", modelRotationMatrix);
	myShader.setMat4("modelScalingMatrix", modelScalingMatrix);

	camX = 20.0* sin(cos(glm::radians(yaw)) * cos(glm::radians(pitch)));
	camY = 20.0* sin(glm::radians(pitch));
	camZ = -20.0*cos(glm::radians(pitch)) * sin(glm::radians(yaw));

	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float)640/(float)480 , 0.1f, 100.0f);
	glm::mat4 Lookat = glm::lookAt(
		glm::vec3(camX, camY , camZ), //cam
		glm::vec3(0.0f, 5.0f, -5.0f), //look
		glm::vec3(0, 1, 0) 
	);
	glm::mat4 Tmp = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.5f, -5.0f));
	glm::mat4 ProjectionMatrix = Projection * Lookat * Tmp;
	myShader.setMat4("projectionMatrix", ProjectionMatrix);

	myShader.setInt("myTextureSampler0", 0);
	
	//Light Effect
	// Special light
	glm::vec4 ambientLight(0.1f * lightbrightness, 0.5f * lightbrightness, 0.1f *lightbrightness, 1.0f);
	myShader.setVec4("specialLight.ambiant", ambientLight);

	glm::mat4 rotationMat = glm::rotate(glm::mat4(),  0.00015f , glm::vec3(0, isRotate, 0));
	if (isRotate == 1) lightPosition = glm::vec3(rotationMat * glm::vec4(lightPosition, 1));
	myShader.setVec3("specialLight.lightPosition", lightPosition);

	glm::vec3 eyePosition(camX, camY, camZ);
	myShader.setVec3("specialLight.eyePosition", eyePosition);
	
	//direactional light
	glm::vec4 ambientLight0(0.1f * lightbrightness, 0.1f * lightbrightness, 0.1f * lightbrightness, 1.0f);
	myShader.setVec4("directionalLight.ambient", ambientLight0);

	glm::vec3 lightPosition0 = glm::vec3(dolphinPosX, 5.0f, dolphinPosY);
	myShader.setVec3("directionalLight.lightPosition", lightPosition0);

	glm::vec3 eyePosition0(camX, camY, camZ);
	myShader.setVec3("directionalLight.eyePosition", eyePosition0);

}
void paintGL(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//Deph test
	glClearDepth(1.0f);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	
	matrix("sea");
	glBindVertexArray(seaVAO);
	if (seaColor == 0)
		seaTexture0.bind(0);
	if (seaColor == 1)
		seaTexture1.bind(0);
	glDrawElements(GL_TRIANGLES, seaObj.indices.size(), GL_UNSIGNED_INT, 0);
	
	matrix("dolphin");
	glBindVertexArray(dolphinVAO);
	if (dolphinColor == 0)
		dolphinTexture0.bind(0);
	if (dolphinColor == 1)
		dolphinTexture1.bind(0);
	glDrawElements(GL_TRIANGLES, dolphinObj.indices.size(), GL_UNSIGNED_INT, 0);
	
	matrix("penguin");
	glBindVertexArray(penguinVAO);
	penguinTexture0.bind(0);
	glDrawElements(GL_TRIANGLES, penguinObj.indices.size(), GL_UNSIGNED_INT, 0);

}



void initializedGL(void) //run only once
{
	glewInit();
	
	myShader.setupShader("VertexShaderCode.glsl","FragmentShaderCode.glsl");
	myShader.use();
	sendDataToOpenGL();
}

int main(int argc, char* argv[])
{
	GLFWwindow* window;

	/* Initialize the glfw */
	if (!glfwInit()) {
		std::cout << "Failed to initialize GLFW" << std::endl;
		return -1;
	}
	/* glfw: configure; necessary for MAC */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Assignment 2", NULL, NULL);
	if (!window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/*register callback functions*/
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	
	initializedGL();

	while (!glfwWindowShouldClose(window)) {
		/* Render here */
		paintGL();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}

void mouseWheel_callback(int wheel, int direction, int x, int y)
{
	// Optional.
}