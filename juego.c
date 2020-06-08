#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "animos.h"
#include "defendiendo_torres.h"
#include "utiles.h"

static const float INTERVALO = 0.4f;

static const int ESTADO_JUGANDO = 0;
static const int ESTADO_GANADO  = 1;
//static const int ESTADO_PERDIDO =-1;


#define OPCION_NUEVO_JUEGO          1
#define OPCION_INICIALIZAR_ANIMOS   2
#define OPCION_MOSTRAR_OPCIONES     3
#define OPCION_SALIR                4

static const int CANTIDAD_NIVELES = 4;

static const int RES_ORCO  = 200;
static const int RES_ORCO_RAND  = 100;

/*
 * Muestra un menu y actualiza la opcion elegida :
 * 1 > Comenzar un nuevo juego
 * 2 > Inicializar los animos / humedad / viento
 * 3 > Opciones ( DLC )
 * 4 > Salir
 */
void menu( int* opcion );

/* 
 * Comienza un nuevo juego
 * pre: recibe un juego inicializado
 */
void nuevo_juego( juego_t* juego );

// muestra opciones/configuracion
void mostrar_opciones( juego_t* juego );

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


int main(){


    //srand(time(NULL)); 
    srand(5); 

    int viento = 0;
    int humedad = 0;
    char animo_legolas = 'B';
    char animo_gimli = 'B';

    juego_t juego;
    inicializar_juego(&juego, viento, humedad, animo_legolas, animo_gimli);

    int opcion = OPCION_INICIALIZAR_ANIMOS;

	while( opcion != OPCION_SALIR ){
        menu( &opcion );

        switch( opcion ){

            case OPCION_NUEVO_JUEGO:
                inicializar_juego(&juego, viento, humedad, animo_legolas, animo_gimli);
                nuevo_juego( &juego );
            break;

            case OPCION_INICIALIZAR_ANIMOS:
                animos(&viento , &humedad , &animo_legolas , &animo_gimli);
            break;

            case OPCION_MOSTRAR_OPCIONES:
                mostrar_opciones( &juego );
            break;

        }

    }

    

    

	return 0;

}

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

void nuevo_juego( juego_t* juego ){

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
            
            detener_el_tiempo( INTERVALO );

        }

    }

    // GANO O PERDIO 

}

void mostrar_opciones( juego_t* juego ){

    int opcion = 0;

    system("clear");
    printf("\n");
    printf("----------------------------------------------------\n");
    printf("-------------         OPCIONES         -------------\n");
    printf("----------------------------------------------------\n");
    printf("\n");
    printf("1: Subir volumen \n");
    printf("2: Bajar volumen \n");
    printf("3: Revivir a Sauron \n");
    printf("4: Salir \n");

    char input[20];
    scanf("%s",input);
    opcion = input[0]-48;

    if( opcion == 0)
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
