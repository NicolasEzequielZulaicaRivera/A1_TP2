#include <stdbool.h>
#include "defendiendo_torres.h"

static const char ANIMO_BUENO   = 'B';
static const char ANIMO_REGULAR = 'R';
static const char ANIMO_MALO    = 'M';

static const char VACIO  = ' ';
static const char CAMINO = '_';

static const char ORCO  = 'O';
//static const int RES_ORCO  = 200;
//static const int RES_ORCO_RAND  = 100;

static const char ELFO  = 'L';
static const char ENANO = 'G';
static const int ATK_ELF = 30;
static const int ATK_ENA = 60;
static const int ATK_NUL =  0;
// static const int CRITICO_ELF =  70;
// static const int CRITICO_ENA = 100;
static const int PROB_CRIT_NUL =  0;
static const int PROB_CRIT_MAL =  0;
static const int PROB_CRIT_REG = 10;
static const int PROB_CRIT_BNO = 25;

static const int DIV_VIENTO_A_FALLO   = 2;
static const int DIV_HUMEADAD_A_FALLO = 2;


static const int RESISTENICA_TORRE_INI = 600;
static const int ELFOS_EXTRA_INI  = 10;
static const int ENANOS_EXTRA_INI = 10;
//static const int COSTO_ELFOS_EXTRA  = 50;
//static const int COSTO_ENANOS_EXTRA = 50;

static const int ESTADO_JUGANDO = 0;
static const int ESTADO_GANADO  = 1;
static const int ESTADO_PERDIDO =-1;

static const int CANTIDAD_NIVELES = 4;

/*
 * Compara si dos cordenadas son iguales
 * Devolvera:
 * > true si lo son
 * > false si no lo son
 */
bool misma_coordenada( coordenada_t p1, coordenada_t p2);

/*
 * Busca una coordenada en un vector de coordenadas
 * Devolvera:
 * > la posicion en el vector si esta
 * > -1 si no esta
 */
int buscar_coordenada( coordenada_t vec[], int tope , coordenada_t coord);

/*
 * Busca un defensor que este en cierta coordenada en un vector de defensores
 * Devolvera:
 * > la posicion en el vector si esta
 * >-1 si no esta
 */
int buscar_defensor_en_coordenada( defensor_t vec[], int tope , coordenada_t coord);

/*
 * Crea un defensor de un tipo y posicion dadas 
 */
defensor_t nuevo_defensor( char tipo, coordenada_t posicion );

void inicializar_juego(juego_t* juego, int viento, int humedad, char animo_legolas, char animo_gimli){

	juego->critico_legolas = 
		(animo_legolas == ANIMO_BUENO)?   PROB_CRIT_BNO:
		(animo_legolas == ANIMO_REGULAR)? PROB_CRIT_REG:
		(animo_legolas == ANIMO_MALO)?    PROB_CRIT_MAL:
		PROB_CRIT_NUL;

	juego->critico_gimli = 
		(animo_gimli == ANIMO_BUENO)?   PROB_CRIT_BNO:
		(animo_gimli == ANIMO_REGULAR)? PROB_CRIT_REG:
		(animo_gimli == ANIMO_MALO)?    PROB_CRIT_MAL:
		PROB_CRIT_NUL;

	juego->fallo_legolas = (viento / DIV_VIENTO_A_FALLO);
	juego->fallo_gimli = (humedad / DIV_HUMEADAD_A_FALLO);

	juego->torres.resistencia_torre_1 = RESISTENICA_TORRE_INI;
	juego->torres.resistencia_torre_2 = RESISTENICA_TORRE_INI;
	juego->torres.elfos_extra = ELFOS_EXTRA_INI;
	juego->torres.enanos_extra = ENANOS_EXTRA_INI;
}

int estado_juego(juego_t juego){

	int estado_juego = ESTADO_JUGANDO;

	if( (juego.torres.resistencia_torre_1 == 0) || (juego.torres.resistencia_torre_2 == 0) )
		estado_juego = ESTADO_PERDIDO;
	else if(juego.nivel_actual > CANTIDAD_NIVELES)
		estado_juego = ESTADO_GANADO;

	return estado_juego;
}

int estado_nivel(nivel_t nivel){

	int estado_nivel = ESTADO_GANADO;
	int i = 0;

	while( (i < nivel.tope_enemigos) && (estado_nivel == ESTADO_GANADO) ){

		if( nivel.enemigos[i].vida > 0 )
			estado_nivel = ESTADO_JUGANDO;

		i++;
	}

	return estado_nivel;
}

int agregar_defensor(nivel_t* nivel, coordenada_t posicion, char tipo){

	bool es_posible = true;

	es_posible = (posicion.fil < MAX_FILAS) && (posicion.col < MAX_COLUMNAS);

	if( es_posible )
		es_posible = ( buscar_coordenada( nivel->camino_1, nivel->tope_camino_1, posicion) == -1 );

	if( es_posible )
		es_posible = ( buscar_coordenada( nivel->camino_2, nivel->tope_camino_2, posicion) == -1 );

	if( es_posible )
		es_posible = ( buscar_defensor_en_coordenada( nivel->defensores, nivel->tope_defensores, posicion) == -1 );

	if( es_posible ){

		nivel->defensores[ nivel->tope_defensores ] = nuevo_defensor(tipo, posicion);

		(nivel->tope_defensores)++;

	}

	return es_posible;
}

void jugar_turno(juego_t* juego){

	juego->nivel.tope_enemigos --;

}

bool misma_coordenada( coordenada_t p1, coordenada_t p2){

	return (p1.fil == p2.fil) && (p1.col == p2.col);
}

int buscar_coordenada( coordenada_t vec[], int tope , coordenada_t coord){

	int pos = -1;
	int i = 0 ; 
	while( (pos == -1) && (i < tope) ){
		if( misma_coordenada(coord, vec[i]) )
				pos = i;			
		i++;
	}

	return pos;
}

int buscar_defensor_en_coordenada( defensor_t vec[], int tope , coordenada_t coord){
	int pos = -1;
	int i = 0 ; 
	while( (pos == -1) && (i < tope) ){
		if( misma_coordenada(coord, vec[i].posicion) )
				pos = i;			
		i++;
	}

	return pos;
}

defensor_t nuevo_defensor( char tipo, coordenada_t posicion ){

	defensor_t nuevo_defensor;
	nuevo_defensor.posicion = posicion;
	nuevo_defensor.tipo = tipo;
	nuevo_defensor.fuerza_ataque =
		( tipo == ELFO )? ATK_ELF :
		( tipo == ENANO)? ATK_ENA :
		ATK_NUL;

	return nuevo_defensor;
}


//----- MOTOR GRAFICO ----- (?)

#include <string.h>

#define SPRITE_SIZE 2 // >= 2
#define MAX_SPRITES 10

typedef char sprite_t[SPRITE_SIZE+1];

typedef struct sprite_map{
	sprite_t sprites[MAX_SPRITES];
	char indice[MAX_SPRITES];
	int tope;
}sprite_map_t;

// carga los sprites del mapa de sprites
void iniciar_sprites( sprite_map_t* sprite_map );

/*
 * Busca el sprite que corresponde a un indice determinado
 * > devuelve el ultimo sprite que se corresponda si hay
 * > devuelve un sprite vacio si no hay
 */
void buscar_sprite( sprite_map_t sprite_map, char indice , sprite_t* sprite);

// muestra datos relevantes el juego y nivel
void mostrar_datos(juego_t juego);

/*
 * Carga un mapa de caracteres segun el juego
 * pre: reccibe un nivel valido
 */
void cargar_mapa( char mapa[MAX_FILAS][MAX_COLUMNAS], nivel_t nivel);

/*
 * Muestra con formato un arreglo,
 * cambiando los caracteres que tengan un sprite asignado
 * por dicho sprite
 */
void mostrar_mapa( char mapa[MAX_FILAS][MAX_COLUMNAS] );

void mostrar_juego(juego_t juego){

	system("clear");

	mostrar_datos( juego);

	char mapa[MAX_FILAS][MAX_COLUMNAS];

	cargar_mapa( mapa, juego.nivel );

	mostrar_mapa(mapa);
}

void mostrar_datos(juego_t juego){
	printf("\n Nivel: %i ",juego.nivel_actual);
	printf("\t Torre 1: %i ",juego.torres.resistencia_torre_1);
	printf("\t Torre 2: %i ",juego.torres.resistencia_torre_2);
	printf("\t Enemigos: %i ",juego.nivel.tope_enemigos);// <-- ACTUALIZAR 
	printf("\n");
}

void mostrar_mapa( char mapa[MAX_FILAS][MAX_COLUMNAS] ){
	
	sprite_map_t sprite_map;
	iniciar_sprites( &sprite_map );

	sprite_t sprite;

	char espacio_blanco[SPRITE_SIZE];
	int i,j;
	for(i = 0; i < SPRITE_SIZE-2;i++)
		espacio_blanco[i]=' ';
	espacio_blanco[i]='\0';

	printf("\n    ");
	for(j=0 ; j < MAX_COLUMNAS; j++){
		printf("%s", espacio_blanco);
		if( j < 10 )
			printf("0");
		printf("%i|", j);
	}
	for (i = 0; i < MAX_FILAS; i++)
	{
		printf("\n");
		if( i < 10 )
			printf("0");
		printf("%i| ",i);
		for( j = 0; j < MAX_COLUMNAS; j++ ){
			buscar_sprite( sprite_map ,mapa[i][j], &sprite);
			printf("%s ",sprite);
		}
	}
	printf("\n\n");
}

// --------------------  ACA  --------------------
void iniciar_sprites( sprite_map_t* sprite_map ){

	sprite_map->tope = 0;

	sprite_map->indice [ sprite_map->tope ] = VACIO;
	strcpy(sprite_map->sprites[ sprite_map->tope ], "  ");
	(sprite_map->tope)++;

	sprite_map->indice [ sprite_map->tope ] = ORCO;
	strcpy(sprite_map->sprites[ sprite_map->tope ], ":(");
	(sprite_map->tope)++;

	sprite_map->indice [ sprite_map->tope ] = ELFO;
	strcpy(sprite_map->sprites[ sprite_map->tope ], "tD");
	(sprite_map->tope)++;

	sprite_map->indice [ sprite_map->tope ] = ENANO;
	strcpy(sprite_map->sprites[ sprite_map->tope ], "tP");
	(sprite_map->tope)++;

	sprite_map->indice [ sprite_map->tope ] = 'T';
	strcpy(sprite_map->sprites[ sprite_map->tope ], "[]");
	(sprite_map->tope)++;

	sprite_map->indice [ sprite_map->tope ] = CAMINO;
	strcpy(sprite_map->sprites[ sprite_map->tope ], "##");
	(sprite_map->tope)++;
}

void buscar_sprite( sprite_map_t sprite_map, char indice , sprite_t* sprite){

	int i;
	for (i = 0; i < SPRITE_SIZE; i++)
		(*sprite)[i] = VACIO;

	for(i = 0; i < sprite_map.tope; i++ )
		if( indice == sprite_map.indice[i] ){
			strcpy( (*sprite), sprite_map.sprites[i] );
			//return; // -> el primero				
		}
}

void cargar_mapa( char mapa[MAX_FILAS][MAX_COLUMNAS], nivel_t nivel){

	int i, j, k;
	for (i = 0; i < MAX_FILAS; i++)
		for( j = 0; j < MAX_COLUMNAS; j++ )
			mapa[i][j] = VACIO;

	for ( k = 0; k < nivel.tope_camino_1 ; k++)
		mapa[ nivel.camino_1[k].fil ][ nivel.camino_1[k].col ] = CAMINO;

	for ( k = 0; k < nivel.tope_camino_2 ; k++)
		mapa[ nivel.camino_2[k].fil ][ nivel.camino_2[k].col ] = CAMINO;

	for ( k = 0; k < nivel.tope_enemigos ; k++){
		if(nivel.enemigos[k].vida > 0)
			mapa[k%10][k%20] = ORCO; //obvio que no
	}
};