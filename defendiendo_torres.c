#include <stdbool.h>
#include "defendiendo_torres.h"


//----- CONSTANTES COMUNES ----- (!)
	static const char ANIMO_BUENO   = 'B';
	static const char ANIMO_REGULAR = 'R';
	static const char ANIMO_MALO    = 'M';
	static const char VACIO  = ' ';
	static const char FIL_PAR  = 'f';
	static const char COL_PAR  = 'c';
	static const char CAMINO = '#';
	static const char CAMINO_1 = '[';
	static const char CAMINO_2 = '(';
	static const char CAMINO_1Y2 = '{';
	static const char TORRE   = 'T';
	static const char TORRE_1 = '1';
	static const char TORRE_2 = '2';
	static const char ENTRADA = 'E';
	static const char ORCO  = 'O';
	static const char ORCO_SANO= 'O';
	static const char ORCO_DEBIL= 'o';
	static const char ORCO_MORIBUNDO= 'd';
	static const char ELFO  = 'L';
	static const char ENANO = 'G';
	//static const char SIN_TIPO = 'X';
//----- CONSTANTES COMUNES ----- (¡)

//----- MOTOR GRAFICO ----- (!)	
	#include <string.h>
	
	#define MAX_ESTILO_SPRITE 20 // \033[1;31;40m\033[0m
	#define TAMANO_SPRITE 2 // >= 2
	#define MAX_SPRITES 20
	
	typedef char sprite_t[TAMANO_SPRITE+MAX_ESTILO_SPRITE+1];
	
	typedef struct mapa_sprites{
		sprite_t sprites[MAX_SPRITES];
		char indice[MAX_SPRITES];
		int tope;
	}mapa_sprites_t;

	// ------------  INICIAR SPRITES ------------
		static const sprite_t SRPITE_VACIO  	= "\033[0;40m  \033[0m";
		static const sprite_t SRPITE_ORCO  		= "\033[1;31;41m¶■\033[0m";
		static const sprite_t SRPITE_ORCO_SANO		= "\033[1;31;41m¶■\033[0m";
		static const sprite_t SRPITE_ORCO_DEBIL		= "\033[2;31;41m¶ \033[0m";
		static const sprite_t SRPITE_ORCO_MORIBUNDO	= "\033[2;31;40m¶ \033[0m";
		static const sprite_t SRPITE_ELFO  		= "\033[1;6;102m{i\033[0m";
		static const sprite_t SRPITE_ENANO 		= "\033[1;6;104mtT\033[0m";
		static const sprite_t SRPITE_TORRE  	= "TT";
		static const sprite_t SRPITE_TORRE_1	= "\033[1;97;44mT1\033[0m";
		static const sprite_t SRPITE_TORRE_2	= "\033[1;97;42mT2\033[0m";
		static const sprite_t SRPITE_ENTRADA	= "\033[1;5;91;45m##\033[0m";
		static const sprite_t SRPITE_CAMINO 	= "00";
		static const sprite_t SRPITE_CAMINO_1 	= "\033[94;44;52m[]\033[0m";
		static const sprite_t SRPITE_CAMINO_2 	= "\033[92;42;51m[]\033[0m";
		static const sprite_t SRPITE_CAMINO_1Y2 = "\033[96;46m[]\033[0m";
		static const sprite_t SRPITE_FIL_PAR  	= "\033[2;40m__\033[0m";
		static const sprite_t SRPITE_COL_PAR  	= "\033[2;40m  \033[0m";
	// ------------  INICIAR SPRITES ------------

	const sprite_t ESTILO_MAPA = "\033[0;40m";
	const sprite_t FIN_ESTILO = "\033[0m";
	
	// carga los sprites del mapa de sprites
	void iniciar_sprites( mapa_sprites_t* mapa_sprites );
	void iniciar_sprites_orcos( mapa_sprites_t* mapa_sprites );
	void iniciar_sprites_defensores( mapa_sprites_t* mapa_sprites );
	void iniciar_sprites_caminos( mapa_sprites_t* mapa_sprites );
	void iniciar_sprites_varios( mapa_sprites_t* mapa_sprites );
	
	/*
	 * Busca el sprite que corresponde a un indice determinado
	 * > devuelve el ultimo sprite que se corresponda si hay
	 * > devuelve un sprite vacio si no hay
	 * - Estan hardcodeados en la funcion pero deberian salir de un archivo
	 */
	void buscar_sprite( mapa_sprites_t mapa_sprites, char indice , sprite_t* sprite);
	
	/*
	 * Muestra con formato un arreglo,
	 * cambiando los caracteres que tengan un sprite asignado
	 * por dicho sprite
	 */
	void mostrar_mapa( char mapa[MAX_FILAS][MAX_COLUMNAS] , int dimension);

	// subrutinas de mostrar_mapa()
	void mostrar_borde_vertical_mapa( int i );
	void mostrar_borde_horizontal_mapa( int dimension );

	// carga un sprite con espacios en blanco segun el tamaño de los sprites
	void cargar_espacio_blanco( sprite_t espacio_blanco );
//----- MOTOR GRAFICO ----- (¡)

//----- MOTOR DE JUEGO ----- (!)
	#define MAX_ENEMIGOS 500

	static const int ATAQUE_ELFO = 30;
	static const int ATAQUE_ENANO = 60;
	static const int ATAQUE_NULO =  0;
	static const int RANGO_ELFO =  3;
	static const int RANGO_ENANO =  1;
	static const int CRITICO_ELFO =  70;
	static const int CRITICO_ENANO = 100;
	static const int PROBABILIDAD_CRITICO_NULA =  0;
	static const int PROBABILIDAD_CRITICO_MALA =  0;
	static const int PROBABILIDAD_CRITICO_REGULAR = 10;
	static const int PROBABILIDAD_CRITICO_BUENA = 25;

	static const int RESISTENCIA_ORCO  = 200;
    static const int RESISTENCIA_ORCO_RAND  = 100;
	
	static const int DIV_VIENTO_A_FALLO   = 2;
	static const int DIV_HUMEADAD_A_FALLO = 2;
	
	
	static const int RESISTENICA_TORRE_INICIAL = 600;
	static const int ELFOS_EXTRA_INICIAL  = 10;
	static const int ENANOS_EXTRA_INICIAL = 10;
	
	static const int ESTADO_JUGANDO = 0;
	static const int ESTADO_GANADO  = 1;
	static const int ESTADO_PERDIDO =-1;

	static const int INVALIDO =-1;
	
	static const int CANTIDAD_NIVELES = 4;
	static const int CAMINO_MIN = 2;
	
	// calcula el dano critico en funcion del animo
	int dano_critico( char animo );

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

	// Los defensores de un tipo especificado juegan su turno
	void jugar_turno_defensores (juego_t* juego, char tipo);

	/* 
	 * El defensor k de los defensores del nivel actual juega su turno
	 * 	segun los parametros especificados
	 * pre: el defensor es del tipo especificado
	 */
	void jugar_turno_defensor (juego_t* juego, char tipo, int k,
		int prob_fallo, int probabilidad_critico, int dano_critico);

	// Los orcos juegan su turno
	void jugar_turno_orcos (juego_t* juego);

	/*
	 * Los orcos se mueven para adelante en sus respectivos caminos
	 * Los orcos que lleguen a la torre de su camino la atacan y mueren
	 */
	void mover_orcos (juego_t* juego);

	// Genera un orco al inicio de cada camino en el nivel
	void generar_orcos (juego_t* juego);

	// Calcula si un enemigo esta en rango de un enano/elfo
	bool en_rango_enano( coordenada_t pos, coordenada_t enemigo );
	bool en_rango_elfo( coordenada_t pos, coordenada_t enemigo );

	/*
	 * Carga un mapa de caracteres segun el juego
	 * pre: reccibe un nivel valido
	 */
	void cargar_mapa( char mapa[MAX_FILAS][MAX_COLUMNAS], nivel_t nivel);
	
	// subrutinas de cargar_mapa()
	void cargar_caminos_mapa( char mapa[MAX_FILAS][MAX_COLUMNAS], nivel_t nivel);
	void cargar_enemigos_mapa( char mapa[MAX_FILAS][MAX_COLUMNAS], nivel_t nivel);
	void cargar_defensores_mapa( char mapa[MAX_FILAS][MAX_COLUMNAS], nivel_t nivel);

	// Compara si una coordenada esta dentro de los limites del mapa
	bool coordenada_valida( coordenada_t coordenada );

	// muestra datos relevantes el juego y nivel
	void mostrar_datos(juego_t juego);
//----- MOTOR DE JUEGO ----- (¡)

//----- UTILES ----- (!)
	//Recibe un nivel valido y devuelve la dimension de su mapa 
	int dimension(nivel_t nivel); 
//----- UTILES ----- (¡)

//----- MOTOR DE JUEGO ----- (!)
	void inicializar_juego(juego_t* juego, int viento, int humedad, char animo_legolas, char animo_gimli){
	
		juego->critico_legolas = dano_critico(animo_legolas);
	
		juego->critico_gimli = dano_critico(animo_gimli);
	
		juego->fallo_legolas = (viento / DIV_VIENTO_A_FALLO);
		juego->fallo_gimli = (humedad / DIV_HUMEADAD_A_FALLO);
	
		juego->torres.resistencia_torre_1 = RESISTENICA_TORRE_INICIAL;
		juego->torres.resistencia_torre_2 = RESISTENICA_TORRE_INICIAL;
		juego->torres.elfos_extra = ELFOS_EXTRA_INICIAL;
		juego->torres.enanos_extra = ENANOS_EXTRA_INICIAL;

		//NIVEL
		juego->nivel.tope_camino_1 = 0;
		juego->nivel.tope_camino_2 = 0;
		juego->nivel.tope_defensores = 0;
		juego->nivel.tope_enemigos = 0;
		for(int i=0; i<MAX_ENEMIGOS; i++){
			juego->nivel.enemigos[i].camino=0;
			juego->nivel.enemigos[i].vida=0;
			juego->nivel.enemigos[i].pos_en_camino=0;
		}
	}
	
	int estado_juego(juego_t juego){
	
		int estado_juego = ESTADO_JUGANDO;
	
		if( (juego.torres.resistencia_torre_1 == 0) || (juego.torres.resistencia_torre_2 == 0) )
			estado_juego = ESTADO_PERDIDO;
		else if( (juego.nivel_actual > CANTIDAD_NIVELES) || 
				 (juego.nivel_actual == CANTIDAD_NIVELES && estado_nivel(juego.nivel)==ESTADO_GANADO) 
				)
			estado_juego = ESTADO_GANADO;
	
		return estado_juego;
	}
	
	int estado_nivel(nivel_t nivel){
	
		int estado_nivel = ESTADO_GANADO;
		int i = 0;
	
		if( nivel.tope_enemigos < nivel.max_enemigos_nivel )
			estado_nivel = ESTADO_JUGANDO;

		while( (i < nivel.tope_enemigos) && (estado_nivel == ESTADO_GANADO) ){
	
			if( nivel.enemigos[i].vida > 0 )
				estado_nivel = ESTADO_JUGANDO;
	
			i++;
		}

		if(nivel.max_enemigos_nivel == 0)
			estado_nivel = ESTADO_GANADO;
	
		return estado_nivel;
	}
	
	int agregar_defensor(nivel_t* nivel, coordenada_t posicion, char tipo){
	
		bool es_posible = true;

		// ESTA DENTRO DEL TABLERO
		es_posible = es_posible && 
			coordenada_valida(posicion);
		
		// NO ESTA EN EL CAMINO 1
		es_posible = es_posible && 
			( buscar_coordenada( nivel->camino_1, nivel->tope_camino_1, posicion) == INVALIDO );
		
		// NO ESTA EN EL CAMINO 2
		es_posible = es_posible && 
			( buscar_coordenada( nivel->camino_2, nivel->tope_camino_2, posicion) == INVALIDO );
	
		// NO HAY DEFENSORES
		es_posible = es_posible && 
			( buscar_defensor_en_coordenada( nivel->defensores, nivel->tope_defensores, posicion) == INVALIDO );
	
		// AGREGAR
		if( es_posible ){
	
			nivel->defensores[ nivel->tope_defensores ] = nuevo_defensor(tipo, posicion);
	
			(nivel->tope_defensores)++;
	
		}
	
		return (es_posible? 0 : -1);
	}
	
	void jugar_turno(juego_t* juego){

		jugar_turno_defensores( juego , ENANO);

		jugar_turno_defensores( juego , ELFO);
		
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
	
	int dano_critico( char animo ){

		int dano_critico = 
			(animo == ANIMO_BUENO)?   PROBABILIDAD_CRITICO_BUENA:
			(animo == ANIMO_REGULAR)? PROBABILIDAD_CRITICO_REGULAR:
			(animo == ANIMO_MALO)?    PROBABILIDAD_CRITICO_MALA:
			PROBABILIDAD_CRITICO_NULA;

		return dano_critico;
	}

	bool misma_coordenada( coordenada_t p1, coordenada_t p2){
	
		return (p1.fil == p2.fil) && (p1.col == p2.col);
	}
	
	int buscar_coordenada( coordenada_t vec[], int tope , coordenada_t coordenada){
	
		int pos = INVALIDO;
		int i = 0 ; 
		while( (pos == -1) && (i < tope) ){
			if( misma_coordenada(coordenada, vec[i]) )
					pos = i;			
			i++;
		}
	
		return pos;
	}
	
	int buscar_defensor_en_coordenada( defensor_t vec[], int tope , coordenada_t coordenada){
		int pos = INVALIDO;
		int i = 0 ; 
		while( (pos == INVALIDO) && (i < tope) ){
			if( misma_coordenada(coordenada, vec[i].posicion) )
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
			( tipo == ELFO )? ATAQUE_ELFO :
			( tipo == ENANO)? ATAQUE_ENANO :
			ATAQUE_NULO;
	
		return nuevo_defensor;
	}

	void jugar_turno_defensores (juego_t* juego, char tipo){

		int probabilidad_critico =
			(tipo==ENANO)? juego->critico_gimli :
			(tipo==ELFO)? juego->critico_legolas:
			PROBABILIDAD_CRITICO_NULA;

		int prob_fallo =
			(tipo==ENANO)? juego->fallo_gimli :
			(tipo==ELFO)? juego->fallo_legolas:
			PROBABILIDAD_CRITICO_NULA;

		int dano_critico =
			(tipo==ENANO)? CRITICO_ENANO :
			(tipo==ELFO)? CRITICO_ELFO :
			ATAQUE_NULO;


		for( int k=0 ; k < juego->nivel.tope_defensores ; k++)
			if( juego->nivel.defensores[k].tipo == tipo  )
				jugar_turno_defensor ( juego, tipo, k,
					prob_fallo, probabilidad_critico, dano_critico);
		

		return;
	}

	void jugar_turno_defensor (juego_t* juego, char tipo, int k,
		int prob_fallo, int probabilidad_critico, int dano_critico){

		bool atacar = true, en_rango = false;
		int dano = 0;
		coordenada_t pos, pos_ataque;

		pos = juego->nivel.defensores[k].posicion;
		for( int m = 0; m < juego->nivel.tope_enemigos; m++ ){

			pos_ataque.fil = pos_ataque.col = INVALIDO;
			if( juego->nivel.enemigos[m].camino == 1 )
				pos_ataque = juego->nivel.camino_1[ juego->nivel.enemigos[m].pos_en_camino ];
			if( juego->nivel.enemigos[m].camino == 2 )
				pos_ataque = juego->nivel.camino_2[ juego->nivel.enemigos[m].pos_en_camino ];
			
			en_rango =
				(tipo==ENANO)? en_rango_enano(pos,pos_ataque):
				(tipo==ELFO)? en_rango_elfo(pos,pos_ataque):
				false;

			if( 
				atacar &&
				(juego->nivel.enemigos[m].vida > 0) &&
				en_rango
			){
				
				if( rand()%100 >= prob_fallo ){
					dano = juego->nivel.defensores[k].fuerza_ataque;
					if( rand()%100 < probabilidad_critico  )
								dano = dano_critico;
					juego->nivel.enemigos[m].vida -= dano;
				}
				if(tipo == ENANO)
					atacar = false;// return;
			}
		}

	}

	bool en_rango_enano( coordenada_t pos, coordenada_t enemigo ){

		if( !coordenada_valida(pos) || !coordenada_valida(enemigo) )
			return false;

		int distancia_fil,distancia_col;
		distancia_fil = pos.fil - enemigo.fil;
		distancia_col = pos.col - enemigo.col;
		if( distancia_fil < 0 ) distancia_fil *= -1;
		if( distancia_col < 0 ) distancia_col *= -1;

		if( (distancia_fil > RANGO_ENANO) || (distancia_col > RANGO_ENANO) )
			return false;

		return true;
	}

	bool en_rango_elfo( coordenada_t pos, coordenada_t enemigo ){

		if( !coordenada_valida(pos) || !coordenada_valida(enemigo) )
			return false;

		int distancia_fil,distancia_col;
		distancia_fil = pos.fil - enemigo.fil;
		distancia_col = pos.col - enemigo.col;
		if( distancia_fil < 0 ) distancia_fil *= -1;
		if( distancia_col < 0 ) distancia_col *= -1;

		if( (distancia_fil + distancia_col) > RANGO_ELFO )
			return false;

		return true;
	}

	void jugar_turno_orcos (juego_t* juego){
		
		mover_orcos (juego);
	
		generar_orcos (juego);	
	}

	void mover_orcos (juego_t* juego){

		for(int i = 0; i < juego->nivel.tope_enemigos; i++){

			if( juego->nivel.enemigos[i].vida > 0 ){

				if( juego->nivel.enemigos[i].pos_en_camino >= 0 ){

					(juego->nivel.enemigos[i].pos_en_camino)++;
	
					if( 
						(juego->nivel.enemigos[i].camino == 1) && 
						(juego->nivel.enemigos[i].pos_en_camino >= juego->nivel.tope_camino_1-1 ) 
					){
						juego->torres.resistencia_torre_1 -= juego->nivel.enemigos[i].vida;
						juego->nivel.enemigos[i].vida = 0;
					}
					if( 
						(juego->nivel.enemigos[i].camino == 2) && 
						(juego->nivel.enemigos[i].pos_en_camino >= juego->nivel.tope_camino_2-1 ) 
					){
						juego->torres.resistencia_torre_2 -= juego->nivel.enemigos[i].vida;
						juego->nivel.enemigos[i].vida = 0;
					}
				}

			}else{
				juego->nivel.enemigos[i].vida = 0;
			}
		}
	}

	void generar_orcos (juego_t* juego){
	
		if( 
			juego->nivel.tope_camino_1 >= CAMINO_MIN && 
			juego->nivel.tope_enemigos < juego->nivel.max_enemigos_nivel 
		){
			juego->nivel.enemigos[juego->nivel.tope_enemigos].pos_en_camino = 0;
			juego->nivel.enemigos[juego->nivel.tope_enemigos].camino = 1;
			
			if(juego->nivel.enemigos[juego->nivel.tope_enemigos].vida <= 0 )
				juego->nivel.enemigos[juego->nivel.tope_enemigos].vida = RESISTENCIA_ORCO + rand() %(RESISTENCIA_ORCO_RAND+1);
			
			juego->nivel.tope_enemigos ++;
		}
		if( 
			juego->nivel.tope_camino_2 >= CAMINO_MIN && 
			juego->nivel.tope_enemigos < juego->nivel.max_enemigos_nivel 
		){
			juego->nivel.enemigos[juego->nivel.tope_enemigos].pos_en_camino = 0;
			juego->nivel.enemigos[juego->nivel.tope_enemigos].camino = 2;
			
			if(juego->nivel.enemigos[juego->nivel.tope_enemigos].vida <= 0 )
				juego->nivel.enemigos[juego->nivel.tope_enemigos].vida = RESISTENCIA_ORCO + rand() %(RESISTENCIA_ORCO_RAND+1);
			
			juego->nivel.tope_enemigos ++;
		}
	}

	void cargar_mapa( char mapa[MAX_FILAS][MAX_COLUMNAS], nivel_t nivel){
	
		int i, j;
		
		for (i = 0; i < MAX_FILAS; i++)
			for( j = 0; j < MAX_COLUMNAS; j++ )
				mapa[i][j] = VACIO;

		// TABLA
		for (i = 1; i < MAX_FILAS; i=i+2)
			for( j = 0; j < MAX_COLUMNAS; j++ )
				mapa[i][j] = FIL_PAR;
		for (i = 0; i < MAX_FILAS; i=i+2)
			for( j = 1; j < MAX_COLUMNAS; j=j+2 )
				mapa[i][j] = COL_PAR;	

		cargar_caminos_mapa( mapa, nivel);

		cargar_enemigos_mapa( mapa, nivel);

		cargar_defensores_mapa( mapa, nivel);

	}

	void cargar_caminos_mapa( char mapa[MAX_FILAS][MAX_COLUMNAS], nivel_t nivel){

		int k;

		// CAMINO 1
		for ( k = 0; k < nivel.tope_camino_1 ; k++)
			mapa[ nivel.camino_1[k].fil ][ nivel.camino_1[k].col ] = CAMINO_1;

		// CAMINO 2
		for ( k = 0; k < nivel.tope_camino_2 ; k++){
			if( mapa[ nivel.camino_2[k].fil ][ nivel.camino_2[k].col ] == CAMINO_1 )
				mapa[ nivel.camino_2[k].fil ][ nivel.camino_2[k].col ] = CAMINO_1Y2;
			else
				mapa[ nivel.camino_2[k].fil ][ nivel.camino_2[k].col ] = CAMINO_2;
		}

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
	}
	void cargar_enemigos_mapa( char mapa[MAX_FILAS][MAX_COLUMNAS], nivel_t nivel){

		int i, j, k;

		for ( k = 0; k < nivel.tope_enemigos ; k++){
	
			if( nivel.enemigos[k].pos_en_camino > 0 ){
				if(nivel.enemigos[k].vida > 0){
	
					char tipo_orco =
						(nivel.enemigos[k].vida > RESISTENCIA_ORCO  )? ORCO_SANO:
						(nivel.enemigos[k].vida > RESISTENCIA_ORCO/2)? ORCO_DEBIL:
						ORCO_MORIBUNDO;

					if( (nivel.enemigos[k].camino == 1) && ( nivel.tope_camino_1 > 2 ) ){
						i = nivel.camino_1[ nivel.enemigos[k].pos_en_camino ].fil;
						j = nivel.camino_1[ nivel.enemigos[k].pos_en_camino ].col;
						mapa[ i ][ j ] = tipo_orco;
					}else if( (nivel.enemigos[k].camino == 2) && ( nivel.tope_camino_2 > 2 ) ){
						i = nivel.camino_2[ nivel.enemigos[k].pos_en_camino ].fil;
						j = nivel.camino_2[ nivel.enemigos[k].pos_en_camino ].col;
						mapa[ i ][ j ] = tipo_orco;
					}
		
				}
			}
		}
	}
	void cargar_defensores_mapa( char mapa[MAX_FILAS][MAX_COLUMNAS], nivel_t nivel){

		int i, j, k;

		for( k = 0; k < nivel.tope_defensores ; k++ ){

			i = nivel.defensores[k].posicion.fil;
			j = nivel.defensores[k].posicion.col;
			mapa[i][j] = nivel.defensores[k].tipo;
		}
	}


	bool coordenada_valida( coordenada_t coordenada ){
		
		return (coordenada.fil >= 0) && (coordenada.col >= 0) && (coordenada.fil < MAX_FILAS ) &&( coordenada.col < MAX_COLUMNAS);
	}

	void mostrar_datos(juego_t juego){
		printf("%s",ESTILO_MAPA);
		printf("\n Nivel: %i ",juego.nivel_actual);
		printf("\t Torre 1: %i ",juego.torres.resistencia_torre_1);
		printf("\t Torre 2: %i ",juego.torres.resistencia_torre_2);
		int enemigos = juego.nivel.max_enemigos_nivel;
		for(int i=0; i<juego.nivel.max_enemigos_nivel; i++){
			if(juego.nivel.enemigos[i].vida < 1)
				enemigos--;
		}
		printf("\t Enemigos: %i/%i/%i",
			enemigos,juego.nivel.max_enemigos_nivel,juego.nivel.tope_enemigos);
		printf("\n");

		printf(" Crit Ena: %i \t", juego.critico_gimli );
		printf(" Crit Elf: %i \t", juego.critico_legolas );
		printf(" Fallo Ena: %i \t", juego.fallo_gimli );
		printf(" Fallo Elf: %i \t", juego.fallo_legolas );
		printf("\n");

		printf(" Enanos extra: %i \t", juego.torres.enanos_extra );
		printf(" Elfos extra: %i \t", juego.torres.elfos_extra );
		printf("\n");

		printf("%s",FIN_ESTILO);
	}
//----- MOTOR DE JUEGO ----- (¡)

//----- MOTOR GRAFICO ----- (!)

	void mostrar_mapa( char mapa[MAX_FILAS][MAX_COLUMNAS], int dimension ){
		
		mapa_sprites_t mapa_sprites;
		iniciar_sprites( &mapa_sprites );
	
		sprite_t sprite;
	
		sprite_t espacio_blanco;
		cargar_espacio_blanco( espacio_blanco );

		int i,j;
		  
		mostrar_borde_horizontal_mapa( dimension );

		for (i = 0; i < dimension; i++)
		{
			printf("\n");
			mostrar_borde_vertical_mapa( i );

			// SPRITE
			for( j = 0; j < dimension; j++ ){
				buscar_sprite( mapa_sprites ,mapa[i][j], &sprite);
				printf("%s",sprite);
				printf("%s\033[2m|%s",ESTILO_MAPA,FIN_ESTILO);
			}

			mostrar_borde_vertical_mapa( i );
		}

		mostrar_borde_horizontal_mapa( dimension );
		printf("\n\n");
	}

	void cargar_espacio_blanco( sprite_t espacio_blanco ){
		
		int i;
		for( i = 0; i < TAMANO_SPRITE-2;i++)
			espacio_blanco[i]=' ';
		espacio_blanco[i]='\0';
	}

	void mostrar_borde_horizontal_mapa( int dimension ){

		sprite_t espacio_blanco;
		cargar_espacio_blanco( espacio_blanco );

		int j;

		printf("\n%s|==||",ESTILO_MAPA);
		for(j=0 ; j < dimension; j++){
			printf("%s", espacio_blanco);
			if( j < 10 ) printf("0");
			printf("%i|", j);
		}
		printf("|==| %s",FIN_ESTILO);
	}

	void mostrar_borde_vertical_mapa( int i ){

		printf("%s|",ESTILO_MAPA);
		if( i < 10 )
			printf("0");
		printf("%i| %s",i,FIN_ESTILO);
	}
	
	void iniciar_sprites( mapa_sprites_t* mapa_sprites ){
	
		mapa_sprites->tope = 0;
	
		mapa_sprites->indice [ mapa_sprites->tope ] = VACIO;
		strcpy(mapa_sprites->sprites[ mapa_sprites->tope ], SRPITE_VACIO);
		(mapa_sprites->tope)++;
	
		iniciar_sprites_orcos( mapa_sprites );

		iniciar_sprites_defensores( mapa_sprites );

		iniciar_sprites_caminos( mapa_sprites );

		iniciar_sprites_varios( mapa_sprites );
	}
	void iniciar_sprites_orcos( mapa_sprites_t* mapa_sprites ){

		mapa_sprites->indice [ mapa_sprites->tope ] = ORCO;
		strcpy(mapa_sprites->sprites[ mapa_sprites->tope ], SRPITE_ORCO);
		(mapa_sprites->tope)++;
		mapa_sprites->indice [ mapa_sprites->tope ] = ORCO_SANO;
		strcpy(mapa_sprites->sprites[ mapa_sprites->tope ], SRPITE_ORCO_SANO);
		(mapa_sprites->tope)++;
		mapa_sprites->indice [ mapa_sprites->tope ] = ORCO_DEBIL;
		strcpy(mapa_sprites->sprites[ mapa_sprites->tope ], SRPITE_ORCO_DEBIL);
		(mapa_sprites->tope)++;
		mapa_sprites->indice [ mapa_sprites->tope ] = ORCO_MORIBUNDO;
		strcpy(mapa_sprites->sprites[ mapa_sprites->tope ], SRPITE_ORCO_MORIBUNDO);
		(mapa_sprites->tope)++;
	}
	void iniciar_sprites_defensores( mapa_sprites_t* mapa_sprites ){

		mapa_sprites->indice [ mapa_sprites->tope ] = ELFO;
		strcpy(mapa_sprites->sprites[ mapa_sprites->tope ], SRPITE_ELFO);
		(mapa_sprites->tope)++;
	
		mapa_sprites->indice [ mapa_sprites->tope ] = ENANO;
		strcpy(mapa_sprites->sprites[ mapa_sprites->tope ], SRPITE_ENANO);
		(mapa_sprites->tope)++;
	}
	void iniciar_sprites_caminos( mapa_sprites_t* mapa_sprites ){

		mapa_sprites->indice [ mapa_sprites->tope ] = TORRE;
		strcpy(mapa_sprites->sprites[ mapa_sprites->tope ], SRPITE_TORRE);
		(mapa_sprites->tope)++;
	
		mapa_sprites->indice [ mapa_sprites->tope ] = TORRE_1;
		strcpy(mapa_sprites->sprites[ mapa_sprites->tope ], SRPITE_TORRE_1);
		(mapa_sprites->tope)++;
	
		mapa_sprites->indice [ mapa_sprites->tope ] = TORRE_2;
		strcpy(mapa_sprites->sprites[ mapa_sprites->tope ], SRPITE_TORRE_2);
		(mapa_sprites->tope)++;
	
		mapa_sprites->indice [ mapa_sprites->tope ] = ENTRADA;
		strcpy(mapa_sprites->sprites[ mapa_sprites->tope ], SRPITE_ENTRADA);
		(mapa_sprites->tope)++;
	
		mapa_sprites->indice [ mapa_sprites->tope ] = CAMINO;
		strcpy(mapa_sprites->sprites[ mapa_sprites->tope ], SRPITE_CAMINO);
		(mapa_sprites->tope)++;
		mapa_sprites->indice [ mapa_sprites->tope ] = CAMINO_1;
		strcpy(mapa_sprites->sprites[ mapa_sprites->tope ], SRPITE_CAMINO_1);
		(mapa_sprites->tope)++;
		mapa_sprites->indice [ mapa_sprites->tope ] = CAMINO_2;
		strcpy(mapa_sprites->sprites[ mapa_sprites->tope ], SRPITE_CAMINO_2);
		(mapa_sprites->tope)++;
		mapa_sprites->indice [ mapa_sprites->tope ] = CAMINO_1Y2;
		strcpy(mapa_sprites->sprites[ mapa_sprites->tope ], SRPITE_CAMINO_1Y2);
		(mapa_sprites->tope)++;
	}
	void iniciar_sprites_varios( mapa_sprites_t* mapa_sprites ){

		mapa_sprites->indice [ mapa_sprites->tope ] = FIL_PAR;
		strcpy(mapa_sprites->sprites[ mapa_sprites->tope ], SRPITE_FIL_PAR);
		(mapa_sprites->tope)++;

		mapa_sprites->indice [ mapa_sprites->tope ] = COL_PAR;
		strcpy(mapa_sprites->sprites[ mapa_sprites->tope ], SRPITE_COL_PAR);
		(mapa_sprites->tope)++;
	}
	
	void buscar_sprite( mapa_sprites_t mapa_sprites, char indice , sprite_t* sprite){
	
		int i;
		for (i = 0; i < TAMANO_SPRITE; i++)
			(*sprite)[i] = VACIO;
	
		for(i = 0; i < mapa_sprites.tope; i++ )
			if( indice == mapa_sprites.indice[i] ){
				strcpy( (*sprite), mapa_sprites.sprites[i] );
				//return; // -> el primero				
			}
	}
//----- MOTOR GRAFICO ----- (¡)

//----- UTILES ----- (!)
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
//----- UTILES ----- (¡)