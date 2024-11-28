// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


std::string loadShaderSource(const char* filePath) {
	std::ifstream file(filePath);
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}
GLuint compileShader(GLenum type, const char* source) {
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);
		std::cerr << "Error compiling shader: " << infoLog << std::endl;
	}
	return shader;
}
GLuint createShaderProgram(const char* vertexPath, const char* fragmentPath) {
	std::string vertexCode = loadShaderSource(vertexPath);
	std::string fragmentCode = loadShaderSource(fragmentPath);
	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexCode.c_str());
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentCode.c_str());
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	GLint success;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
		std::cerr << "Error linking shader program: " << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return shaderProgram;
}
// Funkcja do rysowania trójk¹ta
void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	GLuint shaderProgram = createShaderProgram("vertex_shader.glsl", "fragment_shader.glsl");
	glUseProgram(shaderProgram);
	// Ustawienia oœwietlenia
	GLint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
	GLint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
	GLint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
	GLint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
	glUniform3f(lightPosLoc, 1.0f, 1.0f, 1.0f);
	glUniform3f(viewPosLoc, 0.0f, 0.0f, 3.0f);
	glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
	glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f); // Kolor trójk¹ta
	glBegin(GL_TRIANGLES);
	glVertexAttrib3f(0, 0.0f, 1.0f, 0.0f);
	glVertexAttrib3f(0, -1.0f, -1.0f, 0.0f);
	glVertexAttrib3f(0, 1.0f, -1.0f, 0.0f);
	glEnd();
	glutSwapBuffers();
}
// Funkcja do ustawienia perspektywy
void reshape(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
	glMatrixMode(GL_MODELVIEW);
}
// Funkcja inicjalizuj¹ca
void init() {
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("3D Triangle with Shaders");
	glewInit();
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
