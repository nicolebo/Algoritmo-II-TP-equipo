#define _POSIX_C_SOURCE 200809L
#include "abb.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


typedef struct nodo_abb{
    const char* clave;
    void* valor;
    struct nodo_abb* izq;
    struct nodo_abb* der;
}abb_nodo_t;

struct abb {
    abb_nodo_t* raiz;
    abb_comparar_clave_t cmp;
    abb_destruir_dato_t destruir_dato;
    size_t cantidad;
};

abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato) {
    abb_t* arbol = malloc(sizeof(abb_t));
    if(!arbol) return NULL;
    arbol->raiz = NULL;
    arbol->cantidad = 0;
    arbol->cmp = cmp;
    arbol->destruir_dato = destruir_dato;
    return arbol;
}

abb_nodo_t* crear_nodo(const char *clave, void *dato) {
    abb_nodo_t *nodo = malloc(sizeof(abb_nodo_t));
    if (!nodo) return NULL;
    nodo->clave = strdup(clave);
    nodo->valor = dato;
    nodo->der = NULL;
    nodo->izq = NULL;
    return nodo;
}

abb_nodo_t* obtener_padre(const abb_t *arbol, abb_nodo_t* nodo, const char* clave) {
    if(nodo == NULL) return NULL;
    if(arbol->cmp(nodo->clave, clave) < 0) {
        if(nodo->izq == NULL || arbol->cmp(nodo->izq->clave, clave) == 0) return nodo;
        return obtener_padre(arbol, nodo->izq, clave);
    } else if (arbol->cmp(nodo->clave, clave) > 0) {
        if(nodo->der == NULL || arbol->cmp(nodo->der->clave, clave) == 0) return nodo;
        return obtener_padre(arbol, nodo->der, clave);
    }
    return NULL;
}
abb_nodo_t* buscar_nodo(const abb_t *arbol, abb_nodo_t* nodo, const char* clave) {
    if(nodo == NULL) return NULL;
    if(arbol->cmp(nodo->clave, clave) == 0) return nodo;
    if(nodo->der != NULL) return buscar_nodo(arbol, nodo->der, clave);
    if(arbol->cmp(nodo->clave, clave) < 0) return buscar_nodo(arbol, nodo->izq, clave);
    return NULL;
}

bool abb_guardar(abb_t *arbol, const char *clave, void *dato) {
    if(clave == NULL) return false;
    abb_nodo_t* nuevo_nodo = crear_nodo(clave, dato);
    if(nuevo_nodo == NULL) return false;

    //Paso 1: No tiene raiz
    if(abb_cantidad(arbol) == 0) {
        arbol->raiz = nuevo_nodo;
        arbol->cantidad++;
        return true;
    }

    //Paso 2: Busco en el abb si existe
    abb_nodo_t* nodo_actual = buscar_nodo(arbol, arbol->raiz, clave);
    if(nodo_actual != NULL){
        //Existe, por lo tanto lo remplazo
        if(arbol->destruir_dato != NULL) arbol->destruir_dato(nodo_actual->valor);
        nodo_actual->valor =  dato;
    } else {
        //Paso 3: Busco en el abb donde estaria y agarro al padre
        abb_nodo_t* padre = obtener_padre(arbol, arbol->raiz, clave);
        (arbol->cmp(padre->clave, clave) < 0) ? (padre->izq = nuevo_nodo) : (padre->der = nuevo_nodo);
    }
    return true;
}

void *abb_borrar(abb_t *arbol, const char *clave);

void *abb_obtener(const abb_t *arbol, const char *clave) {
    return buscar_nodo(arbol, arbol->raiz, clave);
}

bool abb_pertenece(const abb_t *arbol, const char *clave) {
    return (buscar_nodo(arbol, arbol->raiz, clave) != NULL);
}

size_t abb_cantidad(abb_t *arbol) {
    return arbol->cantidad;
}

void abb_destruir(abb_t *arbol);
