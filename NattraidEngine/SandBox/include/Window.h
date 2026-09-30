#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>

class Window final {
public:
	Window() = default;
	~Window();

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	
	bool
		Create(HINSTANCE instance, const wchar_t* title, UINT width, UINT height) noexcept;

	void
		Show(int showCommand) noexcept;

	void
		Destroy() noexcept;

	bool
		ProcessMessages() noexcept;

	HWND
		GetHandle() const noexcept { return m_windowHandle; }

	bool
		IsMinimized() const noexcept;

private:
	static LRESULT CALLBACK WindowProcedure(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
	HWND m_windowHandle = nullptr;

	static constexpr const wchar_t* ClassName = L"NattraidEngine Window";

	HINSTANCE m_instance = nullptr; 
	bool m_classRegistered = false; 
};