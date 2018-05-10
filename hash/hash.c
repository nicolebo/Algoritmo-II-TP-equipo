#include "hash.h"
#include <stdlib.h>
#include <string.h>
#define TAM_HASH 1000;

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
	hash_t* hash;
};

size_t funcion_hash(char* clave) {
	size_t pos;
	for (pos = 0; *clave != '\0'; clave++)
		pos = *clave + 31*pos;
	return pos % TAM_HASH;
}

/*
 * Se fija si las claves son iguales
 * Devuelve true si son iguales, de lo contrario devuelve false
 */

bool clave_son_iguales(char* clave1, char* clave2) {
    return !strcmp(clave1, clave2);
}

/*
 * Obtiene la posicion de una clave en el hash, en caso de que no exista la clave
 * se devuelve una posicion disponible para almacenarla
 */
size_t obtener_posicion(hash_t* hash, size_t posicion_hash, char* clave) {

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
item_t* crear_item(void* valor, char* clave) {
	item_t* item = malloc(sizeof(item_t));
	if (item == NULL) return NULL;
	item->valor = valor;
	item->clave = clave;
	return item;
}

/* Crea el hash */
hash_t* hash_crear(hash_destruir_dato_t destruir_dato) {
	//Creo el hash y pido memoria dinamica
	hash_t* hash = malloc(sizeof(hash_t));
	if(hash == NULL) return NULL;
	hash->capacidad = TAM_HASH;
	//pido memoria dinamica para la tabla del hash
	hash->tabla = malloc(hash->capacidad * sizeof(item_t));
	if(hash->tabla == NULL)
    {
        free(hash);
        return NULL;
    }
    hash->destruir_dato = destruir_dato;
    return hash;
}

/* Guarda un elemento en el hash, si la clave ya se encuentra en la
 * estructura, la reemplaza. De no poder guardarlo devuelve false.
 * Pre: La estructura hash fue inicializada
 * Post: Se almacenó el par (clave, dato)
 */
bool hash_guardar(hash_t *hash, const char *clave, void *dato) {
	if (hash->cantidad => hash->capacidad)
		return false; //TODO: Redimensionar!!
	size_t pos = funcion_hash(clave);
	//if (hash->tabla[pos]->clave)
	return true; //Provisorio
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
size_t hash_cantidad(const hash_t *hash);

/* Destruye la estructura liberando la memoria pedida y llamando a la función
 * destruir para cada par (clave, dato).
 * Pre: La estructura hash fue inicializada
 * Post: La estructura hash fue destruida
 */
void hash_destruir(hash_t *hash);

/* Iterador del hash */

// Crea iterador
hash_iter_t *hash_iter_crear(const hash_t *hash);

// Avanza iterador
bool hash_iter_avanzar(hash_iter_t *iter);

// Devuelve clave actual, esa clave no se puede modificar ni liberar.
const char *hash_iter_ver_actual(const hash_iter_t *iter);

// Comprueba si terminó la iteración
bool hash_iter_al_final(const hash_iter_t *iter);

// Destruye iterador
void hash_iter_destruir(hash_iter_t* iter);
