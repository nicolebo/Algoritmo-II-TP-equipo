#include "hash.h"
#include <stdlib.h>

#define TAM_HASH 1000;

//Defino el estado de cada item
typedef enum{VACIO, BORRADO, DATO} estado_t;

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

}

/* Borra un elemento del hash y devuelve el dato asociado.  Devuelve
 * NULL si el dato no estaba.
 * Pre: La estructura hash fue inicializada
 * Post: El elemento fue borrado de la estructura y se lo devolvió,
 * en el caso de que estuviera guardado.
 */
void *hash_borrar(hash_t *hash, const char *clave);

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
