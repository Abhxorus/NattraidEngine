#pragma once
#include "API.h"
#include <cstdint>
#include <DirectXMath.h>
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <sstream>
#include <cstddef>
#include <chrono>
#include <new>

// MACROS
#define SAFE_RELEASE(x) if(x != nullptr) x->Release(); x = nullptr;

#define MESSAGE( classObj, method, state )   \
{                                            \
   std::wostringstream os_;                  \
   os_ << classObj << "::" << method << " : " << "[CREATION OF RESOURCE " << ": " << state << "] \n"; \
   OutputDebugStringW( os_.str().c_str() );  \
}

#define ERROR(classObj, method, errorMSG)                     \
{                                                             \
    try {                                                     \
        std::wostringstream os_;                              \
        os_ << L"ERROR : " << classObj << L"::" << method     \
            << L" : " << errorMSG << L"\n";                   \
        OutputDebugStringW(os_.str().c_str());                \
    } catch (...) {                                           \
        OutputDebugStringW(L"Failed to log error message.\n");\
    }                                                         \
}


enum
    ExtensionType {
    DDS = 0,
    PNG = 1,
    JPG = 2
};

struct
    Vertex {
    float position[3];
    float color[4];
};


template<typename T>
void SafeRelease(T*& object) noexcept
{
    if (object != nullptr)
    {
        object->Release();
        object = nullptr;
    }
}

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