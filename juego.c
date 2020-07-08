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
    static const char NADIE = 'N';

    static const int ESTADO_JUGANDO = 0;
    static const int ESTADO_GANADO  = 1;
    //static const int ESTADO_PERDIDO =-1;
    static const int INVALIDO =-1;

    static const char CONFIRMAR ='S';
    static const char CANCELAR ='X';

    #define CANTIDAD_NIVELES  4

    static const int RESISTENCIA_ORCO  = 200;
    static const int RESISTENCIA_ORCO_RAND  = 100;

    static const int RESISTENICA_TORRE_INICIAL = 600;

    static const int COSTO_ENANO_EXTRA = 50;
    static const int COSTO_ELFO_EXTRA = 50;

    static const float FACTOR_ESPERA_NUEVO_NIVEL = 3;

    static const int VIENTO_INICIAL = 50;
    static const int HUMEDAD_INICIAL= 50;
    static const char ANIMO_INICIAL = 'B';

    /*
     * Tipo de dato que almacena las caracteristicas que se utilizan 
     *  generar un nivel
     */
    typedef struct caracteristicas_nivel{
        
        // numero de nivel
        int num;

        // dimension del tablero (cuadrado) del nivel
        int dimension;

        // direccion cardinal (Norte,Sur,Este,Oeste) de donde provienen los enemigos
        char entrada;

        // Indica que torres/caminos se hallaran en el nivel
        bool torre_1, torre_2;

        // Cantidades de enanos y elfos iniciales en el nivel
        // orcos a entrar en el nivel
        int enanos, elfos, orcos;

        // Turnos cada los cuales se podra agregar un defensor extra
        int turnos_bonus;
    } caracteristicas_nivel_t;

    #define ENTRADA_ESTE  'E'
    #define ENTRADA_OESTE 'O'
    #define ENTRADA_NORTE 'N'
    #define ENTRADA_SUR   'S'

    const caracteristicas_nivel_t PRUEBA ={ 
        .num = 1,
        .dimension = 15,
        .entrada = ENTRADA_ESTE,
        .torre_1 = true, .torre_2 = false,
        .enanos = 1, .elfos = 0, 
        .orcos = 1,
        .turnos_bonus = 25
    };

    const caracteristicas_nivel_t NIVEL_1 ={ 
        .num = 1,
        .dimension = 15,
        .entrada = ENTRADA_ESTE,
        .torre_1 = true, .torre_2 = false,
        .enanos = 5, .elfos = 0, 
        .orcos = 100,
        .turnos_bonus = 25
    };

    const caracteristicas_nivel_t NIVEL_2 ={ 
        .num = 2,
        .dimension = 15,
        .entrada = ENTRADA_OESTE,
        .torre_1 = false, .torre_2 = true,
        .enanos = 0, .elfos = 5, 
        .orcos = 200,
        .turnos_bonus = 50
    };

    const caracteristicas_nivel_t NIVEL_3 ={ 
        .num = 3,
        .dimension = 20,
        .entrada = ENTRADA_NORTE,
        .torre_1 = true, .torre_2 = true,
        .enanos = 3, .elfos = 3, 
        .orcos = 300,
        .turnos_bonus = 50/2
    };

    const caracteristicas_nivel_t NIVEL_4 ={ 
        .num = 4,
        .dimension = 20,
        .entrada = ENTRADA_SUR,
        .torre_1 = true, .torre_2 = true,
        .enanos = 4, .elfos = 4, 
        .orcos = 500,
        .turnos_bonus = 50/2
    };

    const int MAX_INTENTOS = 20;
//  CONSTANTES DE JUEGO (¡)

// HEADER DE PEDIR DATOS (!)
    #define MAX_MENSAJE 100
    #define MAX_OPCIONES 10

    static const char OPCION_SI[MAX_MENSAJE]="SI";
    static const char OPCION_NO[MAX_MENSAJE]="NO";

    // Funciones para pedir datos de distintos tipos
    void pedir_int( int* dato, int min, int max, char mensaje[MAX_MENSAJE] );
    void pedir_float( float* dato, float min, float max, char mensaje[MAX_MENSAJE] );
    void pedir_char( char* dato, char opciones [MAX_OPCIONES], char nombre_opciones [MAX_OPCIONES][MAX_MENSAJE], int tope, char mensaje[MAX_MENSAJE] );
    bool pedir_bool( char mensaje[MAX_MENSAJE] );
    
    void tocar_para_continuar();
// HEADER DE PEDIR DATOS (¡)

// HEADER DE MENU Y CONFIGURACION (!)
    
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
     * 3 > Opciones
     * 4 > Salir
     */
    void menu( int* opcion );

  // CONFIGURACION

    /*
     * Tipo de dato que almacena parametros de configuracion
     *  de un juego
     */
    typedef struct configuracion {

        // tiempo entre turnos 
        float velocidad;

        // regeneracion de vida por nivel
        int bonus_resistencia;

        // utilidades 
        bool saltear_niveles, invencible;

        // colocar defensores automaticamente
        bool auto_defensores; 

        // parametros para los caminos
        int complejidad, rareza_cruzado; 
    } configuracion_t;

    const configuracion_t CONFIGURACION_STANDAR ={
        .velocidad = 0.4f,
        .bonus_resistencia = 0,
        .saltear_niveles = false, .invencible = false,
        .auto_defensores = false,
        .complejidad = 2, .rareza_cruzado = 2
    };

    const configuracion_t CONFIGURACION_DEBUG ={
        .velocidad = 0.03f,
        .bonus_resistencia = 3000,
        .saltear_niveles = true, .invencible = true,
        .auto_defensores = true,
        .complejidad = 1, .rareza_cruzado = 1
    };

    const configuracion_t CONFIGURACION_MIN ={
        .velocidad = 0.01f,
        .bonus_resistencia = 0,
        .saltear_niveles = false, .invencible = false,
        .auto_defensores = false,
        .complejidad = 1, .rareza_cruzado = 1
    };

    const configuracion_t CONFIGURACION_MAX ={
        .velocidad = 1.0f,
        .bonus_resistencia = 15000,
        .saltear_niveles = true, .invencible = true,
        .auto_defensores = true,
        .complejidad = 5, .rareza_cruzado = 50
    };

    // muestra opciones/configuracion
    void mostrar_opciones( juego_t* juego, configuracion_t* configuracion );

    /*
     * pre: es llamada solo dentro de mostrar_opciones
     * post: muestra una interfaz para modificar la opcion seleccionada  
     */
    void modificar_opcion_seleccionada( configuracion_t* configuracion, int opcion );

    // Inicializa la configuracion son los valores std
    void iniciar_configuracion( configuracion_t* configuracion );   
// HEADER DE MENU Y CONFIGURACION (!)

// HEADER JUEGO (!)
    /* 
     * Comienza un nuevo juego
     * pre: recibe un juego inicializado
     */
    void nuevo_juego( juego_t* juego , configuracion_t configuracion );  

    /*
     * Devuelve un nivel_t segun el nivel(numero)
     *  0 > Entrada Este
     *  1 > Entrada Oeste
     *  2 > Entrada Norte
     *  3 > Entrada Sur
     *  Default > Nivel vacio
     */
    nivel_t nuevo_nivel( int nivel, configuracion_t configuracion );

    // Generan los caminos del nivel segun sus caracteristicas y la coniguracion
    void generar_camino_1( nivel_t* nivel, caracteristicas_nivel_t caracteristicas_nivel , configuracion_t configuracion, bool cruzado );
    void generar_extremos_camino_1( coordenada_t* entrada, coordenada_t* torre,
        caracteristicas_nivel_t caracteristicas_nivel, bool cruzado );
    void generar_camino_2( nivel_t* nivel, caracteristicas_nivel_t caracteristicas_nivel , configuracion_t configuracion, bool cruzado );
    void generar_extremos_camino_2( coordenada_t* entrada, coordenada_t* torre,
        caracteristicas_nivel_t caracteristicas_nivel, bool cruzado );
    
    // Genera un camino segun los parametros
    void generar_camino(coordenada_t camino[MAX_LONGITUD_CAMINO], 
        int* tope_camino, coordenada_t entrada, coordenada_t torre, 
        int dimension, configuracion_t configuracion );

    // Muestra un mensaje al pasar de nivel
    void mensaje_nuevo_nivel( int nivel );

    // Muestra un mensaje al finalizar un juego (ganar o perder)
    void mensaje_terminar_juego( int estado );

    // Pide al usuario que coloque defensores segun las especificaciones del nivel
    void agregar_defensores( juego_t* juego, caracteristicas_nivel_t caracteristicas_nivel  );

    // Coloqua defensores segun las especificaciones del nivel
    void auto_agregar_defensores( juego_t* juego, caracteristicas_nivel_t caracteristicas_nivel  );

    // Pide al usuario que coloque un defensor segun las especificaciones del nivel
    void pedir_un_defensor( juego_t* juego, 
        caracteristicas_nivel_t caracteristicas_nivel, char tipo, int* k );
    
    // Pide al usuario que coloque un defensor extra segun las especificaciones del nivel
    void agregar_defensores_bonus( juego_t* juego, caracteristicas_nivel_t caracteristicas_nivel  );

    // Subrutinas de agregar_defensores_bonus()
    void obtener_tipo_defensor_bonus( juego_t juego, char* tipo,
        caracteristicas_nivel_t caracteristicas_nivel );
    void colocar_defensores_bonus( juego_t* juego, char tipo,
        caracteristicas_nivel_t caracteristicas_nivel );
    
    // Coloca automaticamente un defensor extra segun las especificaciones del nivel
    void auto_agregar_defensores_bonus( juego_t* juego, caracteristicas_nivel_t caracteristicas_nivel  );

    // Subrutinas de auto_agregar_defensores_bonus()
    void auto_obtener_tipo_defensor_bonus( juego_t* juego, char* tipo,
        caracteristicas_nivel_t caracteristicas_nivel );
    void auto_colocar_defensores_bonus( juego_t* juego, char tipo,
        caracteristicas_nivel_t caracteristicas_nivel );

    // Devuelve las especificaciones del nivel pedido
    caracteristicas_nivel_t buscar_caracteristicas_nivel( int nivel );

    // Muestra las variables que dependen de los animos y pregunta si se quieren iniciar
    void iniciar_animos(int* viento , int* humedad , char* animo_legolas , char* animo_gimli);
 
    // Aplica acciones extra al comenzar un nivel segun la configuracion
    void bonus_nuevo_nivel( juego_t* juego , configuracion_t configuracion );

    // pasa el juego de nivel, 
    // aplicando los bonus correspoindientes a la configuracion
    void pasar_de_nivel( juego_t* juego , configuracion_t configuracion, 
        caracteristicas_nivel_t* caracteristicas_nivel  );

    // pasa el nivel de turno (se juega un turno), 
    // aplicando los bonus correspoindientes a la configuracion
    void pasar_turno( juego_t* juego , configuracion_t configuracion, 
        caracteristicas_nivel_t caracteristicas_nivel, int turno  );

    // aplica al juego los bonus que corresponden previo a jugar un turno
    void bonus_pre_turno( juego_t* juego , configuracion_t configuracion, 
        caracteristicas_nivel_t caracteristicas_nivel, int turno  );

    // aplica al juego los bonus que corresponden posterior a jugar un turno
    void bonus_post_turno( juego_t* juego , configuracion_t configuracion, 
        caracteristicas_nivel_t caracteristicas_nivel, int turno  );
// HEADER JUEGO (¡)


int main(){
    srand( (unsigned int) time(NULL));
    //srand(105774); 

    configuracion_t configuracion;
    iniciar_configuracion( &configuracion );

    int viento = VIENTO_INICIAL;
    int humedad = HUMEDAD_INICIAL;
    char animo_legolas = ANIMO_INICIAL;
    char animo_gimli = ANIMO_INICIAL;

    juego_t juego;

    int opcion = OPCION_INICIALIZAR_ANIMOS;

	while( opcion != OPCION_SALIR ){
        menu( &opcion );

        switch( opcion ){

            case OPCION_TESTEO:
                opcion = OPCION_NUEVO_JUEGO;
                configuracion = CONFIGURACION_DEBUG;

            case OPCION_NUEVO_JUEGO:
                inicializar_juego(&juego, viento, humedad, animo_legolas, animo_gimli);
                nuevo_juego( &juego , configuracion );
            break;

            case OPCION_INICIALIZAR_ANIMOS:
                iniciar_animos(&viento , &humedad , &animo_legolas , &animo_gimli);
            break;

            case OPCION_MOSTRAR_OPCIONES:
                mostrar_opciones( &juego, &configuracion );
            break;

        }
    }

	return 0;
}

// PEDIR DATOS (!)
    
    void pedir_int( int* dato, int min, int max, char mensaje[MAX_MENSAJE] ){

        printf("%s\n",mensaje );
        printf("[%i - %i] > ",min,max );

        do{
            scanf(" %i",dato);
        } while( (*dato < min) || (*dato > max) );

        return;    
    }

    void pedir_float( float* dato, float min, float max, char mensaje[MAX_MENSAJE] ){
        
        printf("%s\n",mensaje );
        printf("[%f - %f] > ",min,max );

        do{
            scanf(" %f",dato);
        } while( (*dato < min) || (*dato > max) );

        return;
    }

    void pedir_char( char* dato, char opciones [MAX_OPCIONES], char nombre_opciones [MAX_OPCIONES][MAX_MENSAJE], int tope, char mensaje[MAX_MENSAJE] ){

        int i;
        printf("%s\n",mensaje );
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

    bool pedir_bool( char mensaje[MAX_MENSAJE] ){

        char opciones [MAX_OPCIONES]; 
        char nombre_opciones [MAX_OPCIONES][MAX_MENSAJE];
        int tope = 2;
        opciones[0]= CONFIRMAR;
        opciones[1]= CANCELAR;
        strcpy(nombre_opciones[0],OPCION_SI);
        strcpy(nombre_opciones[1],OPCION_NO);

        char respuesta;
        pedir_char(&respuesta,opciones,nombre_opciones,tope,mensaje);

        if( respuesta == CONFIRMAR )
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

// MENU Y CONFIGURACION (!)

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

        char respuesta[20];
        scanf("%s",respuesta);
        *opcion = respuesta[0]-48;
    }

    void mostrar_opciones( juego_t* juego , configuracion_t* configuracion ){

        int opcion = 0;
        while( opcion != 8 ){
            system("clear");
            printf("\n");
            printf("----------------------------------------------------\n");
            printf("-------------         OPCIONES         -------------\n");
            printf("----------------------------------------------------\n");
            printf("\n");
            printf("1: Tiempo entre turnos - [ESPERA : %f] \n",configuracion->velocidad);
            printf("2: Regeneracion por nivel - [BONUS : %i]\n",configuracion->bonus_resistencia);
            printf("3: Complejidad de niveles - [COMPLEJIDAD : %i]\n",configuracion->complejidad);
            printf("4: Rareza de niveles cruzados - [RAREZA : %i]\n",configuracion->rareza_cruzado);
            printf("5: Auto posicionar defensores - [ %c ] \n", 
                    ( (configuracion->auto_defensores)?CONFIRMAR:CANCELAR ) );
            printf("6: Invencibilidad - [ %c ] \n", 
                    ( (configuracion->invencible)?CONFIRMAR:CANCELAR ) );
            printf("7: Saltear niveles - [ %c ] \n", 
                    ( (configuracion->saltear_niveles)?CONFIRMAR:CANCELAR ) );
            printf("8: Volver \n");

            char respuesta[MAX_MENSAJE];
            scanf("%s",respuesta);
            opcion = respuesta[0]-48;

            modificar_opcion_seleccionada( configuracion, opcion );
        }
        return;
    }

    void modificar_opcion_seleccionada( configuracion_t* configuracion, int opcion ){
        
        char mensaje[MAX_MENSAJE];

        switch( opcion ){
            case 1:
                sprintf(mensaje," Ingrese la FRECUENCIA");
                pedir_float( &(configuracion->velocidad), 
                    CONFIGURACION_MIN.velocidad, 
                    CONFIGURACION_MAX.velocidad, 
                    mensaje );
            break;

            case 2:
                sprintf(mensaje," Ingrese el BONUS");
                pedir_int( &(configuracion->bonus_resistencia), 
                    CONFIGURACION_MIN.bonus_resistencia, 
                    CONFIGURACION_MAX.bonus_resistencia, 
                    mensaje );
            break;

            case 3:
                sprintf(mensaje,"Ingrese la COMPLEJIDAD");
                pedir_int( &(configuracion->complejidad), 
                    CONFIGURACION_MIN.complejidad, 
                    CONFIGURACION_MAX.complejidad, 
                    mensaje );
            break;

            case 4:
                sprintf(mensaje,"Ingrese la RAREZA");
                pedir_int( &(configuracion->rareza_cruzado), 
                    CONFIGURACION_MIN.rareza_cruzado, 
                    CONFIGURACION_MAX.rareza_cruzado, 
                    mensaje );
            break;

            case 5:
                sprintf(mensaje,"Auto posicionar defensores");
                configuracion->auto_defensores = pedir_bool(mensaje);
            break;

            case 6:
                sprintf(mensaje,"Ser invencible");
                configuracion->invencible = pedir_bool(mensaje);
            break;

            case 7:
                sprintf(mensaje,"Saltear niveles");
                configuracion->saltear_niveles = pedir_bool(mensaje);
            break;
        }   
    }

    void iniciar_configuracion( configuracion_t* configuracion ){
        
        *configuracion = CONFIGURACION_STANDAR;
    }
// MENU Y CONFIGURACION (!)

// JUEGO (!)

    void nuevo_juego( juego_t* juego , configuracion_t configuracion ){

        int turno = 0;
        caracteristicas_nivel_t caracteristicas_nivel;
        juego->nivel_actual = 0;
        juego->nivel.tope_enemigos = 0;
        juego->nivel = nuevo_nivel( juego->nivel_actual, configuracion );

        while( estado_juego( *juego) == ESTADO_JUGANDO ){

            if( estado_nivel( juego->nivel ) == ESTADO_GANADO  ){
                
                turno = 0;
                pasar_de_nivel( juego , configuracion, &caracteristicas_nivel );
                
            }else{
                
                pasar_turno( juego , configuracion, 
                    caracteristicas_nivel, turno  );
                
                turno ++;

                mostrar_juego( *juego );
                detener_el_tiempo( configuracion.velocidad );
            }
        }

        mensaje_terminar_juego( estado_juego( *juego) );
    }

    void pasar_de_nivel( juego_t* juego , configuracion_t configuracion, 
        caracteristicas_nivel_t* caracteristicas_nivel  ){

        juego->nivel_actual ++;
        *caracteristicas_nivel = buscar_caracteristicas_nivel( juego->nivel_actual );
        juego->nivel = nuevo_nivel( juego->nivel_actual, configuracion );
        mensaje_nuevo_nivel( juego->nivel_actual );

        bonus_nuevo_nivel( juego , configuracion );

        if( (juego->nivel_actual <= CANTIDAD_NIVELES) && (juego->nivel.max_enemigos_nivel > 0) ){

                if( configuracion.auto_defensores )
                auto_agregar_defensores( juego, *caracteristicas_nivel );
            else
                agregar_defensores( juego, *caracteristicas_nivel );
        }

        mostrar_juego( *juego );
        detener_el_tiempo( configuracion.velocidad * FACTOR_ESPERA_NUEVO_NIVEL );
    }

    void pasar_turno( juego_t* juego , configuracion_t configuracion, 
        caracteristicas_nivel_t caracteristicas_nivel, int turno  ){

        bonus_pre_turno( juego , configuracion, 
            caracteristicas_nivel, turno  );

        jugar_turno( juego );

        bonus_post_turno( juego , configuracion, 
            caracteristicas_nivel, turno  );
    }

    void bonus_pre_turno( juego_t* juego , configuracion_t configuracion, 
        caracteristicas_nivel_t caracteristicas_nivel, int turno  ){

        if( (turno > 0) && 
            (turno <= (caracteristicas_nivel.orcos / 
                (caracteristicas_nivel.torre_1*1+caracteristicas_nivel.torre_2*1) )) &&
            (turno % caracteristicas_nivel.turnos_bonus == 0) 
        ){

            if( (configuracion.auto_defensores) )
                auto_agregar_defensores_bonus( juego, caracteristicas_nivel );
            else
                agregar_defensores_bonus( juego, caracteristicas_nivel );
        }
    }

    void bonus_post_turno( juego_t* juego , configuracion_t configuracion, 
        caracteristicas_nivel_t caracteristicas_nivel, int turno  ){

        if(configuracion.invencible){
            juego->torres.resistencia_torre_1 = 
            juego->torres.resistencia_torre_2 = 
            RESISTENICA_TORRE_INICIAL;
        }
    }

    void bonus_nuevo_nivel( juego_t* juego , configuracion_t configuracion ){

        juego->torres.resistencia_torre_1 += configuracion.bonus_resistencia;
        juego->torres.resistencia_torre_2 += configuracion.bonus_resistencia;

        if( configuracion.saltear_niveles ){
            char respuesta;
            printf("\n SALTEAR NIVEL [%c]\n", CONFIRMAR );
            scanf("%c",&respuesta);

            if( toupper(respuesta) == toupper(CONFIRMAR) )
                juego->nivel.max_enemigos_nivel = 0;
         
        }
    }

    void mensaje_nuevo_nivel( int nivel ){

        system("clear");

        if( nivel-1 > 0)
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

    nivel_t nuevo_nivel( int nivel , configuracion_t configuracion ){

        nivel_t nuevo_nivel;

        nuevo_nivel.tope_camino_1 = 0;
        nuevo_nivel.tope_camino_2 = 0;
        nuevo_nivel.tope_defensores = 0;
        nuevo_nivel.tope_enemigos = 0;
        nuevo_nivel.max_enemigos_nivel = 0;

        caracteristicas_nivel_t caracteristicas_nivel;
        caracteristicas_nivel.num = 0;

        caracteristicas_nivel = buscar_caracteristicas_nivel( nivel );

        
        if( nivel <= CANTIDAD_NIVELES ){


            bool cruzado = ( rand()%configuracion.rareza_cruzado == 0 );

            nuevo_nivel.max_enemigos_nivel = caracteristicas_nivel.orcos;

            //CAMINO 1
            generar_camino_1( &nuevo_nivel, caracteristicas_nivel, configuracion, cruzado);
            

            //CAMINO 2
            generar_camino_2( &nuevo_nivel, caracteristicas_nivel, configuracion, cruzado);     
        }


        for(int i = 0; i<nuevo_nivel.max_enemigos_nivel; i++){
            nuevo_nivel.enemigos[i].vida= RESISTENCIA_ORCO + rand() %(RESISTENCIA_ORCO_RAND+1) ;
            nuevo_nivel.enemigos[i].camino = INVALIDO;
            nuevo_nivel.enemigos[i].pos_en_camino = INVALIDO;
        }

        return nuevo_nivel;
    }

    void generar_camino(coordenada_t camino[MAX_LONGITUD_CAMINO], 
        int* tope_camino, coordenada_t entrada, coordenada_t torre, 
        int dimension, configuracion_t configuracion ){

        int intentos = 0;
        *tope_camino = 0;

        while( (*tope_camino < configuracion.complejidad *dimension) 
            && (intentos < MAX_INTENTOS) ){

            obtener_camino( camino, tope_camino, entrada, torre);
        }
    }
    void generar_camino_1( nivel_t* nivel, 
        caracteristicas_nivel_t caracteristicas_nivel , configuracion_t configuracion, bool cruzado ){
        
        int dimension = caracteristicas_nivel.dimension ;
        coordenada_t entrada,torre;
        entrada.fil=entrada.col=0;
        torre.fil=torre.col=10;

        if( caracteristicas_nivel.torre_1 ){

            generar_extremos_camino_1( &entrada, &torre,
                caracteristicas_nivel, cruzado );

            generar_camino(nivel->camino_1, &(nivel->tope_camino_1),
                entrada, torre, dimension, configuracion );

        }else
            nivel->tope_camino_1 = 0;
    }
    void generar_extremos_camino_1( coordenada_t* entrada, coordenada_t* torre,
        caracteristicas_nivel_t caracteristicas_nivel, bool cruzado ){

        int dimension = caracteristicas_nivel.dimension;
        switch( caracteristicas_nivel.entrada ){

            case ENTRADA_ESTE:
                entrada->fil = rand()%dimension;
                entrada->col = dimension-1;
                torre->fil = rand()%dimension;
                torre->col = 0;
            break;

            case ENTRADA_NORTE:
                entrada->fil = 0;
                entrada->col = rand()%(dimension/2)+(dimension/2)*(cruzado);
                torre->fil = dimension-1;
                torre->col = rand()%(dimension/2);
            break;

            case ENTRADA_SUR:
                entrada->fil = dimension-1;
                entrada->col = rand()%(dimension/2)+(dimension/2)*(cruzado);
                torre->fil = 0;
                torre->col = rand()%(dimension/2);
            break;
        }
    }
     
    void generar_camino_2( nivel_t* nivel, 
        caracteristicas_nivel_t caracteristicas_nivel , configuracion_t configuracion, bool cruzado ){
         
        int dimension = caracteristicas_nivel.dimension ;
        coordenada_t entrada,torre;
        entrada.fil=entrada.col=0;
        torre.fil=torre.col=10;

        if( caracteristicas_nivel.torre_2 ){

            generar_extremos_camino_2( &entrada, &torre,
                caracteristicas_nivel, cruzado );

            generar_camino(nivel->camino_2, &(nivel->tope_camino_2),
                entrada, torre, dimension, configuracion );

        }else
            nivel->tope_camino_2 = 0; 
    }
    void generar_extremos_camino_2( coordenada_t* entrada, coordenada_t* torre,
        caracteristicas_nivel_t caracteristicas_nivel, bool cruzado ){

        int dimension = caracteristicas_nivel.dimension;
        switch( caracteristicas_nivel.entrada ){

                case ENTRADA_OESTE:
                    entrada->fil = rand()%dimension;
                    entrada->col = 0;
                    torre->fil = rand()%dimension;
                    torre->col = dimension-1;
                break;

                case ENTRADA_NORTE:
                    entrada->fil = 0;
                    entrada->col = rand()%(dimension/2)+(dimension/2)*(!cruzado);
                    torre->fil = dimension-1;
                    torre->col = rand()%(dimension/2)+(dimension/2);

                break;

                case ENTRADA_SUR:
                    entrada->fil = dimension-1;
                    entrada->col = rand()%(dimension/2)+(dimension/2)*(!cruzado);
                    torre->fil = 0;
                    torre->col = rand()%(dimension/2)+(dimension/2);
                break;
            }
    }

    void agregar_defensores( juego_t* juego, caracteristicas_nivel_t caracteristicas_nivel  ){

        int k;
        
        // ENANOS
        k=0;
        while( k<caracteristicas_nivel.enanos )
            pedir_un_defensor( juego, caracteristicas_nivel, ENANO, &k ) ;
    

        // ELFOS
        k=0;
        while( k<caracteristicas_nivel.elfos)
            pedir_un_defensor( juego, caracteristicas_nivel, ELFO, &k ) ;
    }

    void pedir_un_defensor( juego_t* juego, 
        caracteristicas_nivel_t caracteristicas_nivel, char tipo, int* k  ){
    
        int col, fil, i=*k;
        char mensaje [MAX_MENSAJE], nombre[MAX_MENSAJE];
        coordenada_t posicion;
        char estado;
    
        strcpy(nombre,"defensor");
        if( tipo == ENANO )
            strcpy(nombre,"enano");
        if( tipo == ELFO )
            strcpy(nombre,"elfo");
    
        do{
            mostrar_juego(*juego);
            printf("\n COLOCAR DEFENSORES \n");
            printf("\n Enanos: %i \t Elfos: %i \n",caracteristicas_nivel.enanos,caracteristicas_nivel.elfos);

            sprintf(mensaje, "Ingrese la fila del %s %i",nombre,i+1);
            pedir_int( &fil, 0, caracteristicas_nivel.dimension-1,mensaje);
            sprintf(mensaje, "Ingrese la columna del %s %i",nombre,i+1);
            pedir_int( &col, 0, caracteristicas_nivel.dimension-1,mensaje);

            posicion.fil = fil;
            posicion.col = col;

        } while ( agregar_defensor( &(juego->nivel), posicion, tipo) == INVALIDO );
        i++;
        mostrar_juego(*juego);
        
        printf(" \n CANCELAR > [%c] ",CANCELAR);
        scanf("%c%c",&estado,&estado);
        if(toupper(estado) == toupper(CANCELAR)){
            i--;
            juego->nivel.tope_defensores --;
        }
        *k=i;
    }

    void auto_agregar_defensores( juego_t* juego, caracteristicas_nivel_t caracteristicas_nivel  ){

        int i;
        coordenada_t posicion, aux;
        
        // ENANOS
        i=0;
        while( i<caracteristicas_nivel.enanos ){

            do{
                aux = juego->nivel.camino_1[ rand()%(juego->nivel.tope_camino_1-2)+2 ];
                posicion.fil = aux.fil-1+rand()%3;
                posicion.col = aux.col-1+rand()%3;
            } while ( agregar_defensor( &(juego->nivel), posicion, ENANO) == INVALIDO );
            i++;
        }

        // ELFOS
        i=0;
        while( i<caracteristicas_nivel.elfos){
            do{
                aux = juego->nivel.camino_2[ rand()%(juego->nivel.tope_camino_2-2) ];
                posicion.fil = aux.fil-1+rand()%3;
                posicion.col = aux.col-1+rand()%3;
            } while ( agregar_defensor( &(juego->nivel), posicion, ELFO) == INVALIDO );
            i++;
            mostrar_juego(*juego);
        }    
    }

    void agregar_defensores_bonus( juego_t* juego, caracteristicas_nivel_t caracteristicas_nivel  ){

        char tipo;

        obtener_tipo_defensor_bonus( *juego, &tipo, caracteristicas_nivel );

        if( tipo != ELFO && tipo!=ENANO )
            return;

        colocar_defensores_bonus( juego, tipo, caracteristicas_nivel );
    }

    void obtener_tipo_defensor_bonus( juego_t juego, char* tipo,
        caracteristicas_nivel_t caracteristicas_nivel ){

        bool enanos,elfos;
        char opciones [MAX_OPCIONES];
        char nombre_opciones [MAX_OPCIONES][MAX_MENSAJE];
        int tope;
        char mensaje [MAX_MENSAJE];

        enanos = ( caracteristicas_nivel.torre_1 && juego.torres.enanos_extra > 0);
        elfos = ( caracteristicas_nivel.torre_2 && juego.torres.elfos_extra > 0);

        *tipo = NADIE;
        if( !enanos && !elfos ) return;

        mostrar_juego(juego);
        printf("\n PUEDE COLOCAR UN DEFENSOR EXTRA \n");
        printf(" COSTO: ");
        if(enanos) printf(" Enanos: %i Hp de la torre 1 .",COSTO_ENANO_EXTRA);
        if(elfos) printf(" Elfo: %i Hp de la torre 2 .",COSTO_ELFO_EXTRA);
        printf("\n\n");

        tope=0;
        opciones[tope] = CANCELAR;
        strcpy(nombre_opciones[tope], "CANCELAR");
        tope++;
        if( enanos ){
            opciones[tope] = ENANO;
            strcpy(nombre_opciones[tope],"ENANO");
            tope++;
        }
        if( elfos ){
            opciones[tope] = ELFO;
            strcpy(nombre_opciones[tope],"ELFO");
            tope++;
        }

        sprintf(mensaje," Que defensor desea colocar ?");
        pedir_char( tipo, opciones, nombre_opciones, tope, mensaje );
    }

    void colocar_defensores_bonus( juego_t* juego, char tipo,
        caracteristicas_nivel_t caracteristicas_nivel ){

        int col, fil;
        char estado;
        char mensaje [MAX_MENSAJE];
        coordenada_t posicion;

        do{
            do{
                mostrar_juego(*juego);
                printf("\n COLOCAR DEFENSOR \n");
                
                sprintf(mensaje, "Ingrese la fila del defensor ");
                pedir_int( &fil, 0, caracteristicas_nivel.dimension-1,mensaje);
                sprintf(mensaje, "Ingrese la columna del defensor");
                pedir_int( &col, 0, caracteristicas_nivel.dimension-1,mensaje);

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

    void auto_agregar_defensores_bonus( juego_t* juego, caracteristicas_nivel_t caracteristicas_nivel  ){

        char tipo;
        
        auto_obtener_tipo_defensor_bonus( juego, &tipo, caracteristicas_nivel );
        
        auto_colocar_defensores_bonus( juego, tipo, caracteristicas_nivel );
    }

    void auto_obtener_tipo_defensor_bonus( juego_t* juego, char* tipo,
        caracteristicas_nivel_t caracteristicas_nivel ){

        bool enanos,elfos;

        enanos = ( caracteristicas_nivel.torre_1 && 
            juego->torres.enanos_extra > 0);

        elfos = ( caracteristicas_nivel.torre_2 && 
            juego->torres.elfos_extra > 0);


        if( enanos && elfos ){
            if( rand()%2 ){
                *tipo = ENANO;
                juego->torres.enanos_extra --;
            }else{
                *tipo = ELFO;
                juego->torres.elfos_extra --;
            }
        }
        else if( enanos ){
            *tipo = ENANO;
            juego->torres.enanos_extra --;
        }
        else if( elfos ){
            *tipo = ELFO;
            juego->torres.elfos_extra --;
        }
        else
            *tipo = NADIE;
    }
    void auto_colocar_defensores_bonus( juego_t* juego, char tipo,
        caracteristicas_nivel_t caracteristicas_nivel ){

        coordenada_t posicion,aux;
        
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

    caracteristicas_nivel_t buscar_caracteristicas_nivel( int nivel ){

        caracteristicas_nivel_t caracteristicas_nivel;
        caracteristicas_nivel.num = 0;
        caracteristicas_nivel.orcos = 0;
        caracteristicas_nivel.dimension = 0;
        caracteristicas_nivel.torre_1 = caracteristicas_nivel.torre_2 = false;

        caracteristicas_nivel_t niveles[CANTIDAD_NIVELES] = { NIVEL_1, NIVEL_2, NIVEL_3, NIVEL_4 };

        for( int i = 0; i < CANTIDAD_NIVELES; i++ )
            if( niveles[i].num == nivel )
                caracteristicas_nivel = niveles[i];

        return caracteristicas_nivel;
    }

    void iniciar_animos(int* viento , int* humedad , char* animo_legolas , char* animo_gimli){

        system("clear");
        printf("\n");
        printf("----------------------------------------------------\n");
        printf("-------------          ANIMOS          -------------\n");
        printf("----------------------------------------------------\n");
        printf("\n");
        printf("> Viento : %i \n",*viento);
        printf("> Humedad : %i \n",*humedad);
        printf("> Animo Legolas : %c \n",*animo_legolas);
        printf("> Animo Gimli : %c \n",*animo_gimli);
        printf("\n\n");

        if( pedir_bool("INICIAR ANIMOS") )
            animos(viento , humedad , animo_legolas , animo_gimli);
    }
// JUEGO (¡)

