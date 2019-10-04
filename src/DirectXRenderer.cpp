#define GLFW_EXPOSE_NATIVE_WIN32
#include <iostream>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <assert.h>
#include <d3d11.h>


int main()
{
    std::cout << "Hello DirectX\n";

	int rc = glfwInit();
	assert(rc);


	UINT monitorRefreshRate = 74;//hz

	UINT windowWidth = 1280,windowHeight=720;


	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(640, 480, "Rogue DirectX Renderer", NULL, NULL);
	assert(window);


    HWND windowhandle =  glfwGetWin32Window(window);

	D3D_DRIVER_TYPE driverTypes[] =
	{
	   D3D_DRIVER_TYPE_HARDWARE,
	   D3D_DRIVER_TYPE_WARP,
	   D3D_DRIVER_TYPE_REFERENCE
	};

	UINT driverTypeIndex = ARRAYSIZE(driverTypes);


	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0};

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);



	ID3D11Device* device                      =0;
	D3D_FEATURE_LEVEL passedFeatrureLevel;
	ID3D11DeviceContext* deviceContext		  =0;


	HRESULT res = 0;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numFeatureLevels; driverTypeIndex++)
	{
		HRESULT res =D3D11CreateDevice(NULL,driverTypes[driverTypeIndex],NULL,NULL,featureLevels
			,numFeatureLevels,D3D11_SDK_VERSION,&device,&passedFeatrureLevel, &deviceContext);

		if (SUCCEEDED(res))
		{
			printf("Succeded to craete Directx Device\n");
			break;
		}
	}

	if (FAILED(res))
	{
		//@TODO:Make Error Handling
		printf("failed to Create DirectX Device!\n");
	}

	assert(device);
	assert(passedFeatrureLevel);
	assert(deviceContext);


	//Swapchain
	IDXGIDevice* dxgiDevice=0;
	IDXGIAdapter* dxgiAdapter = 0;
	IDXGIFactory* dxgiFactory=0;
	IDXGISwapChain* swapchain = 0;

	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BufferCount = 1;
	desc.BufferDesc.Width = windowWidth;
	desc.BufferDesc.Height = windowHeight;
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferDesc.RefreshRate.Numerator = monitorRefreshRate;
	desc.BufferDesc.RefreshRate.Denominator = 1;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.OutputWindow = windowhandle;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Windowed = TRUE;

	device->QueryInterface(__uuidof(IDXGIDevice),(void**)&dxgiDevice);
	dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);


	HRESULT swapchainCreateResult= dxgiFactory->CreateSwapChain(device,&desc,&swapchain);

	if (SUCCEEDED(swapchainCreateResult))
	{
		printf("Succeded to create Swapchain\n");
	}

	else if(FAILED(swapchainCreateResult))
	{
		printf("Failed to create Swapchain\n");
	}





	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}


	glfwDestroyWindow(window);

	glfwTerminate();


	return 0;
}
