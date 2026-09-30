/**
 * @file API.h
 * @brief Definición de macros para la exportación e importación de símbolos de la librería dinámica (DLL).
 */

#pragma once

 // Validación exclusiva para la plataforma Windows
#if defined(_WIN32)

    /**
     * @def ENGINE_API
     * @brief Macro para gestionar la visibilidad de clases y funciones en la frontera de la DLL.
     *
     * Si el preprocesador define ENGINE_BUILD_DLL (es decir, estamos compilando el proyecto del motor),
     * la macro se evalúa como __declspec(dllexport) para exponer los símbolos públicamente.
     * Si no está definida (es decir, estamos compilando un cliente como el proyecto SandBox),
     * se evalúa como __declspec(dllimport) para consumir los símbolos de la DLL previamente compilada.
     */
#if defined(ENGINE_BUILD_DLL)
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

#else
    // Fallback genérico para sistemas operativos que no sean Windows (preparación para multiplataforma)
#define ENGINE_API
#endif