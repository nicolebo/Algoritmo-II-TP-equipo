#include "pila.h"
#include <stdlib.h>
#define CAPACIDAD 4
#define REDIMENSIONAR 2
#define MINIMO_DE_REDIMENSIONAR 4

/* Definición del struct pila proporcionado por la cátedra.
 */
struct pila {
    void** datos;
    size_t cantidad;  // Cantidad de elementos almacenados.
    size_t capacidad;  // Capacidad del arreglo 'datos'.
};

// Crea una pila.
// Post: devuelve una nueva pila vacía.
pila_t* pila_crear(void)
{
    pila_t* pila = malloc(sizeof(pila_t));
    if (pila == NULL) return NULL;
    pila->cantidad = 0;
    pila->capacidad = CAPACIDAD;
    pila->datos = malloc(pila->capacidad * sizeof(void*));
    if(pila->datos == NULL)
    {
        free(pila);
        return NULL;
    }
    return pila;
}

// cambia la dimension de una pila
//Post: devuelve true si pudo cambiar el tamano sin errores
bool cambiar_dimension(pila_t *pila, size_t tam)
{
    void *nuevos_datos = realloc(pila->datos, tam * sizeof(void*));
    if (nuevos_datos == NULL) return false;
    pila->datos = nuevos_datos;
    pila->capacidad = tam;
    return true;
}

void pila_destruir(pila_t *pila)
{
    free(pila->datos);
    free(pila);
}

bool pila_esta_vacia(const pila_t *pila)
{
    return (pila->cantidad == 0);
}

// Agrega un nuevo elemento a la pila. Devuelve falso en caso de error.
// Pre: la pila fue creada.
// Post: se agregó un nuevo elemento a la pila, valor es el nuevo tope.
bool pila_apilar(pila_t *pila, void* valor)
{
    if (pila->capacidad == pila->cantidad)
    {
        if (!cambiar_dimension(pila, pila->capacidad * REDIMENSIONAR)) return false;
    }
    pila->datos[pila->cantidad] = valor;
    pila->cantidad += 1;
    return true;

}

// Obtiene el valor del tope de la pila. Si la pila tiene elementos,
// se devuelve el valor del tope. Si está vacía devuelve NULL.
// Pre: la pila fue creada.
// Post: se devolvió el valor del tope de la pila, cuando la pila no está
// vacía, NULL en caso contrario.
void* pila_ver_tope(const pila_t *pila)
{
    if (pila_esta_vacia(pila)) return NULL;
    return pila->datos[pila->cantidad-1];
}

// Saca el elemento tope de la pila. Si la pila tiene elementos, se quita el
// tope de la pila, y se devuelve ese valor. Si la pila está vacía, devuelve
// NULL.
// Pre: la pila fue creada.
// Post: si la pila no estaba vacía, se devuelve el valor del tope anterior
// y la pila contiene un elemento menos.
void* pila_desapilar(pila_t *pila)
{
    if (pila_esta_vacia(pila)) return NULL;
    if (pila->cantidad == pila->capacidad/MINIMO_DE_REDIMENSIONAR && pila->capacidad/REDIMENSIONAR > CAPACIDAD)
    {
        cambiar_dimension(pila, pila->capacidad/REDIMENSIONAR);
    }
    pila->cantidad -= 1;
    return pila->datos[pila->cantidad];
}
