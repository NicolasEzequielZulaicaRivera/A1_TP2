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

    static const int RESISTENCIA_ORCO  = 200;
    static const int RESISTENCIA_ORCO_RAND  = 100;

    static const int RESISTENICA_TORRE_INI = 600;

    static const int COSTO_ENANO_EXTRA = 50;
    static const int COSTO_ELFO_EXTRA = 50;

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

    // Funciones para pedir datos de distintos tipos
    void pedir_int( int* dato, int min, int max, char msg[MAX_MENSAJE] );
    void pedir_float( float* dato, float min, float max, char msg[MAX_MENSAJE] );
    void pedir_char( char* dato, char opciones [MAX_OPCIONES], char nombre_opciones [MAX_OPCIONES][MAX_MENSAJE], int tope, char msg[MAX_MENSAJE] );
    bool pedir_bool( char msg[MAX_MENSAJE] );
    
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
        float velocidad; // tiempo entre turnos 
        int bonus_resistencia; // regeneracion de vida por nivel
        bool saltear_niveles, invencible;
        bool auto_defensores; // colocar defensores automaticamente
        int complejidad, rareza_cruzado; // parametros para los caminos
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

    // muestra opciones/configuracion
    void mostrar_opciones( juego_t* juego, configuracion_t* configuracion );

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

    // Muestra un mensaje al pasar de nivel
    void mensaje_nuevo_nivel( int nivel );

    // Muestra un mensaje al finalizar un juego (ganar o perder)
    void mensaje_terminar_juego( int estado );

    // Pide al usuario que coloque defensores segun las especificaciones del nivel
    void agregar_defensores( juego_t* juego, caracteristicas_nivel_t caracteristicas_nivel  );

    // Coloqua defensores segun las especificaciones del nivel
    void auto_agregar_defensores( juego_t* juego, caracteristicas_nivel_t caracteristicas_nivel  );

    // Pide al usuario que coloque un defensor extra segun las especificaciones del nivel
    void agregar_defensores_bonus( juego_t* juego, caracteristicas_nivel_t caracteristicas_nivel  );
    
    // Coloca automaticamente un defensor extra segun las especificaciones del nivel
    void auto_agregar_defensores_bonus( juego_t* juego, caracteristicas_nivel_t caracteristicas_nivel  );

    // Devuelve las especificaciones del nivel pedido
    caracteristicas_nivel_t buscar_caracteristicas_nivel( int nivel );

    // Muestra las variables que dependen de los animos y pregunta si se quieren iniciar
    void iniciar_animos(int* viento , int* humedad , char* animo_legolas , char* animo_gimli);
 // HEADER JUEGO (¡)


int main(){
    srand( (unsigned int) time(NULL));  // ERROR: unsigned -> integer
    //srand(105774); 

    configuracion_t configuracion;
    iniciar_configuracion( &configuracion );

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
    
    void pedir_int( int* dato, int min, int max, char msg[MAX_MENSAJE] ){

        printf("%s\n",msg );
        printf("[%i - %i] > ",min,max );

        do{
            scanf(" %i",dato);
        } while( (*dato < min) || (*dato > max) );

        return;    
    }

    void pedir_float( float* dato, float min, float max, char msg[MAX_MENSAJE] ){
        
        printf("%s\n",msg );
        printf("[%f - %f] > ",min,max );

        do{
            scanf(" %f",dato);
        } while( (*dato < min) || (*dato > max) );

        return;
    }

    void pedir_char( char* dato, char opciones [MAX_OPCIONES], char nombre_opciones [MAX_OPCIONES][MAX_MENSAJE], int tope, char msg[MAX_MENSAJE] ){

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

    bool pedir_bool( char msg[MAX_MENSAJE] ){

        char opciones [MAX_OPCIONES]; 
        char nombre_opciones [MAX_OPCIONES][MAX_MENSAJE];
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

        char input[20];
        scanf("%s",input);
        *opcion = input[0]-48;
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

            char input[20];
            scanf("%s",input);
            opcion = input[0]-48;

            char msg[MAX_MENSAJE];

            switch( opcion ){
                case 1:
                    sprintf(msg," Ingrese la FRECUENCIA");
                    pedir_float( &(configuracion->velocidad), 0.1f, 1.0f, msg );
                break;

                case 2:
                    sprintf(msg," Ingrese el BONUS");
                    pedir_int( &(configuracion->bonus_resistencia), 0, 9000, msg );
                break;

                case 3:
                    sprintf(msg,"Ingrese la COMPLEJIDAD");
                    pedir_int( &(configuracion->complejidad), 0, 5, msg );
                break;

                case 4:
                    sprintf(msg,"Ingrese la RAREZA");
                    pedir_int( &(configuracion->rareza_cruzado), 1, 50, msg );
                break;

                case 5:
                    sprintf(msg,"Auto posicionar defensores");
                    configuracion->auto_defensores = pedir_bool(msg);
                break;

                case 6:
                    sprintf(msg,"Ser invencible");
                    configuracion->invencible = pedir_bool(msg);
                break;

                case 7:
                    sprintf(msg,"Saltear niveles");
                    configuracion->saltear_niveles = pedir_bool(msg);
                break;
            }
        }
        return;
    }

    void iniciar_configuracion( configuracion_t* configuracion ){
        
        *configuracion = CONFIGURACION_STANDAR;
    }
// MENU Y CONFIGURACION (!)

// JUEGO (!)

    // Aplica acciones extra al comenzar un nivel segun la configuracion
    void bonus_nuevo_nivel( juego_t* juego , configuracion_t configuracion );

    void nuevo_juego( juego_t* juego , configuracion_t configuracion ){

        int turno = 0;
        caracteristicas_nivel_t caracteristicas_nivel;
        juego->nivel_actual = 0;
        juego->nivel.tope_enemigos = 0;
        juego->nivel = nuevo_nivel( juego->nivel_actual, configuracion );

        while( estado_juego( *juego) == ESTADO_JUGANDO ){

            if( estado_nivel( juego->nivel ) == ESTADO_GANADO  ){
                // NUEVO NIVEL
                
                juego->nivel_actual ++;
                caracteristicas_nivel = buscar_caracteristicas_nivel( juego->nivel_actual );
                turno = 0;
                juego->nivel = nuevo_nivel( juego->nivel_actual, configuracion );
                mensaje_nuevo_nivel( juego->nivel_actual );

                bonus_nuevo_nivel( juego , configuracion );

                if( (juego->nivel_actual <= CANTIDAD_NIVELES) && (juego->nivel.max_enemigos_nivel > 0) ){

                    if( configuracion.auto_defensores )
                        auto_agregar_defensores( juego, caracteristicas_nivel );
                    else
                        agregar_defensores( juego, caracteristicas_nivel );
                }

                mostrar_juego( *juego );
                detener_el_tiempo( configuracion.velocidad*3 );

            }else{
                // JUGAR TURNO

                
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

                jugar_turno( juego );
                turno ++;

                mostrar_juego( *juego );

                detener_el_tiempo( configuracion.velocidad );

            }

            if(configuracion.invencible){
                juego->torres.resistencia_torre_1 = 
                juego->torres.resistencia_torre_2 = 
                RESISTENICA_TORRE_INI;
            }
        }

        // GANO O PERDIO
        mensaje_terminar_juego( estado_juego( *juego) );
    }

    void bonus_nuevo_nivel( juego_t* juego , configuracion_t configuracion ){

        juego->torres.resistencia_torre_1 += configuracion.bonus_resistencia;
        juego->torres.resistencia_torre_2 += configuracion.bonus_resistencia;

        if( configuracion.saltear_niveles ){
            char rta;
            printf("\n SALTEAR NIVEL [%c]\n", CONFIRMAR );
            scanf("%c",&rta);

            if( toupper(rta) == toupper(CONFIRMAR) )
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

    // Generan los caminos del nivel segun sus caracteristicas y la coniguracion
    void generar_camino_1( nivel_t* nivel, caracteristicas_nivel_t caracteristicas_nivel , configuracion_t configuracion, bool cruzado );
    void generar_camino_2( nivel_t* nivel, caracteristicas_nivel_t caracteristicas_nivel , configuracion_t configuracion, bool cruzado );

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

    void generar_camino_1( nivel_t* nivel, 
        caracteristicas_nivel_t caracteristicas_nivel , configuracion_t configuracion, bool cruzado ){
        
        int dimension = caracteristicas_nivel.dimension ;
        int intentos = 0;
        coordenada_t entrada,torre;
        entrada.fil=entrada.col=0;
        torre.fil=torre.col=10;

        if( caracteristicas_nivel.torre_1 ){

            switch( caracteristicas_nivel.entrada ){

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
            nivel->tope_camino_1 = 0;

            while( (nivel->tope_camino_1 < configuracion.complejidad *dimension) 
                && (intentos < MAX_INTENTOS) ){

                obtener_camino(
                    nivel->camino_1,
                    &(nivel->tope_camino_1), 
                    entrada, torre
                );
            }
        }else
            nivel->tope_camino_1 = 0;
    }
    void generar_camino_2( nivel_t* nivel, 
        caracteristicas_nivel_t caracteristicas_nivel , configuracion_t configuracion, bool cruzado ){
         
        int dimension = caracteristicas_nivel.dimension ;
        int intentos = 0;
        coordenada_t entrada,torre;
        entrada.fil=entrada.col=0;
        torre.fil=torre.col=10;

        if( caracteristicas_nivel.torre_2 ){

            switch( caracteristicas_nivel.entrada ){

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
            nivel->tope_camino_2 = 0;

            while( (nivel->tope_camino_2 < configuracion.complejidad*dimension) 
                && (intentos < MAX_INTENTOS) ){

                obtener_camino(
                    nivel->camino_2,
                    &nivel->tope_camino_2, 
                    entrada, torre
                );
            }

        }else
            nivel->tope_camino_2 = 0; 
    }


    // Pide al usuario que coloque un defensor segun las especificaciones del nivel
    void pedir_un_defensor( juego_t* juego, 
        caracteristicas_nivel_t caracteristicas_nivel, char tipo, int* k );
    
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
        char msg [MAX_MENSAJE], nombre[MAX_MENSAJE];
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

            sprintf(msg, "Ingrese la fila del %s %i",nombre,i+1);
            pedir_int( &fil, 0, caracteristicas_nivel.dimension-1,msg);
            sprintf(msg, "Ingrese la columna del %s %i",nombre,i+1);
            pedir_int( &col, 0, caracteristicas_nivel.dimension-1,msg);

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

        int col, fil;
        char msg [MAX_MENSAJE];
        coordenada_t posicion;
        char tipo, estado;
        bool enanos,elfos;

        char opciones [MAX_OPCIONES];
        char nombre_opciones [MAX_OPCIONES][MAX_MENSAJE];
        int tope;

        enanos = ( caracteristicas_nivel.torre_1 && 
            juego->torres.enanos_extra > 0);

        elfos = ( caracteristicas_nivel.torre_2 && 
            juego->torres.elfos_extra > 0);

        if( !enanos && !elfos )
            return;

        mostrar_juego(*juego);
        printf("\n PUEDE COLOCAR UN DEFENSOR EXTRA \n");
        printf(" COSTO: ");
        if(enanos)
            printf(" Enanos: %i Hp de la torre 1 .",COSTO_ENANO_EXTRA);
        if(elfos)
            printf(" Elfo: %i Hp de la torre 2 .",COSTO_ELFO_EXTRA);
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
            juego->torres.resistencia_torre_1 -= COSTO_ENANO_EXTRA;
            juego->torres.enanos_extra --;
        }
        else if( toupper(tipo) == toupper(ELFO) ){
            tipo = ELFO;
            juego->torres.resistencia_torre_2 -= COSTO_ELFO_EXTRA;
            juego->torres.elfos_extra --;
        }
        else
            return;

        do{
            do{
                mostrar_juego(*juego);
                printf("\n COLOCAR DEFENSOR \n");
                
                sprintf(msg, "Ingrese la fila del defensor ");
                pedir_int( &fil, 0, caracteristicas_nivel.dimension-1,msg);
                sprintf(msg, "Ingrese la columna del defensor");
                pedir_int( &col, 0, caracteristicas_nivel.dimension-1,msg);

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

        coordenada_t posicion,aux;
        char tipo;
        bool enanos,elfos;

        enanos = ( caracteristicas_nivel.torre_1 && 
            juego->torres.enanos_extra > 0);

        elfos = ( caracteristicas_nivel.torre_2 && 
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

