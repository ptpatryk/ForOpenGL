#include "WindowsWaveDirect.h"
//#include "WindowsWaveDirect.utility.cpp"
//
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

    aa = new Punkt[N_X * N_Y];
    for (int i = 0; i < N_X * N_Y; ++i) {
        aa[i].m = 1.0f;
        aa[i].v = 0.0f;
        aa[i].x = 0.0f;
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
    scd.BufferDesc.Width = width; // SzerokoúÊ okna
    scd.BufferDesc.Height = height; // WysokoúÊ okna
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hwnd;
    scd.SampleDesc.Count = 1;
    scd.SampleDesc.Quality = 0;
    scd.Windowed = TRUE;
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    scd.Flags = 0;

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

    hr = TworzenieBuforaGlebi();

    hr = TworzenieBuforaTylniego();

    // Ustawianie widoku bufora g≥Íbokoúci na kontekúcie urzπdzenia
    deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

    // Set viewport
    UstawienieViewPort();

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
    // Uzyskanie dostÍpu do bufora tylnego
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
    //macieø dla VS
    struct ConstantBuffer {
        DirectX::XMMATRIX WorldViewProjection;
    };

    ConstantBuffer cb;
    cb.WorldViewProjection = DirectX::XMMatrixIdentity(); // Przyk≥adowa macierz

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
    //koniecznie w destruktorze: vsBlob->Release();
    ID3DBlob* vsBlob = nullptr;
    HRESULT hr = D3DCompileFromFile(L"VertexShader.hlsl", nullptr, nullptr, "main", "vs_5_0", 0, 0, &vsBlob, nullptr);
    if (FAILED(hr)) {
        return hr;
    }

    hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader);
    if (FAILED(hr)) {
        return hr;
    }

    ID3DBlob* psBlob = nullptr;
    hr = D3DCompileFromFile(L"PixelShader.hlsl", nullptr, nullptr, "main", "ps_5_0", 0, 0, &psBlob, nullptr);
    if (FAILED(hr)) {
        return hr;
    }

    hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader);
    if (FAILED(hr)) {
        return hr;
    }

    deviceContext->VSSetShader(vertexShader, nullptr, 0);
    deviceContext->PSSetShader(pixelShader, nullptr, 0);

    //Layout wyjúciowy:

    D3D11_INPUT_ELEMENT_DESC layout[] = {
     { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
     { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    //hr = device->CreateInputLayout(layout, ARRAYSIZE(layout), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
    hr = device->CreateInputLayout(layout, ARRAYSIZE(layout), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
    if (FAILED(hr)) {
        // Obs≥uga b≥ÍdÛw
        return hr;
    }

    //todo: bez tej linijki to samo - nadal jeden punkt
    deviceContext->IASetInputLayout(inputLayout);


    return hr;

}

HRESULT WindowsWaveDirect::TworzenieBuforaGlebi()
{
    // Tworzenie zasobu bufora g≥Íbokoúci
    ID3D11Texture2D* depthStencilBuffer = nullptr;
    D3D11_TEXTURE2D_DESC depthStencilDesc = {};
    depthStencilDesc.Width = width; // SzerokoúÊ bufora
    depthStencilDesc.Height = height; // WysokoúÊ bufora
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    HRESULT hr = device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilBuffer);
    if (FAILED(hr)) {
        // Obs≥uga b≥ÍdÛw
        int i = 3;
    }
    else {
        hr = device->CreateDepthStencilView(depthStencilBuffer, nullptr, &depthStencilView);
        if (FAILED(hr)) {
            // Obs≥uga b≥ÍdÛw
            int i = 3;
        }
    }

    //nie wiem czy to coú potrzebne - z odkomentowanego kodu
    swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&depthStencilBuffer);
    device->CreateRenderTargetView(depthStencilBuffer, nullptr, &renderTargetView);


    return hr;
}

void WindowsWaveDirect::InitDirectCompute() {
    // Create compute shader
    //ID3DBlob* csBlob = nullptr;
    //D3DCompileFromFile(L"compute_shader.hlsl", NULL, NULL, "CSMain", "cs_5_0", 0, 0, &csBlob, NULL);
    //device->CreateComputeShader(csBlob->GetBufferPointer(), csBlob->GetBufferSize(), NULL, &computeShader);
    //csBlob->Release(); //TODO: co robi ta funkcja i czy powinienem jej uøywaÊ?


    //ID3D11ComputeShader* computeShader1;
    ID3DBlob* csBlob1 = nullptr;
    D3DCompileFromFile(L"ComputeShader.hlsl", nullptr, nullptr, "obliczWspolrzedne", "cs_5_0", 0, 0, &csBlob1, nullptr);
    device->CreateComputeShader(csBlob1->GetBufferPointer(), csBlob1->GetBufferSize(), nullptr, &computeShader1);
    csBlob1->Release();

    //ID3D11ComputeShader* computeShader2;
    ID3DBlob* csBlob2 = nullptr;
    D3DCompileFromFile(L"ComputeShader.hlsl", nullptr, nullptr, "obliczNormalne", "cs_5_0", 0, 0, &csBlob2, nullptr);
    device->CreateComputeShader(csBlob2->GetBufferPointer(), csBlob2->GetBufferSize(), nullptr, &computeShader2);

    //ID3D11ComputeShader* computeShader3;
    ID3DBlob* csBlob3 = nullptr;
    D3DCompileFromFile(L"ComputeShader.hlsl", nullptr, nullptr, "przygotujTrojkaty", "cs_5_0", 0, 0, &csBlob3, nullptr);
    device->CreateComputeShader(csBlob3->GetBufferPointer(), csBlob3->GetBufferSize(), nullptr, &computeShader3);

    // Create buffers
    CreateStructuredBuffer(device, sizeof(Punkt), N_X * N_Y, &aa[0], &aaBuffer);
    CreateStructuredBuffer(device, sizeof(Punkt), N_X * N_Y, nullptr, &bbBuffer);
    CreateStructuredBuffer(device, sizeof(Vertex), N_X * N_Y, nullptr, &clNbo);
    CreateStructuredBuffer(device, sizeof(float), N_X * N_Y * 36, nullptr, &vertexBuffer);  //nie jestem pewien czy ma byÊ ich razy 36 -- zamiast tego bufora zrobiÊ rysowanie po indeksach
    

    printf("Creating buffer views...");
    CreateBufferUAV(device, aaBuffer, &aaUAV);
    CreateBufferUAV(device, bbBuffer, &bbUAV);
    CreateBufferUAV(device, clNbo, &clUAV);
    CreateBufferUAV(device, vertexBuffer, &vertexUAV);

   
     
//blok ustawiajπce sta≥e w buforze ////////////////////////

    struct Constants
    {
        float dt;
        float w;
        int N_X;
        int N_Y;
        float zv;
        float czas;
    };

    
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(Constants);
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    Constants constants = { 0.016f, 1.0f, 100, 100, 0.5f, 0.0f }; // Przyk≥adowe wartoúci
    initData.pSysMem = &constants;

    HRESULT hr = device->CreateBuffer(&bufferDesc, &initData, &constantBuffer);
    if (FAILED(hr))
    {
        // Obs≥uga b≥Ídu
    }
////koniec tego bloku

}


////wstawka z trÛjkπtem -------------------------------------
//   // WspÛ≥rzÍdne wierzcho≥kÛw trÛjkπta
//struct XVertex {
//    DirectX::XMFLOAT3 position;
//};
//
//XVertex vertices[] = {
//    { DirectX::XMFLOAT3(0.0f,  0.5f, 0.0f) },  // Wierzcho≥ek gÛrny
//    { DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f) }, // Wierzcho≥ek lewy dolny
//    { DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f) }   // Wierzcho≥ek prawy dolny
//};
//
////koniec wstawki z trjkπtem
    // Tworzenie bufora wierzcho≥kÛw
    //D3D11_BUFFER_DESC vertexBufferDesc = {};
    //vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    //vertexBufferDesc.ByteWidth = sizeof(Vertex) * vertexCount;
    //vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    //vertexBufferDesc.CPUAccessFlags = 0;

   
    //ID3D11Buffer* vertexBuffer = this->vertexBuffer;
    //device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

void WindowsWaveDirect::OnRenderFrame() {


    // Czyszczenie ekranu
    float clearColor[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
    deviceContext->ClearRenderTargetView(renderTargetView, clearColor);
    deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    // Ustawianie shaderÛw
    deviceContext->VSSetShader(vertexShader, nullptr, 0);
    deviceContext->PSSetShader(pixelShader, nullptr, 0);

    // Ustawianie bufora wierzcho≥kÛw i layoutu wejúciowego
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    deviceContext->IASetInputLayout(inputLayout);

    // Ustawianie topologii prymitywÛw - jak to wykomÍtuje rysuje jeden punkt, moøe 2
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Rysowanie prymitywÛw
    //deviceContext->Draw(N_X * N_Y * 36, 0);
    deviceContext->Draw(3, 0);

    // Prezentacja
    swapChain->Present(0, 0);


    // Struktura wierzcho≥ka

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
        // Obs≥uga b≥ÍdÛw
    }

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    deviceContext->IASetVertexBuffers(0, 1, &vertexBufferX, &stride, &offset);

    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


    // Czyszczenie ekranu
    float clearColor[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
    deviceContext->ClearRenderTargetView(renderTargetView, clearColor);

    // Ustawienie shaderÛw
    deviceContext->VSSetShader(vertexShader, nullptr, 0);
    deviceContext->PSSetShader(pixelShader, nullptr, 0);

    // Ustawienie input layout
    deviceContext->IASetInputLayout(inputLayout);

    // Rysowanie trÛjkπta
    deviceContext->Draw(3, 0);

    // Prezentacja
    swapChain->Present(1, 0);

    */


    /*
     float clearColor[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
    deviceContext->ClearRenderTargetView(renderTargetView, clearColor);
    int vertexCount = N_X * N_Y;
 
    
    // Ustawienie bufora wierzcho≥kÛw
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    
    ////ustawienie bufora indeksÛw wierzcho≥kÛw:
    //UINT indices[] = { 0, 1, 2 };

    //D3D11_BUFFER_DESC ibd = {};
    //ibd.Usage = D3D11_USAGE_DEFAULT;
    //ibd.ByteWidth = sizeof(indices);
    //ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    //ibd.CPUAccessFlags = 0;

    //D3D11_SUBRESOURCE_DATA iinitData = {};
    //iinitData.pSysMem = indices;

    //ID3D11Buffer* indexBuffer = nullptr;
    //HRESULT hr = device->CreateBuffer(&ibd, &iinitData, &indexBuffer);
    //if (FAILED(hr)) {
    //    // Obs≥uga b≥ÍdÛw
    //}

    //deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    //
    ////koniec ustawienia bufora indeksu wierzcho≥kÛw
    //
    //deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //deviceContext->IASetInputLayout(inputLayout);

   

   



    // Rysowanie trÛjkπtÛw
    //deviceContext->DrawIndexed(indexCount, 0, 0);
   // deviceContext->DrawIndexed(3, 0, 0);

    deviceContext->Draw(3, 0);


    // Render your scene here

    swapChain->Present(1, 0);
   

    */

    //tu prÛbowa≥em uøyÊ innego bufora
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

    // Ustawienie bufora wierzcho≥kÛw
    UINT stride = sizeof(XVertex);
    UINT offset = 0;
    deviceContext->IASetVertexBuffers(0, 1, &vertexBufferX, &stride, &offset);

    float clearColor[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
    deviceContext->ClearRenderTargetView(renderTargetView, clearColor);


    // Rysowanie trÛjkπtÛw
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

void WindowsWaveDirect::RunKernel() {
    deviceContext->CSSetShader(computeShader1, NULL, 0);
    deviceContext->CSSetUnorderedAccessViews(0, 1, &aaUAV, NULL);
    deviceContext->CSSetUnorderedAccessViews(1, 1, &bbUAV, NULL);

    deviceContext->CSSetConstantBuffers(0, 1, &constantBuffer); // Dla compute shader

    deviceContext->Dispatch(N_X, N_Y, 1);

    //2

    deviceContext->CSSetShader(computeShader2, NULL, 0);
    deviceContext->CSSetUnorderedAccessViews(0, 1, &bbUAV, NULL);
    deviceContext->CSSetUnorderedAccessViews(1, 1, &clUAV, NULL);

    deviceContext->CSSetConstantBuffers(0, 1, &constantBuffer); // Dla compute shader

    deviceContext->Dispatch(N_X, N_Y, 1);

    //3

    deviceContext->CSSetShader(computeShader3, NULL, 0);
    deviceContext->CSSetUnorderedAccessViews(0, 1, &clUAV, NULL);
    deviceContext->CSSetUnorderedAccessViews(1, 1, &vertexUAV, NULL);

    deviceContext->CSSetConstantBuffers(0, 1, &constantBuffer); // Dla compute shader

    deviceContext->Dispatch(N_X, N_Y, 1);



    // Swap buffers
    std::swap(aaUAV, bbUAV);

    /*
	//odczytanie danych z bufora

    ID3D11Buffer* stagingBuffer = CreateAndCopyToDebugBuf(device, deviceContext, vertexBuffer);

    deviceContext->CopyResource(stagingBuffer, vertexBuffer);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = deviceContext->Map(stagingBuffer, 0, D3D11_MAP_READ, 0, &mappedResource);
    if (SUCCEEDED(hr))
    {
        // Uzyskaj wskaünik do danych
        float* dane = reinterpret_cast<float*>(mappedResource.pData);
        //Punkt[] dane = (Punkt[])mappedResource.pData;


        // Przetwarzaj dane
        // ...
		for (int i = 0; i < N_X; i++)
		{
			std::cout << dane[i] << std::endl;
		}   
        // Odmapuj bufor
        deviceContext->Unmap(stagingBuffer, 0);
    }
    else
    {
        // Obs≥uga b≥Ídu
    }
    */

    /*
    ID3D11Buffer* stagingBuffer = CreateAndCopyToDebugBuf(device, deviceContext, bbBuffer);

    deviceContext->CopyResource(stagingBuffer, bbBuffer);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = deviceContext->Map(stagingBuffer, 0, D3D11_MAP_READ, 0, &mappedResource);
    if (SUCCEEDED(hr))
    {
        // Uzyskaj wskaünik do danych
        Punkt * dane = reinterpret_cast<Punkt*>(mappedResource.pData);
        //Punkt[] dane = (Punkt[])mappedResource.pData;


        // Przetwarzaj dane
        // ...

        // Odmapuj bufor
        deviceContext->Unmap(stagingBuffer, 0);
    }
    else
    {
        // Obs≥uga b≥Ídu
    }

    */
	int i = 0;  
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

