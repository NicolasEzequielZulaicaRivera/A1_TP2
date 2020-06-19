#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include "animos.h"
#include "defendiendo_torres.h"
#include "utiles.h"

//  CONSTANTES DE JUEGO (!)
    static const char ENANO = 'G';
    static const char ELFO = 'L';

    static const int ESTADO_JUGANDO = 0;
    static const int ESTADO_GANADO  = 1;
    //static const int ESTADO_PERDIDO =-1;
    static const int INVALIDO =-1;

    static const char CONFIRMAR ='S';
    static const char CANCELAR ='X';

    #define CANTIDAD_NIVELES  4

    static const int RES_ORCO  = 200;
    static const int RES_ORCO_RAND  = 100;

    static const int COSTO_ENA_EXT = 50;
    static const int COSTO_ELF_EXT = 50;

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

    const config_nivel_t NIVEL_1 ={ 
        .num = 1,
        .dimension = 15,
        .entrada = ENTRADA_ESTE,
        .torre_1 = true, .torre_2 = false,
        .enanos = 5, .elfos = 0, 
        .orcos = 100,
        .turnos_bonus = 25
    };

    const config_nivel_t NIVEL_2 ={ 
        .num = 2,
        .dimension = 15,
        .entrada = ENTRADA_OESTE,
        .torre_1 = false, .torre_2 = true,
        .enanos = 0, .elfos = 5, 
        .orcos = 200,
        .turnos_bonus = 50
    };

    const config_nivel_t NIVEL_3 ={ 
        .num = 3,
        .dimension = 20,
        .entrada = ENTRADA_NORTE,
        .torre_1 = true, .torre_2 = true,
        .enanos = 3, .elfos = 3, 
        .orcos = 300,
        .turnos_bonus = 50/2
    };

    const config_nivel_t NIVEL_4 ={ 
        .num = 4,
        .dimension = 20,
        .entrada = ENTRADA_SUR,
        .torre_1 = true, .torre_2 = true,
        .enanos = 4, .elfos = 4, 
        .orcos = 250,
        .turnos_bonus = 50/2
    };

    const int MAX_INTENTOS = 20;
    //const int COMPLEJIDAD_NIVELES = 2;
    //const int RAREZA_CRUZADO = 2;
//  CONSTANTES DE JUEGO (¡)

// HEADER DE PEDIR DATOS (!)
    // Hay templates en C ? NO. ( vendria bien un C++ ) 
    #define MAX_MSG 100
    #define MAX_OPT 10

    // Funciones para pedir datos de distintos tipos
    void pedir_int( int* dato, int min, int max, char msg[MAX_MSG] );
    void pedir_float( float* dato, float min, float max, char msg[MAX_MSG] );
    void pedir_char( char* dato, char opciones [MAX_OPT], char nombre_opciones [MAX_OPT][MAX_MSG], int tope, char msg[MAX_MSG] );
    bool pedir_bool( char msg[MAX_MSG] );
    
    void tocar_para_continuar();
// HEADER DE PEDIR DATOS (¡)

// HEADER DE MENU Y CONFIG (!)
    
  // MENU

    #define OPCION_NUEVO_JUEGO          1
    #define OPCION_INICIALIZAR_ANIMOS   2
    #define OPCION_MOSTRAR_OPCIONES     3
    #define OPCION_SALIR                4
    #define OPCION_TESTEO               9

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
        bool saltear_niveles, godmode;
        bool auto_defensores;
        int complejidad, rareza_cruzado;
    } config_t;

    const config_t CONFIG_STD ={
        .velocidad = 0.4f,
        .bonus_resistencia = 0,
        .saltear_niveles = false, .godmode = false,
        .auto_defensores = false,
        .complejidad = 2, .rareza_cruzado = 2
    };

    const config_t CONFIG_DEBUG ={
        .velocidad = 0.1f,
        .bonus_resistencia = 3000,
        .saltear_niveles = true, .godmode = true,
        .auto_defensores = true,
        .complejidad = 1, .rareza_cruzado = 1
    };

    // muestra opciones/configuracion
    void mostrar_opciones( juego_t* juego, config_t* config );

    // Inicializa la config son los valores std
    void iniciar_config( config_t* config );   
// HEADER DE MENU Y CONFIG (!)

// HEADER JUEGO (!)
    /* 
     * Comienza un nuevo juego
     * pre: recibe un juego inicializado
     */
    void nuevo_juego( juego_t* juego , config_t config );  

    /*
     * Devuelve un nivel_t segun el nivel(numero)
     *  0 > Entrada Este
     *  1 > Entrada Oeste
     *  2 > Entrada Norte
     *  3 > Entrada Sur
     *  Default > Nivel vacio
     */
    nivel_t nuevo_nivel( int nivel, config_t config );

    // Muestra un mensaje al pasar de nivel
    void mensaje_nuevo_nivel( int nivel );

    // Muestra un mensaje al finalizar un juego (ganar o perder)
    void mensaje_terminar_juego( int estado );

    // Pide al usuario que coloque defensores segun las especificaciones del nivel
    void agregar_defensores( juego_t* juego, config_nivel_t config_nivel  );

    // Coloqua defensores segun las especificaciones del nivel
    void auto_agregar_defensores( juego_t* juego, config_nivel_t config_nivel  );

    // Pide al usuario que coloque un defensor extra segun las especificaciones del nivel
    void agregar_defensores_bonus( juego_t* juego, config_nivel_t config_nivel  );
    
    // Coloca automaticamente un defensor extra segun las especificaciones del nivel
    void auto_agregar_defensores_bonus( juego_t* juego, config_nivel_t config_nivel  );

    // Devuelve las especificaciones del nivel pedido
    config_nivel_t buscar_config_nivel( int nivel );
// HEADER JUEGO (¡)


int main(){
    srand( (unsigned int) time(NULL));  // ERROR: unsigned -> integer
    //srand(105774); 

    config_t config;
    iniciar_config( &config );

    int viento = 50;
    int humedad = 50;
    char animo_legolas = 'B';
    char animo_gimli = 'B';

    juego_t juego;

    int opcion = OPCION_INICIALIZAR_ANIMOS;

	while( opcion != OPCION_SALIR ){
        menu( &opcion );

        switch( opcion ){

            case OPCION_TESTEO:
                opcion = OPCION_NUEVO_JUEGO;
                config = CONFIG_DEBUG;

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

// PEDIR DATOS (!)
    
    void pedir_int( int* dato, int min, int max, char msg[MAX_MSG] ){

        printf("%s\n",msg );
        printf("[%i - %i] > ",min,max );

        do{
            scanf(" %i",dato);
        } while( (*dato < min) || (*dato > max) );

        return;    
    }

    void pedir_float( float* dato, float min, float max, char msg[MAX_MSG] ){
        
        printf("%s\n",msg );
        printf("[%f - %f] > ",min,max );

        do{
            scanf(" %f",dato);
        } while( (*dato < min) || (*dato > max) );

        return;
    }

    void pedir_char( char* dato, char opciones [MAX_OPT], char nombre_opciones [MAX_OPT][MAX_MSG], int tope, char msg[MAX_MSG] ){

        int i;
        printf("%s\n",msg );
        for( i = 0 ; i < tope ; i++ )
            printf(" [%c : %s] ",opciones[i],nombre_opciones[i] );

        bool dato_valido = false;

        do{
            scanf(" %c",dato);
            *dato = (char) toupper((*dato));

            for( i = 0 ; i < tope ; i++ )
                if( *dato == toupper(opciones[i])  )
                    dato_valido = true;

        } while( !dato_valido );

        return;
    }

    bool pedir_bool( char msg[MAX_MSG] ){

        char opciones [MAX_OPT]; 
        char nombre_opciones [MAX_OPT][MAX_MSG];
        int tope = 2;
        opciones[0]= CONFIRMAR;
        opciones[1]= CANCELAR;
        strcpy(nombre_opciones[0],"SI");
        strcpy(nombre_opciones[1],"NO");

        char dato;
        pedir_char(&dato,opciones,nombre_opciones,tope,msg);

        if( dato == CONFIRMAR )
            return true;

        return false;
    }

    void tocar_para_continuar(){

        printf("\n\t presione para continuar \n" );
        char c;
        fflush(stdout);
        scanf("%c%c",&c,&c);
    }
// PEDIR DATOS (¡)

// MENU Y CONFIG (!)

    void menu( int* opcion ){

        system("clear");
        printf("\n");
        printf("----------------------------------------------------\n");
        printf("-------------  DEFENDIENDO LAS TORRES  -------------\n");
        printf("----------------------------------------------------\n");
        printf("\n");
        printf("%i: Nuevo Juego \n", OPCION_NUEVO_JUEGO);
        printf("%i: Iniciar Animos \n", OPCION_INICIALIZAR_ANIMOS);
        printf("%i: Opciones \n", OPCION_MOSTRAR_OPCIONES);
        printf("%i: Salir \n", OPCION_SALIR);

        char input[20];
        scanf("%s",input);
        *opcion = input[0]-48;
    }

    void mostrar_opciones( juego_t* juego , config_t* config ){

        int opcion = 0;

        system("clear");
        printf("\n");
        printf("----------------------------------------------------\n");
        printf("-------------         OPCIONES         -------------\n");
        printf("----------------------------------------------------\n");
        printf("\n");
        printf("1: Tiempo entre turnos - [ESPERA : %f] \n",config->velocidad);
        printf("2: Regeneracion por nivel - [BONUS : %i]\n",config->bonus_resistencia);
        printf("3: Complejidad de niveles - [COMPLEJIDAD : %i]\n",config->complejidad);
        printf("4: Rareza de niveles cruzados - [RAREZA : %i]\n",config->rareza_cruzado);
        printf("5: Auto posicionar defensores - [ %c ] \n", 
                ( (config->auto_defensores)?CONFIRMAR:CANCELAR ) );
        printf("6: Invencibilidad - [ %c ] \n", 
                ( (config->godmode)?CONFIRMAR:CANCELAR ) );
        printf("7: Saltear niveles - [ %c ] \n", 
                ( (config->saltear_niveles)?CONFIRMAR:CANCELAR ) );
        printf("8: Volver \n");

        char input[20];
        scanf("%s",input);
        opcion = input[0]-48;

        char msg[MAX_MSG];

        switch( opcion ){
            case 1:
                sprintf(msg," Ingrese la FRECUENCIA");
                pedir_float( &(config->velocidad), 0.1f, 1.0f, msg );
            break;

            case 2:
                sprintf(msg," Ingrese el BONUS");
                pedir_int( &(config->bonus_resistencia), 0, 9000, msg );
            break;

            case 3:
                sprintf(msg,"Ingrese la COMPLEJIDAD");
                pedir_int( &(config->complejidad), 0, 5, msg );
            break;

            case 4:
                sprintf(msg,"Ingrese la RAREZA");
                pedir_int( &(config->rareza_cruzado), 1, 50, msg );
            break;

            case 5:
                sprintf(msg,"Auto posicionar defensores");
                config->auto_defensores = pedir_bool(msg);
            break;

            case 6:
                sprintf(msg,"Ser invencible");
                config->godmode = pedir_bool(msg);
            break;

            case 7:
                sprintf(msg,"Saltear niveles");
                config->saltear_niveles = pedir_bool(msg);
            break;
        }
        return;
    }

    void iniciar_config( config_t* config ){
        *config = CONFIG_STD;
    }
// MENU Y CONFIG (!)

// JUEGO (!)

    // Aplica acciones extra al comenzar un nivel segun la configuracion
    void bonus_nuevo_nivel( juego_t* juego , config_t config );

    void nuevo_juego( juego_t* juego , config_t config ){

        int turno = 0;
        config_nivel_t config_nivel;
        juego->nivel_actual = 0;
        juego->nivel.tope_enemigos = 0;

        while( estado_juego( *juego) == ESTADO_JUGANDO ){

            if( estado_nivel( juego->nivel ) == ESTADO_GANADO  ){
                
                juego->nivel_actual ++;
                config_nivel = buscar_config_nivel( juego->nivel_actual );
                turno = 1;
                juego->nivel = nuevo_nivel( juego->nivel_actual, config );
                mensaje_nuevo_nivel( juego->nivel_actual );

                bonus_nuevo_nivel( juego , config );

                if( (juego->nivel_actual <= CANTIDAD_NIVELES) && (juego->nivel.tope_enemigos > 0) ){

                    if( config.auto_defensores )
                        auto_agregar_defensores( juego, config_nivel );
                    else
                        agregar_defensores( juego, config_nivel );
                }

            }else{

                if( (turno % config_nivel.turnos_bonus == 0) && !(config.auto_defensores) )
                    agregar_defensores_bonus( juego, config_nivel );
                else if( (turno % config_nivel.turnos_bonus == 0) && (config.auto_defensores) )
                    auto_agregar_defensores_bonus( juego, config_nivel );

                jugar_turno( juego );
                turno ++;

                detener_el_tiempo( config.velocidad );

                mostrar_juego( *juego );

            }

            if(config.godmode){
                juego->torres.resistencia_torre_1 += 1;
                juego->torres.resistencia_torre_2 += 1;
            }
        }

        // GANO O PERDIO
        mensaje_terminar_juego( estado_juego( *juego) );
    }

    void bonus_nuevo_nivel( juego_t* juego , config_t config ){

        juego->torres.resistencia_torre_1 += config.bonus_resistencia;
        juego->torres.resistencia_torre_2 += config.bonus_resistencia;

        if( config.saltear_niveles ){
            char rta;
            printf("\n SALTEAR NIVEL [%c]\n", CONFIRMAR );
            scanf("%c",&rta);

            if( toupper(rta) == toupper(CONFIRMAR) )
                juego->nivel.tope_enemigos = 0;
         
        }
    }

    void mensaje_nuevo_nivel( int nivel ){

        system("clear");

        if( nivel > 0)
            printf("\n\t HAS COMPLETADO EL NIVEL %i \n", nivel-1 );

        if( nivel <= CANTIDAD_NIVELES )
            printf("\n\t COMIENZA EL NIVEL %i \n", nivel );

        tocar_para_continuar();
    }

    void mensaje_terminar_juego( int estado ){

        system("clear");
        if( estado == ESTADO_GANADO){

            printf("\n\n");
            printf("\t =================================== \n");
            printf("\t ||                               ||\n");
            printf("\t ||  ¡¡¡ HAS GANADO EL JUEGO !!!  ||\n");
            printf("\t ||                               ||\n");
            printf("\t =================================== \n");
        
        }else{

            printf("\n\n");
            printf("\t ==================================== \n");
            printf("\t ||                                ||\n");
            printf("\t ||      HAS PERDIDO EL JUEGO      ||\n");
            printf("\t ||                                ||\n");
            printf("\t ==================================== \n");

        }

        tocar_para_continuar();
    } 

    nivel_t nuevo_nivel( int nivel , config_t datos){

        int dimension; // mapa de dim x dim
        nivel_t nuevo_nivel;

        nuevo_nivel.tope_camino_1 = 0;
        nuevo_nivel.tope_camino_2 = 0;
        nuevo_nivel.tope_defensores = 0;
        nuevo_nivel.tope_enemigos = 0;

        config_nivel_t config;
        config.num = 0;

        config = buscar_config_nivel( nivel );

        
        if( nivel <= CANTIDAD_NIVELES ){

            int intentos;

            bool cruzado = ( rand()%datos.rareza_cruzado == 0 );
            cruzado = true;

            dimension = config.dimension ;

            nuevo_nivel.tope_enemigos = config.orcos;
            nuevo_nivel.max_enemigos_nivel = config.orcos;
            nuevo_nivel.tope_defensores = 0;

            coordenada_t entrada,torre;

            //CAMINO 1
            if( config.torre_1 ){

                switch( config.entrada ){

                    case ENTRADA_ESTE:
                        entrada.fil = rand()%dimension;
                        entrada.col = dimension-1;
                        torre.fil = rand()%dimension;
                        torre.col = 0;
                    break;

                    case ENTRADA_NORTE:
                        entrada.fil = 0;
                        entrada.col = rand()%(dimension/2)+(dimension/2)*(cruzado);
                        torre.fil = dimension-1;
                        torre.col = rand()%(dimension/2);
                    break;

                    case ENTRADA_SUR:
                        entrada.fil = dimension-1;
                        entrada.col = rand()%(dimension/2)+(dimension/2)*(cruzado);
                        torre.fil = 0;
                        torre.col = rand()%(dimension/2);
                    break;

                }

                intentos = 0;
                nuevo_nivel.tope_camino_1 = 0;

                while( (nuevo_nivel.tope_camino_1 < datos.complejidad *dimension) 
                    && (intentos < MAX_INTENTOS) ){

                    obtener_camino(
                        nuevo_nivel.camino_1,
                        &nuevo_nivel.tope_camino_1, 
                        entrada, torre
                    );
                }

            }else
                nuevo_nivel.tope_camino_1 = 0;

            //CAMINO 2
            if( config.torre_2 ){

                switch( config.entrada ){

                    case ENTRADA_OESTE:
                        entrada.fil = rand()%dimension;
                        entrada.col = 0;
                        torre.fil = rand()%dimension;
                        torre.col = dimension-1;
                    break;

                    case ENTRADA_NORTE:
                        entrada.fil = 0;
                        entrada.col = rand()%(dimension/2)+(dimension/2)*(!cruzado);
                        torre.fil = dimension-1;
                        torre.col = rand()%(dimension/2)+(dimension/2);

                    break;

                    case ENTRADA_SUR:
                        entrada.fil = dimension-1;
                        entrada.col = rand()%(dimension/2)+(dimension/2)*(!cruzado);
                        torre.fil = 0;
                        torre.col = rand()%(dimension/2)+(dimension/2);
                    break;

                }

                intentos = 0;
                nuevo_nivel.tope_camino_2 = 0;

                while( (nuevo_nivel.tope_camino_2 < datos.complejidad*dimension) 
                    && (intentos < MAX_INTENTOS) ){

                    obtener_camino(
                        nuevo_nivel.camino_2,
                        &nuevo_nivel.tope_camino_2, 
                        entrada, torre
                    );
                }

            }else
                nuevo_nivel.tope_camino_2 = 0;      
        }


        for(int i = 0; i<nuevo_nivel.tope_enemigos; i++){
            nuevo_nivel.enemigos[i].vida= RES_ORCO + rand() %(RES_ORCO_RAND+1) ;
            nuevo_nivel.enemigos[i].camino = INVALIDO;
            nuevo_nivel.enemigos[i].pos_en_camino = INVALIDO;
        }

        return nuevo_nivel;
    }

    void agregar_defensores( juego_t* juego, config_nivel_t config_nivel  ){

        int col, fil, i;
        char msg [MAX_MSG];
        coordenada_t posicion;
        char estado;
        
        // ENANOS
        i=0;
        while( i<config_nivel.enanos ){

            do{
                mostrar_juego(*juego);
                printf("\n COLOCAR DEFENSORES \n");
                printf("\n Enanos: %i \t Elfos: %i \n",config_nivel.enanos,config_nivel.elfos);

                sprintf(msg, "Ingrese la fila del enano %i",i+1);
                pedir_int( &fil, 0, config_nivel.dimension-1,msg);
                sprintf(msg, "Ingrese la columna del enano %i",i+1);
                pedir_int( &col, 0, config_nivel.dimension-1,msg);

                posicion.fil = fil;
                posicion.col = col;

            } while ( agregar_defensor( &(juego->nivel), posicion, ENANO) == INVALIDO );
            i++;
            mostrar_juego(*juego);
        
            printf(" \n CANCELAR > [%c] ",CANCELAR);
            scanf("%c%c",&estado,&estado);
            if(toupper(estado) == toupper(CANCELAR)){
                i--;
                juego->nivel.tope_defensores --;
            }
            
        }

        // ELFOS
        i=0;
        while( i<config_nivel.elfos){
            do{
                mostrar_juego(*juego);
                printf("\n COLOCAR DEFENSORES \n");
                printf("\n Enanos: %i \t Elfos: %i \n",config_nivel.enanos,config_nivel.elfos);

                sprintf(msg, "Ingrese la fila del elfo %i",i+1);
                pedir_int( &fil, 0, config_nivel.dimension-1,msg);
                sprintf(msg, "Ingrese la columna del elfo %i",i+1);
                pedir_int( &col, 0, config_nivel.dimension-1,msg);

                posicion.fil = fil;
                posicion.col = col;

            } while ( agregar_defensor( &(juego->nivel), posicion, ELFO) == INVALIDO );
            i++;
            mostrar_juego(*juego);

            printf(" \n CANCELAR > [%c] ",CANCELAR);
            scanf("%c%c",&estado,&estado);
            if(toupper(estado) == toupper(CANCELAR)){
                i--;
                juego->nivel.tope_defensores --;
            }
        }    
    }

    void auto_agregar_defensores( juego_t* juego, config_nivel_t config_nivel  ){

        int i;
        coordenada_t posicion, aux;
        
        // ENANOS
        i=0;
        while( i<config_nivel.enanos ){

            do{
                aux = juego->nivel.camino_1[ rand()%juego->nivel.tope_camino_1 ];
                posicion.fil = aux.fil-1+rand()%3;
                posicion.col = aux.col-1+rand()%3;
            } while ( agregar_defensor( &(juego->nivel), posicion, ENANO) == INVALIDO );
            i++;
        }

        // ELFOS
        i=0;
        while( i<config_nivel.elfos){
            do{
                aux = juego->nivel.camino_2[ rand()%juego->nivel.tope_camino_2 ];
                posicion.fil = aux.fil-1+rand()%3;
                posicion.col = aux.col-1+rand()%3;
            } while ( agregar_defensor( &(juego->nivel), posicion, ELFO) == INVALIDO );
            i++;
            mostrar_juego(*juego);
        }    
    }

    void agregar_defensores_bonus( juego_t* juego, config_nivel_t config_nivel  ){

        int col, fil;
        char msg [MAX_MSG];
        coordenada_t posicion;
        char tipo, estado;
        bool enanos,elfos;

        char opciones [MAX_OPT];
        char nombre_opciones [MAX_OPT][MAX_MSG];
        int tope;

        enanos = ( config_nivel.torre_1 && 
            juego->torres.enanos_extra > 0);

        elfos = ( config_nivel.torre_2 && 
            juego->torres.elfos_extra > 0);

        if( !enanos && !elfos )
            return;

        mostrar_juego(*juego);
        printf("\n PUEDE COLOCAR UN DEFENSOR EXTRA \n");
        printf(" COSTO: ");
        if(enanos)
            printf(" Enanos: %i Hp de la torre 1 .",COSTO_ENA_EXT);
        if(elfos)
            printf(" Elfo: %i Hp de la torre 2 .",COSTO_ELF_EXT);
        printf("\n\n");

        opciones[0] = CANCELAR;
        strcpy(nombre_opciones[0], "CANCELAR");
        if( enanos && elfos ){
            opciones[1] = ENANO;
            strcpy(nombre_opciones[1],"ENANO");
            opciones[2] = ELFO;
            strcpy(nombre_opciones[2], "ELFO");
            tope = 3;
        }else if( enanos ){
            opciones[1] = ENANO;
            strcpy(nombre_opciones[1], "ENANO");
            tope = 2;
        }else if( elfos ){
            opciones[1] = ELFO;
            strcpy(nombre_opciones[1], "ELFO");
            tope = 2;
        }else
            return;

        sprintf(msg," Que defensor desea colocar ?");
        pedir_char( &tipo, opciones, nombre_opciones, tope, msg );      
        mostrar_juego(*juego);

        if( toupper(tipo) == toupper(ENANO) ){
            tipo = ENANO;
            juego->torres.resistencia_torre_1 -= COSTO_ENA_EXT;
            juego->torres.enanos_extra --;
        }
        else if( toupper(tipo) == toupper(ELFO) ){
            tipo = ELFO;
            juego->torres.resistencia_torre_2 -= COSTO_ELF_EXT;
            juego->torres.elfos_extra --;
        }
        else
            return;

        do{
            do{
                mostrar_juego(*juego);
                printf("\n COLOCAR DEFENSOR \n");
                
                sprintf(msg, "Ingrese la fila del defensor ");
                pedir_int( &fil, 0, config_nivel.dimension-1,msg);
                sprintf(msg, "Ingrese la columna del defensor");
                pedir_int( &col, 0, config_nivel.dimension-1,msg);

                posicion.fil = fil;
                posicion.col = col;

            } while ( agregar_defensor( &(juego->nivel), posicion, tipo) == INVALIDO );
            mostrar_juego(*juego);
        
            printf(" \n CANCELAR > [%c] ",CANCELAR);
            scanf("%c%c",&estado,&estado);
            if(toupper(estado) == toupper(CANCELAR)){
                juego->nivel.tope_defensores --;
            }
        }while(toupper(estado) == toupper(CANCELAR));
    }

    void auto_agregar_defensores_bonus( juego_t* juego, config_nivel_t config_nivel  ){

        coordenada_t posicion,aux;
        char tipo;
        bool enanos,elfos;

        enanos = ( config_nivel.torre_1 && 
            juego->torres.enanos_extra > 0);

        elfos = ( config_nivel.torre_2 && 
            juego->torres.elfos_extra > 0);


        if( enanos && elfos ){
            if( rand()%2 ){
                tipo = ENANO;
                juego->torres.enanos_extra --;
            }else{
                tipo = ELFO;
                juego->torres.elfos_extra --;
            }
        }
        else if( enanos ){
            tipo = ENANO;
            juego->torres.enanos_extra --;

        }
        else if( elfos ){
            tipo = ELFO;
            juego->torres.elfos_extra --;
        }
        else
            return;

        if( tipo == ENANO ){
            do{
                aux = juego->nivel.camino_1[ rand()%juego->nivel.tope_camino_1 ];
                posicion.fil = aux.fil-1+rand()%3;
                posicion.col = aux.col-1+rand()%3;

            } while ( agregar_defensor( &(juego->nivel), posicion, tipo) == INVALIDO );
        }

        if( tipo == ELFO ){
            do{
                aux = juego->nivel.camino_2[ rand()%juego->nivel.tope_camino_2 ];
                posicion.fil = aux.fil-1+rand()%3;
                posicion.col = aux.col-1+rand()%3;

            } while ( agregar_defensor( &(juego->nivel), posicion, tipo) == INVALIDO );
        }
    }

    config_nivel_t buscar_config_nivel( int nivel ){

        config_nivel_t config_nivel;
        config_nivel.num = 0;
        config_nivel.orcos = 0;
        config_nivel.dimension = 0;
        config_nivel.torre_1 = config_nivel.torre_2 = false;

        config_nivel_t niveles[CANTIDAD_NIVELES] = { NIVEL_1, NIVEL_2, NIVEL_3, NIVEL_4 };

        for( int i = 0; i < CANTIDAD_NIVELES; i++ )
            if( niveles[i].num == nivel )
                config_nivel = niveles[i];

        return config_nivel;
    }
// JUEGO (¡)

