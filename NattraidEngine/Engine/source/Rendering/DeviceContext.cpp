#include "Rendering/DeviceContext.h"

DeviceContext::~DeviceContext()
{
	destroy();
}

void
DeviceContext::destroy() {
	if (m_deviceContext) {
		m_deviceContext->ClearState();
		m_deviceContext->Release();
		SafeRelease(m_deviceContext);
	}
}

ID3D11DeviceContext&
DeviceContext::requireContext() const noexcept
{
	if (!m_deviceContext) {
		ERROR("DeviceContext", "RequireContext", "Device context is not initialized.");
	}

	return *m_deviceContext;
}

void
DeviceContext::ClearRenderTargetView(ID3D11RenderTargetView* pRenderTargetView,
	const float colorRGBA[4]) noexcept {
	if (!pRenderTargetView || !colorRGBA) {
		ERROR("DeviceContext", "ClearRenderTargetView", "Invalid render target view or color parameters.");
		return;
	}

	requireContext().ClearRenderTargetView(pRenderTargetView, colorRGBA);
}

void
DeviceContext::ClearDepthStencilView(ID3D11DepthStencilView* pDepthStencilView,
	unsigned int clearFlags,
	float depth,
	unsigned int stencil) noexcept {
	if (!pDepthStencilView) {
		ERROR("DeviceContext", "ClearDepthStencilView", "Invalid depth-stencil view parameter.");
		return;
	}

	requireContext().ClearDepthStencilView(pDepthStencilView, clearFlags, depth, stencil);
}

void
DeviceContext::OMSetRenderTargets(unsigned int numViews,
	ID3D11RenderTargetView* const* ppRenderTargetViews,
	ID3D11DepthStencilView* pDepthStencilView) noexcept {
	if (!ppRenderTargetViews || numViews == 0) {
		ERROR("DeviceContext", "OMSetRenderTargets", "Invalid render target view parameters.");
		return;
	}

	requireContext().OMSetRenderTargets(numViews, ppRenderTargetViews, pDepthStencilView);
}

void
DeviceContext::RSSetViewports(unsigned int numViewports,
	const D3D11_VIEWPORT* pViewports) noexcept {
	if (!pViewports || numViewports == 0) {
		ERROR("DeviceContext", "RSSetViewports", "Invalid viewport parameters.");
		return;
	}
	requireContext().RSSetViewports(numViewports, pViewports);
}

void
DeviceContext::PSSetShaderResources(unsigned int StartSlot,
	unsigned int NumViews,
	ID3D11ShaderResourceView* const* ppShaderResourceViews) {
	if (!ppShaderResourceViews || NumViews == 0) {
		ERROR("DeviceContext", "PSSetShaderResources", "Invalid shader resource view parameters.");
		return;
	}

	requireContext().PSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}

void
DeviceContext::RSSetState(ID3D11RasterizerState* pRasterizerState) noexcept {
	if (!pRasterizerState) {
		ERROR("DeviceContext", "RSSetState", "Invalid rasterizer state parameter.");
		return;
	}
	requireContext().RSSetState(pRasterizerState);
}

void
DeviceContext::IASetInputLayout(ID3D11InputLayout* pInputLayout) noexcept {
	if (!pInputLayout) {
		ERROR("DeviceContext", "IASetInputLayout", "Invalid input layout parameter.");
		return;
	}
	requireContext().IASetInputLayout(pInputLayout);
}

void
DeviceContext::IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology) noexcept {
	requireContext().IASetPrimitiveTopology(topology);
}

void
DeviceContext::IASetVertexBuffers(unsigned int startSlot,
	unsigned int numBuffers,
	ID3D11Buffer* const* ppVertexBuffers,
	const unsigned int* pStrides,
	const unsigned int* pOffsets) noexcept {
	if (!ppVertexBuffers || numBuffers == 0) {
		ERROR("DeviceContext", "IASetVertexBuffers", "Invalid vertex buffer parameters.");
		return;
	}

	requireContext().IASetVertexBuffers(startSlot, numBuffers, ppVertexBuffers, pStrides, pOffsets);
}

void
DeviceContext::IASetIndexBuffer(ID3D11Buffer* pIndexBuffer,
	DXGI_FORMAT format,
	unsigned int offset) noexcept {
	if (!pIndexBuffer) {
		ERROR("DeviceContext", "IASetIndexBuffer", "Invalid index buffer parameter.");
		return;
	}

	requireContext().IASetIndexBuffer(pIndexBuffer, format, offset);
}

void
DeviceContext::VSSetShader(ID3D11VertexShader* pVertexShader,
	ID3D11ClassInstance* const* ppClassInstances,
	unsigned int numClassInstances) noexcept {
	if (!pVertexShader) {
		ERROR("DeviceContext", "VSSetShader", "Invalid vertex shader parameter.");
		return;
	}
	requireContext().VSSetShader(pVertexShader, ppClassInstances, numClassInstances);
}

void
DeviceContext::PSSetShader(ID3D11PixelShader* pPixelShader,
	ID3D11ClassInstance* const* ppClassInstances,
	unsigned int numClassInstances) noexcept {
	if (!pPixelShader) {
		ERROR("DeviceContext", "PSSetShader", "Invalid pixel shader parameter.");
		return;
	}
	requireContext().PSSetShader(pPixelShader, ppClassInstances, numClassInstances);
}

void
DeviceContext::VSSetConstantBuffers(unsigned int startSlot,
	unsigned int numBuffers,
	ID3D11Buffer* const* ppConstantBuffers) noexcept {
	if (!ppConstantBuffers || numBuffers == 0) {
		ERROR("DeviceContext", "VSSetConstantBuffers", "Invalid constant buffer parameters.");
		return;
	}

	requireContext().VSSetConstantBuffers(startSlot, numBuffers, ppConstantBuffers);
}

void
DeviceContext::UpdateSubresource(ID3D11Resource* pDstResource,
	unsigned int dstSubresource,
	const D3D11_BOX* pDstBox,
	const void* pSrcData,
	unsigned int srcRowPitch,
	unsigned int srcDepthPitch) noexcept {
	if (!pDstResource || !pSrcData) {
		ERROR("DeviceContext", "UpdateSubresource", "Invalid destination resource or source data parameters.");
		return;
	}

	requireContext().UpdateSubresource(pDstResource, dstSubresource, pDstBox, pSrcData, srcRowPitch, srcDepthPitch);
}

void
DeviceContext::DrawIndexed(unsigned int indexCount,
	unsigned int startIndexLocation,
	int baseVertexLocation) noexcept {
	if (indexCount == 0) {
		ERROR("DeviceContext", "DrawIndexed", "Index count must be greater than zero.");
		return;
	}

	requireContext().DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
}