#include <GL/glew.h>

	GLuint compileShader(GLenum type, const char* source) {
		GLint success;
	GLuint createShaderProgram(const char* vertexPath, const char* fragmentPath) {
		GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexCode.c_str());
		GLuint shaderProgram = glCreateProgram();
		GLint success;
		glDeleteShader(vertexShader);
		return shaderProgram;
	// Funkcja do rysowania tr�jk�ta
		GLuint shaderProgram = createShaderProgram("vertex_shader.glsl", "fragment_shader.glsl");
		// Ustawienia o�wietlenia
		glUniform3f(lightPosLoc, 1.0f, 1.0f, 1.0f);
		glBegin(GL_TRIANGLES);
		glutSwapBuffers();
	// Funkcja do ustawienia perspektywy
	// Funkcja inicjalizuj�ca
	
		glewInit();
		glutDisplayFunc(display);
//};