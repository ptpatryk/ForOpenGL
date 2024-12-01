// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "Fale.h"
#include <thread>
#include <chrono>
//struct _punkt { float m, v, x; };

_punkt** plat[2];

_punkt** aa;

float DiffuseLight1[] = { 1.0f, 1.0f, 1.0f, 1.0f };
float LightPosition1[] = { 0.0f, 20.0f, 180.0f, 1.0f };
float SpotDirection1[] = { 0.0f, 0.0f, 1.0f };
float mat[] = { 1.0f, 1.0f, 1.0f, 1.0f };
float mat2[] = { 0.0f, 1.0f, 0.0f, 1.0f };

typedef struct _Vertex3f {
	float x, y, z;
	float nx, ny, nz;
} Vertex3f;

float skala = 0.02f;

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

void GLCalculateNormals(float Ax, float Ay, float Az,
	float Bx, float By, float Bz,
	float Cx, float Cy, float Cz,
	Vertex3f& VN
)
{

	Vertex3f vA, vB, vN;
	float vecLen;

	vA.x = Ax - Cx;
	vA.y = Ay - Cy;
	vA.z = Az - Cz;

	vB.x = Bx - Cx;
	vB.y = By - Cy;
	vB.z = Bz - Cz;

	// 			vN = vA x vB

	vN.x = (vA.y * vB.z) - (vA.z * vB.y);
	vN.y = (vA.z * vB.x) - (vA.x * vB.z);
	vN.z = (vA.x * vB.y) - (vA.y * vB.x);

	//        	normalizacja

	vecLen = sqrt((vN.x * vN.x) + (vN.y * vN.y) + (vN.z * vN.z));

	VN.nx = vN.x / vecLen;
	VN.ny = vN.y / vecLen;
	VN.nz = vN.z / vecLen;

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

GLuint shaderProgram;

// Funkcja do rysowania trójkąta
void display() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	
	glUseProgram(shaderProgram);
	// Ustawienia o�wietlenia
	GLint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
	GLint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
	GLint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
	GLint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
	glUniform3f(lightPosLoc, 1.0f, 1.0f, 1.0f);
	glUniform3f(viewPosLoc, 0.0f, 0.0f, 3.0f);
	glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
	glUniform3f(objectColorLoc, 5.0f, 1.0f, 0.31f); // Kolor tr�jk�ta

	// Definicja macierzy
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

	// Przesy�anie macierzy do shader-a
	GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
	GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
	GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	
	int i, j;

	model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Obrót
	model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 0.0f)); // Przesuni�cie
	model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.05f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	Vertex3f VN;

	for (i = 1; i < N_X; i++)
		for (j = 1; j < N_Y; j++)
		{
			
			//GLCalculateNormals(i, j, 10*aa[i][j].x,
			//	i + 1, j, 10*aa[i + 1][j].x,
			//	i, j + 1, 10*aa[i][j + 1].x,
			//	VN
			//);

			//glNormal3f(VN.nx, VN.ny, VN.nz);
			//glNormal3f(1, 1, 1);
			glBegin(GL_TRIANGLES);
			glVertex3f(0 + i, 1 + j, 10 * aa[i][j + 1].x);
			glVertex3f(0 + i, 0 + j, 10 * aa[i][j].x);
			glVertex3f(1 + i, 0 + j, 10 * aa[i + 1][j].x);
			glEnd();
		}

	for (i = 1; i < N_X; i++)
		for (j = 1; j < N_Y; j++)
		{
			//GLCalculateNormals(i, j, 10*aa[i][j].x,
			//	i + 1, j, 10*aa[i + 1][j].x,
			//	i, j + 1, 10*aa[i][j + 1].x,
			//	VN
			//);

			//glNormal3f(-VN.nx, -VN.ny, -VN.nz);
			//glNormal3f(-1, -1, -1);
			glBegin(GL_TRIANGLES);
			glVertex3f(0 + i, 1 + j, aa[i][j + 1].x);
			glVertex3f(1 + i, 1 + j, aa[i + 1][j + 1].x);
			glVertex3f(1 + i, 0 + j, aa[i + 1][j].x);
			glEnd();
		}

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
// Funkcja inicjalizuj�ca



void init() {
	shaderProgram = createShaderProgram("vertex_shader.glsl", "fragment_shader.glsl");
	
	glEnable(GL_DEPTH_TEST);
	//glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

	glShadeModel(GL_SMOOTH);//d

	//	glShadeModel(GL_FLAT);//w

	/*glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseLight1);
	glLightfv(GL_LIGHT0, GL_POSITION, LightPosition1);

	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 10.0f);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 120.0f);

	glEnable(GL_LIGHT0);*/
}



void GLRenderer(int osx, int osy, int osz)
{
	
	//TO teraz wykomentowalem bo nie mam bladego pojecia po co mi to:
	//	int i, j;
	//	_punkt (*aa)[N_Y] = plat[kol];

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//   glLoadIdentity(); //tego nie bylo


	glPushMatrix();

	//	glRotatef (45,0,0,-1); //tymczasowo wyciete
	//	glRotatef (70,1,0,0);  //tymczasowo wyciete

	glRotatef(osx, 0, 0, -1); //tymczasowo wyciete
	glRotatef(osy, 1, 0, 0);  //tymczasowo wyciete
	glRotatef(osz, 0, 1, 0);  //tymczasowo wyciete

	glTranslatef(-1.0f, -1.0f, 0.0f);
	glScalef(skala, skala, 0.3f);

	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat);

	//tymczasowo wyciete

   //if (!text)

	//DrawInTrigersX();

	glPopMatrix();
	//SwapBuffers(hDC);
	glutSwapBuffers();
	//	Sleep (1);
	
}


void SetPlat(_punkt** dane, int osx, int osy, int osz)
{
	//glEnable(GL_LIGHTING);

	//	glDisable(GL_LIGHTING);
	aa = dane;

	glutPostRedisplay();

	//GLRenderer(osx, osy, osz);
}

Fale* myfale;

void threadFunction() {
	while (true) {
		SetPlat(myfale->PogiezPlat(), 50, 30, 40);
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

}


int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("3D Triangle with Shaders");

	glewInit();
	init();

	

	myfale = new Fale();
	myfale->Inicjuj(1);
	myfale->InicjujStrune(dwa_osrodki, 1);

	aa=myfale->PogiezPlat();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

std::thread myThread(threadFunction);

	glutMainLoop();
	myThread.join();

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
