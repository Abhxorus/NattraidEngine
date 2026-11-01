#pragma once
#include "Engine/Prerequisites.h"

// Forward declarations of classes used in the SwapChain class
class Window;
class Device;
class Texture2D;

/**
 * @brief A class that encapsulates a DirectX swap chain.
 */
class
	SwapChain {
public:
	SwapChain() = default;
	~SwapChain();

	SwapChain(const SwapChain&) = delete;
	SwapChain& operator=(const SwapChain&) = delete;

	SwapChain(SwapChain&&) = delete;
	SwapChain& operator=(SwapChain&&) = delete;

	/**
	 * @brief Initializes the swap chain with the specified device, window, width, and height.
	 * @param device The DirectX device to use for creating the swap chain.
	 * @param window The window associated with the swap chain.
	 * @param width The width of the swap chain's back buffer.
	 * @param height The height of the swap chain's back buffer.
	 * @return HRESULT indicating success or failure of the initialization.
	 */
	HRESULT
		init(Device device, Window window,
			unsigned int width, unsigned int height);

	/**
	 * @brief Destroys the swap chain and releases associated resources.
	 */
	void
		destroy();

	/**
	 * @brief Presents the contents of the swap chain to the screen.
	 * @param syncInterval The number of vertical blanks to wait for before presenting (default is 1).
	 * @param flags Presentation options (default is 0).
	 * @return HRESULT indicating success or failure of the presentation.
	 */
	HRESULT
		Present(unsigned int syncInterval = 1,
			unsigned int flags = 0);

	/**
	 * @brief Retrieves a back buffer from the swap chain.
	 * @param bufferIndex The index of the back buffer to retrieve (typically 0).
	 * @param backBuffer A reference to a Texture2D object that will receive the back buffer.
	 * @return HRESULT indicating success or failure of the operation.
	 */
	HRESULT
		GetBuffer(unsigned int bufferIndex,
			Texture2D backBuffer);

	/**
	 * @brief Resizes the swap chain's buffers to the specified width and height.
	 * @param width The new width of the swap chain's back buffer.
	 * @param height The new height of the swap chain's back buffer.
	 * @return HRESULT indicating success or failure of the resize operation.
	 */
	HRESULT
		ResizeBuffers(unsigned int width,
			unsigned int height);

	/**
	 * @brief Retrieves the underlying IDXGISwapChain pointer.
	 * @return A pointer to the IDXGISwapChain interface.
	 */
	IDXGISwapChain*
		getSwapChain() { return m_swapChain; }

private:
	IDXGISwapChain* m_swapChain = nullptr;
};