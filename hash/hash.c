
#define _POSIX_C_SOURCE 200809L
#include "hash.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TAM_HASH 1000
#define COEF_REDIM 0.7

//Defino el estado de cada item
typedef enum{VACIO, BORRADO, OCUPADO} estado_t;

typedef struct item {
	void* valor;
	char* clave;
	estado_t estado;
} item_t;

struct hash {
	item_t* tabla;
	size_t capacidad;
	size_t cantidad;
    size_t borrados;

    hash_destruir_dato_t destruir_dato;
};

struct hash_iter {
	const hash_t* hash;
	size_t pos;
    size_t recorridos;
};

size_t funcion_hash(const char* clave, size_t tam) {
	size_t pos;
	for (pos = 0; *clave != '\0'; clave++)
		pos = *clave + 31*pos;
	return pos % tam;
}

/*
 * Se fija si las claves son iguales
 * Devuelve true si son iguales, de lo contrario devuelve false
 */

bool clave_son_iguales(const char* clave1, const char* clave2) {
    return !strcmp(clave1, clave2);
}

/*
 * Obtiene la posicion de una clave en el hash, en caso de que no exista la clave
 * se devuelve una posicion disponible para almacenarla
 */
size_t obtener_posicion(const hash_t* hash, size_t posicion_hash, const char* clave) {

    bool encontramos = false;
    while (!encontramos) {
        item_t item = hash->tabla[posicion_hash];
        switch(item.estado){
            case VACIO: encontramos = true;
                break;
            case BORRADO:
				(posicion_hash == hash->capacidad-1) ? posicion_hash = 0 : posicion_hash++;
                break;
            case OCUPADO:
                if (clave_son_iguales(item.clave, clave)) {
					encontramos = true;
				} else {
					(posicion_hash == hash->capacidad-1) ? posicion_hash = 0 : posicion_hash++;
                }
                break;
        }

    }
    return posicion_hash;
}

hash_t* hash_crear_dimen(hash_destruir_dato_t destruir_dato, size_t dimen) {

    hash_t* hash = malloc(sizeof(hash_t));
	if(hash == NULL)
		return NULL;

    hash->capacidad = dimen;

	//pido memoria dinamica para la tabla del hash
	hash->tabla = malloc(hash->capacidad * sizeof(item_t));
	if(hash->tabla == NULL) {
        free(hash);
        return NULL;
    }

    for (int i = 0; i < hash->capacidad; ++i) {
		hash->tabla[i].estado = VACIO;
	}

    hash->destruir_dato = destruir_dato;

    return hash;
}

void hash_destruir_tabla(item_t* tabla, size_t capacidad) {

    for (size_t i = 0; i < capacidad; i++) {
        free(tabla[i].clave);
        free(tabla[i].valor);
    }

    free(tabla);

}

bool hash_redimensionar(hash_t* hash) {

    size_t nuevo_tam = hash->capacidad * 2;
    item_t* nueva_tabla = malloc(sizeof(item_t) * nuevo_tam);
    if (nueva_tabla == NULL)
        return false;

    item_t* vieja_tabla = hash->tabla;
    size_t vieja_capacidad = hash->capacidad;

    hash->tabla = nueva_tabla;
    hash->capacidad = nuevo_tam;
    hash->cantidad = 0;
    hash->borrados = 0;

    for (size_t i = 0; i < vieja_capacidad; i++)
        if (vieja_tabla[i].estado == OCUPADO)
            hash_guardar(hash, vieja_tabla[i].clave, vieja_tabla[i].valor);

    free(vieja_tabla);
    return true;

}

/* Crea el hash */
hash_t* hash_crear(hash_destruir_dato_t destruir_dato) {
    return hash_crear_dimen(destruir_dato, TAM_HASH);
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato) {

    if (clave == NULL)
		return false;

	if (hash->cantidad + hash->borrados >= ((double) hash->capacidad * COEF_REDIM)) {
        if(!hash_redimensionar(hash))
            return false;
    }

	size_t pos = funcion_hash(clave, hash->capacidad);
	pos = obtener_posicion(hash, pos, clave);
	if (hash->tabla[pos].estado == VACIO) {
        hash->tabla[pos].estado = OCUPADO;
        hash->tabla[pos].clave = strdup(clave);
        hash->cantidad++;
	}
    hash->tabla[pos].valor = dato;
    return true;
}


void* hash_borrar(hash_t *hash, const char *clave) {
    if (clave == NULL) return false;

    size_t pos = funcion_hash(clave, hash->capacidad);
    pos = obtener_posicion(hash, pos, clave);

    if(hash->tabla[pos].estado != OCUPADO) return NULL;

    hash->tabla[pos].estado = BORRADO;
    hash->cantidad--;
    hash->borrados++;
    return hash->tabla[pos].valor;
}

void* hash_obtener(const hash_t *hash, const char *clave) {
	if (clave == NULL)
		return false;

	size_t pos = funcion_hash(clave, hash->capacidad);
	pos = obtener_posicion(hash, pos, clave);
	item_t item = hash->tabla[pos];
	if(item.estado == VACIO)
		return NULL;
	return item.valor;
}

bool hash_pertenece(const hash_t *hash, const char *clave) {
    if (clave == NULL)
        return false;
    size_t pos = funcion_hash(clave, hash->capacidad);
    pos = obtener_posicion(hash, pos, clave);
    item_t item = hash->tabla[pos];
    return (item.estado == OCUPADO);

}

size_t hash_cantidad(const hash_t *hash) {
	return hash->cantidad;
}

void hash_destruir(hash_t *hash) {

    for(int i = 0; i < hash->capacidad; i++){
        if(hash->tabla[i].estado != VACIO){
            if(hash->destruir_dato != NULL){
                hash->destruir_dato(hash->tabla[i].valor);
            }
            if(hash->tabla[i].estado == OCUPADO){
                hash->tabla[i].valor = NULL;
                hash->tabla[i].clave = NULL;
                hash->tabla[i].estado = VACIO;
            }
        }
    }
    hash_destruir_tabla(hash->tabla, hash->capacidad);
    free(hash);
}

/* Iterador del hash */

// Crea iterador
hash_iter_t *hash_iter_crear(const hash_t *hash) {

	hash_iter_t* iter = malloc(sizeof(hash_iter_t));
	if (iter == NULL)
		return NULL;

	iter->hash = hash;
    iter->pos = 0;
    iter->recorridos = 0;
	//Si esta vacio, me voy a la ultima posicion asi no puede avanzar mas
	if (hash_cantidad(hash) == 0){
        iter->pos = hash->capacidad;
	} else {
	    //Si tiene algo me voy hasta la primera poscion que este ocupado
        while((hash->tabla[iter->pos].estado != OCUPADO) && iter->pos < hash->capacidad-1)
            iter->pos++;
	}
	return iter;

}

bool hash_iter_avanzar(hash_iter_t *iter) {
	
	if (iter->pos > iter->hash->capacidad || hash_iter_al_final(iter))
		return false;

	iter->recorridos++;
    while ((iter->hash->tabla[iter->pos].estado != OCUPADO) && iter->pos < iter->hash->capacidad - 1) iter->pos++;
    
	return true;
}

// Devuelve clave actual, esa clave no se puede modificar ni liberar.
const char *hash_iter_ver_actual(const hash_iter_t *iter) {
    if(hash_iter_al_final(iter)) return NULL;
    return iter->hash->tabla[iter->pos].clave;

}

// Comprueba si terminó la iteración
bool hash_iter_al_final(const hash_iter_t *iter) {
	return (iter->recorridos == (iter->hash->cantidad));
}

// Destruye iterador
void hash_iter_destruir(hash_iter_t* iter) {
	free(iter);
}
