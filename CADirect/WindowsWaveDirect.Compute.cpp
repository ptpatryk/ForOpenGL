#include "WindowsWaveDirect.h"
#include <d3dcompiler.h>

void WindowsWaveDirect::InitDirectCompute() {
    ID3DBlob* csBlob1 = nullptr;
    D3DCompileFromFile(L"ComputeShader.hlsl", nullptr, nullptr, "obliczWspolrzedne", "cs_5_0", 0, 0, &csBlob1, nullptr);
    device->CreateComputeShader(csBlob1->GetBufferPointer(), csBlob1->GetBufferSize(), nullptr, &computeShader1);
    csBlob1->Release();

    //ID3D11ComputeShader* computeShader2;
    ID3DBlob* csBlob2 = nullptr;
    D3DCompileFromFile(L"ComputeShader.hlsl", nullptr, nullptr, "obliczNormalne", "cs_5_0", 0, 0, &csBlob2, nullptr);
    device->CreateComputeShader(csBlob2->GetBufferPointer(), csBlob2->GetBufferSize(), nullptr, &computeShader2);
    csBlob2->Release();

    // Create buffers
    CreateStructuredBuffer(device, sizeof(Punkt), N_X * N_Y, &aa[0], &aaBuffer);
    CreateStructuredBuffer(device, sizeof(Punkt), N_X * N_Y, nullptr, &bbBuffer);
    CreateStructuredBuffer(device, sizeof(Vertex), N_X * N_Y, nullptr, &vertexBuffer);

    //-------------------------------------------------------------
    //wstawka dla wyúwietlania
        // Set vertex buffer
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

    //-------------------------------------------------------------


    printf("Creating buffer views...");
    CreateBufferUAV(device, aaBuffer, &aaUAV);
    CreateBufferUAV(device, bbBuffer, &bbUAV);
    CreateBufferUAV(device, vertexBuffer, &vertexUAV);


    //blok ustawiajπce sta≥e w buforze ////////////////////////

    //zwiÍkszyÊ rozmiar aby rozmia ry≥ wielokrotnoúciπ 16 majtÛw - bo bufor musi byÊ wielokrotnoúciπ 16 bajtÛw, a jak ≥aduje 32 bajty to mogÍ odwo≥aÊ siÍ do z≥ej pamiÍci



    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = 32; // sizeof(Constants); - bo to rÛwan siÍ 24 - przez zwiÍkszony rozmiar moøe siÍ wykrzaczyÊ!!
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    //Constants constants = { 0.016f, 1.0f, 100, 100, 0.5f, 0.0f }; // Przyk≥adowe wartoúci
    Constants constants;
    constants.czas = czas;
    constants.dt = dt;
    constants.N_X = N_X;
    constants.N_Y = N_Y;
    constants.w = w;
    constants.zv = 0.0f; //tu wartoúÊ cos zaleøna od czasu


    initData.pSysMem = &constants;

    HRESULT hr = device->CreateBuffer(&bufferDesc, &initData, &constantBuffer);
    if (FAILED(hr))
    {
        //const char* errorString = DXGetErrorString(hr);
        //const char* errorDescription = DXGetErrorDescription(hr);
        //printf("Error: %s - %s\n", errorString, errorDescription);
        int t = 99;
        // Obs≥uga b≥Ídu
    }
    ////koniec tego bloku

}

void WindowsWaveDirect::RunKernel() {

    //float mysin = sin(czas);

    Constants constants;
    constants.czas = czas;
    constants.dt = dt;
    constants.N_X = N_X;
    constants.N_Y = N_Y;
    constants.w = w;
    constants.zv = sin(czas);

    deviceContext->UpdateSubresource(constantBuffer, 0, nullptr, &constants, 0, 0);

    deviceContext->CSSetShader(computeShader1, NULL, 0);
    deviceContext->CSSetUnorderedAccessViews(0, 1, &aaUAV, NULL);
    deviceContext->CSSetUnorderedAccessViews(1, 1, &bbUAV, NULL);

    deviceContext->CSSetConstantBuffers(1, 1, &constantBuffer); // Dla compute shader

    deviceContext->Dispatch(N_X, N_Y, 1);

    //2

    deviceContext->CSSetShader(computeShader2, NULL, 0);
    deviceContext->CSSetUnorderedAccessViews(0, 1, &bbUAV, NULL);
    deviceContext->CSSetUnorderedAccessViews(1, 1, &vertexUAV, NULL);

    deviceContext->CSSetConstantBuffers(1, 1, &constantBuffer); // Dla compute shader

    deviceContext->Dispatch(N_X, N_Y, 1);


    // Swap buffers
    std::swap(aaUAV, bbUAV);

    
    //odczytanie danych z bufora - wyciek by≥ jak czyta≥em dane z bufora

    //ID3D11Buffer* stagingBuffer = CreateAndCopyToDebugBuf(device, deviceContext, vertexBuffer);

    //deviceContext->CopyResource(stagingBuffer, vertexBuffer);

    //D3D11_MAPPED_SUBRESOURCE mappedResource;
    //HRESULT hr = deviceContext->Map(stagingBuffer, 0, D3D11_MAP_READ, 0, &mappedResource);
    //if (SUCCEEDED(hr))
    //{
    //    // Uzyskaj wskaünik do danych
    //    Vertex* dane = reinterpret_cast<Vertex*>(mappedResource.pData);
    //    //Punkt[] dane = (Punkt[])mappedResource.pData;


    //    // Przetwarzaj dane
    //    // ...
    //     for (int i = 0; i < N_X; i++)
    //    {
    //        //std::cout << dane[i] << std::endl;
    //    }
    //    // Odmapuj bufor
    //    deviceContext->Unmap(stagingBuffer, 0);
    //}
    //else
    //{
    //    // Obs≥uga b≥Ídu
    //}

    int i = 0;
}

