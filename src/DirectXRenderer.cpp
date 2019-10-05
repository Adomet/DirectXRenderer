#define GLFW_EXPOSE_NATIVE_WIN32
#include <iostream>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <assert.h>
#include <d3d11.h>
#include <d3dcompiler.h>


class DirectXRenderer {


	ID3D11InputLayout* m_layout = 0;
	ID3D11PixelShader* m_ps = NULL;
	ID3D11VertexShader* m_vs = NULL;
	ID3DBlob* m_psblob = NULL;
	ID3DBlob* m_vsblob = NULL;
	ID3D11Buffer* vertex_buffer = 0;
	ID3D11RenderTargetView* renderTargetView;
	UINT monitorRefreshRate = 74;//hz
	UINT windowWidth = 1280, windowHeight = 720;


	bool clearRenderTargetColor(ID3D11DeviceContext* deviceContext, ID3D11RenderTargetView* renderView, FLOAT clear_color[])
	{
		deviceContext->ClearRenderTargetView(renderView, clear_color);
		deviceContext->OMSetRenderTargets(1, &renderView, NULL);
		return true;
	}

	bool present(bool vsync, IDXGISwapChain* swapchain)
	{
		swapchain->Present(vsync, NULL);
		return true;
	}

	void setVertexBuffer(ID3D11DeviceContext* deviceContext, UINT vertex_size)
	{
		UINT stride = vertex_size;
		UINT offset = 0;
		deviceContext->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
		deviceContext->IASetInputLayout(m_layout);
	}


	void load(ID3D11Device* device, void* list_vertices, UINT size_vertex, UINT size_list,void* shader_byte_code,UINT size_shader)
	{

		if (vertex_buffer)vertex_buffer->Release();
		if (m_layout)m_layout->Release();

		D3D11_BUFFER_DESC buff_desc = {};
		buff_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buff_desc.Usage = D3D11_USAGE_DEFAULT;
		buff_desc.ByteWidth = size_vertex * size_list;
		buff_desc.CPUAccessFlags = 0;
		buff_desc.MiscFlags = 0;
		buff_desc.StructureByteStride = size_vertex;

		D3D11_SUBRESOURCE_DATA init_data = {};
		init_data.pSysMem = list_vertices;


		if (FAILED(device->CreateBuffer(&buff_desc, &init_data, &vertex_buffer)))
		{
			printf("vertexbuffer creatiion failed");
		}

		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			//SEMANTIC NAME - SEMANTIC INDEX - FORMAT - INPUT SLOT - ALIGNED BYTE OFFSET - INPUT SLOT CLASS - INSTANCE DATA STEP RATE
			{"POSITION", 0,  DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,D3D11_INPUT_PER_VERTEX_DATA ,0},
			{ "COLOR", 0,  DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,D3D11_INPUT_PER_VERTEX_DATA ,0 }
		};

		UINT size_layout = ARRAYSIZE(layout);

		// get them

		if (FAILED(device->CreateInputLayout(layout, size_layout, shader_byte_code, size_shader, &m_layout)))
		{
			printf("failed to create Input Layout");
		}
	}

	bool createandSetShaders(ID3D11Device* device, ID3D11DeviceContext* devicecontext)
	{

		ID3DBlob* errblob = nullptr;
		//D3DCompileFromFile(L"VertexShader.hlsl", nullptr, nullptr, "vsmain", "vs_5_0", NULL, NULL, &m_vsblob, &errblob);
		//D3DCompileFromFile(L"PixelShader.hlsl", nullptr, nullptr, "psmain", "ps_5_0", NULL, NULL, &m_psblob, &errblob);
		D3DCompileFromFile(L"shader.fx", nullptr, nullptr, "vsmain", "vs_5_0", NULL, NULL, &m_vsblob, &errblob);
		D3DCompileFromFile(L"shader.fx", nullptr, nullptr, "psmain", "ps_5_0", NULL, NULL, &m_psblob, &errblob);

		device->CreateVertexShader(m_vsblob->GetBufferPointer(), m_vsblob->GetBufferSize(), nullptr, &m_vs);
		device->CreatePixelShader(m_psblob->GetBufferPointer(), m_psblob->GetBufferSize(), nullptr, &m_ps);

		return true;
	}

	bool setShaders(ID3D11DeviceContext* devicecontext, ID3D11VertexShader* m_vs, ID3D11PixelShader* m_ps)
	{
		devicecontext->VSSetShader(m_vs, nullptr, 0);
		devicecontext->PSSetShader(m_ps, nullptr, 0);

		return true;
	}


	void drawTriangleList(UINT vertex_count, UINT start_vertex_index, ID3D11DeviceContext* deviceContext)
	{
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		deviceContext->Draw(vertex_count, start_vertex_index);

	}

	void setViewportSize(UINT width, UINT height, ID3D11DeviceContext* deviceContext)
	{

		D3D11_VIEWPORT vp;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		vp.Width = (float)width;
		vp.Height = (float)height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		deviceContext->RSSetViewports(1, &vp);
	}

	struct vec3
	{
		float	x, y, z;
	};

	struct vertex
	{
		vec3 positions;
		vec3 color;
	};

	GLFWwindow* createWindow(UINT windowWidth, UINT windowHeight,HWND* windowhandle)
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		GLFWwindow* window = glfwCreateWindow(windowWidth,windowHeight, "Rogue DirectX Renderer", NULL, NULL);
		assert(window);

		*windowhandle = glfwGetWin32Window(window);

		return window;
	}

	IDXGISwapChain* createSwapchain(HWND windowhandle, ID3D11Device* device)
	{

		IDXGIDevice* dxgiDevice = 0;
		IDXGIAdapter* dxgiAdapter = 0;
		IDXGIFactory* dxgiFactory = 0;
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

		device->QueryInterface(__uuidof(IDXGIDevice), (void**)& dxgiDevice);
		dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)& dxgiAdapter);
		dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)& dxgiFactory);


		HRESULT swapchainCreateResult = dxgiFactory->CreateSwapChain(device, &desc, &swapchain);

		if (SUCCEEDED(swapchainCreateResult))
		{
			printf("Succeded to create Swapchain\n");
		}

		else if (FAILED(swapchainCreateResult))
		{
			printf("Failed to create Swapchain\n");
		}

		return swapchain;
	}

	void drawTriangle(ID3D11DeviceContext* deviceContext,UINT size_list)
	{
		setViewportSize(windowWidth, windowHeight, deviceContext);
		setShaders(deviceContext, m_vs, m_ps);
		setVertexBuffer(deviceContext, sizeof(vertex));
		drawTriangleList(size_list, 0, deviceContext);
	}

	ID3D11Device* createDevice(ID3D11DeviceContext** pdeviceContext)
	{

		ID3D11Device* device = 0;
		D3D_FEATURE_LEVEL passedFeatrureLevel;
		ID3D11DeviceContext* deviceContext = 0;

		D3D_DRIVER_TYPE driverTypes[] =
		{
		   D3D_DRIVER_TYPE_HARDWARE,
		   D3D_DRIVER_TYPE_WARP,
		   D3D_DRIVER_TYPE_REFERENCE
		};

		UINT driverTypeIndex = ARRAYSIZE(driverTypes);


		D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

		UINT numFeatureLevels = ARRAYSIZE(featureLevels);

		HRESULT res = 0;

		UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
		// If the project is in a debug build, enable the debug layer.
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		for (UINT driverTypeIndex = 0; driverTypeIndex < numFeatureLevels; driverTypeIndex++)
		{
			HRESULT res = D3D11CreateDevice(NULL, driverTypes[driverTypeIndex], NULL, creationFlags, featureLevels
				, numFeatureLevels, D3D11_SDK_VERSION, &device, &passedFeatrureLevel, &deviceContext);

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

		*pdeviceContext = deviceContext;

		return device;
	}


public: void main()
	{
		std::cout << "Hello DirectX\n";

		int rc = glfwInit();
		assert(rc);


		HWND windowhandle=NULL;
		GLFWwindow* window = createWindow(windowWidth, windowHeight,&windowhandle);

		//Create Device
		ID3D11DeviceContext* deviceContext = 0;
		ID3D11Device* device = createDevice(&deviceContext);	


		//Swapchain

		IDXGISwapChain* swapchain = 0;
		swapchain = createSwapchain(windowhandle, device);

		//FrameBuffer
		ID3D11Texture2D* buffer = nullptr;
		HRESULT bf = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)& buffer);

		if (FAILED(bf))
		{
			printf("Buffer creation failed");
		}


		HRESULT rtc = device->CreateRenderTargetView(buffer, NULL, &renderTargetView);

		buffer->Release();
		if (FAILED(rtc))
		{
			printf("RenderTargetView creation failed");
		}

		//Clear color
		FLOAT clear_color[] = { 0.4f, 0.4f, 0.4f, 1.0f };

		//Vertex List
		vertex list[] =
		{
			{-0.5f,-0.5f,0.0f,    1.0f,0.0f,0.0f},
			{ 0.0f, 0.5f,0.0f,    0.0f,1.0f,0.0f},
			{ 0.5f,-0.5f,0.0f,    0.0f,0.0f,1.0f}
		};
		UINT size_list = ARRAYSIZE(list);


		//Create shaders and vertex buffer
		createandSetShaders(device, deviceContext);
		load(device, list, sizeof(vertex), size_list, m_vsblob->GetBufferPointer(), m_vsblob->GetBufferSize());

		while (!glfwWindowShouldClose(window))
		{
			//Render
			clearRenderTargetColor(deviceContext, renderTargetView, clear_color);
			drawTriangle(deviceContext, size_list);
			//Present to screen
			present(false, swapchain);
			Sleep(1);
			glfwPollEvents();
		}

		//@TODO: Release The buffers and properly exit from code
		glfwDestroyWindow(window);
		glfwTerminate();

	}
};

//Entry Point
int main()
{
	DirectXRenderer renderer;
	
	renderer.main();

	return 0;
}