/**
 * @file Engine.cpp
 * @brief Implementación de la clase Engine y encapsulación gráfica con DirectX 11.
 *
 * Contiene toda la lógica de inicialización del dispositivo, manejo de shaders,
 * buffers y el ciclo principal de renderizado 3D de NattraidEngine.
 */

#include <Engine/Engine.h>
#include <DirectXMath.h>
#include <chrono>
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#include <cstddef>
#include <cstdint>
#include <new>

 // ============================================================================
 // MACROS DE UTILIDAD
 // ============================================================================

 /**
  * @def SAFE_RELEASE(x)
  * @brief Macro clásica para liberar interfaces COM de forma segura.
  */
#define SAFE_RELEASE(x) if(x != nullptr) { x->Release(); x = nullptr; }

  /**
   * @def MESSAGE(classObj, method, state)
   * @brief Envía un mensaje de éxito/información a la consola de depuración de Windows.
   */
#define MESSAGE( classObj, method, state )                                                           \
{                                                                                                    \
   std::wostringstream os_;                                                                          \
   os_ << classObj << "::" << method << " : " << "[CREATION OF RESOURCE " << ": " << state << "] \n";\
   OutputDebugStringW( os_.str().c_str() );                                                          \
}

   /**
    * @def ERROR(classObj, method, errorMSG)
    * @brief Envía un mensaje de error formateado a la consola de depuración de Windows de manera segura.
    */
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

    /**
     * @brief Libera de forma segura un puntero a un objeto COM (Component Object Model) y lo anula.
     *
     * @tparam T Tipo de la interfaz de DirectX (ej. ID3D11Device).
     * @param object Referencia al puntero que se desea liberar.
     */
template<typename T>
void SafeRelease(T*& object) noexcept
{
    if (object != nullptr)
    {
        object->Release();
        object = nullptr;
    }
}

// ============================================================================
// IMPLEMENTACIÓN PRIVADA (PIMPL)
// ============================================================================

/**
 * @struct Engine::implementation
 * @brief Estructura oculta que contiene todos los recursos nativos de DirectX 11.
 */
struct Engine::implementation
{
    /**
     * @struct Vertex
     * @brief Define la estructura de los datos geométricos por vértice.
     */
    struct Vertex
    {
        float position[3]; ///< Posición 3D (X, Y, Z).
        float color[4];    ///< Color del vértice en formato RGBA.
    };

    /**
     * @struct TransformBuffer
     * @brief Buffer constante (CBuffer) para pasar matrices al shader.
     * Debe estar alineado a 16 bytes por requerimientos de DirectX.
     */
    struct alignas(16) TransformBuffer
    {
        DirectX::XMFLOAT4X4 worldViewProjection; ///< Matriz combinada de Mundo, Vista y Proyección.
    };

    HWND windowHandle = nullptr;   ///< Handle de la ventana de renderizado.

    std::uint32_t width = 0;       ///< Ancho del área de renderizado.
    std::uint32_t height = 0;      ///< Alto del área de renderizado.

    // Recursos core de DirectX
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* deviceContext = nullptr;
    IDXGISwapChain* swapChain = nullptr;
    ID3D11RenderTargetView* renderTargetView = nullptr;

    // Recursos de profundidad
    ID3D11Texture2D* depthStencilBuffer = nullptr;
    ID3D11DepthStencilView* depthStencilView = nullptr;

    // Buffers de la geometría
    ID3D11Buffer* vertexBuffer = nullptr;
    ID3D11Buffer* indexBuffer = nullptr;
    ID3D11Buffer* transformBuffer = nullptr;

    // Estados del pipeline
    ID3D11RasterizerState* rasterizerState = nullptr;

    // Control de tiempo para animaciones
    std::chrono::steady_clock::time_point startTime{};

    // Shaders y layout
    ID3D11VertexShader* vertexShader = nullptr;
    ID3D11PixelShader* pixelShader = nullptr;
    ID3D11InputLayout* inputLayout = nullptr;

    /**
     * @brief Lee y compila un archivo HLSL en tiempo de ejecución.
     *
     * @param filename Ruta relativa o absoluta del archivo shader (.hlsl).
     * @param entryPoint Nombre de la función principal dentro del shader (ej. "VSMain").
     * @param shaderModel Versión del perfil de shader a compilar (ej. "vs_5_0").
     * @param shaderBlob Puntero doble donde se guardará el bytecode compilado.
     * @return true si el shader se compiló correctamente, false en caso de error.
     */
    static bool CompileShader(
        const wchar_t* filename,
        const char* entryPoint,
        const char* shaderModel,
        ID3DBlob** shaderBlob
    ) noexcept
    {
        if (!filename || !entryPoint || !shaderModel || !shaderBlob)
        {
            return false;
        }

        *shaderBlob = nullptr;
        UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#ifdef _DEBUG
        compileFlags |= D3DCOMPILE_DEBUG;
        compileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        compileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

        ID3DBlob* errors = nullptr;

        const HRESULT result = D3DCompileFromFile(
            filename,
            nullptr,
            D3D_COMPILE_STANDARD_FILE_INCLUDE,
            entryPoint,
            shaderModel,
            compileFlags,
            0,
            shaderBlob,
            &errors
        );

        // Volcar errores de compilación de HLSL a la consola si los hay
        if (errors)
        {
            OutputDebugStringA(static_cast<const char*>(errors->GetBufferPointer()));
            SAFE_RELEASE(errors);
        }

        if (FAILED(result))
        {
            SafeRelease(*shaderBlob);
            *shaderBlob = nullptr;
            return false;
        }

        return true;
    }

    /**
     * @brief Libera todos los recursos gráficos instanciados y limpia el estado del dispositivo.
     */
    void ReleaseResources() noexcept
    {
        if (deviceContext)
        {
            deviceContext->ClearState();
            deviceContext->Flush();
        }

        SafeRelease(rasterizerState);
        SafeRelease(transformBuffer);
        SafeRelease(indexBuffer);
        SafeRelease(vertexBuffer);

        SafeRelease(inputLayout);
        SafeRelease(pixelShader);
        SafeRelease(vertexShader);

        SafeRelease(depthStencilView);
        SafeRelease(depthStencilBuffer);
        SafeRelease(renderTargetView);

        SafeRelease(swapChain);
        SafeRelease(deviceContext);
        SafeRelease(device);

        windowHandle = nullptr;
        width = 0;
        height = 0;
    }
};

// ============================================================================
// CLASE ENGINE (PÚBLICA)
// ============================================================================

Engine::Engine() noexcept : m_implementation(new (std::nothrow) implementation{}) {}

Engine::~Engine() noexcept
{
    Shutdown();
    delete m_implementation;
    m_implementation = nullptr;
}

bool Engine::Initialize(void* nativeWindow, std::uint32_t width, std::uint32_t height) noexcept
{
    if (!m_implementation || !nativeWindow || width == 0 || height == 0)
    {
        return false;
    }

    implementation& engine = *m_implementation;

    // Permite reinicializar la instancia de forma segura limpiando restos anteriores.
    engine.ReleaseResources();

    engine.windowHandle = static_cast<HWND>(nativeWindow);
    engine.width = width;
    engine.height = height;

    // ------------------------------------------------------------------------
    // 1. Configuración del Swap Chain
    // ------------------------------------------------------------------------
    DXGI_SWAP_CHAIN_DESC swapChainDescription{};
    swapChainDescription.BufferCount = 1;
    swapChainDescription.BufferDesc.Width = engine.width;
    swapChainDescription.BufferDesc.Height = engine.height;
    swapChainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDescription.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDescription.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDescription.OutputWindow = engine.windowHandle;
    swapChainDescription.SampleDesc.Count = 1;
    swapChainDescription.SampleDesc.Quality = 0; // Sin antialiasing por defecto
    swapChainDescription.Windowed = TRUE;
    swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    constexpr D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
    D3D_FEATURE_LEVEL selectedFeatureLevel{};

    // Crea el dispositivo y el swap chain en hardware
    HRESULT result = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        featureLevels,
        ARRAYSIZE(featureLevels),
        D3D11_SDK_VERSION,
        &swapChainDescription,
        &engine.swapChain,
        &engine.device,
        &selectedFeatureLevel,
        &engine.deviceContext
    );

    // Fallback: Si falla la GPU física, utiliza el rasterizador por software de Windows (WARP)
    if (FAILED(result))
    {
        SafeRelease(engine.swapChain);
        SafeRelease(engine.deviceContext);
        SafeRelease(engine.device);

        result = D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_WARP,
            nullptr,
            0,
            featureLevels,
            ARRAYSIZE(featureLevels),
            D3D11_SDK_VERSION,
            &swapChainDescription,
            &engine.swapChain,
            &engine.device,
            &selectedFeatureLevel,
            &engine.deviceContext
        );
    }

    if (FAILED(result))
    {
        engine.ReleaseResources();
        return false;
    }

    // ------------------------------------------------------------------------
    // 2. Creación del Render Target View (RTV)
    // ------------------------------------------------------------------------
    ID3D11Texture2D* backBuffer = nullptr;
    result = engine.swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));

    if (FAILED(result))
    {
        engine.ReleaseResources();
        return false;
    }

    result = engine.device->CreateRenderTargetView(backBuffer, nullptr, &engine.renderTargetView);
    SafeRelease(backBuffer);

    if (FAILED(result))
    {
        engine.ReleaseResources();
        return false;
    }

    // ------------------------------------------------------------------------
    // 3. Creación del Depth Stencil Buffer y View (DSV)
    // ------------------------------------------------------------------------
    D3D11_TEXTURE2D_DESC depthBufferDescription{};
    depthBufferDescription.Width = engine.width;
    depthBufferDescription.Height = engine.height;
    depthBufferDescription.MipLevels = 1;
    depthBufferDescription.ArraySize = 1;
    depthBufferDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDescription.SampleDesc.Count = 1;
    depthBufferDescription.SampleDesc.Quality = 0;
    depthBufferDescription.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    result = engine.device->CreateTexture2D(&depthBufferDescription, nullptr, &engine.depthStencilBuffer);

    if (FAILED(result))
    {
        engine.ReleaseResources();
        return false;
    }

    result = engine.device->CreateDepthStencilView(engine.depthStencilBuffer, nullptr, &engine.depthStencilView);

    if (FAILED(result))
    {
        engine.ReleaseResources();
        return false;
    }

    // ------------------------------------------------------------------------
    // 4. Configuración del Viewport
    // ------------------------------------------------------------------------
    D3D11_VIEWPORT viewport{};
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(engine.width);
    viewport.Height = static_cast<float>(engine.height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    engine.deviceContext->RSSetViewports(1, &viewport);

    // ------------------------------------------------------------------------
    // 5. Carga y compilación de Shaders
    // ------------------------------------------------------------------------
    ID3DBlob* vertexShaderBlob = nullptr;
    ID3DBlob* pixelShaderBlob = nullptr;

    if (!implementation::CompileShader(L"shaders\\Cube.hlsl", "VSMain", "vs_5_0", &vertexShaderBlob))
    {
        engine.ReleaseResources();
        return false;
    }

    if (!implementation::CompileShader(L"shaders\\Cube.hlsl", "PSMain", "ps_5_0", &pixelShaderBlob))
    {
        SafeRelease(vertexShaderBlob);
        engine.ReleaseResources();
        return false;
    }

    result = engine.device->CreateVertexShader(
        vertexShaderBlob->GetBufferPointer(),
        vertexShaderBlob->GetBufferSize(),
        nullptr,
        &engine.vertexShader
    );

    if (FAILED(result))
    {
        SafeRelease(pixelShaderBlob);
        SafeRelease(vertexShaderBlob);
        engine.ReleaseResources();
        return false;
    }

    result = engine.device->CreatePixelShader(
        pixelShaderBlob->GetBufferPointer(),
        pixelShaderBlob->GetBufferSize(),
        nullptr,
        &engine.pixelShader
    );

    if (FAILED(result))
    {
        SafeRelease(pixelShaderBlob);
        SafeRelease(vertexShaderBlob);
        engine.ReleaseResources();
        return false;
    }

    // ------------------------------------------------------------------------
    // 6. Configuración del Input Layout
    // ------------------------------------------------------------------------
    constexpr D3D11_INPUT_ELEMENT_DESC inputElements[]{
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, static_cast<UINT>(offsetof(implementation::Vertex, position)), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, static_cast<UINT>(offsetof(implementation::Vertex, color)),D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    result = engine.device->CreateInputLayout(
        inputElements,
        ARRAYSIZE(inputElements),
        vertexShaderBlob->GetBufferPointer(),
        vertexShaderBlob->GetBufferSize(),
        &engine.inputLayout
    );

    SafeRelease(pixelShaderBlob);
    SafeRelease(vertexShaderBlob);

    if (FAILED(result))
    {
        engine.ReleaseResources();
        return false;
    }

    // ------------------------------------------------------------------------
    // 7. Definición de la Geometría (Cubo Mágico)
    // ------------------------------------------------------------------------
    constexpr implementation::Vertex vertices[] =
    {
        // Frente
        { { -1.0f,  1.0f, -1.0f }, {  1.0f,  0.0f,  0.0f, 1.0f } },
        { {  1.0f,  1.0f, -1.0f }, {  0.0f,  1.0f,  0.0f, 1.0f } },
        { {  1.0f, -1.0f, -1.0f }, {  0.0f,  0.0f,  1.0f, 1.0f } },
        { { -1.0f, -1.0f, -1.0f }, {  1.0f,  1.0f,  0.0f, 1.0f } },

        // Atrás
        { { -1.0f,  1.0f,  1.0f }, {  1.0f,  0.0f,  1.0f, 1.0f } },
        { {  1.0f,  1.0f,  1.0f }, {  0.0f,  1.0f,  1.0f, 1.0f } },
        { {  1.0f, -1.0f,  1.0f }, {  1.0f,  1.0f,  1.0f, 1.0f } },
        { { -1.0f, -1.0f,  1.0f }, {  0.2f,  0.4f,  1.0f, 1.0f } }
    };

    D3D11_BUFFER_DESC vertexBufferDescription{};
    vertexBufferDescription.ByteWidth = static_cast<UINT>(sizeof(vertices));
    vertexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
    vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDescription.CPUAccessFlags = 0;
    vertexBufferDescription.MiscFlags = 0;
    vertexBufferDescription.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA initialVertexData{};
    initialVertexData.pSysMem = vertices;

    result = engine.device->CreateBuffer(&vertexBufferDescription, &initialVertexData, &engine.vertexBuffer);

    if (FAILED(result))
    {
        engine.ReleaseResources();
        return false;
    }

    constexpr std::uint16_t indices[] =
    {
        // Frente
        0, 1, 2,  0, 2, 3,
        // Atrás
        5, 4, 7,  5, 7, 6,
        // Izquierda
        4, 0, 3,  4, 3, 7,
        // Derecha
        1, 5, 6,  1, 6, 2,
        // Arriba
        4, 5, 1,  4, 1, 0,
        // Abajo
        3, 2, 6,  3, 6, 7
    };

    D3D11_BUFFER_DESC indexBufferDescription{};
    indexBufferDescription.ByteWidth = static_cast<UINT>(sizeof(indices));
    indexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
    indexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA indexData{};
    indexData.pSysMem = indices;

    result = engine.device->CreateBuffer(&indexBufferDescription, &indexData, &engine.indexBuffer);

    if (FAILED(result))
    {
        engine.ReleaseResources();
        return false;
    }

    // ------------------------------------------------------------------------
    // 8. Buffers Constantes y Rasterizador
    // ------------------------------------------------------------------------
    D3D11_BUFFER_DESC transformBufferDescription{};
    transformBufferDescription.ByteWidth = sizeof(implementation::TransformBuffer);
    transformBufferDescription.Usage = D3D11_USAGE_DEFAULT;
    transformBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    result = engine.device->CreateBuffer(&transformBufferDescription, nullptr, &engine.transformBuffer);

    if (FAILED(result))
    {
        engine.ReleaseResources();
        return false;
    }

    D3D11_RASTERIZER_DESC rasterizerDescription{};
    rasterizerDescription.FillMode = D3D11_FILL_SOLID;
    rasterizerDescription.CullMode = D3D11_CULL_NONE;
    rasterizerDescription.DepthClipEnable = TRUE;

    result = engine.device->CreateRasterizerState(&rasterizerDescription, &engine.rasterizerState);

    if (FAILED(result))
    {
        engine.ReleaseResources();
        return false;
    }

    // Guardar el tiempo de inicio para las animaciones matriciales
    engine.startTime = std::chrono::steady_clock::now();

    return true;
}

void Engine::Render() noexcept
{
    if (!m_implementation)
    {
        return;
    }

    implementation& engine = *m_implementation;

    // Prevención de cuelgues si los recursos no se cargaron adecuadamente
    if (!engine.deviceContext || !engine.swapChain || !engine.renderTargetView ||
        !engine.depthStencilView || !engine.vertexBuffer || !engine.indexBuffer ||
        !engine.transformBuffer || !engine.inputLayout || !engine.vertexShader ||
        !engine.pixelShader)
    {
        return;
    }

    // ------------------------------------------------------------------------
    // 1. Limpieza de pantalla (Clear)
    // ------------------------------------------------------------------------
    constexpr float clearColor[] = { 0.03f, 0.04f, 0.08f, 1.0f }; // Fondo oscuro azulado

    engine.deviceContext->OMSetRenderTargets(1, &engine.renderTargetView, engine.depthStencilView);
    engine.deviceContext->ClearRenderTargetView(engine.renderTargetView, clearColor);
    engine.deviceContext->ClearDepthStencilView(
        engine.depthStencilView,
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
        1.0f,
        0
    );

    // ------------------------------------------------------------------------
    // 2. Cálculos de matrices (Transformaciones y Cámara)
    // ------------------------------------------------------------------------
    const auto currentTime = std::chrono::steady_clock::now();
    const float elapsedSeconds = std::chrono::duration<float>(currentTime - engine.startTime).count();

    using namespace DirectX;

    // Matriz de Mundo (Rotación basada en el tiempo)
    const XMMATRIX world = XMMatrixRotationX(elapsedSeconds * 0.4f) * XMMatrixRotationY(elapsedSeconds * 0.8f);

    // Matriz de Vista (Cámara)
    const XMVECTOR cameraPosition = XMVectorSet(0.0f, 1.5f, -5.0f, 1.0f);
    const XMVECTOR cameraTarget = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
    const XMVECTOR cameraUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    const XMMATRIX view = XMMatrixLookAtLH(cameraPosition, cameraTarget, cameraUp);

    // Matriz de Proyección (Perspectiva)
    const float aspectRatio = static_cast<float>(engine.width) / static_cast<float>(engine.height);
    const XMMATRIX projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspectRatio, 0.1f, 100.0f);

    // Volcar datos al buffer de hardware
    implementation::TransformBuffer transform{};
    XMStoreFloat4x4(&transform.worldViewProjection, XMMatrixTranspose(world * view * projection));

    engine.deviceContext->UpdateSubresource(engine.transformBuffer, 0, nullptr, &transform, 0, 0);

    // ------------------------------------------------------------------------
    // 3. Enlace de recursos al pipeline y Dibujado (Draw Call)
    // ------------------------------------------------------------------------
    constexpr UINT stride = sizeof(implementation::Vertex);
    constexpr UINT offset = 0;

    engine.deviceContext->IASetVertexBuffers(0, 1, &engine.vertexBuffer, &stride, &offset);
    engine.deviceContext->IASetIndexBuffer(engine.indexBuffer, DXGI_FORMAT_R16_UINT, 0);
    engine.deviceContext->IASetInputLayout(engine.inputLayout);
    engine.deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    engine.deviceContext->RSSetState(engine.rasterizerState);

    engine.deviceContext->VSSetShader(engine.vertexShader, nullptr, 0);
    engine.deviceContext->VSSetConstantBuffers(0, 1, &engine.transformBuffer);

    engine.deviceContext->PSSetShader(engine.pixelShader, nullptr, 0);

    // Ejecutar el dibujado basado en índices (36 índices para un cubo completo)
    engine.deviceContext->DrawIndexed(36, 0, 0);

    // ------------------------------------------------------------------------
    // 4. Presentación (Swap buffers)
    // ------------------------------------------------------------------------
    engine.swapChain->Present(1, 0);
}

void Engine::Shutdown() noexcept
{
    if (m_implementation)
    {
        m_implementation->ReleaseResources();
    }
}