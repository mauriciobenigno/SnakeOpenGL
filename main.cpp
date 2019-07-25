#include <GL/glut.h>
#include <GL/freeglut.h>
#include <math.h>
#include "windows.h"
#include <iostream> 
#include <utility>
#include <stdlib.h>   
#include <time.h>
#include <list>
#include <sstream>

using namespace std;

// Variaveis do jogo
bool bateu=false;
bool comeu=false;
int pontuacao=0;
int tamanho=3;
int direcao=1; // ESQUERDA 0, DIREITA 1, CIMA 2 E BAIXO 3
#define ESQUERDA 0
#define DIREITA 1
#define CIMA 2
#define BAIXO 3
#define FIM 999
float velocidade = 1;
int contVelocidade=0;
list < pair<int,int> > corpo;
pair <int,int> bonus;

void Escrever(float x, float y, void *font, const char* string, float R,float G, float B )
{  
	glColor3f(R,G,B); 
	glRasterPos2f(x, y);
	const unsigned char* t = reinterpret_cast<const unsigned char *>(string);
	glutBitmapString(font, t);
}

// Desenha na tela
void Desenhar(void)
{
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClear(GL_COLOR_BUFFER_BIT);

	// Fundo do campo - REGRA 2
	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_QUADS);
		glVertex2i(3,235);
	    glVertex2i(3,3);
	    glVertex2i(325,3);
	    glVertex2i(325,235);
	glEnd();
	
	glPushMatrix();
		// Pontuação
		string texto2 = "Pontuacao: ";
		stringstream stream;
		stream << pontuacao;
		texto2 += stream.str();
		char texto3[texto2.length() + 1]; 
    	strcpy(texto3, texto2.c_str()); 
		Escrever(20.0f, 240.0f, GLUT_BITMAP_HELVETICA_12, texto3, 1.0f, 1.0f, 1.0f);
		// Velocidade
		string texto4 = "Velocidade: ";
		stringstream stream2;
		stream2 << velocidade;
		texto4 += stream2.str();
		char texto5[texto4.length() + 1]; 
    	strcpy(texto5, texto4.c_str()); 
		Escrever(60.0f, 240.0f, GLUT_BITMAP_HELVETICA_12, texto5, 1.0f, 1.0f, 1.0f);
		
	glPopMatrix();
	
	
	if(bateu==true)
	{
		// TELA DE GAME OVER
		glColor3f(0.0f, 0.0f, 0.0f);
		glBegin(GL_QUADS);
		glVertex2i(50,225);
	    glVertex2i(50,50);
	    glVertex2i(275,50);
	    glVertex2i(275,225);
		glEnd();
		
		direcao=FIM;
		
		glColor3f(1.0, 1.0, 1.0); 
		glRasterPos2f(130, 180);	
		const unsigned char* texto = reinterpret_cast<const unsigned char *> ("GAME OVER");
		glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24,texto);
		string texto2 = "Pontuacao: ";
		stringstream stream;
		stream << pontuacao;
		texto2 += stream.str();
		char texto3[texto2.length() + 1]; 
    	strcpy(texto3, texto2.c_str()); 
    	
		Escrever(130.0f, 120.0f, GLUT_BITMAP_TIMES_ROMAN_24, texto3, 1.0f, 1.0f, 1.0f);
	}
	else{
		// Bonus
		glColor3f(0.0f, 1.0f, 0.0f);
		glBegin(GL_QUADS);
			glVertex2i(bonus.first ,bonus.second+10);
			glVertex2i(bonus.first ,bonus.second);
			glVertex2i(bonus.first+10,bonus.second);
			glVertex2i(bonus.first+10,bonus.second+10);
		glEnd();
		for (list< pair<int,int> >::iterator it=corpo.begin(); it != corpo.end(); ++it)
		{
			glColor3f(1.0f, 0.0f, 0.0f);
			glBegin(GL_QUADS);
				pair<int,int> aux = *it;
				glVertex2i(aux.first ,aux.second+10);
				glVertex2i(aux.first ,aux.second);
				glVertex2i(aux.first+10,aux.second);
				glVertex2i(aux.first+10,aux.second+10);
			glEnd();
		}
	}		
    glutSwapBuffers();
}

void NovoBonus(){
	int posX,posY;
	srand (time(NULL));
	posX = rand() % 26 + 1;
	posY = rand() % 20 + 1;
	int cont=0;
	bool pode = true;
	for (list< pair<int,int> >::iterator it=corpo.begin(); it != corpo.end(); ++it)
	{
		if(cont>0)
		{
			pair<int,int> aux = *it;
			if(posX==aux.first&&posY==aux.second)
			{
				pode=false;
				break;
			}
		}
		cont++;
	}
	if(pode==false)
	{
		NovoBonus();	
	}
	else{
		bonus.first=posX*11;
		bonus.second=posY*11;
	}	
}

void VerificaColisao()
{
	// Verifica se comeu um bonus
	if(corpo.front().first==bonus.first&&corpo.front().second==bonus.second)
	{
		comeu=true;
		pontuacao++;
		tamanho++;
		contVelocidade++;
		if(contVelocidade==5)
		{
			velocidade++;
			contVelocidade=0;
		}
		NovoBonus();
	}
	// Verifica se bateu na propria cauda - REGRA 4
	int cont=0;
	for (list< pair<int,int> >::iterator it=corpo.begin(); it != corpo.end(); ++it)
	{
		if(cont>0)
		{
			pair<int,int> aux = *it;
			if(corpo.front().first==aux.first&&corpo.front().second==aux.second)
			{
				bateu=true;
				break;
			}
		}
		cont++;
	}
	// Verifica se bateu na parede - REGRA 4
	switch(direcao)
	{
		case CIMA: // frente
			if(corpo.front().second>=230)
				bateu=true;
    		break;
		case BAIXO: // tras
		  	if(corpo.front().second<=10)
				bateu=true;
		    break;
		case ESQUERDA: // esquera
     		if(corpo.front().first<=10)
				bateu=true;
    		break;
		case DIREITA: // direita
     	 	 if(corpo.front().first>=310)
				bateu=true;
			break;
	}
}

void Controle(unsigned char key, int a, int b)
{
	bool direcaoInvalida=false;
    switch (key)
    {			
    	// Direcionais WASD
        case 'w': // frente
  			if(!(corpo.front().second>=230)&&direcao!=BAIXO){
				corpo.push_front(make_pair(corpo.front().first,corpo.front().second+11));
				direcao=CIMA;
			}
			if(direcao==BAIXO)
				direcaoInvalida=true;
							 			
            break;
        case 's': // tras
        	if(!(corpo.front().second<=10)&&direcao!=CIMA){
				corpo.push_front(make_pair(corpo.front().first,corpo.front().second-11));
				direcao=BAIXO;
			}
			if(direcao==CIMA)
				direcaoInvalida=true;
            break;
        case 'a': // esquera
        	if(!(corpo.front().first<=10)&&direcao!=DIREITA){
				corpo.push_front(make_pair(corpo.front().first-11,corpo.front().second));
				direcao=ESQUERDA;
			}
			if(direcao==DIREITA)
				direcaoInvalida=true;
            break;
        case 'd': // direita
        	if(!(corpo.front().first>=310)&&direcao!=ESQUERDA){
				corpo.push_front(make_pair(corpo.front().first+11,corpo.front().second));
				direcao=DIREITA;
			}
			if(direcao==ESQUERDA)
				direcaoInvalida=true;
            break; 
        default:
        	switch(direcao)
			{
				case CIMA: // frente
		  			if(!(corpo.front().second>=230)){
						corpo.push_front(make_pair(corpo.front().first,corpo.front().second+11));
						direcao=CIMA;
					}	
					
		        case BAIXO: // tras
		        	if(!(corpo.front().second<=10)){
						corpo.push_front(make_pair(corpo.front().first,corpo.front().second-11));
						direcao=BAIXO;
					}
		            break;
		        case ESQUERDA: // esquera
		        	if(!(corpo.front().first<=10)){
						corpo.push_front(make_pair(corpo.front().first-11,corpo.front().second));
						direcao=ESQUERDA;
					}
		            break;
		        case DIREITA: // direita
		        	if(!(corpo.front().first>=310)){
						corpo.push_front(make_pair(corpo.front().first+11,corpo.front().second));
						direcao=DIREITA;
					}

				}
        	break;
    }
    VerificaColisao();
    if(comeu==false&&direcaoInvalida==false)
	{
		corpo.pop_back();
	}
	if(comeu==true)
	{
		comeu=false;
	}
    Desenhar();
    Sleep(500/velocidade);
    //glutPostRedisplay();
}

static void 
Espera(void)
{
	switch(direcao)
	{
		case CIMA: // frente
		 	 Controle('w', 99999999, 99999999);
		 	 break;
		case BAIXO: // tras
     		Controle('s', 99999999, 99999999);
		    break;
		case ESQUERDA: // esquera
		   	Controle('a', 99999999, 99999999);
		    break;
    	case DIREITA: // direita
		   	Controle('d', 99999999, 99999999);
 			break;
 		case FIM:
 	 		break;
	}
	//Sleep(500/velocidade);
    //glutPostRedisplay();
}

void Iniciar(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    // Definindo Bonus - REGRA 3
    NovoBonus();
    //INCIIANDO A COBRA COM TAMANHO 3, CORFORME REGRA 1
    corpo.push_front(make_pair(3*11,5*11));
    corpo.push_front(make_pair(4*11,5*11));
    corpo.push_front(make_pair(5*11,5*11));
    //Movimenta();
}

//  Corrige o tamanho da tela caso seja alterada
void CorrigeTela(GLsizei w, GLsizei h)
{
    // Não permitir que a tela seja modificada
    if(w < 800 || w> 800) glutReshapeWindow(800, h);
    if(h < 600 || h> 600) glutReshapeWindow(w, 600);

	// Evita a divisao por zero
	if(h == 0) h = 1;    
	// Especifica as dimensões da Viewport
	glViewport(0, 0, w, h);
	// Inicializa o sistema de coordenadas
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// Estabelece a janela de seleção (left, right, bottom, top)
	if (w <= h) 
        gluOrtho2D (0.0f, 250.0f, 0.0f, 250.0f*h/w);
   else 
		gluOrtho2D (0.0f, 250.0f*w/h, 0.0f, 250.0f);

}


// Programa Principal
int main(int argc, char*argv[])
{
    glutInit (&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(150, 50);
    glutCreateWindow("Cobrinha");
    glutDisplayFunc(Desenhar);
    glutReshapeFunc(CorrigeTela);
    glutKeyboardFunc(Controle);
    glutIdleFunc(Espera);
    Iniciar();
    glutMainLoop();
}
