#pragma once
#include "API.h"
#include <cstdint>
#include <Windows.h>

extern "C" {
	ENGINE_API bool
		Engine_Initialize(HWND hwnd, int width, int height) noexcept;

	ENGINE_API void
		Engine_Update() noexcept;

	ENGINE_API void
		Engine_Render() noexcept;

	ENGINE_API void
		Engine_Shutdown() noexcept;
}