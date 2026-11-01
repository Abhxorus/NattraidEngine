#pragma once
#include "Engine/Prerequisites.h"

class
	DeviceContext {
public:
	DeviceContext() = default;
	~DeviceContext();

	void
		init();

	void
		destroy();

	ID3D11DeviceContext*
		getDeviceContext() const noexcept
	{
		return m_deviceContext;
	}

	ID3D11DeviceContext**
		getDeviceContextAddress() noexcept
	{
		return &m_deviceContext;
	}

	/**
		* @brief Returns a reference to the ID3D11DeviceContext. Throws an error if the device context is not initialized.
		* @return A reference to the ID3D11DeviceContext.
		*/
	ID3D11DeviceContext&
		requireContext() const noexcept;

	/**
		* @brief Clears the render target view with the specified color.
		* @param pRenderTargetView A pointer to the render target view to clear.
		* @param colorRGBA An array of four floats representing the RGBA color to clear with.
		*/
	void
		ClearRenderTargetView(ID3D11RenderTargetView* pRenderTargetView,
			const float colorRGBA[4]) noexcept;

	/**
		* @brief Clears the depth-stencil view with the specified depth and stencil values.
		* @param pDepthStencilView A pointer to the depth-stencil view to clear.
		* @param clearFlags Flags indicating which parts of the depth-stencil view to clear (D3D11_CLEAR_DEPTH, D3D11_CLEAR_STENCIL).
		* @param depth The depth value to clear to (typically between 0.0f and 1.0f).
		* @param stencil The stencil value to clear to (typically between 0 and 255).
		*/
	void
		ClearDepthStencilView(ID3D11DepthStencilView* pDepthStencilView,
			unsigned int clearFlags,
			float depth,
			unsigned int stencil) noexcept;

	/**
		* @brief Sets the render targets for the output-merger stage.
		* @param NumViews The number of render target views to set.
		* @param ppRenderTargetViews A pointer to an array of render target view pointers.
		* @param pDepthStencilView A pointer to the depth-stencil view to set (can be nullptr if not used).
		*/
	void
		OMSetRenderTargets(unsigned int numViews,
			ID3D11RenderTargetView* const* ppRenderTargetViews,
			ID3D11DepthStencilView* pDepthStencilView) noexcept;

	/**
	  * @brief Sets the viewports for the rasterizer stage.
	  * @param NumViewports The number of viewports to set.
	  * @param pViewports A pointer to an array of D3D11_VIEWPORT structures that describe the viewports.
	  */
	void
		RSSetViewports(unsigned int numViewports,
			const D3D11_VIEWPORT* pViewports) noexcept;

	/**
		* @brief Sets the shader resources for the pixel-shader stage.
		* @param StartSlot The first slot to set.
		* @param NumViews The number of shader resource views to set.
		* @param ppShaderResourceViews A pointer to an array of shader resource view pointers.
		*/
	void
		PSSetShaderResources(unsigned int StartSlot,
			unsigned int NumViews,
			ID3D11ShaderResourceView* const* ppShaderResourceViews);

	/**
		* @brief Sets the rasterizer state for the rasterizer stage.
		* @param pRasterizerState A pointer to the ID3D11RasterizerState interface that describes the rasterizer state.
		*/
	void
		RSSetState(ID3D11RasterizerState* pRasterizerState) noexcept;

	/**
		* @brief Sets the input layout for the input-assembler stage.
		* @param pInputLayout A pointer to the ID3D11InputLayout interface that describes the input layout.
		*/
	void
		IASetInputLayout(ID3D11InputLayout* pInputLayout) noexcept;

	/**
		* @brief Sets the primitive topology for the input-assembler stage.
		* @param topology The primitive topology to set (e.g., D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST).
		*/
	void
		IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology) noexcept;

	/**
		* @brief Sets the vertex buffers for the input-assembler stage.
		* @param startSlot The first input slot for binding.
		* @param numBuffers The number of vertex buffers to bind.
		* @param ppVertexBuffers A pointer to an array of vertex buffer pointers.
		* @param pStrides A pointer to an array of stride values (size of each vertex).
		* @param pOffsets A pointer to an array of offset values (offset from the start of each buffer).
		*/
	void
		IASetVertexBuffers(unsigned int startSlot,
			unsigned int numBuffers,
			ID3D11Buffer* const* ppVertexBuffers,
			const unsigned int* pStrides,
			const unsigned int* pOffsets) noexcept;

	/**
		  * @brief Sets the index buffer for the input-assembler stage.
		  * @param pIndexBuffer A pointer to the index buffer to set.
		  * @param format The format of the index buffer (e.g., DXGI_FORMAT_R32_UINT).
		  * @param offset The offset into the index buffer.
		  */
	void
		IASetIndexBuffer(ID3D11Buffer* pIndexBuffer,
			DXGI_FORMAT format,
			unsigned int offset) noexcept;

	/**
		  * @brief Sets the vertex shader for the vertex-shader stage.
		  * @param pVertexShader A pointer to the ID3D11VertexShader interface that describes the vertex shader.
		  * @param ppClassInstances A pointer to an array of class instance pointers (can be nullptr if not used).
		  * @param numClassInstances The number of class instances (can be 0 if not used).
		  */
	void
		VSSetShader(ID3D11VertexShader* pVertexShader,
			ID3D11ClassInstance* const* ppClassInstances = nullptr,
			unsigned int numClassInstances = 0) noexcept;

	/**
		  * @brief Sets the pixel shader for the pixel-shader stage.
		  * @param pPixelShader A pointer to the ID3D11PixelShader interface that describes the pixel shader.
		  * @param ppClassInstances A pointer to an array of class instance pointers (can be nullptr if not used).
		  * @param numClassInstances The number of class instances (can be 0 if not used).
		  */
	void
		PSSetShader(ID3D11PixelShader* pPixelShader,
			ID3D11ClassInstance* const* ppClassInstances = nullptr,
			unsigned int numClassInstances = 0) noexcept;

	/**
		  * @brief Sets the constant buffers for the vertex-shader stage.
		  * @param startSlot The first constant buffer slot for binding.
		  * @param numBuffers The number of constant buffers to bind.
		  * @param ppConstantBuffers A pointer to an array of constant buffer pointers.
		  */
	void
		VSSetConstantBuffers(unsigned int startSlot,
			unsigned int numBuffers,
			ID3D11Buffer* const* ppConstantBuffers) noexcept;

	/**
		* @brief Updates a subresource.
		* @param pDstResource A pointer to the destination resource.
		* @param dstSubresource The subresource index of the destination resource.
		* @param pDstBox A pointer to the bounding box for the destination resource.
		* @param pSrcData A pointer to the source data.
		*/
	void
		UpdateSubresource(ID3D11Resource* pDstResource,
			unsigned int dstSubresource,
			const D3D11_BOX* pDstBox,
			const void* pSrcData,
			unsigned int srcRowPitch,
			unsigned int srcDepthPitch) noexcept;

	/**
		* @brief Draws indexed geometry.
		* @param indexCount The number of indices to draw.
		* @param startIndexLocation The location of the first index to draw.
		* @param baseVertexLocation The location of the first vertex to use.
		*/
	void
		DrawIndexed(unsigned int indexCount,
			unsigned int startIndexLocation = 0,
			int baseVertexLocation = 0) noexcept;


private:
	ID3D11DeviceContext* m_deviceContext = nullptr;
};