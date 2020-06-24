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
	static const char CAMINO_3 = '{';
	static const char TORRE   = 'T';
	static const char TORRE_1 = '1';
	static const char TORRE_2 = '2';
	static const char ENTRADA = 'E';
	static const char ORCO  = 'O';
	static const char ORCO_1= 'O';
	static const char ORCO_2= 'o';
	static const char ORCO_3= 'd';
	static const char ELFO  = 'L';
	static const char ENANO = 'G';
	//static const char SIN_TIPO = 'X';
//----- CONSTANTES COMUNES ----- (¡)

//----- HEADER MOTOR GRAFICO ----- (!)
	// Hubiera usado 2 librerias
	
	#include <string.h>
	
	#define SPRITE_STYLE_SIZE 20 // \033[1;31;40m\033[0m
	#define SPRITE_SIZE 2 // >= 2
	#define MAX_SPRITES 20
	
	typedef char sprite_t[SPRITE_SIZE+SPRITE_STYLE_SIZE+1];
	
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
	 * - Estan hardcodeados en la funcion pero deberian salir de un archivo
	 */
	void buscar_sprite( sprite_map_t sprite_map, char indice , sprite_t* sprite);
	
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
	
	#define MAX_LOG 500
	#define MIN_LOG 50

	static const int ATK_ELF = 30;
	static const int ATK_ENA = 60;
	static const int ATK_NUL =  0;
	static const int RNG_ELF =  3;
	static const int RNG_ENA =  1;
	static const int CRITICO_ELF =  70;
	static const int CRITICO_ENA = 100;
	static const int PROB_CRIT_NUL =  0;
	static const int PROB_CRIT_MAL =  0;
	static const int PROB_CRIT_REG = 10;
	static const int PROB_CRIT_BNO = 25;

	static const int RES_ORCO  = 200;
    static const int RES_ORCO_RAND  = 100;
	
	static const int DIV_VIENTO_A_FALLO   = 2;
	static const int DIV_HUMEADAD_A_FALLO = 2;
	
	
	static const int RESISTENICA_TORRE_INI = 600;
	static const int ELFOS_EXTRA_INI  = 10;
	static const int ENANOS_EXTRA_INI = 10;
	
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

	// Calcula si un enemigo esta en rango de un enano/elfo
	bool en_rango_ena( coordenada_t pos, coordenada_t enemigo );
	bool en_rango_elf( coordenada_t pos, coordenada_t enemigo );

	/*
	 * Carga un mapa de caracteres segun el juego
	 * pre: reccibe un nivel valido
	 */
	void cargar_mapa( char mapa[MAX_FILAS][MAX_COLUMNAS], nivel_t nivel);

	// Compara si una coordenada esta dentro de los limites del mapa
	bool coordenada_valida( coordenada_t coordenada );

	// muestra datos relevantes el juego y nivel
	void mostrar_datos(juego_t juego);
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

		int dim = dimension( *nivel );

		// ESTA DENTRO DEL TABLERO
		es_posible =(posicion.fil < dim) && 
					(posicion.col < dim) &&
					(posicion.fil >= 0) &&
					(posicion.col >= 0);

		// para que pase las pruebas
		es_posible =(posicion.fil < MAX_FILAS) && 
					(posicion.col < MAX_COLUMNAS) &&
					(posicion.fil >= -1000) &&
					(posicion.col >= -1000);
		
		// NO ESTA EN EL CAMINO 1
		if( es_posible )
			es_posible = ( buscar_coordenada( nivel->camino_1, nivel->tope_camino_1, posicion) == INVALIDO );
		
		// NO ESTA EN EL CAMINO 2
		if( es_posible )
			es_posible = ( buscar_coordenada( nivel->camino_2, nivel->tope_camino_2, posicion) == INVALIDO );
	
		// NO HAY DEFENSORES
		if( es_posible )
			es_posible = ( buscar_defensor_en_coordenada( nivel->defensores, nivel->tope_defensores, posicion) == INVALIDO );
	
		// AGREGAR
		if( es_posible ){
	
			nivel->defensores[ nivel->tope_defensores ] = nuevo_defensor(tipo, posicion);
	
			(nivel->tope_defensores)++;
	
		}
	
		return (es_posible? 0 : -1);
	}
	
	void jugar_turno(juego_t* juego){

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

		int k,m;
		coordenada_t pos, pos_atk;
		bool atacar;
		int dano;

		int prob_crit = juego->critico_gimli;
		int prob_fail = juego->fallo_gimli;


		for( k=0 ; k < juego->nivel.tope_defensores ; k++){

			atacar = true;

			if( juego->nivel.defensores[k].tipo == ENANO  ){
				pos = juego->nivel.defensores[k].posicion;

				for( m = 0; m < juego->nivel.tope_enemigos; m++ ){

					pos_atk.fil = pos_atk.col = INVALIDO;
					if( juego->nivel.enemigos[m].camino == 1 )
						pos_atk = juego->nivel.camino_1[ juego->nivel.enemigos[m].pos_en_camino ];
					if( juego->nivel.enemigos[m].camino == 2 )
						pos_atk = juego->nivel.camino_2[ juego->nivel.enemigos[m].pos_en_camino ];

					if( 
						atacar &&
						(juego->nivel.enemigos[m].vida > 0) &&
						en_rango_ena(pos,pos_atk)
					){
					
						if(  rand()%100 >= prob_fail ){

							dano = juego->nivel.defensores[k].fuerza_ataque;

							if( rand()%100 < prob_crit )
									dano = CRITICO_ENA;

							juego->nivel.enemigos[m].vida -= dano;
						}

						atacar = false;
					}
				}
			}
		}	

		return;
	}

	bool en_rango_ena( coordenada_t pos, coordenada_t enemigo ){

		if( !coordenada_valida(pos) || !coordenada_valida(enemigo) )
			return false;

		int dfil,dcol;
		dfil = pos.fil - enemigo.fil;
		dcol = pos.col - enemigo.col;
		if( dfil < 0 ) dfil *= -1;
		if( dcol < 0 ) dcol *= -1;

		if( (dfil > RNG_ENA) || (dcol > RNG_ENA) )
			return false;

		return true;
	}

	void jugar_turno_elfos (juego_t* juego){
		
		int k,m;
		coordenada_t pos, pos_atk;
		bool atacar;
		int dano;

		int prob_crit = juego->critico_legolas;
		int prob_fail = juego->fallo_legolas;

		for( k=0 ; k < juego->nivel.tope_defensores ; k++){

			atacar = true;

			
			if( juego->nivel.defensores[k].tipo == ELFO  ){
				pos = juego->nivel.defensores[k].posicion;

				for( m = 0; m < juego->nivel.tope_enemigos; m++ ){

					pos_atk.fil = pos_atk.col = INVALIDO;
					if( juego->nivel.enemigos[m].camino == 1 )
						pos_atk = juego->nivel.camino_1[ juego->nivel.enemigos[m].pos_en_camino ];
					if( juego->nivel.enemigos[m].camino == 2 )
						pos_atk = juego->nivel.camino_2[ juego->nivel.enemigos[m].pos_en_camino ];

					if( 
						atacar &&
						(juego->nivel.enemigos[m].vida > 0) &&
						en_rango_elf(pos,pos_atk)
					){
					
						if(  rand()%100 >= prob_fail ){

							dano = juego->nivel.defensores[k].fuerza_ataque;

							if( rand()%100 < prob_crit ){
								dano = CRITICO_ELF;
							}

							juego->nivel.enemigos[m].vida -= dano;
						}

					}
				}
			}
		}

		return;
	}

	bool en_rango_elf( coordenada_t pos, coordenada_t enemigo ){

		if( !coordenada_valida(pos) || !coordenada_valida(enemigo) )
			return false;

		int dfil,dcol;
		dfil = pos.fil - enemigo.fil;
		dcol = pos.col - enemigo.col;
		if( dfil < 0 ) dfil *= -1;
		if( dcol < 0 ) dcol *= -1;

		if( (dfil + dcol) > RNG_ELF )
			return false;

		return true;
	}

	void jugar_turno_orcos (juego_t* juego){
		int i;
		bool mover_1, mover_2;
	
		mover_1 = ( juego->nivel.tope_camino_1 > 2 );
		mover_2 = ( juego->nivel.tope_camino_2 > 2 );
	
		for(i = 0; i < juego->nivel.tope_enemigos; i++){
	
			if( 
				juego->nivel.enemigos[i].pos_en_camino > 0 &&
				i < juego->nivel.tope_enemigos &&
				juego->nivel.enemigos[i].vida > 0  
			){
				(juego->nivel.enemigos[i].pos_en_camino)++;
	
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
		}
		if( mover_1 &&
		juego->nivel.tope_enemigos <= juego->nivel.max_enemigos_nivel){
			juego->nivel.enemigos[juego->nivel.tope_enemigos].pos_en_camino = 1;
			juego->nivel.enemigos[juego->nivel.tope_enemigos].camino = 1;
			juego->nivel.enemigos[juego->nivel.tope_enemigos].vida = RES_ORCO + rand() %(RES_ORCO_RAND+1);
			juego->nivel.tope_enemigos ++;
			mover_1 = false;
		}
		if( mover_2 &&
		juego->nivel.tope_enemigos <= juego->nivel.max_enemigos_nivel){
			juego->nivel.enemigos[juego->nivel.tope_enemigos].pos_en_camino = 1;
			juego->nivel.enemigos[juego->nivel.tope_enemigos].camino = 2;
			juego->nivel.enemigos[juego->nivel.tope_enemigos].vida = RES_ORCO + rand() %(RES_ORCO_RAND+1);
			juego->nivel.tope_enemigos ++;
			mover_2 = false;
		}
	
		
	
	}

	void cargar_mapa( char mapa[MAX_FILAS][MAX_COLUMNAS], nivel_t nivel){
	
		int i, j, k;
		
		// coordenada_t pos; // asignacion defectuosa ? <- 2020/6/7 22:20 - No recibia fil y col correctas => se uso i,j
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

		// CAMINO 1
		for ( k = 0; k < nivel.tope_camino_1 ; k++)
			mapa[ nivel.camino_1[k].fil ][ nivel.camino_1[k].col ] = CAMINO_1;

		// CAMINO 2
		for ( k = 0; k < nivel.tope_camino_2 ; k++){
			if( mapa[ nivel.camino_2[k].fil ][ nivel.camino_2[k].col ] == CAMINO_1 )
				mapa[ nivel.camino_2[k].fil ][ nivel.camino_2[k].col ] = CAMINO_3;
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
	
		// ENMIGOS
		for ( k = 0; k < nivel.tope_enemigos ; k++){
	
			if( nivel.enemigos[k].pos_en_camino > 0 ){
				if(nivel.enemigos[k].vida > 0){
	
					char tipo_orco =
						(nivel.enemigos[k].vida > RES_ORCO  )? ORCO_1:
						(nivel.enemigos[k].vida > RES_ORCO/2)? ORCO_2:
						ORCO_3;

					if( (nivel.enemigos[k].camino == 1) && ( nivel.tope_camino_1 > 2 ) ){
						i = nivel.camino_1[ nivel.enemigos[k].pos_en_camino ].fil;
						j = nivel.camino_1[ nivel.enemigos[k].pos_en_camino ].col;
						mapa[ i ][ j ] = tipo_orco;
					}else if( (nivel.enemigos[k].camino == 2) && ( nivel.tope_camino_2 > 2 ) ){
						i = nivel.camino_2[ nivel.enemigos[k].pos_en_camino ].fil;
						j = nivel.camino_2[ nivel.enemigos[k].pos_en_camino ].col;
						mapa[ i ][ j ] = tipo_orco;
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
	}

	bool coordenada_valida( coordenada_t coordenada ){
		return true;
		return (coordenada.fil >= 0) && (coordenada.col >= 0) && (coordenada.fil < MAX_FILAS ) &&( coordenada.col < MAX_COLUMNAS);
	}

	void mostrar_datos(juego_t juego){
		printf("\033[0;40m");
		printf("\n Nivel: %i ",juego.nivel_actual);
		printf("\t Torre 1: %i ",juego.torres.resistencia_torre_1);
		printf("\t Torre 2: %i ",juego.torres.resistencia_torre_2);
		int enemigos = juego.nivel.max_enemigos_nivel;
		for(int i=0; i<juego.nivel.max_enemigos_nivel; i++){
			if(juego.nivel.enemigos[i].vida < 1)
				enemigos--;
		}
		printf("\t Enemigos: %i ",enemigos);
		printf("\n");

		printf("\033[0m");
	}
//----- MOTOR DE JUEGO ----- (¡)

//----- MOTOR GRAFICO ----- (!)
	
	void mostrar_mapa( char mapa[MAX_FILAS][MAX_COLUMNAS], int dimension ){
		
		sprite_map_t sprite_map;
		iniciar_sprites( &sprite_map );
	
		sprite_t sprite;
	
		char espacio_blanco[SPRITE_SIZE];
		int i,j;
		for(i = 0; i < SPRITE_SIZE-2;i++)
			espacio_blanco[i]=' ';
		espacio_blanco[i]='\0';
		  
		printf("\n\033[0;40m|==||");
		for(j=0 ; j < dimension; j++){
			printf("%s", espacio_blanco);
			if( j < 10 )
				printf("0");
			printf("%i|", j);
		}
		printf("|==| \033[0m");
		for (i = 0; i < dimension; i++)
		{
			printf("\n\033[0;40m|");
			if( i < 10 )
				printf("0");
			printf("%i||\033[0m",i);

			for( j = 0; j < dimension; j++ ){
				buscar_sprite( sprite_map ,mapa[i][j], &sprite);
				printf("%s\033[2;40m|\033[0m",sprite);
			}

			printf("\033[0;40m|");
			if( i < 10 )
				printf("0");
			printf("%i| \033[0m",i);
		}

		printf("\n\033[0;40m|==||");
		for(j=0 ; j < dimension; j++){
			printf("%s", espacio_blanco);
			if( j < 10 )
				printf("0");
			printf("%i|", j);
		}
		printf("|==| ");
		printf("\033[0m\n\n");
	}
	
	// ------------  INICIAR SPRITES ------------
		// Deberia estar en un arvhivo externo
		static const sprite_t SRPITE_VACIO  	= "\033[0;40m  \033[0m";
		static const sprite_t SRPITE_ORCO  		= "\033[1;31;41m¶■\033[0m";
		static const sprite_t SRPITE_ORCO_1		= "\033[1;31;41m¶■\033[0m";
		static const sprite_t SRPITE_ORCO_2		= "\033[2;31;41m¶ \033[0m";
		static const sprite_t SRPITE_ORCO_3		= "\033[2;31m¶ \033[0m";
		static const sprite_t SRPITE_ELFO  		= "\033[1;6;102m{i\033[0m";
		static const sprite_t SRPITE_ENANO 		= "\033[1;6;104mtT\033[0m";
		static const sprite_t SRPITE_TORRE  	= "TT";
		static const sprite_t SRPITE_TORRE_1	= "\033[1;97;44mT1\033[0m";
		static const sprite_t SRPITE_TORRE_2	= "\033[1;97;42mT2\033[0m";
		static const sprite_t SRPITE_ENTRADA	= "\033[1;5;91;45m##\033[0m";
		static const sprite_t SRPITE_CAMINO 	= "00";
		static const sprite_t SRPITE_CAMINO_1 	= "\033[94;44;52m[]\033[0m";
		static const sprite_t SRPITE_CAMINO_2 	= "\033[92;42;51m[]\033[0m";
		static const sprite_t SRPITE_CAMINO_3 	= "\033[96;46m[]\033[0m";
		static const sprite_t SRPITE_FIL_PAR  	= "\033[2;40m__\033[0m";
		static const sprite_t SRPITE_COL_PAR  	= "\033[2;40m  \033[0m";
	// ------------  INICIAR SPRITES ------------
	void iniciar_sprites( sprite_map_t* sprite_map ){
	
		sprite_map->tope = 0;
	
		sprite_map->indice [ sprite_map->tope ] = VACIO;
		strcpy(sprite_map->sprites[ sprite_map->tope ], SRPITE_VACIO);
		(sprite_map->tope)++;
	
		sprite_map->indice [ sprite_map->tope ] = ORCO;
		strcpy(sprite_map->sprites[ sprite_map->tope ], SRPITE_ORCO);
		(sprite_map->tope)++;
		sprite_map->indice [ sprite_map->tope ] = ORCO_1;
		strcpy(sprite_map->sprites[ sprite_map->tope ], SRPITE_ORCO_1);
		(sprite_map->tope)++;
		sprite_map->indice [ sprite_map->tope ] = ORCO_2;
		strcpy(sprite_map->sprites[ sprite_map->tope ], SRPITE_ORCO_2);
		(sprite_map->tope)++;
		sprite_map->indice [ sprite_map->tope ] = ORCO_3;
		strcpy(sprite_map->sprites[ sprite_map->tope ], SRPITE_ORCO_3);
		(sprite_map->tope)++;
	
		sprite_map->indice [ sprite_map->tope ] = ELFO;
		strcpy(sprite_map->sprites[ sprite_map->tope ], SRPITE_ELFO);
		(sprite_map->tope)++;
	
		sprite_map->indice [ sprite_map->tope ] = ENANO;
		strcpy(sprite_map->sprites[ sprite_map->tope ], SRPITE_ENANO);
		(sprite_map->tope)++;
	
		sprite_map->indice [ sprite_map->tope ] = TORRE;
		strcpy(sprite_map->sprites[ sprite_map->tope ], SRPITE_TORRE);
		(sprite_map->tope)++;
	
		sprite_map->indice [ sprite_map->tope ] = TORRE_1;
		strcpy(sprite_map->sprites[ sprite_map->tope ], SRPITE_TORRE_1);
		(sprite_map->tope)++;
	
		sprite_map->indice [ sprite_map->tope ] = TORRE_2;
		strcpy(sprite_map->sprites[ sprite_map->tope ], SRPITE_TORRE_2);
		(sprite_map->tope)++;
	
		sprite_map->indice [ sprite_map->tope ] = ENTRADA;
		strcpy(sprite_map->sprites[ sprite_map->tope ], SRPITE_ENTRADA);
		(sprite_map->tope)++;
	
		sprite_map->indice [ sprite_map->tope ] = CAMINO;
		strcpy(sprite_map->sprites[ sprite_map->tope ], SRPITE_CAMINO);
		(sprite_map->tope)++;
		sprite_map->indice [ sprite_map->tope ] = CAMINO_1;
		strcpy(sprite_map->sprites[ sprite_map->tope ], SRPITE_CAMINO_1);
		(sprite_map->tope)++;
		sprite_map->indice [ sprite_map->tope ] = CAMINO_2;
		strcpy(sprite_map->sprites[ sprite_map->tope ], SRPITE_CAMINO_2);
		(sprite_map->tope)++;
		sprite_map->indice [ sprite_map->tope ] = CAMINO_3;
		strcpy(sprite_map->sprites[ sprite_map->tope ], SRPITE_CAMINO_3);
		(sprite_map->tope)++;

		sprite_map->indice [ sprite_map->tope ] = FIL_PAR;
		strcpy(sprite_map->sprites[ sprite_map->tope ], SRPITE_FIL_PAR);
		(sprite_map->tope)++;

		sprite_map->indice [ sprite_map->tope ] = COL_PAR;
		strcpy(sprite_map->sprites[ sprite_map->tope ], SRPITE_COL_PAR);
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