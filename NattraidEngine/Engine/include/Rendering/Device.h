#pragma once
#include "Engine/Prerequisites.h"

class
	Device {
public:
	Device() = default;
	~Device();

	void
		init();

	void
		destroy();

	ID3D11Device*
		getDevice() const noexcept
	{
		return m_device;
	}

	ID3D11Device**
		getDeviceAddress() noexcept
	{
		return &m_device;
	}

	/**
		* @brief Creates a render target view for the specified resource.
		* @param pResource A pointer to the resource for which to create the render target view.
		* @param pDesc A pointer to a D3D11_RENDER_TARGET_VIEW_DESC structure that describes the render target view. If this parameter is NULL, a default view is created.
		* @param ppRTView A pointer to a pointer that receives the created render target view interface. If the method fails, this pointer is set to NULL.
		* @return HRESULT The result of the operation. S_OK if successful, or an error code otherwise.
		*/
	HRESULT
		CreateRenderTargetView(ID3D11Resource* pResource,
			const D3D11_RENDER_TARGET_VIEW_DESC* pDesc,
			ID3D11RenderTargetView** ppRTView);
	/**
		* @brief Creates a depth stencil view for the specified resource.
		* @param pResource A pointer to the resource for which to create the depth stencil view.
		* @param pDesc A pointer to a D3D11_DEPTH_STENCIL_VIEW_DESC structure that describes the depth stencil view. If this parameter is NULL, a default view is created.
		* @param ppDepthStencilView A pointer to a pointer that receives the created depth stencil view interface. If the method fails, this pointer is set to NULL.
		* @return HRESULT The result of the operation. S_OK if successful, or an error code otherwise.
		*/
	HRESULT
		CreateTexture2D(const D3D11_TEXTURE2D_DESC* pDesc,
			const D3D11_SUBRESOURCE_DATA* pInitialData,
			ID3D11Texture2D** ppTexture2D);
	/**
		* @brief Creates a depth stencil view for the specified resource.
		* @param pResource A pointer to the resource for which to create the depth stencil view.
		* @param pDesc A pointer to a D3D11_DEPTH_STENCIL_VIEW_DESC structure that describes the depth stencil view. If this parameter is NULL, a default view is created.
		* @param ppDepthStencilView A pointer to a pointer that receives the created depth stencil view interface. If the method fails, this pointer is set to NULL.
		* @return HRESULT The result of the operation. S_OK if successful, or an error code otherwise.
		*/
	HRESULT
		CreateDepthStencilView(ID3D11Resource* pResource,
			D3D11_DEPTH_STENCIL_VIEW_DESC* pDesc,
			ID3D11DepthStencilView** ppDepthStencilView);

	/**
		* @brief Creates a vertex shader from compiled shader bytecode.
		* @param pShaderBytecode A pointer to the compiled shader bytecode.
		* @param BytecodeLength The size of the compiled shader bytecode in bytes.
		* @param pClassLinkage A pointer to an ID3D11ClassLinkage interface for class linkage. Can be NULL if not used.
		* @param ppVertexShader A pointer to a pointer that receives the created vertex shader interface. If the method fails, this pointer is set to NULL.
		* @return HRESULT The result of the operation. S_OK if successful, or an error code otherwise.
		*/
	HRESULT
		CreateVertexShader(const void* pShaderBytecode,
			unsigned int BytecodeLength,
			ID3D11ClassLinkage* pClassLinkage,
			ID3D11VertexShader** ppVertexShader);

	/**
		* @brief Creates a pixel shader from compiled shader bytecode.
		* @param pShaderBytecode A pointer to the compiled shader bytecode.
		* @param BytecodeLength The size of the compiled shader bytecode in bytes.
		* @param pClassLinkage A pointer to an ID3D11ClassLinkage interface for class linkage. Can be NULL if not used.
		* @param ppPixelShader A pointer to a pointer that receives the created pixel shader interface. If the method fails, this pointer is set to NULL.
		* @return HRESULT The result of the operation. S_OK if successful, or an error code otherwise.
		*/
	HRESULT
		CreatePixelShader(const void* pShaderBytecode,
			unsigned int BytecodeLength,
			ID3D11ClassLinkage* pClassLinkage,
			ID3D11PixelShader** ppPixelShader);

	/**
		* @brief Creates an input layout for the specified vertex shader.
		* @param pInputElementDescs A pointer to an array of D3D11_INPUT_ELEMENT_DESC structures that describe the input layout.
		* @param NumElements The number of elements in the pInputElementDescs array.
		* @param pShaderBytecodeWithInputSignature A pointer to the compiled shader bytecode that contains the input signature.
		* @param BytecodeLength The size of the compiled shader bytecode in bytes.
		* @param ppInputLayout A pointer to a pointer that receives the created input layout interface. If the method fails, this pointer is set to NULL.
		* @return HRESULT The result of the operation. S_OK if successful, or an error code otherwise.
		*/
	HRESULT
		CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs,
			unsigned int NumElements,
			const void* pShaderBytecodeWithInputSignature,
			unsigned int BytecodeLength,
			ID3D11InputLayout** ppInputLayout);

	/**
		* @brief Creates a buffer resource.
		* @param pDesc A pointer to a D3D11_BUFFER_DESC structure that describes the buffer.
		* @param pInitialData A pointer to a D3D11_SUBRESOURCE_DATA structure that describes the initial data for the buffer. Can be NULL if no initial data is provided.
		* @param ppBuffer A pointer to a pointer that receives the created buffer interface. If the method fails, this pointer is set to NULL.
		* @return HRESULT The result of the operation. S_OK if successful, or an error code otherwise.
		*/
	HRESULT
		CreateBuffer(const D3D11_BUFFER_DESC* pDesc,
			const D3D11_SUBRESOURCE_DATA* pInitialData,
			ID3D11Buffer** ppBuffer);

	/**
		* @brief Creates a rasterizer state object.
		* @param pRasterizerDesc A pointer to a D3D11_RASTERIZER_DESC structure that describes the rasterizer state.
		* @param ppRasterizerState A pointer to a pointer that receives the created rasterizer state interface. If the method fails, this pointer is set to NULL.
		* @return HRESULT The result of the operation. S_OK if successful, or an error code otherwise.
		*/
	HRESULT
		CreateRasterizerState(const D3D11_RASTERIZER_DESC* pRasterizerDesc,
			ID3D11RasterizerState** ppRasterizerState);
private:
	ID3D11Device* m_device = nullptr;
	D3D_FEATURE_LEVEL* m_featureLevels = nullptr;
};