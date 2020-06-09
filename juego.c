#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include "animos.h"
#include "defendiendo_torres.h"
#include "utiles.h"

//  CONSTANTES DE JUEGO (!)
    static const int ESTADO_JUGANDO = 0;
    static const int ESTADO_GANADO  = 1;
    //static const int ESTADO_PERDIDO =-1;
    static const int CANTIDAD_NIVELES = 4;

    static const int RES_ORCO  = 200;
    static const int RES_ORCO_RAND  = 100; 

    typedef struct config_nivel{
        int num;
        int dimension;
        char entrada;
        bool torre_1, torre_2;
        int enanos, elfos, orcos;
        int turnos_bonus;

    } config_nivel_t;

    #define ENTRADA_ESTE  'E'
    #define ENTRADA_OESTE 'O'
    #define ENTRADA_NORTE 'N'
    #define ENTRADA_SUR   'S'

    const config_nivel_t nivel_1 ={ 
        .num = 1,
        .dimension = 15,
        .entrada = ENTRADA_ESTE,
        .torre_1 = true, .torre_2 = false,
        .enanos = 5, .elfos = 0, 
        .orcos = 100,
        .turnos_bonus = 25
    };

    const config_nivel_t nivel_2 ={ 
        .num = 2,
        .dimension = 15,
        .entrada = ENTRADA_OESTE,
        .torre_1 = false, .torre_2 = true,
        .enanos = 0, .elfos = 5, 
        .orcos = 200,
        .turnos_bonus = 50
    };

    const config_nivel_t nivel_3 ={ 
        .num = 3,
        .dimension = 20,
        .entrada = ENTRADA_NORTE,
        .torre_1 = true, .torre_2 = true,
        .enanos = 3, .elfos = 3, 
        .orcos = 300,
        .turnos_bonus = 50
    };

    const config_nivel_t nivel_4 ={ 
        .num = 4,
        .dimension = 20,
        .entrada = ENTRADA_SUR,
        .torre_1 = true, .torre_2 = false,
        .enanos = 5, .elfos = 0, 
        .orcos = 100,
        .turnos_bonus = 25
    };

//  CONSTANTES DE JUEGO (¡)

// HEADER DE MENU Y CONFIG (!)
    
  // MENU

    #define OPCION_NUEVO_JUEGO          1
    #define OPCION_INICIALIZAR_ANIMOS   2
    #define OPCION_MOSTRAR_OPCIONES     3
    #define OPCION_SALIR                4

    /*
     * Muestra un menu y actualiza la opcion elegida :
     * 1 > Comenzar un nuevo juego
     * 2 > Inicializar los animos / humedad / viento
     * 3 > Opciones ( DLC )
     * 4 > Salir
     */
    void menu( int* opcion );

  // CONFIG

    typedef struct config {
        float velocidad;
        int bonus_resistencia;
    } config_t;
    static const float VELOCIDAD_STD = 0.4f;
    static const int BONUS_RES_STD = 0;

    // Inicializa la config son los valores std
    void iniciar_config( config_t* config );   
// HEADER DE MENU Y CONFIG (!)


// HEADER JUEGO (!)
    /* 
     * Comienza un nuevo juego
     * pre: recibe un juego inicializado
     */
    void nuevo_juego( juego_t* juego , config_t config );

    // muestra opciones/configuracion
    void mostrar_opciones( juego_t* juego, config_t* config );

    /*
     * Devuelve un nivel_t segun el nivel(numero)
     *  0 > Entrada Este
     *  1 > Entrada Oeste
     *  2 > Entrada Norte
     *  3 > Entrada Sur
     *  Default > Nivel vacio
     */
    nivel_t nuevo_nivel( int nivel );

    // Muestra un mensaje al pasar de nivel
    void mensaje_nuevo_nivel( int nivel );
// HEADER JUEGO (¡)


int main(){
    srand( (unsigned int) time(NULL));  // ERROR: unsigned -> integer
    //srand(105774); 

    config_t config;
    iniciar_config( &config );

    int viento = 0;
    int humedad = 0;
    char animo_legolas = 'B';
    char animo_gimli = 'B';

    juego_t juego;

    int opcion = OPCION_INICIALIZAR_ANIMOS;

	while( opcion != OPCION_SALIR ){
        menu( &opcion );

        switch( opcion ){

            case OPCION_NUEVO_JUEGO:
                inicializar_juego(&juego, viento, humedad, animo_legolas, animo_gimli);
                nuevo_juego( &juego , config );
            break;

            case OPCION_INICIALIZAR_ANIMOS:
                animos(&viento , &humedad , &animo_legolas , &animo_gimli);
            break;

            case OPCION_MOSTRAR_OPCIONES:
                mostrar_opciones( &juego, &config );
            break;

        }

    }

	return 0;
}

// MENU Y CONFIG (!)

    void menu( int* opcion ){

        system("clear");
        printf("\n");
        printf("----------------------------------------------------\n");
        printf("-------------  DEFENDIENDO LAS TORRES  -------------\n");
        printf("----------------------------------------------------\n");
        printf("\n");
        printf("%i: Nuevo Juego \n", OPCION_NUEVO_JUEGO);
        printf("%i: Animos \n", OPCION_INICIALIZAR_ANIMOS);
        printf("%i: Opciones \n", OPCION_MOSTRAR_OPCIONES);
        printf("%i: Salir \n", OPCION_SALIR);

        char input[20];
        scanf("%s",input);
        *opcion = input[0]-48;
    }

    void iniciar_config( config_t* config ){
        config->velocidad = VELOCIDAD_STD;
        config->bonus_resistencia = BONUS_RES_STD;
    }
// MENU Y CONFIG (!)

// JUEGO (!)
    void nuevo_juego( juego_t* juego , config_t config ){

        juego->nivel_actual = 0;
        juego->nivel.tope_enemigos = 0;

        while( estado_juego( *juego) == ESTADO_JUGANDO ){

            if( estado_nivel( juego->nivel ) == ESTADO_GANADO  ){
                
                juego->nivel_actual ++;
                juego->nivel = nuevo_nivel( juego->nivel_actual );
                mensaje_nuevo_nivel( juego->nivel_actual );

            }else{

                jugar_turno( juego );

                mostrar_juego( *juego );
                
                detener_el_tiempo( config.velocidad );

            }

        }

        // GANO O PERDIO 
    }

    void mostrar_opciones( juego_t* juego , config_t* config ){

        int opcion = 0;

        system("clear");
        printf("\n");
        printf("----------------------------------------------------\n");
        printf("-------------         OPCIONES         -------------\n");
        printf("----------------------------------------------------\n");
        printf("\n");
        printf("1: Velocidad - [FRECUENCIA : %f] \n",config->velocidad);
        printf("2: Regeneracion por nivel - [BONUS : %i]\n",config->bonus_resistencia);
        printf("3: Revivir a Sauron \n");
        printf("4: Salir \n");

        char input[20];
        scanf("%s",input);
        opcion = input[0]-48;

        switch( opcion ){
            case 1:
                printf("Ingrese la FRECUENCIA [ 0.1 - 1.0 ]  > ");
                scanf("%f", &(config->velocidad) );
            break;

            case 2:
                printf("Ingrese el BONUS [ 0 - 1000 ]  > ");
                scanf("%i", &(config->bonus_resistencia) );
            break;
        }
        return;
    }

    void mensaje_nuevo_nivel( int nivel ){

        system("tput clear");

        if( nivel > 0)
            printf("\n\t HAS COMPLETADO EL NIVEL %i \n", nivel-1 );

        if( nivel <= CANTIDAD_NIVELES )
            printf("\n\t COMIENZA EL NIVEL %i \n", nivel );

        printf("\n\t presione para continuar \n" );

        char c;
        scanf("%c",&c);
    }

    nivel_t nuevo_nivel( int nivel ){

        int dimension; // mapa de dim x dim
        nivel_t nuevo_nivel;

        nuevo_nivel.tope_camino_1 = 0;
        nuevo_nivel.tope_camino_2 = 0;
        nuevo_nivel.tope_defensores = 0;
        nuevo_nivel.tope_enemigos = 0;

        if( nivel <= CANTIDAD_NIVELES )
            nuevo_nivel.tope_enemigos = 10;

        if( nivel == 1 ){

            dimension = 15 ;

            nuevo_nivel.tope_enemigos = 100;
            nuevo_nivel.tope_camino_2 = 0;

            coordenada_t entrada,torre;

            entrada.fil = rand()%dimension;
            entrada.col = dimension-1;

            torre.fil = rand()%dimension;
            torre.col = 0;

            obtener_camino(
                nuevo_nivel.camino_1,
                &nuevo_nivel.tope_camino_1, 
                entrada, torre
            );

        }

        if( nivel == 2 ){

            dimension = 15 ;

            nuevo_nivel.tope_enemigos = 200;
            nuevo_nivel.tope_camino_1 = 0;

            coordenada_t entrada,torre;

            entrada.fil = rand()%dimension;
            entrada.col = 0;

            torre.fil = rand()%dimension;
            torre.col = dimension-1;

            obtener_camino(
                nuevo_nivel.camino_2,
                &nuevo_nivel.tope_camino_2, 
                entrada, torre
            );

        }

        if( nivel == 3 ){

            dimension = 20;
            nuevo_nivel.tope_enemigos = 300;

            coordenada_t entrada,torre;

            // TORRE 1
            entrada.fil = 0;
            entrada.col = rand()%(dimension/2);

            torre.fil = dimension-1;
            torre.col = rand()%(dimension/2);

            obtener_camino(
                nuevo_nivel.camino_1,
                &nuevo_nivel.tope_camino_1, 
                entrada, torre
            );

            // TORRE 2
            entrada.fil = 0;
            entrada.col = rand()%(dimension/2)+(dimension/2);

            torre.fil = dimension-1;
            torre.col = rand()%(dimension/2)+(dimension/2);

            obtener_camino(
                nuevo_nivel.camino_2,
                &nuevo_nivel.tope_camino_2, 
                entrada, torre
            );

        }

        if( nivel == 4 ){

            dimension = 20;
            nuevo_nivel.tope_enemigos = 500;

            coordenada_t entrada,torre;

            // TORRE 1
            entrada.fil = dimension-1;
            entrada.col = rand()%(dimension/2);

            torre.fil = 0;
            torre.col = rand()%(dimension/2);

            obtener_camino(
                nuevo_nivel.camino_1,
                &nuevo_nivel.tope_camino_1, 
                entrada, torre
            );

            // TORRE 2
            entrada.fil = dimension-1;
            entrada.col = rand()%(dimension/2)+(dimension/2);

            torre.fil = 0;
            torre.col = rand()%(dimension/2)+(dimension/2);

            obtener_camino(
                nuevo_nivel.camino_2,
                &nuevo_nivel.tope_camino_2, 
                entrada, torre
            );

        }



        for(int i = 0; i<nuevo_nivel.tope_enemigos; i++){
            nuevo_nivel.enemigos[i].vida= RES_ORCO + rand() %(RES_ORCO_RAND+1) ;
            nuevo_nivel.enemigos[i].camino = 0;
            nuevo_nivel.enemigos[i].pos_en_camino = -1;
        }

        return nuevo_nivel;
    }
// JUEGO (¡)

