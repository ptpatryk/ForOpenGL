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


LRESULT CALLBACK WindowProcFor(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

//--------------------------------------------------------------------------------------
// Create Structured Buffer
//--------------------------------------------------------------------------------------
_Use_decl_annotations_
HRESULT CreateStructuredBuffer(ID3D11Device* pDevice, UINT uElementSize, UINT uCount, void* pInitData, ID3D11Buffer** ppBufOut)
{
    *ppBufOut = nullptr;

    D3D11_BUFFER_DESC desc = {};
    desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
    desc.ByteWidth = uElementSize * uCount;
    desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    desc.StructureByteStride = uElementSize;

    if (pInitData)
    {
        D3D11_SUBRESOURCE_DATA InitData;
        InitData.pSysMem = pInitData;
        return pDevice->CreateBuffer(&desc, &InitData, ppBufOut);
    }
    else
        return pDevice->CreateBuffer(&desc, nullptr, ppBufOut);
}
//--------------------------------------------------------------------------------------
// Create Unordered Access View for Structured or Raw Buffers
//-------------------------------------------------------------------------------------- 
_Use_decl_annotations_
HRESULT CreateBufferUAV(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11UnorderedAccessView** ppUAVOut)
{
    D3D11_BUFFER_DESC descBuf = {};
    pBuffer->GetDesc(&descBuf);

    D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
    desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    desc.Buffer.FirstElement = 0;

    if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
    {
        // This is a Raw Buffer

        desc.Format = DXGI_FORMAT_R32_TYPELESS; // Format must be DXGI_FORMAT_R32_TYPELESS, when creating Raw Unordered Access View
        desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
        desc.Buffer.NumElements = descBuf.ByteWidth / 4;
    }
    else
        if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
        {
            // This is a Structured Buffer

            desc.Format = DXGI_FORMAT_UNKNOWN;      // Format must be must be DXGI_FORMAT_UNKNOWN, when creating a View of a Structured Buffer
            desc.Buffer.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
        }
        else
        {
            return E_INVALIDARG;
        }

    return pDevice->CreateUnorderedAccessView(pBuffer, &desc, ppUAVOut);
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
    InitDirectX();
    InitDirectCompute();
}


WindowsWaveDirect::~WindowsWaveDirect() {
    // Clean up DirectX resources
    if (deviceContext) deviceContext->ClearState();
    if (swapChain) swapChain->Release();
    if (renderTargetView) renderTargetView->Release();
    if (deviceContext) deviceContext->Release();
    if (device) device->Release();
}

void WindowsWaveDirect::InitDirectX() {
    // Create device and swap chain
    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 1;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hwnd;
    scd.SampleDesc.Count = 1;
    scd.Windowed = TRUE;

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

    // Create render target view
    ID3D11Texture2D* backBuffer;
    swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
    device->CreateRenderTargetView(backBuffer, NULL, &renderTargetView);
    backBuffer->Release();

    deviceContext->OMSetRenderTargets(1, &renderTargetView, NULL);

    // Set viewport
    D3D11_VIEWPORT viewport = {};
    viewport.Width = (FLOAT)width;
    viewport.Height = (FLOAT)height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    deviceContext->RSSetViewports(1, &viewport);
}

void WindowsWaveDirect::InitDirectCompute() {
    // Create compute shader
    //ID3DBlob* csBlob = nullptr;
    //D3DCompileFromFile(L"compute_shader.hlsl", NULL, NULL, "CSMain", "cs_5_0", 0, 0, &csBlob, NULL);
    //device->CreateComputeShader(csBlob->GetBufferPointer(), csBlob->GetBufferSize(), NULL, &computeShader);
    //csBlob->Release(); //TODO: co robi ta funkcja i czy powinienem jej u¿ywaæ?

    //ID3D11ComputeShader* computeShader1;
    ID3DBlob* csBlob1 = nullptr;
    D3DCompileFromFile(L"ComputeShader.hlsl", nullptr, nullptr, "obliczWspolrzedne", "cs_5_0", 0, 0, &csBlob1, nullptr);
    device->CreateComputeShader(csBlob1->GetBufferPointer(), csBlob1->GetBufferSize(), nullptr, &computeShader1);

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
    CreateStructuredBuffer(device, sizeof(PunktNormal), N_X * N_Y, nullptr, &aaBuffer);
    CreateStructuredBuffer(device, sizeof(float), N_X * N_Y * 36, nullptr, &aaBuffer);  //nie jestem pewien czy ma byæ ich razy 36

    /*
    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    uavDesc.Format = DXGI_FORMAT_UNKNOWN;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer.FirstElement = 0;
    uavDesc.Buffer.NumElements = N_X * N_Y;

    device->CreateUnorderedAccessView(aaBuffer, &uavDesc, &aaUAV);
    device->CreateUnorderedAccessView(bbBuffer, &uavDesc, &bbUAV);
    */
    //todo: zamiast tego to: - ale i tak nie wiem dlaczego musi byæ to bufor uav?
    CreateBufferUAV(device, aaBuffer, &aaUAV);
    CreateBufferUAV(device, bbBuffer, &bbUAV);

}

void WindowsWaveDirect::OnRenderFrame() {
    float clearColor[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
    deviceContext->ClearRenderTargetView(renderTargetView, clearColor);

    // Render your scene here

    swapChain->Present(1, 0);
}

void WindowsWaveDirect::OnUpdateFrame() {
    RunKernel();
    czas += dt;
}

void WindowsWaveDirect::RunKernel() {
    deviceContext->CSSetShader(computeShader1, NULL, 0);
    deviceContext->CSSetUnorderedAccessViews(0, 1, &aaUAV, NULL);
    deviceContext->CSSetUnorderedAccessViews(1, 1, &bbUAV, NULL);

    deviceContext->Dispatch(N_X, N_Y, 1);

    // Swap buffers
    std::swap(aaUAV, bbUAV);
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