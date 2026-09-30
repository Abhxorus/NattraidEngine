/**
 * @file Window.h
 * @brief Declaración de la clase Window para la gestión de ventanas Win32.
 */

#pragma once

 // Optimizaciones de inclusión de cabeceras de Windows
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>

/**
 * @class Window
 * @brief Gestiona el ciclo de vida, los eventos y el renderizado base de una ventana nativa de Windows (Win32).
 *
 * Esta clase está marcada como final y no permite copias ni asignaciones
 * para garantizar que exista un control estricto y único sobre el handle nativo de la ventana.
 */
class Window final {
public:
    /**
     * @brief Constructor por defecto.
     */
    Window() = default;

    /**
     * @brief Destructor de la clase. Llama internamente a Destroy() de forma segura.
     */
    ~Window();

    /**
     * @name Constructores eliminados
     * Se desactiva la semántica de copia para evitar duplicar referencias al mismo HWND.
     */
     ///@{
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    ///@}

    /**
     * @brief Crea e inicializa la ventana en el sistema operativo.
     *
     * @param instance Instancia de la aplicación (HINSTANCE).
     * @param title Título que se mostrará en la barra superior.
     * @param width Ancho del área cliente de la ventana en píxeles.
     * @param height Alto del área cliente de la ventana en píxeles.
     * @return true si la ventana y la clase se registraron correctamente, false en caso de error.
     */
    bool Create(HINSTANCE instance, const wchar_t* title, UINT width, UINT height) noexcept;

    /**
     * @brief Actualiza la visualización de la ventana en pantalla.
     *
     * @param showCommand Comando de visualización de Win32 (ej. SW_SHOW, SW_MINIMIZE).
     */
    void Show(int showCommand) noexcept;

    /**
     * @brief Destruye el handle de la ventana y anula el registro de la clase Win32.
     */
    void Destroy() noexcept;

    /**
     * @brief Procesa la cola de mensajes del sistema para esta ventana.
     *
     * @return true si la aplicación debe continuar, false si recibió un mensaje WM_QUIT.
     */
    bool ProcessMessages() noexcept;

    /**
     * @brief Obtiene el handle nativo de la ventana.
     *
     * @return HWND El identificador de la ventana de Win32.
     */
    HWND GetHandle() const noexcept { return m_windowHandle; }

    /**
     * @brief Verifica si la ventana está reducida a la barra de tareas.
     *
     * @return true si la ventana está minimizada, false de lo contrario.
     */
    bool IsMinimized() const noexcept;

private:
    /**
     * @brief Procedimiento interno estático de Win32 para procesar los eventos del sistema.
     *
     * @param window Handle de la ventana que recibe el evento.
     * @param message Identificador del mensaje (ej. WM_SIZE, WM_DESTROY).
     * @param wParam Parámetro adicional del mensaje.
     * @param lParam Parámetro adicional del mensaje.
     * @return LRESULT Resultado del procesamiento del mensaje.
     */
    static LRESULT CALLBACK WindowProcedure(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

    HWND m_windowHandle = nullptr; ///< Handle nativo asignado por el sistema operativo.

    static constexpr const wchar_t* ClassName = L"NattraidEngine Window"; ///< Identificador para el registro de la clase WNDCLASSEXW.

    HINSTANCE m_instance = nullptr;  ///< Instancia actual vinculada a la ventana.
    bool m_classRegistered = false;  ///< Indica si la clase de ventana de Win32 se registró exitosamente.
};