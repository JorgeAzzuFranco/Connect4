#include <stdlib.h>
#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <cmath>
#include <unistd.h>
#include <SDL/SDL_ttf.h>
#include <cstring>
#include <fstream>

//Macros
#define FIL 6
#define COL 7
#define MAX 20

using namespace std;

//Funciones

//Funciones para SDL
SDL_Surface * iniciar_sdl (int w, int h, char * titulo);
SDL_Surface * crear_fondo (SDL_Surface * screen);
void agregarImagen(SDL_Surface *img, int fil, int col, SDL_Surface * screen);
void putpixel(SDL_Surface *theScreen, int x, int y, Uint32 pixel);
void agregarImagenPantalla(SDL_Surface *, int, int, SDL_Surface *);

//Funciones para texto
void dibujarCuadrado(Uint32 c,SDL_Rect *r);
int leerCad(SDL_Surface *screen,int ancho,int alto,int x,int y,char *cad,int n,TTF_Font *f,SDL_Color ct,SDL_Color cf,int hide,int blinking,int tiempo);
void mostrar(SDL_Surface *screen,int x,int y,char *cad,int n,TTF_Font *f,SDL_Color ct,SDL_Color cf,int b,int t,int h);
void xyTextoFC(SDL_Surface *screen,int x,int y,const char *t,TTF_Font *f,SDL_Color c);
void escribirPantalla();

//Funciones de tiempo
bool timer();
void *tiempo(void *arg);

//Funciones para el juego
bool validarGanador(char token);
void inicializarTablero();
void verificarPosicion(int fila, int columna, int x, int y);
void jugarTurno(int x, int y);
void listener();
void elegirFicha();

//Variables

//Variables para SDL
SDL_Event event;
SDL_Surface * screen;
SDL_Surface * canvas;
SDL_Surface * imagenX;
SDL_Surface * imageny;
SDL_Surface * imagen1;
SDL_Surface * imagen2;
SDL_Surface * imagen3;
SDL_Surface * play1;
SDL_Surface * play2;
SDL_Surface * ganadorp1;
SDL_Surface * ganadorp2;
SDL_Surface * empate;
SDL_Surface * blanco;
SDL_Surface * ini1;
SDL_Surface * ini2;
SDL_Surface * ini3;
SDL_Surface * ini4;
SDL_Surface * fondo;
SDL_Surface * elegir;
SDL_Surface * fichaP1;
SDL_Surface * fichaP2;
SDL_Surface * fichaP3;
SDL_Surface * ledA;
SDL_Surface * ledBlanco;
SDL_Surface * ledV;
SDL_Surface * histo;
SDL_Surface * nombre;
SDL_Surface * nombre1;
SDL_Surface * nombre2;
SDL_Surface * consola;

//Variables para texto
SDL_Surface * imagen;
TTF_Font *font=NULL;				//fuente a utilizar para la escritura GUI
SDL_Color BLACK = {0,0,0};			//color de la palabra escrita
SDL_Color WHITE = {255,255,255};	//color de fondo de la palabra
SDL_Event e;						//captura evento
char cadena[MAX];					//guarda la cadena escrita en la ventana
char cadena1[MAX];

// Variables miscelaneas
int elegido;
bool c=false;
bool turno=true;
bool ganador = false;
bool jugar=false;
bool bandera=false;
int contador=0;
int contador1=0;
int cuentaFichas=0;
int cx, cy;
int c0=5, c1=5, c2=5, c3=5, c4=5, c5=5, c6=5;
int a0=0, a1=0, a2=0, a3=0, a4=0, a5=0, a6=0;
char tablero[FIL][COL];
string cade;
Uint32 pixel;

int main(){
	//Cargando Imagenes
	imagen1 = IMG_Load ("f1.png");
	imagen2 = IMG_Load ("f2.png");
	imagen3 = IMG_Load("f3.png");
	ganadorp1 = IMG_Load ("P1Win.png");
	ganadorp2 = IMG_Load ("P2Win.png");
	empate = IMG_Load ("Empate.png");
	blanco = IMG_Load ("blanco.jpg");
	ini1 = IMG_Load("Inicio1.png");
	ini2 = IMG_Load("Inicio2.png");
	ini3 = IMG_Load("Inicio3.png");
	ini4 = IMG_Load("Inicio4.png");
	elegir = IMG_Load("Elegir.png");
	fondo = IMG_Load("Fondo.png");
	fichaP1 = IMG_Load("Ficha1.png");
	fichaP2 = IMG_Load("Ficha2.png");
	fichaP3 = IMG_Load("Ficha3.png");
	ledA = IMG_Load("LedA.png");
	ledBlanco = IMG_Load("LedB.png");
	ledV = IMG_Load("LedV.png");
	histo = IMG_Load("BotonUltimos.png");
	nombre = IMG_Load("Nombre.png");
	nombre1 = IMG_Load("Nombre1.png");
	nombre2 = IMG_Load("Nombre2.png");
	consola = IMG_Load("Consola.png");
	
	
	//inicializando SDL
	char cadena[9] = {"Connect4"};
	screen = iniciar_sdl (900, 600, cadena);
	if (!screen){
		exit (1);
	}

	//levantando lienzo de trabajo
	canvas = crear_fondo (screen);
	if (!canvas){
		exit (1);
	}
	
	//Creando hilo para inicio
	pthread_t h1;	
	pthread_create(&h1,NULL, tiempo,NULL);
	pthread_join (h1,NULL);
	//cout << endl;
	
	inicializarTablero();
	
	//Muestra la matriz en terminaĺ
	/*for (int i = 0; i < FIL; i++){
		for (int j = 0; j < COL; j++){
			cout << "[" << i << "," << j << "]";
		}
		cout << endl;
	}*/

	while(true){
		listener();	
	}
	
	return 0;
}

//Funciones de SDL
SDL_Surface * iniciar_sdl (int w, int h, char * titulo){
	int flags = SDL_SWSURFACE;
	SDL_Surface * screen;

	if (SDL_Init (SDL_INIT_VIDEO) == -1){
		printf ("%s\n", SDL_GetError ());
		return NULL;
	}
	//INICIALIZANDO FONTS
	if(TTF_Init() < 0){
		printf("Error al iniciar SDL_ttf: %s", SDL_GetError());
		exit(1);
	}
	font = TTF_OpenFont("fuente.ttf", 50);
	if (!font) {
		printf( "Error al cargar una fuente: %s.\n", SDL_GetError() );
		exit(1);
	}
	//esto hacia que no sirviera :v
	atexit (SDL_Quit);

	SDL_WM_SetCaption (titulo, NULL);
	
	screen = SDL_SetVideoMode (w, h, 16, flags);
	
	if (!screen){
		printf ("%s\n", SDL_GetError ());
		return NULL;
	}
	
	return screen;
}

SDL_Surface * crear_fondo (SDL_Surface * screen){
	SDL_Surface * tmp;
	Uint32 color = SDL_MapRGB (screen->format, 255, 255, 255);
	tmp = SDL_DisplayFormat (screen);
	if(!tmp){
		printf ("%s\n", SDL_GetError ());
		return NULL;
	}

	SDL_FillRect (tmp, NULL, color);
	SDL_BlitSurface (tmp, NULL, screen, NULL);
	SDL_Flip (screen);
	
	return tmp;
}

void agregarImagen(SDL_Surface *img, int fil, int col, SDL_Surface * screen){
	SDL_Rect destino;
	destino.x = col;
	destino.y = fil;
	SDL_BlitSurface(img, NULL, screen, &destino);
	SDL_Flip(screen);
}

void agregarImagenPantalla(SDL_Surface *imagen, int x, int y, SDL_Surface * screen){
	SDL_Rect destino2;
	destino2.x = y;
	destino2.y = x;
	SDL_BlitSurface(imagen, NULL, screen, &destino2);
	SDL_Flip(screen);
}

void putpixel(SDL_Surface *theScreen, int x, int y, Uint32 pixel) {
    int byteperpixel = theScreen->format->BytesPerPixel;

    Uint8 *p = (Uint8*)theScreen->pixels + y * theScreen->pitch + x * byteperpixel;

	// Adress to pixel
    *(Uint32 *)p = pixel;
}

//Funciones para mostrar en pantalla
void dibujarCuadrado(Uint32 color,SDL_Rect *r){
    SDL_FillRect(screen,r,color);
    SDL_Flip(screen);
}

int leerCad(SDL_Surface *screen,int ancho,int alto,int x,int y,char *cad,int n,TTF_Font *f,SDL_Color ct,SDL_Color cf,int hide,int blinking,int tiempo){
	SDL_Rect blanco;
	blanco.x =  x;
	blanco.y =  y;
	blanco.w =  220;
	blanco.h = 30;
	//agregarImagen(nombre1, x, y, screen);
	//dibujarCuadrado(0xffffff,&blanco);
	int i=strlen(cad);
	char car;
	SDL_Event event;
	while(true){
		mostrar(screen,x,y,cad,n,f,ct,cf,blinking,tiempo,hide);
		if(SDL_PollEvent(&event)){
			if(event.type==SDL_KEYDOWN){
				switch(event.key.keysym.sym){
					case SDLK_RETURN:
						mostrar(screen,x,y,cad,n,f,ct,cf,-1,0,hide);
						return 1;
					case SDLK_BACKSPACE:
						if(i>0) cad[--i]='\0';
						break;
					case SDLK_ESCAPE:
						mostrar(screen,x,y,cad,n,f,ct,cf,-1,0,hide);
						return 0;
					default:
					 if(i<n && event.key.keysym.sym<255){
						car=event.key.keysym.sym;
						if((car>=32 && car <=126)||(car>=128 && car<=253)){
							cad[i]=event.key.keysym.sym;
							if(car>=97 && car<=122){
								if(((event.key.keysym.mod&KMOD_CAPS) &&
									!(event.key.keysym.mod&KMOD_SHIFT)) ||
									(!(event.key.keysym.mod&KMOD_CAPS) &&
									 (event.key.keysym.mod&KMOD_SHIFT))){
									cad[i]=cad[i]-32; // Obtiene caracter
								}
							}
								if(car>=42 && car<=57){
								if(((event.key.keysym.mod&KMOD_CAPS) &&
									!(event.key.keysym.mod&KMOD_SHIFT)) ||
									(!(event.key.keysym.mod&KMOD_CAPS) &&
									 (event.key.keysym.mod&KMOD_SHIFT))){								
										if(car==43)
											cad[i]=cad[i]-1;	//"*"
										if(car>48 && car<=57 && car!=55)
											cad[i]=cad[i]-16;	//caracteres sobre los numeros								
										if(car==55)
											cad[i]=cad[i]-8;	//"=" sobre el 0	
										if(car==48)
											cad[i]=cad[i]+13;	//"=" sobre el 0
								}
							}
							cad[++i]='\0';
						}
					}
				}
			}
		}
	}
}

	
void mostrar(SDL_Surface *screen,int x,int y,char *cad,int n,TTF_Font *f,SDL_Color ct,SDL_Color cf,int b,int t,int h){
	static Sint32 ant=-1;
	static char *cadAnt=NULL;
	Uint32 s,act,i;
	char salida[n+2],temporal[n+1];
	if(cadAnt==NULL){
		cadAnt=(char *)malloc(n+2);
		if(cadAnt==NULL) return;
		cadAnt[0]='\0'; // Inicia como cadena nula
	}
	if(h){
		for(i=0;cad[i];i++) temporal[i]='*';
		temporal[i]='\0';
		cad=temporal;
	}
	if(b==-1){
		sprintf(salida,"%s|",cad);
		xyTextoFC(screen,x,y,salida,f,cf);
		xyTextoFC(screen,x,y,cad,f,ct);
		free(cadAnt);
		cadAnt=NULL;
		return;
	}
	if(b==0){
		sprintf(salida,"%s|",cad);
		if(strcmp(salida,cadAnt)){
			xyTextoFC(screen,x,y,cadAnt,f,cf);
			sprintf(cadAnt,"%s",salida);
			xyTextoFC(screen,x,y,salida,f,ct);
		}
		return;
	}
	s=SDL_GetTicks();
	act=(s/t)%2;
	if(act==0)
		sprintf(salida,"%s|",cad);
	else
		sprintf(salida,"%s",cad);
	if(act==ant && !strcmp(cadAnt,salida)) return;
	ant=act;
	xyTextoFC(screen,x,y,cadAnt,f,cf);
	sprintf(cadAnt,"%s",salida);
	xyTextoFC(screen,x,y,salida,f,ct);
	SDL_Flip(screen);
}

void xyTextoFC(SDL_Surface *screen,int x,int y,const char *t,
	TTF_Font *f,SDL_Color c){
	int ancho,alto;
	SDL_Rect rect;
	SDL_Surface *text=NULL;
	text=TTF_RenderText_Solid(f,t,c);
	if(text==NULL) return;
	rect.x=x;
	rect.y=y;
	TTF_SizeText(f,t,&ancho,&alto);
	SDL_BlitSurface(text,NULL,screen,&rect);
	SDL_UpdateRect(screen,x,y,ancho,alto);
	SDL_FreeSurface(text);
}

void escribirPantalla(){
	//escribiendo pregunta
	xyTextoFC(screen,80,90,"Ingrese su nombre por favor",font,BLACK);
	xyTextoFC(screen,80,170,"Player 1. ",font,BLACK);

	SDL_Rect rectangulo;
	rectangulo.x =  90;
	rectangulo.y =  240;
	rectangulo.w =  450;
	rectangulo.h = 50;
	dibujarCuadrado(0x000000,&rectangulo);

	rectangulo.x =  92;
	rectangulo.y =  242;
	rectangulo.w =  446;
	rectangulo.h = 46;
	dibujarCuadrado(0xffffff,&rectangulo);
	
	xyTextoFC(screen,80,300,"Player 2. ",font,BLACK);

	//SDL_Rect rectangulo;
	rectangulo.x =  90;
	rectangulo.y =  370;
	rectangulo.w =  450;
	rectangulo.h = 50;
	dibujarCuadrado(0x000000,&rectangulo);

	rectangulo.x =  92;
	rectangulo.y =  372;
	rectangulo.w =  446;
	rectangulo.h = 46;
	dibujarCuadrado(0xffffff,&rectangulo);
}

//Funciones del Juego
bool validarGanador(char token){
	//Columnas
	//Respecto a 6
	if(tablero[0][6] == token && tablero[1][6] == token && tablero[2][6] == token && tablero[3][6]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[1][6] == token && tablero[2][6] == token && tablero[3][6] == token && tablero[4][6]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[2][6] == token && tablero[3][6] == token && tablero[4][6] == token && tablero[5][6]== token){
		SDL_Flip(screen);
		return true;
	}
	//Respecto a 5
	if(tablero[0][5] == token && tablero[1][5] == token && tablero[2][5] == token && tablero[3][5]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[1][5] == token && tablero[2][5] == token && tablero[3][5] == token && tablero[4][5]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[2][5] == token && tablero[3][5] == token && tablero[4][5] == token && tablero[5][5]== token){
		SDL_Flip(screen);
		return true;
	}
	//Respecto a 4
	if(tablero[0][4] == token && tablero[1][4] == token && tablero[2][4] == token && tablero[3][4]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[1][4] == token && tablero[2][4] == token && tablero[3][4] == token && tablero[4][4]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[2][4] == token && tablero[3][4] == token && tablero[4][4] == token && tablero[5][4]== token){
		SDL_Flip(screen);
		return true;
	}
	//Respecto a 3
	if(tablero[0][3] == token && tablero[1][3] == token && tablero[2][3] == token && tablero[3][3]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[1][3] == token && tablero[2][3] == token && tablero[3][3] == token && tablero[4][3]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[2][3] == token && tablero[3][3] == token && tablero[4][3] == token && tablero[5][3]== token){
		SDL_Flip(screen);
		return true;
	}
	//Respecto a 2
	if(tablero[0][2] == token && tablero[1][2] == token && tablero[2][2] == token && tablero[3][2]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[1][2] == token && tablero[2][2] == token && tablero[3][2] == token && tablero[4][2]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[2][2] == token && tablero[3][2] == token && tablero[4][2] == token && tablero[5][2]== token){
		SDL_Flip(screen);
		return true;
	}
	//Respecto a 1
	if(tablero[0][1] == token && tablero[1][1] == token && tablero[2][1] == token && tablero[3][1]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[1][1] == token && tablero[2][1] == token && tablero[3][1] == token && tablero[4][1]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[2][1] == token && tablero[3][1] == token && tablero[4][1] == token && tablero[5][1]== token){
		SDL_Flip(screen);
		return true;
	}
	//Respecto a 0
	if(tablero[0][0] == token && tablero[1][0] == token && tablero[2][0] == token && tablero[3][0]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[1][0] == token && tablero[2][0] == token && tablero[3][0] == token && tablero[4][0]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[2][0] == token && tablero[3][0] == token && tablero[4][0] == token && tablero[5][0]== token){
		SDL_Flip(screen);
		return true;
	}

	//FILAS
	//Respecto a 5
	if(tablero[5][0] == token && tablero[5][1] == token && tablero[5][2]== token && tablero[5][3]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[5][1] == token && tablero[5][2] == token && tablero[5][3]== token && tablero[5][4]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[5][2] == token && tablero[5][3] == token && tablero[5][4]== token && tablero[5][5]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[5][3] == token && tablero[5][4] == token && tablero[5][5]== token && tablero[5][6]== token){
		SDL_Flip(screen);
		return true;
	}
	//Respecto a 4
	if(tablero[4][0] == token && tablero[4][1] == token && tablero[4][2]== token && tablero[4][3]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[4][1] == token && tablero[4][2] == token && tablero[4][3]== token && tablero[4][4]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[4][2] == token && tablero[4][3] == token && tablero[4][4]== token && tablero[4][5]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[4][3] == token && tablero[4][4] == token && tablero[4][5]== token && tablero[4][6]== token){
		SDL_Flip(screen);
		return true;
	}
	//Respecto a 3
	if(tablero[3][0] == token && tablero[3][1] == token && tablero[3][2]== token && tablero[3][3]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[3][1] == token && tablero[3][2] == token && tablero[3][3]== token && tablero[3][4]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[3][2] == token && tablero[3][3] == token && tablero[3][4]== token && tablero[3][5]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[3][3] == token && tablero[3][4] == token && tablero[3][5]== token && tablero[3][6]== token){
		SDL_Flip(screen);
		return true;
	}
	//Respecto a 2
	if(tablero[2][0] == token && tablero[2][1] == token && tablero[2][2]== token && tablero[2][3]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[2][1] == token && tablero[2][2] == token && tablero[2][3]== token && tablero[2][4]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[2][2] == token && tablero[2][3] == token && tablero[2][4]== token && tablero[2][5]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[2][3] == token && tablero[2][4] == token && tablero[2][5]== token && tablero[2][6]== token){
		SDL_Flip(screen);
		return true;
	}
	//Respecto a 1
	if(tablero[1][0] == token && tablero[1][1] == token && tablero[1][2]== token && tablero[1][3]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[1][1] == token && tablero[1][2] == token && tablero[1][3]== token && tablero[1][4]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[1][2] == token && tablero[1][3] == token && tablero[1][4]== token && tablero[1][5]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[1][3] == token && tablero[1][4] == token && tablero[1][5]== token && tablero[1][6]== token){
		SDL_Flip(screen);
		return true;
	}
	//Respecto a 0
	if(tablero[0][0] == token && tablero[0][1] == token && tablero[0][2]== token && tablero[0][3]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[0][1] == token && tablero[0][2] == token && tablero[0][3]== token && tablero[0][4]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[0][2] == token && tablero[0][3] == token && tablero[0][4]== token && tablero[0][5]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[0][3] == token && tablero[0][4] == token && tablero[0][5]== token && tablero[0][6]== token){
		SDL_Flip(screen);
		return true;
	}

	//Diagonales a la Derecha
	//Respecto a 5
	if(tablero[5][0] == token && tablero[4][1] == token && tablero[3][2]== token && tablero[2][3]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[5][1] == token && tablero[4][2] == token && tablero[3][3]== token && tablero[2][4]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[5][2] == token && tablero[4][3] == token && tablero[3][4]== token && tablero[2][5]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[5][3] == token && tablero[4][4] == token && tablero[3][5]== token && tablero[2][6]== token){
		SDL_Flip(screen);
		return true;
	}
	//Respecto a 4
	if(tablero[4][0] == token && tablero[3][1] == token && tablero[2][2]== token && tablero[1][3]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[4][1] == token && tablero[3][2] == token && tablero[2][3]== token && tablero[1][4]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[4][2] == token && tablero[3][3] == token && tablero[2][4]== token && tablero[1][5]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[4][3] == token && tablero[3][4] == token && tablero[2][5]== token && tablero[1][6]== token){
		SDL_Flip(screen);
		return true;
	}
	//Respecto a 3
	if(tablero[3][0] == token && tablero[2][1] == token && tablero[1][2]== token && tablero[0][3]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[3][1] == token && tablero[2][2] == token && tablero[1][3]== token && tablero[0][4]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[3][2] == token && tablero[2][3] == token && tablero[1][4]== token && tablero[0][5]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[3][3] == token && tablero[2][4] == token && tablero[1][5]== token && tablero[0][6]== token){
		SDL_Flip(screen);
		return true;
	}

	//DIAGONALES HACIA LA IZQUIERDA
	if(tablero[0][3] == token && tablero[1][4] == token && tablero[2][5]== token && tablero[3][6]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[4][6] == token && tablero[3][5] == token && tablero[2][4]== token && tablero[1][3]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[5][6] == token && tablero[4][5] == token && tablero[3][4]== token && tablero[2][3]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[5][5] == token && tablero[4][4] == token && tablero[3][3]== token && tablero[2][2]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[5][4] == token && tablero[4][3] == token && tablero[3][2]== token && tablero[2][1]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[5][3] == token && tablero[4][2] == token && tablero[3][1]== token && tablero[2][0]== token){
		SDL_Flip(screen);
		return true;
	}	
	if(tablero[3][5] == token && tablero[2][4] == token && tablero[1][3]== token && tablero[0][2]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[4][5] == token && tablero[3][4] == token && tablero[2][3]== token && tablero[1][2]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[3][4] == token && tablero[2][3] == token && tablero[1][2]== token && tablero[0][1]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[4][4] == token && tablero[3][3] == token && tablero[2][2]== token && tablero[1][1]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[3][3] == token && tablero[2][2] == token && tablero[1][1]== token && tablero[0][0]== token){
		SDL_Flip(screen);
		return true;
	}
	if(tablero[4][3] == token && tablero[3][2] == token && tablero[2][1]== token && tablero[1][0]== token){
		SDL_Flip(screen);
		return true;
	}
	return false;
}

void inicializarTablero(){
	int i,j; 
	for(i=0; i<FIL; i++){
		for(j=0; j<COL; j++){
			tablero[i][j] = ' ';
		}
	}
	return; 
}

void verificarPosicion(int fila, int columna, int x, int y){
	char token;
	if(tablero[fila][columna] == ' ' && tablero[fila+1][columna] != ' '){
		cuentaFichas++;
		//cout << "cuentaFichas lleva: " << cuentaFichas << endl;
		
		if(turno){
			//cout << "j1" << endl;
			token = 'X';
			if(turno){
				agregarImagen(ledBlanco, 533, 40, screen);
				agregarImagen(ledV, 533, 807, screen);
			}

			SDL_Flip(screen);
			tablero[fila][columna] = token;
			agregarImagen(imagenX, x, y, screen);
			
			//Mostrando estado de la matriz
			/*for (int i = 0; i < FIL; i++){
				for (int j = 0; j < COL; j++){
					cout << "[" << tablero[i][j] << "]";
				}
				cout << endl;
			}*/
			if(validarGanador(token)){
				ganador=true;
				jugar=false;
				if(token=='X'){
					agregarImagen(ganadorp1, 520, 0, screen);
					ofstream gana1("Historial.txt",ios::app);
									
							gana1<<"Gana Player 1"<<endl;
					gana1.close();
				}			
			}
			else if(cuentaFichas > 41){
				agregarImagen(empate, 520, 0, screen);
				ofstream empa("Historial.txt",ios::app);	
						empa<<"Empate"<<endl;
					empa.close();	
				jugar=false;						
				ganador = true;
			}
			
			turno = false;
		}
		else{
			//cout << "j2" << endl;
			token = '0';
			if(!turno){
				agregarImagen(ledBlanco, 533, 807, screen);
				agregarImagen(ledA, 533, 40, screen);
			}
			SDL_Flip(screen);	
			tablero[fila][columna] = token;
			agregarImagen(imageny, x, y, screen);
			
			//Mostrando estado de la matriz
			/*for (int i = 0; i < FIL; i++){
				for (int j = 0; j < COL; j++){
					cout << "[" << tablero[i][j] << "]";
				}
				cout << endl;
			}*/
		
			
			if(validarGanador(token)){
				ganador=true;
				jugar=false;
				agregarImagen(ganadorp2, 520, 0, screen);
				ofstream gana2("Historial.txt",ios::app);
									
					gana2<<"Gana Player 2"<<endl;
				gana2.close();
			}
			else if(cuentaFichas > 41){
				//agregarImagen(blanco, 60, 100, screen);
				agregarImagen(empate, 520, 0, screen);				
				ofstream empa1("Historial.txt",ios::app);	
						empa1<<"Empate"<<endl;
					empa1.close();					
				ganador = true;
				jugar=false;
			}
			turno = true;
		}
	}		
	//else{
	//	cout << "ERROR Esa posicion es inaccesible debido a que no hay fichas bajo ella" << endl;
	//}
	return;
}

void jugarTurno(int x, int y){
	//Parametros de VerPos(fila, columna, y, x)
	if(x>144 && x<204 && y>440 && y<490){
		verificarPosicion(c0, 0, 355-a0*72, 131);
		c0--;
		a0++;
	}
	else if(x>234 && x<294 && y>440 && y<490){
		verificarPosicion(c1, 1, 355-a1*72, 227);
		c1--;
		a1++;
	}
	else if(x>320 && x<390 && y>440 && y<490){
		verificarPosicion(c2, 2, 355-a2*72, 322);
		c2--;
		a2++;
	}
	else if(x>400 && x<485 && y>440 && y<490){
		verificarPosicion(c3, 3, 355-a3*72, 416);	
		c3--;
		a3++;
	}
	else if(x>500 && x<570 && y>440 && y<490){
		verificarPosicion(c4, 4, 354-a4*72, 512);
		c4--;
		a4++;
	}
	else if(x>580 && x<640 && y>440 && y<490){
		verificarPosicion(c5, 5, 354-a5*72, 605);
		c5--;
		a5++;
	}
	else if(x>685 && x<742 && y>440 && y<490){
		verificarPosicion(c6, 6, 355-a6*72, 700);
		c6--;
		a6++;
	}	
	return;						
}

void listener(){ 
	while (SDL_WaitEvent (&event)){		
		switch (event.type){
			case SDL_QUIT:
				SDL_FreeSurface (canvas);
				SDL_Quit();
				exit(0);
				break;	
			case SDL_MOUSEBUTTONDOWN:	
				switch (event.button.button){
					case SDL_BUTTON_LEFT:					
						cx = event.button.x;
						cy = event.button.y;
						if(bandera == false){
							if(cx>640 and cx<760 and cy>440 and cy<550){
								agregarImagenPantalla(elegir, 0, 0, screen);
								SDL_Flip (screen);
								elegirFicha();
								agregarImagenPantalla(nombre, 0, 0, screen);									
								leerCad(screen,640,480,225,300,cadena,50,font,BLACK,WHITE,0,1,500);
								ofstream archi("Historial.txt",ios::app);
									archi<<"1.";
									while(cadena[contador]!='\0'){
										archi<<cadena[contador];
										contador++;
									}
									archi<<endl;
								archi.close();
								leerCad(screen,640,480,225,478,cadena1,50,font,BLACK,WHITE,0,1,500);
								ofstream archi1("Historial.txt",ios::app);						
									archi1<<"2.";
									while(cadena1[contador1]!='\0'){
										archi1<<cadena1[contador1];
										contador1++;
									}
									archi1<<endl;							
								archi1.close();
								jugar=true;

								agregarImagenPantalla(fondo, 0, 0, screen);
								agregarImagen(ledA, 533, 40, screen);	
								agregarImagen(play1, 400, 20, screen);		
								agregarImagen(play2, 400, 791, screen);
								bandera = true;
							}
							else if(cx>51 and cx<156 and cy>396 and cy<501){
								//cout<<"Si sirve we"<<endl;
								agregarImagen(consola, 396, 160, screen);							system("clear");
								ifstream fromfile("Historial.txt");

							   	while(getline(fromfile,cade)) {
							      		cout << cade << endl;
							   	}
							   	cout << endl;
							   	fromfile.close();
							}
						}
						else{
							if(jugar==true){
								if(ganador!=true){
									//cout << "Clic en: " << cx << "," << cy << endl;
									jugarTurno(cx, cy);
								}
							}
							else if(cx>770 and cx<890 and cy>515 and cy<595){
								ganador=false;
								//JUGAR DE NUEVO
									SDL_Flip (screen);
									agregarImagenPantalla(fondo, 0, 0, screen);
									agregarImagen(ledA, 533, 40, screen);

									for (int i = 0; i < FIL; i++){
										for (int j = 0; j < COL ; j++){
											tablero[i][j] = ' ';	
										}
									}
		
									jugar=true;
									cuentaFichas=0;
									c0=5, c1=5, c2=5, c3=5, c4=5, c5=5, c6=5;
									a0=0, a1=0, a2=0, a3=0, a4=0, a5=0, a6=0;
							}
							else if(cx>20 and cx<112 and cy>520 and cy<594){
								//VOLVER A AL INICIO
								ganador=false;
								contador=0;
								contador1=0;
								//cout<<"Entra en el else";

									for (int i = 0; i < FIL; i++){
										for (int j = 0; j < COL ; j++){
											tablero[i][j] = ' ';	
										}
									}
		
									jugar=true;
									cuentaFichas=0;
									c0=5, c1=5, c2=5, c3=5, c4=5, c5=5, c6=5;
									a0=0, a1=0, a2=0, a3=0, a4=0, a5=0, a6=0;
								pthread_t h2;
		
								pthread_create(&h2,NULL, tiempo,NULL);
								pthread_join (h2,NULL);

								//cout << endl;
								bandera=false;
							}
						}
						break;

					default:
						break;
				}
			default:
				break;
		} 
	}	
}
void elegirFicha(){
	//cout << "Entra e funcion" << endl;
	elegido = 0;
	while(elegido!=2){
		//cout << "Entra e while" << endl;
		while (SDL_WaitEvent (&event)){		
			switch (event.type){
				case SDL_QUIT:
					SDL_FreeSurface (canvas);
					SDL_Quit();
					exit(0);
					break;	
				case SDL_MOUSEBUTTONDOWN:	
					switch (event.button.button){					
						case SDL_BUTTON_LEFT:					
							cx = event.button.x;
							cy = event.button.y;
							//cout << "1-" << cx << "," << cy << endl;
							if(elegido==0){
								//cout <<"2-" << cx << "," << cy << endl;
								if(cx>67 and cx<269 and cy>254 and cy<372){
									imagenX = imagen1;
									play1 = fichaP1;
									elegido++;
								}
								else if(cx>339 and cx<541 and cy>254 and cy<372){
									imagenX = imagen2;
									play1 = fichaP2;
									elegido++;
								}
								else if(cx>618 and cx<824 and cy>254 and cy<372){
									imagenX = imagen3;
									play1 = fichaP3;
									elegido++;
								}
							}
							else if(elegido==1){
								//cout <<"3-" << cx << "," << cy << endl;
								if(imagenX==imagen1){
								 	if(cx>339 and cx<541 and cy>254 and cy<372){
										imageny = imagen2;
										play2 = fichaP2;
										elegido++;
									}
									else if(cx>618 and cx<824 and cy>254 and cy<372){
										imageny = imagen3;
										play2 = fichaP3;
										elegido++;
									}
								}
								if(imagenX==imagen2){
									if(cx>67 and cx<269 and cy>254 and cy<372){
										imageny = imagen1;
										play2 = fichaP1;
										elegido++;
									}
									else if(cx>618 and cx<824 and cy>254 and cy<372){
										imageny = imagen3;
										play2 = fichaP3;
										elegido++;
									}
								}
								if(imagenX==imagen3){
									if(cx>67 and cx<269 and cy>254 and cy<372){
										imageny = imagen1;
										play2 = fichaP1;
										elegido++;
									}
									else if(cx>339 and cx<541 and cy>254 and cy<372){
										imageny = imagen2;
										play2 = fichaP2;
										elegido++;
									}	
								}
							}						
						break;
					}
				break;
			}
		break;
		}
	}
}

//Funciones de Tiempo

bool timer(){
    //Number to count down from
    int counter = 4;
    //When changed, a second has passed
    int second = (unsigned)time(NULL);
    //If not equal to each other, counter is printed
    int second_timer = second;
	//xyTextoFC(screen,80,400,"tiempo: ",font,BLACK);
	//SDL_Rect rectangulo;
    while (counter > 0) {
        second = (unsigned)time(NULL);     
		while (second != second_timer) {
			switch(counter){
				case 4:
					agregarImagenPantalla(ini1, 0, 0, screen);
					break;
				case 3:
					agregarImagenPantalla(ini2, 0, 0, screen);
					break;
				case 2:
					agregarImagenPantalla(ini3, 0, 0, screen);
					break;
				case 1:
					agregarImagenPantalla(ini4, 0, 0, screen);
					agregarImagenPantalla(histo, 396, 51, screen);	
					break;	
			}
		    cout <<counter--<< flush;
		    //New value is assigned to match the current second
		    second_timer = second;
		}
    }	
    //comparamos el contenido de la cadena
	return false;
}

void *tiempo(void *arg){
        timer();
}