
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
	hash_destruir_dato_t destruir_dato;
};

struct hash_iter {
	const hash_t* hash;
	size_t pos;
};

size_t funcion_hash(const char* clave) {
	size_t pos;
	for (pos = 0; *clave != '\0'; clave++)
		pos = *clave + 31*pos;
	return pos % TAM_HASH;
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
size_t obtener_posicion(hash_t* hash, size_t posicion_hash, const char* clave) {

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

/* Creo un item */
item_t crear_item(void* valor, const char* clave, estado_t estado) {
	item_t item;
	item.valor = valor;
	item.clave = strdup(clave);
	item.estado = estado;
	return item;
}

hash_t* hash_crear_dimen(hash_destruir_dato_t destruir_dato, size_t dimen) {
	//Creo el hash y pido memoria dinamica
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

    item_t item = crear_item(NULL, NULL, VACIO);
	for (int i = 0; i < hash->capacidad; ++i) {
		hash->tabla[i] = item;
	}

    hash->destruir_dato = destruir_dato;

    return hash;
}

bool hash_redimensionar(hash_t* hash) {

	size_t nuevo_tam = hash->capacidad * 2;

	hash_t* nuevo_hash = hash_crear_dimen(hash->destruir_dato, nuevo_tam);
	if (nuevo_hash == NULL)
		return false;

	for (size_t i = 0; i < hash->capacidad; i++) {
		if (hash->tabla[i].estado == OCUPADO) {
			hash_guardar(
				nuevo_hash, 
				hash->tabla[i].clave, 
				hash->tabla[i].valor
			);
		}
	}

	hash_destruir(hash);
	hash = nuevo_hash;
	return true;

}

/* Crea el hash */
hash_t* hash_crear(hash_destruir_dato_t destruir_dato) {
    return hash_crear_dimen(destruir_dato, TAM_HASH);
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato) {
	
	if (clave == NULL)
		return false;

	if (hash->cantidad >= ((double) hash->capacidad * COEF_REDIM))
		return false; //TODO: Redimensionar!!

	size_t pos = funcion_hash(clave);
	pos = obtener_posicion(hash, pos, clave);
	hash->tabla[pos] = crear_item(dato, clave, OCUPADO);
	hash->cantidad++;
	
	return true; 
}

/* Borra un elemento del hash y devuelve el dato asociado.  Devuelve
 * NULL si el dato no estaba.
 * Pre: La estructura hash fue inicializada
 * Post: El elemento fue borrado de la estructura y se lo devolvió,
 * en el caso de que estuviera guardado.
 */
void* hash_borrar(hash_t *hash, const char *clave);

/* Obtiene el valor de un elemento del hash, si la clave no se encuentra
 * devuelve NULL.
 * Pre: La estructura hash fue inicializada
 */
void *hash_obtener(const hash_t *hash, const char *clave);

/* Determina si clave pertenece o no al hash.
 * Pre: La estructura hash fue inicializada
 */
bool hash_pertenece(const hash_t *hash, const char *clave);

/* Devuelve la cantidad de elementos del hash.
 * Pre: La estructura hash fue inicializada
 */
size_t hash_cantidad(const hash_t *hash) {
	return hash->cantidad;
}

/* Destruye la estructura liberando la memoria pedida y llamando a la función
 * destruir para cada par (clave, dato).
 * Pre: La estructura hash fue inicializada
 * Post: La estructura hash fue destruida
 */
void hash_destruir(hash_t *hash) {
	
}

/* Iterador del hash */

// Crea iterador
hash_iter_t *hash_iter_crear(const hash_t *hash) {
	
	hash_iter_t* iter = malloc(sizeof(hash_iter_t));
	if (iter == NULL)
		return NULL;

	iter->hash = hash;
	iter->pos = 0;

	return iter;
}

// Avanza iterador
bool hash_iter_avanzar(hash_iter_t *iter) {
	
	if (iter->pos > iter->hash->capacidad)
		return false;
	
	iter->pos++;
	return true;
}

// Devuelve clave actual, esa clave no se puede modificar ni liberar.
const char *hash_iter_ver_actual(const hash_iter_t *iter) {
	char* clave = "";

	if (iter->hash->tabla[iter->pos].estado == OCUPADO)
		clave = strdup(iter->hash->tabla[iter->pos].clave);
	
	return clave;
}

// Comprueba si terminó la iteración
bool hash_iter_al_final(const hash_iter_t *iter) {
	return iter->pos == iter->hash->capacidad;
}

// Destruye iterador
void hash_iter_destruir(hash_iter_t* iter) {
	free(iter);
}
