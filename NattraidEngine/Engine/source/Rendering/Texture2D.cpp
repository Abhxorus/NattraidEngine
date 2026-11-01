//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"
#include "Rendering/Texture2D.h"
#include "Rendering/Device.h"
#include "Rendering/DeviceContext.h"

Texture2D::~Texture2D() noexcept {
	destroy();
}

HRESULT
Texture2D::init(Device& device,
	const std::string& textureName,
	ExtensionType extensionType) {
	if (!device.getDevice()) {
		ERROR("Texture", "init", "Device is null.");
		return E_POINTER;
	}
	if (textureName.empty()) {
		ERROR("Texture", "init", "Texture name cannot be empty.");
		return E_INVALIDARG;
	}

	HRESULT hr = S_OK;

	switch (extensionType) {
	case DDS:
		break;
	case PNG: {
		m_textureName = textureName + ".png";
		hr = InitTextureFromImage(device, m_textureName, *this);
		break;
	}
	case JPG: {
		m_textureName = textureName + ".jpg";
		hr = InitTextureFromImage(device, m_textureName, *this);
		break;
	}
	default:
		ERROR("Texture", "init", "Unsupported extension type");
		return E_INVALIDARG;
	}

	return hr;
}

HRESULT
Texture2D::init(Device& device,
	unsigned int width,
	unsigned int height,
	DXGI_FORMAT Format,
	unsigned int BindFlags,
	unsigned int sampleCount,
	unsigned int qualityLevels) {
	if (!device.getDevice()) {
		ERROR("Texture", "init", "Device is null.");
		return E_POINTER;
	}
	if (width == 0 || height == 0) {
		ERROR("Texture", "init", "Width and height must be greater than 0");
		return E_INVALIDARG;
	}

	D3D11_TEXTURE2D_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = Format;
	desc.SampleDesc.Count = sampleCount;
	desc.SampleDesc.Quality = qualityLevels;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = BindFlags;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	HRESULT hr = device.CreateTexture2D(&desc, nullptr, &m_texture);

	if (FAILED(hr)) {
		ERROR("Texture", "init",
			("Failed to create texture with specified params. HRESULT: " + std::to_string(hr)).c_str());
		return hr;
	}

	return S_OK;
}

HRESULT Texture2D::init(Device& device, Texture2D& textureRef, DXGI_FORMAT format)
{
	return E_NOTIMPL;
}

HRESULT Texture2D::InitTextureFromImage(Device& device, const std::string& fullPath, Texture2D& texture)
{
	return E_NOTIMPL;
}

void
Texture2D::render(DeviceContext& deviceContext,
	unsigned int StartSlot,
	unsigned int NumViews) {
	if (!deviceContext.getDeviceContext()) {
		ERROR("Texture", "render", "Device Context is null.");
		return;
	}

	if (m_textureFromImg) {
		deviceContext.PSSetShaderResources(StartSlot, NumViews, &m_textureFromImg);
	}
}

void
Texture2D::destroy() {
	if (m_texture != nullptr) {
		SAFE_RELEASE(m_texture);
	}
	if (m_textureFromImg != nullptr) {
		SAFE_RELEASE(m_textureFromImg);
	}
}

HRESULT Texture2D::CreateCubemap(Device& device, DeviceContext& deviceContext, const std::array<std::string, 6>& facePaths, bool generateMips)
{
	return E_NOTIMPL;
}