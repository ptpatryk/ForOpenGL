#ifndef WINDOWSWAVEDIRECT_H
#define WINDOWSWAVEDIRECT_H

#include <string>
#include <d3d11.h>
#include <DirectXMath.h>

class WindowsWaveDirect {
public:
    WindowsWaveDirect(int width, int height, const std::string& title);
    ~WindowsWaveDirect();

    void Run();

private:
    void InitDirectX();
    void InitDirectCompute();
    void OnRenderFrame();
    void OnUpdateFrame();
    void RunKernel();

	int width;
	int height;

    HWND hwnd;
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* deviceContext = nullptr;
    IDXGISwapChain* swapChain = nullptr;
    ID3D11RenderTargetView* renderTargetView = nullptr;
    ID3D11ComputeShader* computeShader1 = nullptr;
    ID3D11ComputeShader* computeShader2 = nullptr;
    ID3D11ComputeShader* computeShader3 = nullptr;
    ID3D11Buffer* aaBuffer = nullptr;
    ID3D11Buffer* bbBuffer = nullptr;
    ID3D11Buffer* clNbo = nullptr;
    ID3D11Buffer* vertexBuffer = nullptr;
    ID3D11UnorderedAccessView* aaUAV = nullptr;
    ID3D11UnorderedAccessView* bbUAV = nullptr;

    struct Punkt {
        float m;
        float v;
        float x;
    };

    struct PunktNormal {
        float x, y, z, nx, ny, nz;
    };

    Punkt* aa = nullptr;
    float czas = 0.0f;
    float dt = 0.01f;
    int N_X = 100;
    int N_Y = 100;
};

#endif // WINDOWSWAVEDIRECT_H