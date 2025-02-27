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

HRESULT WindowsWaveDirect::InitDirectX() {

	//------------------------------------------------------------------------------------
	// Create device and swap chain
	DXGI_SWAP_CHAIN_DESC scd = {};
	scd.BufferCount = 1;
	//scd.BufferDesc.Width = width; // Szeroko�� okna
	//scd.BufferDesc.Height = height; // Wysoko�� okna
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

	//---------------------------------------------------------------------------------------
	HRESULT hr;

	// Create a render target view
	hr = TworzenieBuforaTylniego();

	// Create depth stencil texture  - to by�o chyba robione przez: TworzenieBuforaGlebi()
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = device->CreateTexture2D(&descDepth, nullptr, &g_pDepthStencil);
	if (FAILED(hr))
		return hr;

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = device->CreateDepthStencilView(g_pDepthStencil, &descDSV, &g_pDepthStencilView);
	if (FAILED(hr))
		return hr;

	deviceContext->OMSetRenderTargets(1, &renderTargetView, g_pDepthStencilView);
	
	// Set viewport
	UstawienieViewPort();

	//shader
	hr = VPShaderTworz();

	BuforuIndeksowPlata();

	hr = UstawienieMaciezy();

	return S_OK;
}

HRESULT WindowsWaveDirect::TworzenieBuforaTylniego()
{
	// Uzyskanie dost�pu do bufora tylnego
	ID3D11Texture2D* backBuffer = nullptr;
	HRESULT hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
	if (FAILED(hr)) {
		return hr;
	}

	hr = device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
	backBuffer->Release();
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

	////todo: to w bloku render? - blok do wywalenia - a mo�e jednak tu zamiast w onrender
	//deviceContext->VSSetShader(vertexShader, nullptr, 0);
	//deviceContext->PSSetShader(pixelShader, nullptr, 0);

	return hr;
}


//na obliczone wieszcho�ki, p�niej:
HRESULT WindowsWaveDirect::BuforuIndeksowPlata()
{
	////ustawienie bufora indeks�w wierzcho�k�w: ----------------------
	//przyk�ad u�ycia od ai
	D3D11_BUFFER_DESC ibd = {};
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = static_cast<UINT>(indices.size() * sizeof(UINT)); // Poprawione 
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData = {};
	iinitData.pSysMem = indices.data(); // Poprawione przekazanie danych

	ID3D11Buffer* indexBuffer = nullptr;
	HRESULT hr = device->CreateBuffer(&ibd, &iinitData, &indexBuffer);
	if (FAILED(hr)) {
		return hr;
	}

	// Set index buffer
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set primitive topology
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return hr;
}

HRESULT WindowsWaveDirect::UstawienieMaciezy()
{
	D3D11_BUFFER_DESC bd = {};
	// Create the constant buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	HRESULT hr = device->CreateBuffer(&bd, nullptr, &g_pConstantBuffer);
	if (FAILED(hr))
		return hr;

	/*
	// Initialize the world matrices
	g_World = XMMatrixIdentity();

	// Initialize the view matrix
	XMVECTOR Eye = XMVectorSet(0.0f, 4.0f, -10.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	g_View = XMMatrixLookAtLH(Eye, At, Up);

	// Initialize the projection matrix
	g_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (FLOAT)height, 0.01f, 100.0f);
	*/

	//tu si� zastanowi� - do poprawki i mo�e w zwi�zku z tym zmodyfikowa� VS
	// Ustawienia macierzy
	//g_World = XMMatrixIdentity();
	g_World = XMMatrixRotationY(3.4f);

	// Dodanie translacji do macierzy �wiata
	XMMATRIX translationMatrixWorld = XMMatrixTranslation(150.0f, 0.0f, 0.0f);
	g_World = g_World * translationMatrixWorld;

	g_View = XMMatrixLookAtLH(XMVectorSet(0.0f, 0.0f, 3.0f, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

	// Ustawienie macierzy widoku
	g_View = XMMatrixLookAtLH(XMVectorSet(0.0f, 0.0f, 3.0f, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

	// Dodanie translacji do macierzy widoku
	XMMATRIX translationMatrixView = XMMatrixTranslation(-100.0f, -150.0f, 0.0f);
	g_View = translationMatrixView * g_View;

	// Ustawienie k�t�w obrotu
    constexpr float angleZ = XMConvertToRadians(45.0f);
	float angleX = XMConvertToRadians(30.0f);

	// Tworzenie macierzy obrotu
	XMMATRIX rotationMatrixZ = XMMatrixRotationZ(angleZ);
	XMMATRIX rotationMatrixX = XMMatrixRotationX(angleX);
	XMMATRIX rotationMatrix = rotationMatrixZ * rotationMatrixX;

	// Tworzenie macierzy rzutowania ortograficznego
	g_Projection = rotationMatrix * XMMatrixOrthographicLH(300.0f, 300.0f, -150.0f, 150.0f);
	//g_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (FLOAT)height, 0.01f, 100.0f);


//// Tworzenie macierzy rzutowania ortograficznego
//	XMMATRIX orthoMatrix = XMMatrixOrthographicLH(300.0f, 300.0f, -50.0f, 50.0f);
//
//	// Dodanie translacji, aby przesun�� pocz�tek uk�adu wsp�rz�dnych do lewego dolnego rogu
//	XMMATRIX translationMatrix = XMMatrixTranslation(-150.0f, -150.0f, 0.0f);
//
//	// Po��czenie macierzy rzutowania ortograficznego z translacj�
//	g_Projection = rotationMatrix * orthoMatrix * translationMatrix;

	// Setup our lighting parameters - pozmienia� �eby
	XMFLOAT4 vLightDirs[2] =
	{
		//XMFLOAT4(-0.577f, 0.577f, -0.577f, 1.0f),
		XMFLOAT4(5.2f, 5.0f, -30.0f, 1.0f), //ale b�d� wykorzystywa� tylko to
		XMFLOAT4(0.0f, 0.0f, -1.0f, 1.0f),
	};
	XMFLOAT4 vLightColors[2] =
	{
		XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f),
		XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f)
	};

//
// Update matrix variables and lighting variables
//
	ConstantBuffer cb1;
	cb1.mWorld = XMMatrixTranspose(g_World);
	cb1.mView = XMMatrixTranspose(g_View);
	cb1.mProjection = XMMatrixTranspose(g_Projection);

	cb1.vLightDir[0] = vLightDirs[0];
	cb1.vLightDir[1] = vLightDirs[1];
	cb1.vLightColor[0] = vLightColors[0];
	cb1.vLightColor[1] = vLightColors[1];
	cb1.vOutputColor = XMFLOAT4(0, 0, 0, 0);
	deviceContext->UpdateSubresource(g_pConstantBuffer, 0, nullptr, &cb1, 0, 0);


	return S_OK;
}

void WindowsWaveDirect::OnRenderFrame() {
	//
	// Clear the back buffer
	//
	deviceContext->ClearRenderTargetView(renderTargetView, Colors::MidnightBlue);

	//
	// Clear the depth buffer to 1.0 (max depth)
	//
	deviceContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	//
	// Render the cube - nie jestem pewny czy wystarczy jak w jednym miejscu b�d� ustawia� tylko te rzeczy
	//
	deviceContext->VSSetShader(vertexShader, nullptr, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
	deviceContext->PSSetShader(pixelShader, nullptr, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &g_pConstantBuffer);
	deviceContext->DrawIndexed(2*3*N_X*N_Y, 0, 0);

	swapChain->Present(0, 0);
}

void WindowsWaveDirect::OnRenderFrame(bool executeShaders) {
	//
	// Clear the back buffer
	//
	deviceContext->ClearRenderTargetView(renderTargetView, Colors::MidnightBlue);

	//
	// Clear the depth buffer to 1.0 (max depth)
	//
	deviceContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	if (executeShaders) {
		//
		// Render the cube
		//
		deviceContext->VSSetShader(vertexShader, nullptr, 0);
		deviceContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
		deviceContext->PSSetShader(pixelShader, nullptr, 0);
		deviceContext->PSSetConstantBuffers(0, 1, &g_pConstantBuffer);
	}

	deviceContext->DrawIndexed(2 * 3 * N_X * N_Y, 0, 0);

	swapChain->Present(0, 0);
}


void WindowsWaveDirect::OnUpdateFrame() {
	RunKernel();
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

