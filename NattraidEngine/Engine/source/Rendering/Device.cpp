#include "Rendering/Device.h"

Device::~Device() {
	destroy();
}

void
Device::destroy() {
	SafeRelease(m_device);
}

HRESULT
Device::CreateRenderTargetView(ID3D11Resource* pResource,
	const D3D11_RENDER_TARGET_VIEW_DESC* pDesc,
	ID3D11RenderTargetView** ppRTView) {
	if (ppRTView == nullptr) {
		ERROR("Device", "CreateRenderTargetView", "Output pointer is null.");
		return E_POINTER;
	}

	*ppRTView = nullptr; // Initialize the output pointer to nullptr

	if (m_device == nullptr || pResource == nullptr) {
		ERROR("Device", "CreateRenderTargetView", "Device or resource pointer is null.");
		return E_INVALIDARG; // Device not initialized
	}

	HRESULT result = m_device->CreateRenderTargetView(pResource, pDesc, ppRTView);

	if (FAILED(result)) {

		ERROR("Device",
			"CreateRenderTargetView",
			("Failed to create render target view. HRESULT: " + std::to_string(result)).c_str());
		return result;
	}

	MESSAGE("Device", "CreateRenderTargetView", "SUCCES");
	return result;
}

HRESULT
Device::CreateTexture2D(const D3D11_TEXTURE2D_DESC* pDesc,
	const D3D11_SUBRESOURCE_DATA* pInitialData,
	ID3D11Texture2D** ppTexture2D) {
	if (ppTexture2D == nullptr) {
		ERROR("Device", "CreateTexture2D", "Output pointer is null.");
		return E_POINTER;
	}

	*ppTexture2D = nullptr; // Initialize the output pointer to nullptr

	if (m_device == nullptr || pDesc == nullptr) {
		ERROR("Device", "CreateTexture2D", "Device or description pointer is null.");
		return E_INVALIDARG;
	}

	HRESULT result = m_device->CreateTexture2D(pDesc, pInitialData, ppTexture2D);

	if (FAILED(result)) {
		ERROR("Device", "CreateTexture2D", ("Failed to create texture 2D. HRESULT: " + std::to_string(result)).c_str());
		return result;
	}

	MESSAGE("Device", "CreateTexture2D", "SUCCESS");
	return result;
}

HRESULT
Device::CreateDepthStencilView(ID3D11Resource* pResource,
	D3D11_DEPTH_STENCIL_VIEW_DESC* pDesc,
	ID3D11DepthStencilView** ppDepthStencilView) {
	if (ppDepthStencilView == nullptr) {
		ERROR("Device", "CreateDepthStencilView", "Output pointer is null.");
		return E_POINTER;
	}

	*ppDepthStencilView = nullptr; // Initialize the output pointer to nullptr

	if (m_device == nullptr || pResource == nullptr) {
		ERROR("Device", "CreateDepthStencilView", "Device or resource pointer is null.");
		return E_INVALIDARG;
	}

	HRESULT result = m_device->CreateDepthStencilView(pResource, pDesc, ppDepthStencilView);

	if (FAILED(result)) {
		ERROR("Device", "CreateDepthStencilView", ("Failed to create depth stencil view. HRESULT: " + std::to_string(result)).c_str());
		return result;
	}

	MESSAGE("Device", "CreateDepthStencilView", "SUCCESS");
	return result;
}

HRESULT
Device::CreateVertexShader(const void* pShaderBytecode,
	unsigned int BytecodeLength,
	ID3D11ClassLinkage* pClassLinkage,
	ID3D11VertexShader** ppVertexShader) {
	if (ppVertexShader == nullptr) {
		ERROR("Device", "CreateVertexShader", "Output pointer is null.");
		return E_POINTER;
	}

	*ppVertexShader = nullptr; // Initialize the output pointer to nullptr

	if (m_device == nullptr || pShaderBytecode == nullptr || BytecodeLength == 0) {
		ERROR("Device", "CreateVertexShader", "Device or shader bytecode pointer is null, or bytecode length is zero.");
		return E_INVALIDARG;
	}

	// The actual implementation for creating a vertex shader is not provided in the original code.
	HRESULT result = m_device->CreateVertexShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppVertexShader);

	if (FAILED(result)) {
		ERROR("Device", "CreateVertexShader", ("Failed to create vertex shader. HRESULT: " + std::to_string(result)).c_str());
		return result;
	}

	MESSAGE("Device", "CreateVertexShader", "SUCCESS");
	return result;
}

HRESULT
Device::CreatePixelShader(const void* pShaderBytecode,
	unsigned int BytecodeLength,
	ID3D11ClassLinkage* pClassLinkage,
	ID3D11PixelShader** ppPixelShader) {
	if (ppPixelShader == nullptr) {
		ERROR("Device", "CreatePixelShader", "Output pointer is null.");
		return E_POINTER;
	}

	*ppPixelShader = nullptr; // Initialize the output pointer to nullptr

	if (m_device == nullptr || pShaderBytecode == nullptr || BytecodeLength == 0) {
		ERROR("Device", "CreatePixelShader", "Device or shader bytecode pointer is null, or bytecode length is zero.");
		return E_INVALIDARG;
	}

	// The actual implementation for creating a pixel shader is not provided in the original code.
	HRESULT result = m_device->CreatePixelShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppPixelShader);

	if (FAILED(result)) {
		ERROR("Device", "CreatePixelShader", ("Failed to create pixel shader. HRESULT: " + std::to_string(result)).c_str());
		return result;
	}

	MESSAGE("Device", "CreatePixelShader", "SUCCESS");
	return result;
}

HRESULT
Device::CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs,
	unsigned int NumElements,
	const void* pShaderBytecodeWithInputSignature,
	unsigned int BytecodeLength,
	ID3D11InputLayout** ppInputLayout) {
	if (ppInputLayout == nullptr) {
		ERROR("Device", "CreateInputLayout", "Output pointer is null.");
		return E_POINTER;
	}

	*ppInputLayout = nullptr; // Initialize the output pointer to nullptr

	if (m_device == nullptr) {
		ERROR("Device", "CreateInputLayout", "Device is not initialized.");
		return E_INVALIDARG;
	}

	if (NumElements == 0) {
		ERROR("Device", "CreateInputLayout", "Number of elements is zero.");
		return E_INVALIDARG;
	}

	if (pInputElementDescs == nullptr) {
		ERROR("Device", "CreateInputLayout", "Input element descriptions pointer is null.");
		return E_INVALIDARG;
	}

	if (pShaderBytecodeWithInputSignature == nullptr) {
		ERROR("Device", "CreateInputLayout", "Shader bytecode pointer is null.");
		return E_INVALIDARG;
	}

	if (BytecodeLength == 0) {
		ERROR("Device", "CreateInputLayout", "Bytecode length is zero.");
		return E_INVALIDARG;
	}

	if (ppInputLayout == nullptr) {
		ERROR("Device", "CreateInputLayout", "Output pointer is null.");
		return E_POINTER;
	}

	HRESULT result = m_device->CreateInputLayout(pInputElementDescs,
		NumElements,
		pShaderBytecodeWithInputSignature,
		BytecodeLength,
		ppInputLayout);

	if (FAILED(result)) {
		ERROR("Device", "CreateInputLayout", ("Failed to create input layout. HRESULT: " + std::to_string(result)).c_str());
		return result;
	}

	MESSAGE("Device", "CreateInputLayout", "SUCCESS");
	return result;
}

HRESULT
Device::CreateBuffer(const D3D11_BUFFER_DESC* pDesc,
	const D3D11_SUBRESOURCE_DATA* pInitialData,
	ID3D11Buffer** ppBuffer) {
	if (ppBuffer == nullptr) {
		ERROR("Device", "CreateBuffer", "Output pointer is null.");
		return E_POINTER;
	}

	*ppBuffer = nullptr; // Initialize the output pointer to nullptr

	if (m_device == nullptr || pDesc == nullptr) {
		ERROR("Device", "CreateBuffer", "Device or buffer description pointer is null.");
		return E_INVALIDARG;
	}

	HRESULT result = m_device->CreateBuffer(pDesc, pInitialData, ppBuffer);

	if (FAILED(result)) {
		ERROR("Device", "CreateBuffer", ("Failed to create buffer. HRESULT: " + std::to_string(result)).c_str());
		return result;
	}

	MESSAGE("Device", "CreateBuffer", "SUCCESS");
	return result;
}

HRESULT
Device::CreateRasterizerState(const D3D11_RASTERIZER_DESC* pRasterizerDesc,
	ID3D11RasterizerState** ppRasterizerState) {
	if (ppRasterizerState == nullptr) {
		ERROR("Device", "CreateRasterizerState", "Output pointer is null.");
		return E_POINTER;
	}

	*ppRasterizerState = nullptr; // Initialize the output pointer to nullptr

	HRESULT result = m_device->CreateRasterizerState(pRasterizerDesc, ppRasterizerState);

	if (FAILED(result)) {
		ERROR("Device", "CreateRasterizerState", ("Failed to create rasterizer state. HRESULT: " + std::to_string(result)).c_str());
		return result;
	}

	MESSAGE("Device", "CreateRasterizerState", "SUCCESS");
	return result;
}