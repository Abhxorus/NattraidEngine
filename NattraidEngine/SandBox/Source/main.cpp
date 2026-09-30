/**
 * @file main.cpp
 * @brief Punto de entrada principal para el Sandbox del motor gráfico.
 */

#include <Engine/Engine.h>
#include "Window.h"

 /**
  * @brief Función principal de la aplicación en entornos Windows (Win32).
  *
  * Gestiona el ciclo de vida de alto nivel de la aplicación: inicializa la ventana nativa,
  * arranca el motor gráfico, ejecuta el bucle principal de renderizado (Game Loop)
  * y asegura la liberación de recursos antes de finalizar el programa.
  *
  * @param instance Handle de la instancia actual de la aplicación proporcionado por el sistema operativo.
  * @param previousInstance Handle de la instancia anterior (obsoleto en aplicaciones Win32 modernas, siempre es NULL).
  * @param commandLine Puntero a la cadena de texto con los argumentos pasados por línea de comandos.
  * @param showCommand Bandera que indica el estado visual inicial de la ventana (ej. SW_SHOW, SW_MINIMIZE).
  * @return int Devuelve 0 si la aplicación finaliza correctamente, o 1 si se produce un error crítico de inicialización.
  */
int WINAPI wWinMain(
    HINSTANCE instance,
    HINSTANCE previousInstance,
    PWSTR commandLine,
    int showCommand
) {
    // Se suprimen las advertencias del compilador para los parámetros que exige la API de Windows pero no se utilizan
    UNREFERENCED_PARAMETER(previousInstance);
    UNREFERENCED_PARAMETER(commandLine);

    // Dimensiones iniciales del área cliente de la ventana
    constexpr UINT CLIENT_WIDTH = 1280;
    constexpr UINT CLIENT_HEIGHT = 720;

    // ========================================================================
    // 1. Creación e inicialización de la ventana nativa
    // ========================================================================
    Window window;

    if (!window.Create(instance, L"NattraidEngine", CLIENT_WIDTH, CLIENT_HEIGHT))
    {
        MessageBoxW(
            nullptr,
            L"No se pudo crear la ventana.",
            L"Window Error",
            MB_OK | MB_ICONERROR
        );
        return 1;
    }

    // ========================================================================
    // 2. Inicialización del motor gráfico y DirectX
    // ========================================================================
    Engine engine;

    if (!engine.Initialize(window.GetHandle(), CLIENT_WIDTH, CLIENT_HEIGHT))
    {
        MessageBoxW(
            window.GetHandle(),
            L"No se pudo inicializar el Engine.\n\n"
            L"Verifica que exista:\n"
            L"shaders\\Triangle.hlsl\n\n"
            L"Revisa también la ventana Output.",
            L"Engine Error",
            MB_OK | MB_ICONERROR
        );

        return 1;
    }

    // ========================================================================
    // 3. Visualización y ejecución del Bucle Principal (Game Loop)
    // ========================================================================
    window.Show(showCommand);

    while (window.ProcessMessages())
    {
        // Si la ventana está minimizada, detenemos el renderizado para ahorrar ciclos de GPU y CPU
        if (window.IsMinimized())
        {
            WaitMessage();
            continue;
        }

        // Dibuja el siguiente frame
        engine.Render();
    }

    // ========================================================================
    // 4. Limpieza y cierre seguro
    // ========================================================================
    engine.Shutdown();
    window.Destroy();

    return 0;
}