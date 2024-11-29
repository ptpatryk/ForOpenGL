//#include "Fale.h"

//---------------------------------------------------------------------------

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#pragma hdrstop

#include "Fale.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

Fale::Fale()
{
	//okresla sposob pobudzenia, 1 - fala kulista, 2 - fala plaska
	sp = SposobPlata::kolista;
	//	myCB = new TCheckBox(par);
	for (int i = 0; i < 2; i++)
	{
		plat[i] = new _punkt * [N_Y + 1];
		for (int ii = 0; ii < N_Y + 1; ii++) {
			plat[i][ii] = new _punkt[N_X + 1];
		}
	}

}

void Fale::InicjujStrune(Sposob sposob, float ile)
{
	//czas=0.0f;

	if (sposob == dwa_osrodki) czas = 0.0f;

	for (int i = 0; i < 2; i++)
	{
		struna[i] = new _punkt[N];

		for (int ii = 0; ii < N; ii++)
		{
			struna[i][ii].m = ((ii > 100) || sposob != dwa_osrodki ? 1 : 2);
			struna[i][ii].x = (sposob != dwa_osrodki ? sin((double)(ii - 1) * ile * M_PI / ((double)N - 2)) : 0);
			struna[i][ii].v = 0;
		}


	}


}

Fale::~Fale()
{
	for (int i = 0; i < 2; i++)
	{
		//plat[i] = new _punkt*[N_Y+1];
		for (int ii = 0; ii < N_Y + 1; ii++) {
			delete plat[i][ii];
		}
		delete plat[i];
	}

}

void Fale::akcja(void)
{
	// float a  = 0.1f;
	// float T  = 10.0f;
	float dt = dtk;
	int i, j;
	//float w = -T/a;
	float w = -2.0;
	float F;

	_punkt** aa; aa = plat[kol];
	_punkt** bb; bb = plat[!kol];

	Pobudz();

	//obliczam nowych wspolrzednych pumktow
	/*
	 for (i=1; i<N_X-1; i++)
	   for (j=1; j<N_Y-1; j++)
	   {
		F = w * (4*aa[i][j].x
					- aa[i][j+1].x - aa[i][j-1].x
					- aa[i+1][j].x - aa[i-1][j].x);
		bb[i][j].v = aa[i][j].v + F/aa[i][j].m * dt;
		bb[i][j].x = aa[i][j].x + bb[i][j].v * dt;
	   }
	*/
	for (i = 1; i < N_X - 1; i++)
		for (j = 1; j < N_Y - 1; j++)
		{
			F = w * (8 * aa[i][j].x
				- aa[i][j + 1].x - aa[i][j - 1].x
				- aa[i + 1][j].x - aa[i - 1][j].x
				- aa[i + 1][j + 1].x - aa[i + 1][j - 1].x
				- aa[i - 1][j - 1].x - aa[i - 1][j + 1].x
				);
			bb[i][j].v = aa[i][j].v + F / aa[i][j].m * dt;
			bb[i][j].x = aa[i][j].x + bb[i][j].v * dt;
		}

	//zatrzymanie krawedzi
	// if (sp==1)
	{
		for (i = 0; i < N_X; i++)
		{
			bb[i][0].x = 0.0;
			bb[i][N_Y].x = 0.0;
		}

		for (i = 0; i < N_Y; i++)
		{
			bb[0][i].x = 0.0;
			bb[N_X][i].x = 0.0;
		}
	}

}

//---------------------------------------------------------------------------


void Fale::akcjaStruna(void)
{
	// float a  = 0.1f;
	// float T  = 10.0f;
	float dt = 0.1f;
	int i, j;
	//float w = -T/a;
	float T = 10;
	float F;
	float a = 0.1f;


	_punkt* aa; aa = struna[kol];
	_punkt* bb; bb = struna[!kol];

	PobudzStrune();
	for (j = 2; j < N - 1; j++)
	{
		F = -T / a * (2 * aa[j].x - aa[j + 1].x - aa[j - 1].x);
		bb[j].v = aa[j].v + F / aa[j].m * dt;
		bb[j].x = aa[j].x + bb[j].v * dt;
	}

	//zatrzymanie krawedzi
	bb[N - 1].x = 0;

}

//---------------------------------------------------------------------------


void Fale::Inicjuj(int a)
{
	int i, j, k, ii;
	for (k = 0; k < 2; k++)
		for (i = 0; i < N_X; i++)
			for (j = 0; j < N_Y; j++)
			{
				plat[k][i][j].m = 1.0f;
				plat[k][i][j].v = 0;
				plat[k][i][j].x = 0;
			}

	czas = 0.0;
	kol = 0;

}

//---------------------------------------------------------------------------

void Fale::InicjujPlat(SposobPlata sposob)
{
	czas = 0.0f;
	sp = sposob;
	int i, j, k, ii;
	//warunkowe ustawienie osrodka niejednorodnego
	if (sposob == plaska)
		for (k = 0; k < 2; k++)
			for (i = N_X / 2, ii = 1; i < N_X; i++, ii++)
				for (j = N_Y; j > N_Y - ii; j--)
					plat[k][i][j].m = 2.0f;
	else
	{
		for (k = 0; k < 2; k++)
			for (i = 0; i < N_X; i++)
				for (j = 0; j < N_Y; j++)
				{
					plat[k][i][j].m = 1.0f;
					plat[k][i][j].v = 0;
					plat[k][i][j].x = 0;
				}

	}

}
//---------------------------------------------------------------------------

void Fale::Pobudz(void)
{
	_punkt** aa; aa = plat[kol];
	int i;

	if (sp == kolista)
	{
		if (czas < 4 * M_PI)
			aa[50][50].v = sin(czas);
		else
		{
			if (czas < 5 * M_PI)
				aa[50][50].v = 0.0f;
		}
		//	else aa[50][50].v = 0.0f;
	}

	if (sp == plaska)
	{
		if (czas < 4 * M_PI)
			for (i = 0; i < N_Y; i++)
				aa[1][i].v = 0.2 * sin(czas);
		else for (i = 0; i < N_Y; i++)
			aa[1][i].v = 0.0f;

	}
}

//---------------------------------------------------------------------------

void Fale::PobudzStrune()
{
	_punkt* aa; aa = struna[kol];
	int i;

	if (czas < M_PI)
		aa[1].x = sin(czas);
	else
	{
		aa[1].x = 0.0f;
	}
}

//--------------------------------------------------------------------------

void Fale::Dzialaj(void)
{

	akcja();

	kol = !kol; // stare = nowe
	czas += dtk;
}

_punkt** Fale::PogiezPlat()
{
	Dzialaj();

	return plat[kol];
}

_punkt* Fale::PobiezStrune()
{

	kol = !kol; // stare = nowe
	czas += dtk;
	akcjaStruna();
	return struna[kol];
}

//---------------------------------------------------------------------------


