#define _POSIX_C_SOURCE 200809L
#include "abb.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pila.h"

typedef struct nodo_abb{
    const char* clave;
    void* valor;
    struct nodo_abb* izq;
    struct nodo_abb* der;
}abb_nodo_t;

struct abb{
    abb_nodo_t* raiz;
    abb_comparar_clave_t cmp;
    abb_destruir_dato_t destruir_dato;
    size_t cantidad;
};
struct abb_iter{
    pila_t* pila;
};

//Funciones auxiliares:
abb_nodo_t* crear_nodo(const char* clave, void* dato){
    abb_nodo_t *nodo = malloc(sizeof(abb_nodo_t));
    if (!nodo) return NULL;
    nodo->clave = strdup(clave);
    nodo->valor = dato;
    nodo->der = NULL;
    nodo->izq = NULL;
    return nodo;
}

void* borrar_nodo(abb_t* arbol, abb_nodo_t* nodo){
    void* dato = nodo->valor;
    free((char*)nodo->clave);
    free(nodo);
    arbol->cantidad--;
    return dato;
}

void swap_nodos(abb_nodo_t* nodo1, abb_nodo_t* nodo2){
    const char* clave1 = nodo1->clave;
    void* valor1 = nodo1->valor;
    nodo1->clave = nodo2->clave;
    nodo1->valor = nodo2->valor;
    nodo2->clave = clave1;
    nodo2->valor = valor1;
}

abb_nodo_t* obtener_padre(const abb_t *arbol, abb_nodo_t* nodo, const char* clave){
    if(arbol->cmp(clave, nodo->clave) > 0){
        if( nodo->der == NULL || arbol->cmp(nodo->der->clave, clave) == 0) return nodo;
        return obtener_padre(arbol, nodo->der, clave);
    } else if(arbol->cmp(clave, nodo->clave) < 0){
        if(nodo->izq == NULL || arbol->cmp(nodo->izq->clave, clave) == 0) return nodo;
        return obtener_padre(arbol, nodo->izq, clave);
    }
    return NULL;
}

//borra una hoja
void* borrar_hoja(abb_t* arbol, const char* clave, abb_nodo_t* nodo_borrar, abb_nodo_t* padre){
    if(nodo_borrar == arbol->raiz)
        arbol->raiz = NULL;
    else if(arbol->cmp(padre->clave, nodo_borrar->clave) > 0)
        padre->izq = NULL;
    else
        padre->der = NULL;
    return borrar_nodo(arbol, nodo_borrar);
}
//borra nodo con un hijo
void* borrar_un_hijo(abb_t* arbol, abb_nodo_t* nodo_borrar, abb_nodo_t* padre){
    abb_nodo_t* nodo_reemplazante;
    if (nodo_borrar->izq)
        nodo_reemplazante = nodo_borrar->izq;
    else
        nodo_reemplazante = nodo_borrar->der;
    if(padre == NULL)
        arbol->raiz = nodo_reemplazante;
    else if(arbol->cmp(padre->clave, nodo_borrar->clave) > 0)
        padre->izq = nodo_reemplazante;
    else
        padre->der = nodo_reemplazante;
    return borrar_nodo(arbol, nodo_borrar);
}
//Busco el mas a la izquierda
abb_nodo_t* buscar_mas_a_la_izquierda(abb_nodo_t* nodo){
    if (!nodo->izq) return nodo;
    return buscar_mas_a_la_izquierda(nodo->izq);
}
//borrar nodo con dos hijos
void* borrar_dos_hijos(abb_t *arbol, abb_nodo_t* nodo_borrar, abb_nodo_t* padre){
    abb_nodo_t* nodo_remplazante = buscar_mas_a_la_izquierda(nodo_borrar->der);
    abb_nodo_t* nodo_remplazo_padre = obtener_padre(arbol, arbol->raiz, nodo_remplazante->clave);
    swap_nodos(nodo_borrar, nodo_remplazante);
    if (nodo_remplazo_padre == nodo_borrar)
        nodo_remplazo_padre->der = nodo_remplazante->der;
    else
        nodo_remplazo_padre->izq = nodo_remplazante->der;
    return borrar_nodo(arbol, nodo_remplazante);
}
abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato){
    abb_t* arbol = malloc(sizeof(abb_t));
    if(!arbol) return NULL;
    arbol->raiz = NULL;
    arbol->cantidad = 0;
    arbol->cmp = cmp;
    arbol->destruir_dato = destruir_dato;
    return arbol;
}
abb_nodo_t* buscar_nodo(const abb_t *arbol, abb_nodo_t* nodo, const char* clave){
    if(arbol == NULL || nodo == NULL) return NULL;
    if(arbol->cmp(clave, nodo->clave) == 0) return nodo;
    if(arbol->cmp(clave, nodo->clave) < 0) return buscar_nodo(arbol, nodo->izq, clave);
    if(arbol->cmp(clave, nodo->clave) > 0) return buscar_nodo(arbol, nodo->der, clave);
    return NULL;
}
bool abb_guardar(abb_t *arbol, const char* clave, void* dato){
    if(clave == NULL || arbol == NULL) return false;
    abb_nodo_t* nuevo_nodo = crear_nodo(clave, dato);
    if(nuevo_nodo == NULL) return false;
    if(arbol->cantidad == 0) {
        arbol->raiz = nuevo_nodo;
        arbol->cantidad++;
        return true;
    }
    //Paso 2: Busco en el abb si existe
    abb_nodo_t* nodo_actual = buscar_nodo(arbol, arbol->raiz, clave);
    if(nodo_actual != NULL){
        //Existe, por lo tanto lo remplazo
        if(arbol->destruir_dato && nodo_actual->valor) arbol->destruir_dato(nodo_actual->valor);
        nodo_actual->valor =  dato;
    } else {
        //Paso 3: Busco en el abb donde estaria y agarro al padre
        abb_nodo_t* padre = obtener_padre(arbol, arbol->raiz, clave);
        if(padre == NULL) return NULL;
        (arbol->cmp(clave, padre->clave) > 0) ? (padre->der = nuevo_nodo) : (padre->izq = nuevo_nodo);
        arbol->cantidad++;
    }
    return true;
}
void* abb_borrar(abb_t* arbol, const char* clave){
    if(!arbol || arbol->cantidad == 0) return NULL;
    abb_nodo_t* nodo_borrar = buscar_nodo(arbol, arbol->raiz, clave);
    abb_nodo_t* padre = obtener_padre(arbol, arbol->raiz, clave);

    if(nodo_borrar == NULL) return NULL;

    if (!nodo_borrar->izq && !nodo_borrar->der)
        return borrar_hoja(arbol, clave, nodo_borrar, padre);
    else if (!nodo_borrar->izq || !nodo_borrar->der)
        return borrar_un_hijo(arbol, nodo_borrar, padre);
    else
        return borrar_dos_hijos(arbol, nodo_borrar, padre);
}
void *abb_obtener(const abb_t *arbol, const char *clave){
    abb_nodo_t* nodo = buscar_nodo(arbol, arbol->raiz, clave);
    if (nodo == NULL) return NULL;
    return nodo->valor;
}
bool abb_pertenece(const abb_t* arbol, const char* clave){
    return (buscar_nodo(arbol, arbol->raiz, clave) != NULL);
}
size_t abb_cantidad(abb_t* arbol){
    return arbol->cantidad;
}
void abb_destruir_recursivo(abb_t *arbol, abb_nodo_t* nodo){
    if(arbol == NULL || nodo == NULL) return;
    if (nodo->izq)
        abb_destruir_recursivo(arbol, nodo->izq);
    if (nodo->der)
        abb_destruir_recursivo(arbol, nodo->der);
    void* dato = borrar_nodo(arbol, nodo);
    if (arbol->destruir_dato) arbol->destruir_dato(dato);
}
void abb_destruir(abb_t* arbol){
    if (arbol == NULL) return;
    if (arbol->raiz != NULL) abb_destruir_recursivo(arbol, arbol->raiz);
    free(arbol);
}
//Iterador Interno
void in_order(abb_nodo_t* nodo, bool visitar(const char *, void *, void *), void *extra){
    if(!nodo) return;
    in_order(nodo->izq, visitar, extra);
    if(!visitar(nodo->clave, nodo->valor, extra)) return;
    in_order(nodo->der, visitar, extra);
}
void abb_in_order(abb_t* arbol, bool visitar(const char *, void *, void *), void *extra){
    in_order(arbol->raiz, visitar, extra);
}
// Iterador externo
void apilar_recursivo_inorder(pila_t* pila, abb_nodo_t* nodo){
    if (nodo == NULL) return;
    pila_apilar(pila, nodo);
    apilar_recursivo_inorder(pila, nodo->izq);
}
abb_iter_t* abb_iter_in_crear(const abb_t* arbol){
    if (!arbol) return NULL;
    abb_iter_t* iter = malloc(sizeof(abb_iter_t));
    if (!iter) return NULL;
    iter->pila = pila_crear();
    if(!iter->pila){
        free(iter);
        return NULL;
    }
    apilar_recursivo_inorder(iter->pila, arbol->raiz);
    return iter;
}
bool abb_iter_in_avanzar(abb_iter_t* iter){
    if(abb_iter_in_al_final(iter)) return false;
    abb_nodo_t* actual = pila_desapilar(iter->pila);
    if(actual->der) apilar_recursivo_inorder(iter->pila, actual->der);
    return true;
}
const char* abb_iter_in_ver_actual(const abb_iter_t* iter){
    if(pila_esta_vacia(iter->pila)) return NULL;
    abb_nodo_t* actual = pila_ver_tope(iter->pila);
    return actual->clave;
}
bool abb_iter_in_al_final(const abb_iter_t *iter){
    return(pila_esta_vacia(iter->pila));
}
void abb_iter_in_destruir(abb_iter_t* iter){
    pila_destruir(iter->pila);
    free(iter);
}