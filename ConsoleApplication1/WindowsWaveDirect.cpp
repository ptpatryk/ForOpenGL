#include "WindowsWaveDirect.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <math.h>
#include <chrono>
#include <thread>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

//ID3D11Buffer* CreateAndCopyToDebugBuf(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pd3dImmediateContext, _In_ ID3D11Buffer* pBuffer);

LRESULT CALLBACK WindowProcFor(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

WindowsWaveDirect::WindowsWaveDirect(int width, int height, const std::string& title) {

    PrzygotujTablice();

    // Register the window class
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProcFor;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"WindowsWaveClass";
    RegisterClass(&wc);

    this->width = width;
	this->height = height;

    // Create the window
    hwnd = CreateWindowEx(
        0,
        L"WindowsWaveClass",
        std::wstring(title.begin(), title.end()).c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        NULL,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );

    if (!hwnd) {
        std::cerr << "Failed to create window" << std::endl;
        exit(EXIT_FAILURE);
    }



    ShowWindow(hwnd, SW_SHOW);

    // Initialize DirectX
    HRESULT hr = InitDirectX();
    if (FAILED(hr))
    {
        int i = 4;
    }

    UzupelnienieBuforuTrojkatem();
    //InitDirectCompute();
}

WindowsWaveDirect::~WindowsWaveDirect() {
    // Clean up DirectX resources
    if (deviceContext) deviceContext->ClearState();
    if (swapChain) swapChain->Release();
    if (renderTargetView) renderTargetView->Release();
    if (deviceContext) deviceContext->Release();
    if (device) device->Release();
}

HRESULT WindowsWaveDirect::InitDirectX() {
    // Create device and swap chain
    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 1;
    //scd.BufferDesc.Width = width; // Szerokoœæ okna
    //scd.BufferDesc.Height = height; // Wysokoœæ okna
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hwnd;
    scd.SampleDesc.Count = 1;
    //scd.SampleDesc.Quality = 0;
    scd.Windowed = TRUE;
    //scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    //scd.Flags = 0;

    D3D11CreateDeviceAndSwapChain(
        NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        0,
        NULL,
        0,
        D3D11_SDK_VERSION,
        &scd,
        &swapChain,
        &device,
        NULL,
        &deviceContext
    );

    HRESULT hr;

    //hr = TworzenieBuforaGlebi();

    //hr = TworzenieBuforaTylniego();

     // Create render target view
    ID3D11Texture2D * backBuffer;
    swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
    device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
    backBuffer->Release();
    // Ustawianie widoku bufora g³êbokoœci na kontekœcie urz¹dzenia
    //deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
    deviceContext->OMSetRenderTargets(1, &renderTargetView, NULL);
    // Set viewport
    //UstawienieViewPort();

    hr = VPShaderTworz();

    UstawienieMaciezySwiata();

    return hr;
}

HRESULT WindowsWaveDirect::UzupelnienieBuforuTrojkatem()
{
    Vertex vertices[] = {
{ DirectX::XMFLOAT3(0.0f,  0.5f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) },
{ DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) },
{ DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) }
    };

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(vertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initDataX = {};
    initDataX.pSysMem = vertices;

    //ID3D11Buffer* vertexBuffer = nullptr;
    HRESULT hr = device->CreateBuffer(&bd, &initDataX, &vertexBuffer);
    if (FAILED(hr)) {
        return hr;
    }
    return hr;
}

HRESULT WindowsWaveDirect::TworzenieBuforaTylniego()
{
    // Uzyskanie dostêpu do bufora tylnego
    ID3D11Texture2D* backBuffer = nullptr;
    HRESULT hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
    if (FAILED(hr)) {
        return hr;
    }

    hr = device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
    //backBuffer->Release();
    if (FAILED(hr)) {
        return hr;
    }
    return hr;
}

void WindowsWaveDirect::UstawienieViewPort()
{
    D3D11_VIEWPORT viewport = {};
    viewport.Width = (FLOAT)width;
    viewport.Height = (FLOAT)height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    deviceContext->RSSetViewports(1, &viewport);

}

void WindowsWaveDirect::UstawienieMaciezySwiata()
{
    //macie¿ dla VS
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

HRESULT WindowsWaveDirect::VPShaderTworz()
{
    //VS ----------------------------------------------------------------------
    // Compile the vertex shader
    ID3DBlob* vsBlob = nullptr;
    HRESULT hr = D3DCompileFromFile(L"All.fxh", nullptr, nullptr, "VS", "vs_5_0", 0, 0, &vsBlob, nullptr);
    if (FAILED(hr)) {
        return hr;
    }

    // Create the vertex shader
    hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader);
    if (FAILED(hr))
    {
        vsBlob->Release();
        return hr;
    }

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] = {
     { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
     { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    // Create the input layout
    hr = device->CreateInputLayout(layout, ARRAYSIZE(layout), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
    if (FAILED(hr)) {
        return hr;
    }
	vsBlob->Release();

    deviceContext->IASetInputLayout(inputLayout);

	//PS ----------------------------------------------------------------------
	// Compile the pixel shader
    ID3DBlob* psBlob = nullptr;
    hr = D3DCompileFromFile(L"All.fxh", nullptr, nullptr, "PS", "ps_5_0", 0, 0, &psBlob, nullptr);
    if (FAILED(hr)) {
        return hr;
    }

	// Create the pixel shader
    hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader);
    if (FAILED(hr)) {
        return hr;
    }
	psBlob->Release();


    //todo: to w bloku render?
    deviceContext->VSSetShader(vertexShader, nullptr, 0);
    deviceContext->PSSetShader(pixelShader, nullptr, 0);



    return hr;

}

void WindowsWaveDirect::OnRenderFrame() {

    /*
    // Czyszczenie ekranu
    float clearColor[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
    deviceContext->ClearRenderTargetView(renderTargetView, clearColor);
    deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    // Ustawianie shaderów
    deviceContext->VSSetShader(vertexShader, nullptr, 0);
    deviceContext->PSSetShader(pixelShader, nullptr, 0);

    // Ustawianie bufora wierzcho³ków i layoutu wejœciowego
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    deviceContext->IASetInputLayout(inputLayout);

    // Ustawianie topologii prymitywów - jak to wykomêtuje rysuje jeden punkt, mo¿e 2
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Rysowanie prymitywów
    //deviceContext->Draw(N_X * N_Y * 36, 0);
    deviceContext->Draw(3, 0);

    // Prezentacja
    swapChain->Present(0, 0);

    */
    // Struktura wierzcho³ka

    /*
    Vertex vertices[] = {
    { DirectX::XMFLOAT3(0.0f,  0.5f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) },
    { DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) },
    { DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) }
    };

    
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(vertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices;

    ID3D11Buffer* vertexBufferX = nullptr;
    HRESULT hr = device->CreateBuffer(&bd, &initData, &vertexBufferX);
    if (FAILED(hr)) {
        // Obs³uga b³êdów
    }

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    deviceContext->IASetVertexBuffers(0, 1, &vertexBufferX, &stride, &offset);

    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


    // Czyszczenie ekranu
    float clearColor[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
    deviceContext->ClearRenderTargetView(renderTargetView, clearColor);

    // Ustawienie shaderów
    deviceContext->VSSetShader(vertexShader, nullptr, 0);
    deviceContext->PSSetShader(pixelShader, nullptr, 0);

    // Ustawienie input layout
    deviceContext->IASetInputLayout(inputLayout);

    // Rysowanie trójk¹ta
    deviceContext->Draw(3, 0);

    // Prezentacja
    swapChain->Present(1, 0);

    */


    
     float clearColor[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
    deviceContext->ClearRenderTargetView(renderTargetView, clearColor);
    int vertexCount = N_X * N_Y;
 
    
    // Ustawienie bufora wierzcho³ków
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    
    ////ustawienie bufora indeksów wierzcho³ków:
    //UINT indices[] = { 0, 1, 2 };
        //przyk³ad u¿ycia od ai

   
    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = static_cast<UINT>(indices.size() * sizeof(WORD)); // Poprawione 
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA iinitData = {};
    iinitData.pSysMem = indices.data(); // Poprawione przekazanie danych

    ID3D11Buffer* indexBuffer = nullptr;
    HRESULT hr = device->CreateBuffer(&ibd, &iinitData, &indexBuffer);
    if (FAILED(hr)) {
        // Obs³uga b³êdów
    }

	

    deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    
    //koniec ustawienia bufora indeksu wierzcho³ków
    
    //deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //deviceContext->IASetInputLayout(inputLayout);

   

   



    // Rysowanie trójk¹tów
    //deviceContext->DrawIndexed(indexCount, 0, 0);
   // deviceContext->DrawIndexed(3, 0, 0);

    deviceContext->Draw(3, 0);


    // Render your scene here

    swapChain->Present(1, 0);
   

    

    //tu próbowa³em u¿yæ innego bufora
    /*
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(vertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices;

    ID3D11Buffer* vertexBufferX = nullptr;
    device->CreateBuffer(&bd, &initData, &vertexBufferX);

    // Ustawienie bufora wierzcho³ków
    UINT stride = sizeof(XVertex);
    UINT offset = 0;
    deviceContext->IASetVertexBuffers(0, 1, &vertexBufferX, &stride, &offset);

    float clearColor[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
    deviceContext->ClearRenderTargetView(renderTargetView, clearColor);


    // Rysowanie trójk¹tów
    //deviceContext->DrawIndexed(indexCount, 0, 0);
    deviceContext->DrawIndexed(3, 0, 0);




    // Render your scene here

    swapChain->Present(1, 0);
    */
}

void WindowsWaveDirect::OnUpdateFrame() {
    //RunKernel();
    czas += dt;
}

void WindowsWaveDirect::Run() {
    MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            OnUpdateFrame();
            OnRenderFrame();
        }
    }
}

