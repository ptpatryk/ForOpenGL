// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
//#include "Fale.h"
#include <thread>
#include <chrono>

#include "WindowsWave.h"



int main(int argc, char** argv) {
	WindowsWave wave(1600, 1200, "WindowsWave");
	//WindowsWaveDirect wave(1600, 1200, "WindowsWave");
	wave.Run();
	return 0;
}
