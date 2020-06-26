#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "defendiendo_torres.h"


void mostrar_vida(juego_t juego, int dim, int camino);

int main()
{

	int dim = 9,k;
	int caminos = 1;
	juego_t juego;


	nivel_t nivel;
	enemigo_t orc;

	char tipo = 'L';
	inicializar_juego(&juego, 0, 0, 'M', 'M');

	start:

	nivel.tope_defensores=0;
	nivel.tope_camino_1=0;
	nivel.tope_camino_2=0;

	coordenada_t posicion;posicion.fil=dim/2;posicion.col=dim/2;
	if(tipo=='L' || tipo=='G')
		agregar_defensor(&nivel, posicion,tipo);
	if(tipo=='B'){
		agregar_defensor(&nivel, posicion,'L');
		posicion.col++;
		agregar_defensor(&nivel, posicion,'G');
		posicion.col--;
	}

	if( caminos==1 || caminos==3 ){
		for(int i=0; i<dim; i++){
			for(int j=0; j<dim; j++){

				if( (i!=posicion.fil || j!=posicion.col) &&
					( !(tipo=='B') || (i!=posicion.fil || j!=posicion.col+1) )
				 ){

				nivel.camino_1[nivel.tope_camino_1].fil=i;
				nivel.camino_1[nivel.tope_camino_1].col=j;
				nivel.tope_camino_1 ++;
				}

			}
		}
	}
	if( caminos==2 || caminos==3 ){
		for(int i=0; i<dim; i++){
			for(int j=0; j<dim; j++){

				if( (i!=posicion.fil || j!=posicion.col) &&
					( !(tipo=='B') || (i!=posicion.fil || j!=posicion.col+1) )
				 ){

				nivel.camino_2[nivel.tope_camino_2].fil=i;
				nivel.camino_2[nivel.tope_camino_2].col=j;
				nivel.tope_camino_2 ++;
				}

			}
		}
	}


	nivel.tope_enemigos=0;


	if( caminos==1 || caminos==3 ){
		for(int i=2; i<(dim*dim)-3; i++ ){
			orc.vida=201;
			if( tipo== 'O')
				orc.vida=(dim*dim)-3-i;
			orc.camino = 1;
			orc.pos_en_camino = i;
			nivel.enemigos[ nivel.tope_enemigos++ ]=orc;
		}
	}
	if( caminos==2 || caminos==3 ){
		for(int i=2; i<(dim*dim)-3; i++ ){
			orc.vida=201;
			if( tipo== 'O')
				orc.vida=(dim*dim)-3-i;
			orc.camino = 2;
			orc.pos_en_camino = i;
			nivel.enemigos[ nivel.tope_enemigos++ ]=orc;
		}
	}


	nivel.max_enemigos_nivel=nivel.tope_enemigos;



	juego.nivel = nivel;
	juego.nivel_actual = -1;
	juego.torres.resistencia_torre_1 = 5000;

	char c;
	mostrar_juego(juego);
	mostrar_vida(juego,dim,1);
	mostrar_vida(juego,dim,2);
	printf("START\n");

	do{
		scanf("%c",&c);
		jugar_turno(&juego);
		for(int o=0; o<nivel.max_enemigos_nivel; o++)
			if(juego.nivel.enemigos[o].vida>0 && tipo!='O')
				juego.nivel.enemigos[o].pos_en_camino--;
		mostrar_juego(juego);
		mostrar_vida(juego,dim,1);
		mostrar_vida(juego,dim,2);


		if(c=='r'){
			scanf("%c",&c);
			goto start;
		}
		if(c=='f'){
			printf("F >\n");
			scanf(" %i",&(juego.fallo_legolas));
			scanf("%c",&c);
			juego.fallo_gimli=juego.fallo_legolas;
			goto start;
		}
		if(c=='c'){
			printf("C >\n");
			scanf(" %i",&(juego.critico_legolas));
			scanf("%c",&c);
			juego.critico_gimli=juego.critico_legolas;
			goto start;
		}
		if(c=='g'){
			scanf("%c",&c);
			tipo='G';
			goto start;
		}
		if(c=='l'){
			scanf("%c",&c);
			tipo='L';
			goto start;
		}
		if(c=='o'){
			scanf("%c",&c);
			tipo='O';
			goto start;
		}
		if(c=='b'){
			scanf("%c",&c);
			tipo='B';
			goto start;
		}
		if(c=='1'){
			scanf("%c",&c);
			caminos=1;
			goto start;
		}
		if(c=='2'){
			scanf("%c",&c);
			caminos=2;
			goto start;
		}
		if(c=='3'){
			scanf("%c",&c);
			caminos=3;
			goto start;
		}
		if(c=='a'){
			scanf("%c",&c);
			scanf("%i",&k);
			inicializar_juego(&juego, k/2, k/2, c, c);
			mostrar_juego(juego);
			scanf("%c",&c);
			goto start;
		}

	}while(c!='0');
                

	return 0;
}

const char H1 [20]="0;30;42m";
const char H2 [20]="0;30;43m";
const char H3 [20]="0;30;41m ";
const char H4 [20]="0;30;41m  ";
const char H5 [20]="0;30;45m ";
const char H6 [20]="0;30;45m";

void mostrar_vida(juego_t juego, int dimension, int camino){

	int i,j;
	int mapa[dimension][dimension];
	coordenada_t pos;
	char sty [20];

	for(i=0;i<dimension;i++)
		for(j=0;j<dimension;j++)
			mapa[i][j]=-1;

	for(int i=0; i<juego.nivel.tope_enemigos; i++){

		if( camino == 1 && juego.nivel.enemigos[i].camino==1 ){
			pos=juego.nivel.camino_1[ juego.nivel.enemigos[i].pos_en_camino ];
			mapa[pos.fil][pos.col]= juego.nivel.enemigos[i].vida;
		}
		if( camino == 2 && juego.nivel.enemigos[i].camino==2 ){
			pos=juego.nivel.camino_2[ juego.nivel.enemigos[i].pos_en_camino ];
			mapa[pos.fil][pos.col]= juego.nivel.enemigos[i].vida;
		}
	}

	printf("\n\033[0;40m|==||");
		for(j=0 ; j < dimension; j++){
			printf("%s", " ");
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

				strcpy(sty,
				( mapa[i][j]>200)? H1:
				( mapa[i][j]>99 )? H2:
				( mapa[i][j]>9  )? H3:
				( mapa[i][j]>=0 )? H4:
				( mapa[i][j]>-10)? H5:
				H6);

				printf("\033[%s%i\033[0;2;40m|\033[0m",sty,mapa[i][j]);
			}

			printf("\033[0;40m|");
			if( i < 10 )
				printf("0");
			printf("%i| \033[0m",i);
		}

		printf("\n\033[0;40m|==||");
		for(j=0 ; j < dimension; j++){
			printf("%s", " ");
			if( j < 10 )
				printf("0");
			printf("%i|", j);
		}
		printf("|==| ");
		printf("\033[0m\n\n");

}



