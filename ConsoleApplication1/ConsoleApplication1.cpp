// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
//#include "Fale.h"
#include <thread>
#include <chrono>

//#include "WindowsWave.h"
#include "WindowsWaveDirect.h"


int main(int argc, char** argv) {
	//WindowsWave wave(1600, 1200, "WindowsWave");
	WindowsWaveDirect wave(1600, 1200, "WindowsWave");
	wave.Run();
	return 0;
	/*glutInit(&argc, argv);
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

	return 0;*/
}
