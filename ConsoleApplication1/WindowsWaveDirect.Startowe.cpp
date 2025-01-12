#include "WindowsWaveDirect.h"

void WindowsWaveDirect::PrzygotujTablice()
{
    //wartoœci pocz¹tkowe punktów na p³aszczyŸnie
    aa = new Punkt[N_X * N_Y];
    for (int i = 0; i < N_X * N_Y; ++i) {
        aa[i].m = 1.0f;
        aa[i].v = 0.0f;
        aa[i].x = 0.0f;
    }

    //tablice indeksów trójk¹tów dla punktów na p³aszczyŸnie
    for (int i = 0; i < N_Y - 1; ++i) {
        for (int j = 0; j < N_X - 1; ++j) {
            // First triangle
            indices.push_back(i * N_Y + j);
            indices.push_back(i * N_Y + (j + 1));
            indices.push_back((i + 1) * N_Y + j);

            // Second triangle
            indices.push_back((i + 1) * N_Y + j);
            indices.push_back(i * N_Y + (j + 1));
            indices.push_back((i + 1) * N_Y + (j + 1));
        }
    }

    //// Print indices for verification
    //for (size_t i = 0; i < indices.size(); i += 3) {
    //    std::cout << "Triangle " << i / 3 << ": ";
    //    std::cout << indices[i] << ", " << indices[i + 1] << ", " << indices[i + 2] << std::endl;
    //}
}