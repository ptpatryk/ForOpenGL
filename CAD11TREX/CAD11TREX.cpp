#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <windows.h>
#include <tchar.h>

#include <fstream>
#include <vector>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")


// Wspó³rzêdne wierzcho³ków trójk¹ta
struct Vertex {
    DirectX::XMFLOAT3 position;
};

Vertex vertices[] = {
    { DirectX::XMFLOAT3(0.0f,  0.5f, 0.0f) },  // Wierzcho³ek górny
    { DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f) }, // Wierzcho³ek lewy dolny
    { DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f) }   // Wierzcho³ek prawy dolny
};

ID3D11Device* device = nullptr;
ID3D11DeviceContext* deviceContext = nullptr;
void initX();

// Funkcje pomocnicze do tworzenia urz¹dzenia i kontekstu DirectX
HRESULT InitDevice(HWND hWnd, ID3D11Device** deviceG, ID3D11DeviceContext** context, IDXGISwapChain** swapChain, ID3D11RenderTargetView** renderTargetView) {
    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 1;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hWnd;
    scd.SampleDesc.Count = 1;
    scd.Windowed = TRUE;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0,
        D3D11_SDK_VERSION, &scd, swapChain, deviceG, nullptr, context
    );

    if (FAILED(hr)) {
        return hr;
    }

    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = 800;
    viewport.Height = 600;
    (*context)->RSSetViewports(1, &viewport);

    ID3D11Texture2D* backBuffer = nullptr;
    hr = (*swapChain)->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
    if (FAILED(hr)) {
        return hr;
    }

    hr = (*deviceG)->CreateRenderTargetView(backBuffer, nullptr, renderTargetView);
    backBuffer->Release();
    if (FAILED(hr)) {
        return hr;
    }

    (*context)->OMSetRenderTargets(1, renderTargetView, nullptr);

    return S_OK;
}

// Funkcja g³ówna
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Inicjalizacja okna
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, DefWindowProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, _T("DirectXExample"), nullptr };
    RegisterClassEx(&wc);
    HWND hWnd = CreateWindow(wc.lpszClassName, _T("Rysowanie trójk¹ta"), WS_OVERLAPPEDWINDOW, 100, 100, 800, 600, nullptr, nullptr, wc.hInstance, nullptr);

    // Inicjalizacja DirectX
    //ID3D11Device* device = nullptr;
    //ID3D11DeviceContext* context = nullptr;
    IDXGISwapChain* swapChain = nullptr;
    ID3D11RenderTargetView* renderTargetView = nullptr;
    if (FAILED(InitDevice(hWnd, &device, &deviceContext, &swapChain, &renderTargetView))) {
        return 0;
    }

    initX();

    // Tworzenie bufora wierzcho³ków
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(vertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices;

    ID3D11Buffer* vertexBuffer = nullptr;
    device->CreateBuffer(&bd, &initData, &vertexBuffer);

    // Pêtla renderuj¹ca
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);


    MSG msg = { 0 };
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            // Czyszczenie ekranu
            float clearColor[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
            deviceContext->ClearRenderTargetView(renderTargetView, clearColor);

            // Ustawianie bufora wierzcho³ków
            UINT stride = sizeof(Vertex);
            UINT offset = 0;
            deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
            deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            // Rysowanie trójk¹ta
            deviceContext->Draw(3, 0);

            // Prezentacja
            swapChain->Present(0, 0);
        }
    }

    // Czyszczenie zasobów
    vertexBuffer->Release();
    renderTargetView->Release();
    swapChain->Release();
    deviceContext->Release();
    device->Release();
    UnregisterClass(wc.lpszClassName, wc.hInstance);

    return (int)msg.wParam;
}

void initX()
{
    //Upewnij siê, ¿e shadery s¹ kompilowane i ustawiane w kodzie:
    ID3D11VertexShader* vertexShader = nullptr;
    ID3D11PixelShader* pixelShader = nullptr;
    ID3D11InputLayout* inputLayout = nullptr;

    //koniecznie w destruktorze: vsBlob->Release();
    ID3DBlob* vsBlob = nullptr;
    HRESULT hr = D3DCompileFromFile(L"VertexShader.hlsl", nullptr, nullptr, "VS", "vs_5_0", 0, 0, &vsBlob, nullptr);
    if (FAILED(hr)) {
        // Obs³uga b³êdów
    }

    hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader);
    if (FAILED(hr)) {
        // Obs³uga b³êdów
        int tt = 3;
    }

    ID3DBlob* psBlob = nullptr;
    hr = D3DCompileFromFile(L"PixelShader.hlsl", nullptr, nullptr, "PS", "ps_5_0", 0, 0, &psBlob, nullptr);
    if (FAILED(hr)) {
        // Obs³uga b³êdów
        int yy = 3;
    }

    hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader);
    if (FAILED(hr)) {
        // Obs³uga b³êdów
        int yy = 3;
    }
    

    // Kompilacja i tworzenie shaderów
    // ... (kod do kompilacji shaderów)

    deviceContext->VSSetShader(vertexShader, nullptr, 0);
    deviceContext->PSSetShader(pixelShader, nullptr, 0);



    D3D11_INPUT_ELEMENT_DESC layout[] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    hr = device->CreateInputLayout(layout, ARRAYSIZE(layout), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
    if (FAILED(hr)) {
        // Obs³uga b³êdów
		int yy = 3;
    }

    deviceContext->IASetInputLayout(inputLayout);

    //utworzenie sta³ego bufora w shaderze
    //context->VSSetConstantBuffers(0, 1, &constantBuffer); // Dla vertex shader
    //context->PSSetConstantBuffers(0, 1, &constantBuffer); // Dla pixel shader

    //SprawdŸ, czy bufor sta³ych jest poprawnie ustawiony: Upewnij siê, ¿e bufor sta³ych jest poprawnie ustawiony i aktualizowany:
    struct ConstantBuffer {
        DirectX::XMMATRIX WorldViewProjection;
    };

    ConstantBuffer cb;
    cb.WorldViewProjection = DirectX::XMMatrixIdentity(); // Przyk³adowa macierz

    D3D11_BUFFER_DESC cbd = {};
    cbd.Usage = D3D11_USAGE_DEFAULT;
    cbd.ByteWidth = sizeof(ConstantBuffer);
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = &cb;

    ID3D11Buffer* constantBuffer = nullptr;
    device->CreateBuffer(&cbd, &initData, &constantBuffer);

    deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);

}

