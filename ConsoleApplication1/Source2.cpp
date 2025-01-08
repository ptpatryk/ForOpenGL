#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <windows.h>
#include <tchar.h>

// Wspó³rzêdne wierzcho³ków trójk¹ta
struct Vertex {
    DirectX::XMFLOAT3 position;
};

Vertex vertices[] = {
    { DirectX::XMFLOAT3(0.0f,  0.5f, 0.0f) },  // Wierzcho³ek górny
    { DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f) }, // Wierzcho³ek lewy dolny
    { DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f) }   // Wierzcho³ek prawy dolny
};

// Funkcje pomocnicze do tworzenia urz¹dzenia i kontekstu DirectX
HRESULT InitDevice(HWND hWnd, ID3D11Device** device, ID3D11DeviceContext** context, IDXGISwapChain** swapChain, ID3D11RenderTargetView** renderTargetView) {
    // Tworzenie urz¹dzenia i kontekstu DirectX
    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 1;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hWnd;
    scd.SampleDesc.Count = 1;
    scd.Windowed = TRUE;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0,
        D3D11_SDK_VERSION, &scd, swapChain, device, nullptr, context
    );

    if (FAILED(hr)) {
        return hr;
    }

    // Tworzenie widoku renderowania
    ID3D11Texture2D* backBuffer = nullptr;
    (*swapChain)->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
    (*device)->CreateRenderTargetView(backBuffer, nullptr, renderTargetView);
    backBuffer->Release();

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
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    IDXGISwapChain* swapChain = nullptr;
    ID3D11RenderTargetView* renderTargetView = nullptr;
    if (FAILED(InitDevice(hWnd, &device, &context, &swapChain, &renderTargetView))) {
        return 0;
    }

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
            context->ClearRenderTargetView(renderTargetView, clearColor);

            // Ustawianie bufora wierzcho³ków
            UINT stride = sizeof(Vertex);
            UINT offset = 0;
            context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
            context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            // Rysowanie trójk¹ta
            context->Draw(3, 0);

            // Prezentacja
            swapChain->Present(0, 0);
        }
    }

    // Czyszczenie zasobów
    vertexBuffer->Release();
    renderTargetView->Release();
    swapChain->Release();
    context->Release();
    device->Release();
    UnregisterClass(wc.lpszClassName, wc.hInstance);

    return (int)msg.wParam;
}
