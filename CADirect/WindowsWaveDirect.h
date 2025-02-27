#ifndef WINDOWSWAVEDIRECT_H
#define WINDOWSWAVEDIRECT_H

#include <string>
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <windows.h>
//#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>

using namespace DirectX;

class WindowsWaveDirect {
public:
    WindowsWaveDirect(int width, int height, const std::string& title);
    ~WindowsWaveDirect();

    void Run();

private:
    HRESULT InitDirectX();
    void InitDirectCompute();
    void OnRenderFrame();
    void OnRenderFrame(bool executeShaders);
    void OnUpdateFrame();
    void RunKernel();

    HRESULT VPShaderTworz();
    void UstawienieViewPort();
    HRESULT TworzenieBuforaTylniego();
    HRESULT UzupelnienieBuforuTrojkatemIBuforIndeksow();
    HRESULT BuforuIndeksowPlata();

    HRESULT UstawienieMaciezy();

    //funkcje pom
    HRESULT CreateStructuredBuffer(ID3D11Device* pDevice, UINT uElementSize, UINT uCount, void* pInitData, ID3D11Buffer** ppBufOut);
    HRESULT CreateBufferUAV(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11UnorderedAccessView** ppUAVOut);
    
	int width;
	int height;

    HWND hwnd;
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* deviceContext = nullptr;
    IDXGISwapChain* swapChain = nullptr;
    ID3D11RenderTargetView* renderTargetView = nullptr;
    ID3D11ComputeShader* computeShader1 = nullptr;
    ID3D11ComputeShader* computeShader2 = nullptr;
    ID3D11Buffer* aaBuffer = nullptr;
    ID3D11Buffer* bbBuffer = nullptr;
    ID3D11Buffer* vertexBuffer = nullptr;
    ID3D11UnorderedAccessView* aaUAV = nullptr;
    ID3D11UnorderedAccessView* bbUAV = nullptr;
    ID3D11UnorderedAccessView* vertexUAV = nullptr;
    ID3D11VertexShader* vertexShader = nullptr;
    ID3D11PixelShader* pixelShader = nullptr;
    ID3D11InputLayout* inputLayout = nullptr;

    ID3D11Buffer* constantBuffer = nullptr;

    ID3D11Buffer* CreateAndCopyToDebugBuf(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pd3dImmediateContext, _In_ ID3D11Buffer* pBuffer);

    

    //zmienne globalne z tutoriala
    ID3D11Texture2D* g_pDepthStencil = nullptr;
    ID3D11DepthStencilView* g_pDepthStencilView = nullptr;
    //ID3D11DepthStencilView* depthStencilView = nullptr; - to by�o do tej pory
    ID3D11Buffer* g_pIndexBuffer = nullptr;
    ID3D11Buffer* g_pConstantBuffer = nullptr;
    XMMATRIX                g_World;
    XMMATRIX                g_View;
    XMMATRIX                g_Projection;
    D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;


    struct Punkt {
        float m;
        float v;
        float x;
    };


    struct Vertex {
        DirectX::XMFLOAT3 Position;
        DirectX::XMFLOAT3 Normal;
    };

    struct Constants
    {
        float dt;
        float w;
        int N_X;
        int N_Y;
        float zv;
        float czas;
    };

    Punkt* aa = nullptr;
    std::vector<int> indices;
    void PrzygotujTablice();
    //float czas = 0.0f;
    float dt = 0.01f, w = -2.0f, czas = 0.0f;;
    int N_X = 400;
    int N_Y = 400;

    int liczmik = 0;


    struct ConstantBuffer
    {
        XMMATRIX mWorld;
        XMMATRIX mView;
        XMMATRIX mProjection;
        XMFLOAT4 vLightDir[2];
        XMFLOAT4 vLightColor[2];
        XMFLOAT4 vOutputColor;
    };
};

#endif // WINDOWSWAVEDIRECT_H