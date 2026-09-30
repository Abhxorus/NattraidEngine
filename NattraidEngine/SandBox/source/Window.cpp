#include "Window.h"
#include <Engine/Engine.h>

Window::~Window() {
	Destroy();
}

bool
Window::Create(HINSTANCE instance, const wchar_t* title, UINT width, UINT height) noexcept {

	if (m_windowHandle || !instance || !title || width == 0 || height == 0) {
		return false;
	}

	m_instance = instance;

	// Register the window class
	WNDCLASSEXW windowClass{};
	windowClass.cbSize = sizeof(WNDCLASSEXW);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProcedure;
	windowClass.hInstance = m_instance;
	windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	windowClass.lpszClassName = ClassName;

	if (!RegisterClassExW(&windowClass)) {
		return false;
	}

	m_classRegistered = true;
	// Create the window
	constexpr DWORD windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

	RECT rectangle{ 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };

	if (!AdjustWindowRect(&rectangle, windowStyle, FALSE)) {
		Destroy();
		return false;
	}

	const int outerWidth = rectangle.right - rectangle.left;
	const int outerHeight = rectangle.bottom - rectangle.top;

	m_windowHandle = CreateWindowExW(
		0,
		ClassName,
		title,
		windowStyle,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		outerWidth,
		outerHeight,
		nullptr,
		nullptr,
		m_instance,
		this // Pass the pointer to this instance for use in the window procedure
	);

	if (!m_windowHandle) {
		Destroy();
		return false;
	}
	return true;
}

void
Window::Show(int showCommand) noexcept
{
	if (m_windowHandle)
	{
		ShowWindow(m_windowHandle, showCommand);
		UpdateWindow(m_windowHandle);
	}
}

void Window::Destroy() noexcept
{
	if (m_windowHandle)
	{
		DestroyWindow(m_windowHandle);
		m_windowHandle = nullptr;
	}

	if (m_classRegistered)
	{
		UnregisterClassW(ClassName, m_instance);
		m_classRegistered = false;
	}

	m_instance = nullptr;
}

bool Window::ProcessMessages() noexcept
{
	MSG message{};

	while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
	{
		if (message.message == WM_QUIT)
			return false;

		TranslateMessage(&message);
		DispatchMessageW(&message);
	}

	return true;
}

bool Window::IsMinimized() const noexcept
{
	return m_windowHandle && IsIconic(m_windowHandle);
}

LRESULT CALLBACK Window::WindowProcedure(
	HWND handle,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
)
{
	switch (message)
	{
	case WM_ERASEBKGND:
		// DirectX limpia el back buffer.
		return 1;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProcW(handle, message, wParam, lParam);
	}
}