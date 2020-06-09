#include <stdbool.h>
#include "defendiendo_torres.h"


//----- CONSTANTES COMUNES ----- (!)
	static const char ANIMO_BUENO   = 'B';
	static const char ANIMO_REGULAR = 'R';
	static const char ANIMO_MALO    = 'M';
	static const char VACIO  = ' ';
	static const char CAMINO = '#';
	static const char CAMINO_1 = '!';
	static const char CAMINO_2 = '"';
	static const char TORRE   = 'T';
	static const char TORRE_1 = '1';
	static const char TORRE_2 = '2';
	static const char ENTRADA = 'E';
	static const char ORCO  = 'O';
	static const char ELFO  = 'L';
	static const char ENANO = 'G';
	//static const char SIN_TIPO = 'X';
//----- CONSTANTES COMUNES ----- (¡)

//----- HEADER MOTOR GRAFICO ----- (!)
	// Hubiera usado 2 librerias
	
	#include <string.h>
	
	#define SPRITE_SIZE 2 // >= 2
	#define MAX_SPRITES 15
	
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
	 * Muestra con formato un arreglo,
	 * cambiando los caracteres que tengan un sprite asignado
	 * por dicho sprite
	 */
	void mostrar_mapa( char mapa[MAX_FILAS][MAX_COLUMNAS] , int dimension);
	
	
	//Recibe un nivel valido y devuelve la dimension de su mapa 
	int dimension(nivel_t nivel); 
//----- HEADER MOTOR GRAFICO ----- (¡)

//----- HEADER MOTOR DE JUEGO ----- (!)
	// Hubiera usado 2 librerias
	// #include "motor_grafico.h"
	
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

	static const int INVALIDO =-1;
	
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

	// Subprocesos de jugar_turno()
	void jugar_turno_enanos(juego_t* juego);
	void jugar_turno_elfos (juego_t* juego);
	void jugar_turno_orcos (juego_t* juego);


	/*
	 * Carga un mapa de caracteres segun el juego
	 * pre: reccibe un nivel valido
	 */
	void cargar_mapa( char mapa[MAX_FILAS][MAX_COLUMNAS], nivel_t nivel);

//----- HEADER MOTOR DE JUEGO ----- (¡)

//----- MOTOR DE JUEGO ----- (!)
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

		juego->nivel.tope_camino_1 = 0;
		juego->nivel.tope_camino_2 = 0;
		juego->nivel.tope_defensores = 0;
		juego->nivel.tope_enemigos = 0;

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

		// ESTA DENTRO DEL TABLERO -> aplicar dimension
		es_posible = (posicion.fil < MAX_FILAS) && (posicion.col < MAX_COLUMNAS);
		
		// NO ESTA EN EL CAMINO 1
		if( es_posible )
			es_posible = ( buscar_coordenada( nivel->camino_1, nivel->tope_camino_1, posicion) == INVALIDO );
		
		// NO ESTA EN EL CAMINO 2
		if( es_posible )
			es_posible = ( buscar_coordenada( nivel->camino_2, nivel->tope_camino_2, posicion) == INVALIDO );
	
		if( es_posible )
			es_posible = ( buscar_defensor_en_coordenada( nivel->defensores, nivel->tope_defensores, posicion) == INVALIDO );
	
		if( es_posible ){
	
			nivel->defensores[ nivel->tope_defensores ] = nuevo_defensor(tipo, posicion);
	
			(nivel->tope_defensores)++;
	
		}
	
		return (es_posible? 0 : -1);
	}
	
	void jugar_turno(juego_t* juego){
	
		// map?

		jugar_turno_enanos( juego );

		jugar_turno_elfos ( juego );

		jugar_turno_orcos ( juego );
	
		// FIN
		if( juego->torres.resistencia_torre_1 < 0 )
			juego->torres.resistencia_torre_1 = 0;
		if( juego->torres.resistencia_torre_2 < 0 )
			juego->torres.resistencia_torre_2 = 0;
	}

	void mostrar_juego(juego_t juego){

		system("clear");

		mostrar_datos( juego );

		char mapa[MAX_FILAS][MAX_COLUMNAS];
		int dim = dimension( juego.nivel );

		cargar_mapa( mapa, juego.nivel );

		mostrar_mapa(mapa, dim);
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
		int pos = INVALIDO;
		int i = 0 ; 
		while( (pos == INVALIDO) && (i < tope) ){
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

	void jugar_turno_enanos(juego_t* juego){
		return;
	}

	void jugar_turno_elfos (juego_t* juego){
		return;
	}

	void jugar_turno_orcos (juego_t* juego){
		int i;
		bool mover_1, mover_2;
	
		mover_1 = ( juego->nivel.tope_camino_1 > 2 );
		mover_2 = ( juego->nivel.tope_camino_2 > 2 );
	
		for(i = 0; i < juego->nivel.tope_enemigos; i++){
	
			if( juego->nivel.enemigos[i].vida > 0 ){
	
				if( juego->nivel.enemigos[i].pos_en_camino > 0 ){
					(juego->nivel.enemigos[i].pos_en_camino)++;
	
					// NOOOOO
					juego->nivel.enemigos[i].vida -= 
						rand() % 
						(800 /
							( 
								(juego->nivel.tope_camino_1 > juego->nivel.tope_camino_2)?
								(juego->nivel.tope_camino_1) : (juego->nivel.tope_camino_2)
							) 
						) 
						* (i%3+1);
					// NOOOOO */
	
					if( (juego->nivel.enemigos[i].camino == 1) && 
						(juego->nivel.enemigos[i].pos_en_camino >= juego->nivel.tope_camino_1-1 ) ){
							juego->torres.resistencia_torre_1 -= juego->nivel.enemigos[i].vida;
							juego->nivel.enemigos[i].vida = 0;
					}else if( (juego->nivel.enemigos[i].camino == 2) && 
						(juego->nivel.enemigos[i].pos_en_camino >= juego->nivel.tope_camino_2-1 ) ){
							juego->torres.resistencia_torre_2 -= juego->nivel.enemigos[i].vida;
							juego->nivel.enemigos[i].vida = 0;
					}
				}
				else if( mover_1 ){
					juego->nivel.enemigos[i].pos_en_camino = 1;
					juego->nivel.enemigos[i].camino = 1;
					mover_1 = false;
				}
				else if( mover_2 ){
					juego->nivel.enemigos[i].pos_en_camino = 1;
					juego->nivel.enemigos[i].camino = 2;
					mover_2 = false;
				}
	
			}
	
		}
	}

	void cargar_mapa( char mapa[MAX_FILAS][MAX_COLUMNAS], nivel_t nivel){
	
		int i, j, k;
		//int dim = dimension(nivel);
		// coordenada_t pos; // asignacion defectuosa ? <- 2020/6/7 22:20 - No recibia fil y col correctas => se uso i,j
		for (i = 0; i < MAX_FILAS; i++)
			for( j = 0; j < MAX_COLUMNAS; j++ )
				mapa[i][j] = VACIO;
	
		// CAMINO 1
		for ( k = 0; k < nivel.tope_camino_1 ; k++)
			mapa[ nivel.camino_1[k].fil ][ nivel.camino_1[k].col ] = CAMINO_1;

		// CAMINO 2
		for ( k = 0; k < nivel.tope_camino_2 ; k++)
			mapa[ nivel.camino_2[k].fil ][ nivel.camino_2[k].col ] = CAMINO_2;

		// ENTRADA Y TORRE  1
		if( nivel.tope_camino_1 > 2){
			mapa[ nivel.camino_1[0].fil ][ nivel.camino_1[0].col ] = ENTRADA;
			mapa[ nivel.camino_1[nivel.tope_camino_1-1].fil ][ nivel.camino_1[nivel.tope_camino_1-1].col ] = TORRE_1;		
		}

		// ENTRADA Y TORRE  2
		if( nivel.tope_camino_2 > 2){
			mapa[ nivel.camino_2[0].fil ][ nivel.camino_2[0].col ] = ENTRADA;
			mapa[ nivel.camino_2[ nivel.tope_camino_2-1].fil ][ nivel.camino_2[nivel.tope_camino_2-1].col ] = TORRE_2;		
		}
	
		// ENMIGOS
		for ( k = 0; k < nivel.tope_enemigos ; k++){
	
			if( nivel.enemigos[k].pos_en_camino > 0 ){
				if(nivel.enemigos[k].vida > 0){
	
					if( (nivel.enemigos[k].camino == 1) && ( nivel.tope_camino_1 > 2 ) ){
						i = nivel.camino_1[ nivel.enemigos[k].pos_en_camino ].fil;
						j = nivel.camino_1[ nivel.enemigos[k].pos_en_camino ].col;
						mapa[ i ][ j ] = ORCO;
					}else if( (nivel.enemigos[k].camino == 2) && ( nivel.tope_camino_2 > 2 ) ){
						i = nivel.camino_2[ nivel.enemigos[k].pos_en_camino ].fil;
						j = nivel.camino_2[ nivel.enemigos[k].pos_en_camino ].col;
						mapa[ i ][ j ] = ORCO;
					}
		
				}else{
					//MOSTRAR CADAVER. ah re morboso
				}
			}
		}	

		// DEFENSORES
		for( k = 0; k < nivel.tope_defensores ; k++ ){

			i = nivel.defensores[k].posicion.fil;
			j = nivel.defensores[k].posicion.col;
			mapa[i][j] = nivel.defensores[k].tipo;
		}
	};
//----- MOTOR DE JUEGO ----- (¡)

//----- MOTOR GRAFICO ----- (!)
 
	void mostrar_datos(juego_t juego){
		printf("\n Nivel: %i ",juego.nivel_actual);
		printf("\t Torre 1: %i ",juego.torres.resistencia_torre_1);
		printf("\t Torre 2: %i ",juego.torres.resistencia_torre_2);
		printf("\t Enemigos: %i ",juego.nivel.tope_enemigos);// <-- ACTUALIZAR 
		printf("\n");
	}
	
	void mostrar_mapa( char mapa[MAX_FILAS][MAX_COLUMNAS], int dimension ){
		
		sprite_map_t sprite_map;
		iniciar_sprites( &sprite_map );
	
		sprite_t sprite;
	
		char espacio_blanco[SPRITE_SIZE];
		int i,j;
		for(i = 0; i < SPRITE_SIZE-2;i++)
			espacio_blanco[i]=' ';
		espacio_blanco[i]='\0';
	
		printf("\n    ");
		for(j=0 ; j < dimension; j++){
			printf("%s", espacio_blanco);
			if( j < 10 )
				printf("0");
			printf("%i|", j);
		}
		for (i = 0; i < dimension; i++)
		{
			printf("\n");
			if( i < 10 )
				printf("0");
			printf("%i| ",i);
			for( j = 0; j < dimension; j++ ){
				buscar_sprite( sprite_map ,mapa[i][j], &sprite);
				printf("%s ",sprite);
			}
		}
		printf("\n\n");
	}
	
	// ------------  INICIAR SPRITES ACA ------------
	void iniciar_sprites( sprite_map_t* sprite_map ){
	
		sprite_map->tope = 0;
	
		sprite_map->indice [ sprite_map->tope ] = VACIO;
		strcpy(sprite_map->sprites[ sprite_map->tope ], "  ");
		(sprite_map->tope)++;
	
		sprite_map->indice [ sprite_map->tope ] = ORCO;
		strcpy(sprite_map->sprites[ sprite_map->tope ], ":(");
		(sprite_map->tope)++;
	
		sprite_map->indice [ sprite_map->tope ] = ELFO;
		strcpy(sprite_map->sprites[ sprite_map->tope ], "{L");
		(sprite_map->tope)++;
	
		sprite_map->indice [ sprite_map->tope ] = ENANO;
		strcpy(sprite_map->sprites[ sprite_map->tope ], "TG");
		(sprite_map->tope)++;
	
		sprite_map->indice [ sprite_map->tope ] = TORRE;
		strcpy(sprite_map->sprites[ sprite_map->tope ], "TT");
		(sprite_map->tope)++;
	
		sprite_map->indice [ sprite_map->tope ] = TORRE_1;
		strcpy(sprite_map->sprites[ sprite_map->tope ], "T1");
		(sprite_map->tope)++;
	
		sprite_map->indice [ sprite_map->tope ] = TORRE_2;
		strcpy(sprite_map->sprites[ sprite_map->tope ], "T2");
		(sprite_map->tope)++;
	
		sprite_map->indice [ sprite_map->tope ] = ENTRADA;
		strcpy(sprite_map->sprites[ sprite_map->tope ], "##");
		(sprite_map->tope)++;
	
		sprite_map->indice [ sprite_map->tope ] = CAMINO;
		strcpy(sprite_map->sprites[ sprite_map->tope ], "[]");
		(sprite_map->tope)++;
		sprite_map->indice [ sprite_map->tope ] = CAMINO_1;
		strcpy(sprite_map->sprites[ sprite_map->tope ], "[]");
		(sprite_map->tope)++;
		sprite_map->indice [ sprite_map->tope ] = CAMINO_2;
		strcpy(sprite_map->sprites[ sprite_map->tope ], "()");
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
	
	int dimension(nivel_t nivel){
	
		int dimension = 0;
		int i;
	
		if( nivel.tope_camino_1 > 2 ){
	
			int aux[4] = {
				nivel.camino_1[0].fil,
				nivel.camino_1[0].col,
				nivel.camino_1[ nivel.tope_camino_1 -1 ].fil,
				nivel.camino_1[ nivel.tope_camino_1 -1 ].col
			};
	
			for (i = 0; i < 4; i++)
				dimension = ( aux[i]>dimension )? aux[i] : dimension;
	
		}
	
		if( nivel.tope_camino_2 > 2 ){
	
			int aux[4] = {
				nivel.camino_2[0].fil,
				nivel.camino_2[0].col,
				nivel.camino_2[ nivel.tope_camino_2 -1 ].fil,
				nivel.camino_2[ nivel.tope_camino_2 -1 ].col
			};
	
			for (i = 0; i < 4; i++)
				dimension = ( aux[i]>dimension )? aux[i] : dimension;
	
		}
	
		return dimension+1;
	}
//----- MOTOR GRAFICO ----- (¡)