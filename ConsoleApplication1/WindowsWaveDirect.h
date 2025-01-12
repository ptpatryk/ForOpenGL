#ifndef WINDOWSWAVEDIRECT_H
#define WINDOWSWAVEDIRECT_H

#include <string>
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

class WindowsWaveDirect {
public:
    WindowsWaveDirect(int width, int height, const std::string& title);
    ~WindowsWaveDirect();

    void Run();

private:
    HRESULT InitDirectX();
    void InitDirectCompute();
    void OnRenderFrame();
    void OnUpdateFrame();
    void RunKernel();

    HRESULT TworzenieBuforaGlebi();
    HRESULT VPShaderTworz();
    void UstawienieMaciezySwiata();
    void UstawienieViewPort();
    HRESULT TworzenieBuforaTylniego();
    HRESULT UzupelnienieBuforuTrojkatem();

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

    ID3D11DepthStencilView* depthStencilView = nullptr;

    struct Punkt {
        float m;
        float v;
        float x;
    };


    struct Vertex {
        DirectX::XMFLOAT3 Position;
        DirectX::XMFLOAT3 Normal;
    };


    Punkt* aa = nullptr;
    std::vector<WORD> indices;
    void PrzygotujTablice();
    float czas = 0.0f;
    float dt = 0.01f;
    int N_X = 100;
    int N_Y = 100;
};

#endif // WINDOWSWAVEDIRECT_H