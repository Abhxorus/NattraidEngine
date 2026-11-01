#include <Engine/Engine.h>
#include <Engine/MeshComponent.h>
#include <Rendering/Device.h>
#include <Rendering/DeviceContext.h>
#include <Rendering/Texture2D.h>
#include <Rendering/Buffer.h>


struct
    Engine::Implementation {
    struct alignas(16) TransformBuffer
    {
        DirectX::XMFLOAT4X4 worldViewProjection;
    };

    HWND window = nullptr;

    std::uint32_t width = 0;
    std::uint32_t height = 0;

    Device device;
    DeviceContext context;
    IDXGISwapChain* swapChain = nullptr;
    ID3D11RenderTargetView* renderTarget = nullptr;
    Texture2D backBuffer;
    Texture2D depthStencilBuffer;
    ID3D11DepthStencilView* depthStencilView = nullptr;

    Buffer vertexBuffer;
    Buffer indexBuffer;
    Buffer transformBuffer;

    ID3D11RasterizerState* rasterizerState = nullptr;

    std::chrono::steady_clock::time_point startTime{};

    MeshComponent mesh;

    ID3D11VertexShader* vertexShader = nullptr;
    ID3D11PixelShader* pixelShader = nullptr;
    ID3D11InputLayout* inputLayout = nullptr;

    static bool
        CompileShader(const wchar_t* filename, const char* entryPoint,
            const char* shaderModel, ID3DBlob** shaderBlob) noexcept {
        if (!filename || !entryPoint || !shaderModel || !shaderBlob) {
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

        if (errors)
        {
            OutputDebugStringA(
                static_cast<const char*>(
                    errors->GetBufferPointer()
                    )
            );

            SafeRelease(errors);
        }

        if (FAILED(result))
        {
            SafeRelease(*shaderBlob);
            return false;
        }

        return true;
    }

    void ReleaseResources() noexcept
    {
        //if (context)
        //{
        //  context->ClearState();
        //  context->Flush();
        //}


        SafeRelease(rasterizerState);
        transformBuffer.destroy();
        //SafeRelease(transformBuffer);
        //SafeRelease(indexBuffer);
        //SafeRelease(vertexBuffer);
        indexBuffer.destroy();
        vertexBuffer.destroy();
        SafeRelease(inputLayout);
        SafeRelease(pixelShader);
        SafeRelease(vertexShader);

        SafeRelease(depthStencilView);
        depthStencilBuffer.destroy();
        //SafeRelease(depthStencilBuffer);
        SafeRelease(renderTarget);

        SafeRelease(swapChain);
        backBuffer.destroy();
        //SafeRelease(context);
        context.destroy();
        device.destroy();
        //SafeRelease(device);

        window = nullptr;
        width = 0;
        height = 0;
    }

};

Engine::Engine() noexcept
    : m_implementation(
        new (std::nothrow) Implementation{}
    )
{
}

Engine::~Engine() noexcept
{
    Shutdown();

    delete m_implementation;
    m_implementation = nullptr;
}

bool Engine::Initialize(
    void* nativeWindow,
    std::uint32_t width,
    std::uint32_t height
) noexcept
{
    if (!m_implementation ||
        !nativeWindow ||
        width == 0 ||
        height == 0)
    {
        return false;
    }

    Implementation& engine = *m_implementation;

    // Permite reinicializar la instancia de forma segura.
    engine.ReleaseResources();

    engine.window = static_cast<HWND>(nativeWindow);
    engine.width = width;
    engine.height = height;

    // Swapchain configuration 
    /* Brief: This configuration is used to create the swapchain for the Direct3D device.
  It specifies the number of buffers, the format of the back buffer, the refresh rate,
  and other properties related to how the swapchain will behave.*/
    DXGI_SWAP_CHAIN_DESC swapChainDescription{};

    swapChainDescription.BufferCount = 2;
    swapChainDescription.BufferDesc.Width = engine.width;
    swapChainDescription.BufferDesc.Height = engine.height;
    swapChainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDescription.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDescription.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDescription.OutputWindow = engine.window;
    swapChainDescription.SampleDesc.Count = 1;
    swapChainDescription.SampleDesc.Quality = 0; // Antialising
    swapChainDescription.Windowed = TRUE;
    swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    constexpr D3D_FEATURE_LEVEL featureLevels[]{
        D3D_FEATURE_LEVEL_11_0
    };

    D3D_FEATURE_LEVEL selectedFeatureLevel{};

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
        engine.device.getDeviceAddress(),
        &selectedFeatureLevel,
        engine.context.getDeviceContextAddress()
    );

    // Si falla la GPU física, utiliza el rasterizador
    // por software de Windows.
    if (FAILED(result))
    {
        SafeRelease(engine.swapChain);
        engine.context.destroy();
        //SafeRelease(engine.context);
        engine.device.destroy();
        //SafeRelease(engine.device);

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
            engine.device.getDeviceAddress(),
            &selectedFeatureLevel,
            engine.context.getDeviceContextAddress()
        );
    }

    if (FAILED(result))
    {
        engine.ReleaseResources();
        return false;
    }

    result = engine.swapChain->GetBuffer(
        0,
        __uuidof(ID3D11Texture2D),
        reinterpret_cast<void**>(engine.backBuffer.GetTextureAddress())
    );

    if (FAILED(result))
    {
        ERROR("Engine", "Initialize", ("Failed to get swap chain buffer." + std::to_string(result)).c_str());
        engine.ReleaseResources();
        return false;
    }

    result = engine.device.CreateRenderTargetView(engine.backBuffer.GetTexture(),
        nullptr,
        &engine.renderTarget);

    if (FAILED(result))
    {
        engine.ReleaseResources();
        return false;
    }

    result = engine.depthStencilBuffer.init(engine.device,
        engine.width,
        engine.height,
        DXGI_FORMAT_D24_UNORM_S8_UINT,
        D3D11_BIND_DEPTH_STENCIL);

    if (FAILED(result))
    {
        engine.ReleaseResources();
        return false;
    }

    result = engine.device.CreateDepthStencilView(engine.depthStencilBuffer.GetTexture(), nullptr, &engine.depthStencilView);

    if (FAILED(result))
    {
        engine.ReleaseResources();
        return false;
    }

    D3D11_VIEWPORT viewport{};

    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;

    viewport.Width =
        static_cast<float>(engine.width);

    viewport.Height =
        static_cast<float>(engine.height);

    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    engine.context.RSSetViewports(1, &viewport);

    ID3DBlob* vertexShaderBlob = nullptr;
    ID3DBlob* pixelShaderBlob = nullptr;

    if (!Implementation::CompileShader(
        L"shaders\\Cube.hlsl",
        "VSMain",
        "vs_5_0",
        &vertexShaderBlob))
    {
        engine.ReleaseResources();
        return false;
    }

    if (!Implementation::CompileShader(
        L"shaders\\Cube.hlsl",
        "PSMain",
        "ps_5_0",
        &pixelShaderBlob))
    {
        SafeRelease(vertexShaderBlob);
        engine.ReleaseResources();
        return false;
    }

    result = engine.device.CreateVertexShader(vertexShaderBlob->GetBufferPointer(),
        vertexShaderBlob->GetBufferSize(),
        nullptr,
        &engine.vertexShader);

    if (FAILED(result))
    {
        SafeRelease(pixelShaderBlob);
        SafeRelease(vertexShaderBlob);
        engine.ReleaseResources();
        return false;
    }

    result = engine.device.CreatePixelShader(pixelShaderBlob->GetBufferPointer(),
        pixelShaderBlob->GetBufferSize(),
        nullptr,
        &engine.pixelShader);

    if (FAILED(result))
    {
        SafeRelease(pixelShaderBlob);
        SafeRelease(vertexShaderBlob);
        engine.ReleaseResources();
        return false;
    }

    constexpr D3D11_INPUT_ELEMENT_DESC inputElements[]{
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, static_cast<UINT>(offsetof(Vertex, position)), D3D11_INPUT_PER_VERTEX_DATA, 0},
    { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, static_cast<UINT>(offsetof(Vertex, color)),D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    result = engine.device.CreateInputLayout(inputElements,
        ARRAYSIZE(inputElements),
        vertexShaderBlob->GetBufferPointer(),
        vertexShaderBlob->GetBufferSize(),
        &engine.inputLayout);

    SafeRelease(pixelShaderBlob);
    SafeRelease(vertexShaderBlob);

    if (FAILED(result))
    {
        engine.ReleaseResources();
        return false;
    }

    constexpr Vertex vertices[]{
        // Frente
        {
            { -1.0f,  1.0f, -1.0f },
            {  1.0f,  0.0f,  0.0f, 1.0f }
        },
        {
            {  1.0f,  1.0f, -1.0f },
            {  0.0f,  1.0f,  0.0f, 1.0f }
        },
        {
            {  1.0f, -1.0f, -1.0f },
            {  0.0f,  0.0f,  1.0f, 1.0f }
        },
        {
            { -1.0f, -1.0f, -1.0f },
            {  1.0f,  1.0f,  0.0f, 1.0f }
        },

        // Atrás
        {
            { -1.0f,  1.0f, 1.0f },
            {  1.0f,  0.0f, 1.0f, 1.0f }
        },
        {
            {  1.0f,  1.0f, 1.0f },
            {  0.0f,  1.0f, 1.0f, 1.0f }
        },
        {
            {  1.0f, -1.0f, 1.0f },
            {  1.0f,  1.0f, 1.0f, 1.0f }
        },
        {
            { -1.0f, -1.0f, 1.0f },
            {  0.2f,  0.4f, 1.0f, 1.0f }
        }
    };

    // Store vertices in the mesh component
    for (const auto& vertex : vertices) {
        engine.mesh.m_vertex.push_back(vertex);
    }

    engine.mesh.m_numVertex = static_cast<unsigned int>(engine.mesh.m_vertex.size());

    result = engine.vertexBuffer.init(engine.device, engine.mesh, D3D11_BIND_VERTEX_BUFFER);

    if (FAILED(result))
    {
        engine.ReleaseResources();
        return false;
    }

    constexpr unsigned int indices[]
    {
        // Frente
        0, 1, 2,
        0, 2, 3,

        // Atrás
        5, 4, 7,
        5, 7, 6,

        // Izquierda
        4, 0, 3,
        4, 3, 7,

        // Derecha
        1, 5, 6,
        1, 6, 2,

        // Arriba
        4, 5, 1,
        4, 1, 0,

        // Abajo
        3, 2, 6,
        3, 6, 7
    };

    // Store indices in the mesh component
    for (const auto& index : indices) {
        engine.mesh.m_index.push_back(index);
    }

    engine.mesh.m_numIndex = static_cast<std::uint32_t>(engine.mesh.m_index.size());

    result = engine.indexBuffer.init(engine.device, engine.mesh, D3D11_BIND_INDEX_BUFFER);

    if (FAILED(result))
    {
        engine.ReleaseResources();
        return false;
    }

    result = engine.transformBuffer.init(engine.device, sizeof(Implementation::TransformBuffer));

    if (FAILED(result))
    {
        engine.ReleaseResources();
        return false;
    }

    D3D11_RASTERIZER_DESC rasterizerDescription{};

    rasterizerDescription.FillMode = D3D11_FILL_SOLID;

    rasterizerDescription.CullMode = D3D11_CULL_NONE;

    rasterizerDescription.DepthClipEnable = TRUE;

    result = engine.device.CreateRasterizerState(&rasterizerDescription, &engine.rasterizerState);

    if (FAILED(result))
    {
        engine.ReleaseResources();
        return false;
    }

    engine.startTime =
        std::chrono::steady_clock::now();

    return true;
}

void Engine::Render() noexcept
{
    if (!m_implementation)
        return;

    Implementation& engine = *m_implementation;

    if (!engine.context.getDeviceContext() ||
        !engine.swapChain ||
        !engine.renderTarget ||
        !engine.depthStencilView ||
        !engine.vertexBuffer.getBuffer() ||
        !engine.indexBuffer.getBuffer() ||
        !engine.transformBuffer.getBuffer() ||
        !engine.inputLayout ||
        !engine.vertexShader ||
        !engine.pixelShader)
    {
        return;
    }

    constexpr float clearColor[]
    {
        0.03f,
        0.04f,
        0.08f,
        1.0f
    };

    engine.context.OMSetRenderTargets(
        1,
        &engine.renderTarget,
        engine.depthStencilView
    );

    engine.context.ClearRenderTargetView(
        engine.renderTarget,
        clearColor
    );

    engine.context.ClearDepthStencilView(
        engine.depthStencilView,
        D3D11_CLEAR_DEPTH |
        D3D11_CLEAR_STENCIL,
        1.0f,
        0
    );

    const auto currentTime =
        std::chrono::steady_clock::now();

    const float elapsedSeconds =
        std::chrono::duration<float>(
            currentTime - engine.startTime
        ).count();

    using namespace DirectX;

    const XMMATRIX world =
        XMMatrixRotationX(elapsedSeconds * 0.4f) *
        XMMatrixRotationY(elapsedSeconds * 0.8f);

    const XMVECTOR cameraPosition =
        XMVectorSet(0.0f, 1.5f, -5.0f, 1.0f);

    const XMVECTOR cameraTarget =
        XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

    const XMVECTOR cameraUp =
        XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    const XMMATRIX view =
        XMMatrixLookAtLH(
            cameraPosition,
            cameraTarget,
            cameraUp
        );

    const float aspectRatio =
        static_cast<float>(engine.width) /
        static_cast<float>(engine.height);

    const XMMATRIX projection =
        XMMatrixPerspectiveFovLH(
            XM_PIDIV4,
            aspectRatio,
            0.1f,
            100.0f
        );

    Implementation::TransformBuffer transform{};

    XMStoreFloat4x4(&transform.worldViewProjection, XMMatrixTranspose(world * view * projection));

    engine.transformBuffer.update(engine.context,
        engine.transformBuffer.getBuffer(),
        0,
        nullptr,
        &transform,
        0,
        0);

    engine.vertexBuffer.render(engine.context, 0, 1);

    engine.indexBuffer.render(engine.context, 0, 1, false, DXGI_FORMAT_R32_UINT);

    engine.context.IASetInputLayout(
        engine.inputLayout
    );

    engine.context.IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
    );

    engine.context.RSSetState(
        engine.rasterizerState
    );

    engine.context.VSSetShader(
        engine.vertexShader,
        nullptr,
        0
    );

    engine.transformBuffer.render(engine.context, 0, 1);

    engine.context.PSSetShader(
        engine.pixelShader,
        nullptr,
        0
    );

    engine.context.DrawIndexed(
        engine.mesh.m_numIndex,
        0,
        0
    );

    engine.swapChain->Present(1, 0);
}

void Engine::Shutdown() noexcept
{
    if (m_implementation)
        m_implementation->ReleaseResources();
}