/**
 * @file Window.cpp
 * @brief Implementación de los métodos de la clase Window para la gestión nativa en Win32.
 */

#include "Window.h"
#include <Engine/Engine.h>

 /**
  * @brief Destructor de la clase Window.
  *
  * Llama al método Destroy() para asegurar la liberación correcta
  * de los recursos de la ventana nativa antes de destruir el objeto.
  */
Window::~Window()
{
    Destroy();
}

/**
 * @brief Crea la ventana nativa y ajusta su tamaño al área cliente especificada.
 *
 * @param instance Instancia de la aplicación (HINSTANCE).
 * @param title Título que aparecerá en la barra superior de la ventana.
 * @param width Ancho deseado para el área de renderizado (área cliente).
 * @param height Alto deseado para el área de renderizado (área cliente).
 * @return true si la ventana se creó satisfactoriamente, false en caso de error.
 */
bool Window::Create(HINSTANCE instance, const wchar_t* title, UINT width, UINT height) noexcept
{
    // Validación inicial para evitar doble creación o parámetros inválidos
    if (m_windowHandle || !instance || !title || width == 0 || height == 0)
    {
        return false;
    }

    m_instance = instance;

    // 1. Configurar y registrar la clase de ventana en Win32
    WNDCLASSEXW windowClass{};
    windowClass.cbSize = sizeof(WNDCLASSEXW);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProcedure;
    windowClass.hInstance = m_instance;
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    windowClass.lpszClassName = ClassName;

    if (!RegisterClassExW(&windowClass))
    {
        return false;
    }

    m_classRegistered = true;

    // 2. Configurar el estilo y calcular el tamaño exterior de la ventana
    constexpr DWORD windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

    RECT rectangle{ 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };

    // Ajusta el rectángulo considerando los bordes y la barra de título
    if (!AdjustWindowRect(&rectangle, windowStyle, FALSE))
    {
        Destroy();
        return false;
    }

    const int outerWidth = rectangle.right - rectangle.left;
    const int outerHeight = rectangle.bottom - rectangle.top;

    // 3. Crear la instancia de la ventana
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
        this // Puntero a la instancia de la clase, útil para recuperar el objeto en WindowProcedure
    );

    if (!m_windowHandle)
    {
        Destroy();
        return false;
    }

    return true;
}

/**
 * @brief Hace visible la ventana y actualiza su área cliente.
 *
 * @param showCommand Comando de estilo visual (ej. SW_SHOW).
 */
void Window::Show(int showCommand) noexcept
{
    if (m_windowHandle)
    {
        ShowWindow(m_windowHandle, showCommand);
        UpdateWindow(m_windowHandle);
    }
}

/**
 * @brief Libera el handle de la ventana y elimina la clase registrada en Windows.
 */
void Window::Destroy() noexcept
{
    // Destruir la ventana física si existe
    if (m_windowHandle)
    {
        DestroyWindow(m_windowHandle);
        m_windowHandle = nullptr;
    }

    // Desregistrar la clase del sistema operativo
    if (m_classRegistered)
    {
        UnregisterClassW(ClassName, m_instance);
        m_classRegistered = false;
    }

    m_instance = nullptr;
}

/**
 * @brief Extrae y traduce los mensajes de la cola de Windows de forma no bloqueante.
 *
 * @return true si se deben seguir procesando frames, false si se recibió un WM_QUIT.
 */
bool Window::ProcessMessages() noexcept
{
    MSG message{};

    // PeekMessage permite mantener el game loop corriendo sin pausar el hilo principal
    while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
    {
        if (message.message == WM_QUIT)
        {
            return false;
        }

        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    return true;
}

/**
 * @brief Verifica el estado de minimización de la ventana.
 *
 * @return true si la ventana está minimizada (icono), false si es visible normalmente.
 */
bool Window::IsMinimized() const noexcept
{
    return m_windowHandle && IsIconic(m_windowHandle);
}

/**
 * @brief Callback estático que procesa los mensajes del sistema enviados a la ventana.
 *
 * @param handle Handle nativo de la ventana que recibe el evento.
 * @param message Identificador numérico del mensaje del sistema.
 * @param wParam Datos adicionales del mensaje.
 * @param lParam Datos adicionales del mensaje.
 * @return LRESULT Respuesta del procedimiento para el sistema operativo.
 */
LRESULT CALLBACK Window::WindowProcedure(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_ERASEBKGND:
        // Retornar 1 (true) evita que Windows pinte un fondo blanco/negro antes de renderizar,
        // eliminando los parpadeos ya que DirectX se encarga de limpiar el back buffer.
        return 1;

    case WM_DESTROY:
        // Envía el mensaje WM_QUIT a la cola de mensajes al cerrar la ventana.
        PostQuitMessage(0);
        return 0;

    default:
        // Mensajes no manejados se pasan al procedimiento por defecto de Windows.
        return DefWindowProcW(handle, message, wParam, lParam);
    }
}