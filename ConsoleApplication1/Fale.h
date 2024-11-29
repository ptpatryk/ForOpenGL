
#ifndef FaleH
#define FaleH


#include <math.h>
//#include <stdio.h>
#include <stdlib.h>
//#include <cmath>


#define         N_X                 100
#define         N_Y                 100
#define			N					200
#define         dtk                 0.1f

//---------------------------------------------------------------------------
struct _punkt { float m, v, x; };
enum Sposob { dwa_osrodki, stojaca, atom };
enum SposobPlata { kolista, plaska, dyfin };


#pragma once
class Fale
{
private:
	//	_punkt plat[2][N_X][N_Y];
	//	_punkt ***plat;
	_punkt** plat[2];
	_punkt* struna[2];
	//(_punkt(*)[100])
	_punkt** PogiezPlat();
	_punkt* PobiezStrune();
	//okresla sposob pobudzenia, 1 - fala kulista, 2 - fala plaska
	SposobPlata sp;
	float czas; //okresla czas pobudzenia
	//zmienna okresla ktora tablica plata jest biezaca
	int kol;
public:
	Fale();
	~Fale();
	void akcja(void);       //oblicza parametry plata biezacego po czasie 'dt'
	void akcjaStruna(void);
	void Inicjuj(int);      //ustawia parametry poczatkowe platow (0-dla f. kulistej, 1-dla zalamania)
	void InicjujStrune(Sposob sposob, float ile);
	void InicjujPlat(SposobPlata sposob);
	void Pobudz(void);       //pobudz przez okreslony czas powieszchnie plata
	void PobudzStrune();
	void Dzialaj(void);     //widac w implementacji
	//_punkt** GetPlat = { PogiezPlat };
	//_punkt* GetStruna = { PobiezStrune };
};


#endif
