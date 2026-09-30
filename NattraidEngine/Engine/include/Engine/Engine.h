/**
 * @file Engine.h
 * @brief Declaración de la clase principal Engine para el motor gráfico.
 */

#pragma once

#include "Prerequisites.h"

 /**
  * @class Engine
  * @brief Clase núcleo responsable de la inicialización, renderizado y gestión de la API gráfica (DirectX).
  *
  * Esta clase implementa el patrón de diseño Pimpl (Pointer to Implementation) para ocultar
  * las dependencias internas de DirectX a los proyectos clientes (como el Sandbox).
  * Está marcada como final y no permite copias ni movimientos para garantizar la gestión
  * segura y única de los recursos del hardware.
  */
class ENGINE_API Engine final {
public:
    /**
     * @brief Constructor por defecto. Asigna la memoria inicial para la estructura de implementación.
     */
    Engine() noexcept;

    /**
     * @brief Destructor de la clase. Llama internamente a Shutdown() de forma segura.
     */
    ~Engine() noexcept;

    /**
     * @name Constructores y operadores eliminados
     * Se desactiva la semántica de copia y movimiento para proteger los punteros COM de DirectX.
     */
     ///@{
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    Engine(Engine&&) = delete;
    Engine& operator=(Engine&&) = delete;
    ///@}

    /**
     * @brief Inicializa el motor gráfico, el dispositivo gráfico y la cadena de intercambio (Swap Chain).
     *
     * @param nativeWindow Puntero genérico al handle de la ventana nativa (por ejemplo, HWND en Win32).
     * @param width Ancho en píxeles del área de renderizado (viewport).
     * @param height Alto en píxeles del área de renderizado (viewport).
     * @return true si la inicialización de recursos gráficos fue exitosa, false en caso de error.
     */
    bool Initialize(
        void* nativeWindow,
        std::uint32_t width,
        std::uint32_t height
    ) noexcept;

    /**
     * @brief Ejecuta el ciclo de dibujado. Limpia los buffers, actualiza la matriz de transformación y presenta el frame final.
     */
    void Render() noexcept;

    /**
     * @brief Libera explícitamente todos los recursos gráficos (Buffers, Shaders, Views) alojados en la memoria de la GPU.
     */
    void Shutdown() noexcept;

private:
    /**
     * @brief Estructura opaca que contiene los punteros nativos y estado interno (Patrón Pimpl).
     */
    struct implementation;

    implementation* m_implementation = nullptr; ///< Puntero a la implementación interna de los recursos de DirectX.
};