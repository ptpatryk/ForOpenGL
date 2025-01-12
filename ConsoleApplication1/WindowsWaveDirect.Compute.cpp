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


    printf("Creating buffer views...");
    CreateBufferUAV(device, aaBuffer, &aaUAV);
    CreateBufferUAV(device, bbBuffer, &bbUAV);
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

void WindowsWaveDirect::RunKernel() {
    deviceContext->CSSetShader(computeShader1, NULL, 0);
    deviceContext->CSSetUnorderedAccessViews(0, 1, &aaUAV, NULL);
    deviceContext->CSSetUnorderedAccessViews(1, 1, &bbUAV, NULL);

    deviceContext->CSSetConstantBuffers(0, 1, &constantBuffer); // Dla compute shader

    deviceContext->Dispatch(N_X, N_Y, 1);

    //2

    deviceContext->CSSetShader(computeShader2, NULL, 0);
    deviceContext->CSSetUnorderedAccessViews(0, 1, &bbUAV, NULL);
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

