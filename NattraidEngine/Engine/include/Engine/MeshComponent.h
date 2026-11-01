/**
 * @file MeshComponent.h
 * @brief Declara la API de MeshComponent dentro del subsistema Core.
 * @ingroup core
 */
#pragma once
#include "Engine/Prerequisites.h"

 /**
  * @class MeshComponent
  * @brief Componente que almacena la informaci�n de geometr�a (malla) de una entidad.
  *
  * Un @c MeshComponent contiene los v�rtices e �ndices que describen la geometr�a de un objeto.
  * Es un componente de datos puro del ECS.
  *
  * La malla incluye:
  * - Lista de v�rtices (posici�n, normal, UV, etc.).
  * - Lista de �ndices que definen las primitivas (tri�ngulos, l�neas).
  * - Contadores de v�rtices e �ndices.
  */
class
    MeshComponent {
public:
    /**
     * @brief Constructor por defecto.
     */
    MeshComponent() : m_numVertex(0), m_numIndex(0) {}

public:
    /**
     * @brief Nombre de la malla.
     */
    std::string m_name;
    /**
     * @brief Transform local de la malla dentro del modelo importado.
     */
    DirectX::XMFLOAT4X4 m_localTransform = DirectX::XMFLOAT4X4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);

    /**
     * @brief Lista de v�rtices de la malla.
     */
    std::vector<Vertex> m_vertex;
    //std::vector<SkyboxVertex> m_skyVertex;

    /**
     * @brief Lista de �ndices que definen las primitivas de la malla.
     */
    std::vector<unsigned int> m_index;

    /**
     * @brief N�mero total de v�rtices en la malla.
     */
    int m_numVertex;

    /**
     * @brief N�mero total de �ndices en la malla.
     */
    int m_numIndex;
};