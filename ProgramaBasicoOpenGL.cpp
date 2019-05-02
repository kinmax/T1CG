// **********************************************************************
// PUCRS/Escola PolitŽcnica
// COMPUTA‚ÌO GRçFICA
//
// Programa basico para criar aplicacoes 2D em OpenGL
//
// Marcio Sarroglia Pinho
// pinho@pucrs.br
// **********************************************************************


// Para uso no Xcode:
// Abra o menu Product -> Scheme -> Edit Scheme -> Use custom working directory
// Selecione a pasta onde voce descompactou o ZIP que continha este arquivo.
//

#include <iostream>
#include <cmath>
#include <ctime>
#include <fstream>
#include <string>
#include <cstdlib>
using namespace std;

#ifdef WIN32
#include <windows.h>
#include <glut.h>
static DWORD last_idle_time;
#else
#include <sys/time.h>
static struct timeval last_idle_time;
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif

#ifdef __linux__
#include <glut.h>
#endif

ifstream arquivo;

float mov = 10;
float posx = 0;
float posy = 0;

typedef struct Ponto
{
    float x, y;
} Ponto;

typedef struct Cor
{
    int r, g, b, id;
} Cor;

typedef struct Personagem
{
    float x_max, y_max, x_min, y_min;
    float x;
    float y;
    float ang;
    int altura;
    int largura;
    int desenho[8][12];
    int visivel;
    Cor *cores;
    float t;
    Ponto p0, p1, p2;
} Personagem;

typedef struct Tiro
{
    float x;
    float y;
    float xr, yr;
    float x_min, x_max, y_min, y_max;
    float ang;
    bool visivel;
    Cor cor;
    bool inimigo;
    float movim;
} Tiro;

Personagem inimigos[8];
Personagem jogador;


Tiro tirosInimigos[8];
Tiro tirosJogador[10];

int contaTirosJogador = 0;
int contaTirosVaziosJogador = 9;
int contaTirosInimigos = 0;

float dtime;
int pps = 5; //pixels por segundo

int vidas = 3;



// **********************************************************************
//  void animate ( unsigned char key, int x, int y )
//
//
// **********************************************************************
void animate()
{
    float dt;
    static float AccumTime=0;

#ifdef _WIN32
    DWORD time_now;
    time_now = GetTickCount();
    dt = (float) (time_now - last_idle_time) / 1000.0;
    dtime = dt;
#else
    // Figure out time elapsed since last call to idle function
    struct timeval time_now;
    gettimeofday(&time_now, NULL);
    dt = (float)(time_now.tv_sec  - last_idle_time.tv_sec) +
    1.0e-6*(time_now.tv_usec - last_idle_time.tv_usec);
    dtime = dt
#endif
    AccumTime +=dt;
    if (AccumTime >=3) // imprime o FPS a cada 3 segundos
    {
        cout << 1.0/dt << " FPS"<< endl;
        AccumTime = 0;
    }
    // cout << "AccumTime: " << AccumTime << endl;
    // Anima cubos
    //AngY++;
    // Salva o tempo para o pr—ximo ciclo de rendering
    last_idle_time = time_now;

    //if  (GetAsyncKeyState(32) & 0x8000) != 0)
    //  cout << "Espaco Pressionado" << endl;

    // Redesenha
    glutPostRedisplay();
}

bool colisao(Personagem p, Tiro t)
{
    float x1_min, x1_max, y1_min, y1_max, x2_min, x2_max, y2_min, y2_max;
    x1_min = p.x_min;
    x1_max = p.x_max;
    y1_min = p.y_min;
    y1_min = p.y_max;

    x2_min = t.x - 3.5f;
    x2_max = t.x + 3.5f;
    y2_min = t.y - 3.5f;
    y2_min = t.y + 3.5f;


    if(((x1_max > x2_min && x1_max < x2_max) || (x1_min > x2_min && x1_min < x2_max)) && ((y1_max > y2_min && y1_max < y2_max) || (y1_min > y2_min && y1_min < y2_max)))
    {
        return true;
        cout << "true" << endl;
    }
    return false;
}

bool colisao(Personagem p1, Personagem p2)
{
    float x1_min, x1_max, y1_min, y1_max, x2_min, x2_max, y2_min, y2_max;
    x1_min = p1.x_min;
    x1_max = p1.x_max;
    y1_min = p1.y_min;
    y1_min = p1.y_max;

    x2_min = p2.x_min;
    x2_max = p2.x_max;
    y2_min = p2.y_min;
    y2_min = p2.y_max;

    if(((x1_max > x2_min && x1_max < x2_max) || (x1_min > x2_min && x1_min < x2_max)) && ((y1_max > y2_min && y1_max < y2_max) || (y1_min > y2_min && y1_min < y2_max)))
    {
        return true;
    }
    return false;
}

bool colisao(float x1, float y1, float x2, float y2)
{
    float x1_min, x1_max, y1_min, y1_max, x2_min, x2_max, y2_min, y2_max;
    x1_min = x1-25.0f;
    x1_max = x1+34.0f;
    y1_min = y1;
    y1_min = y1+39.0f;

    x2_min = x2-25.0f;
    x2_max = x2+34.0f;
    y2_min = y2;
    y2_min = y2+39.0f;

    if(((x1_max > x2_min && x1_max < x2_max) || (x1_min > x2_min && x1_min < x2_max)) && ((y1_max > y2_min && y1_max < y2_max) || (y1_min > y2_min && y1_min < y2_max)))
    {
        return true;
    }
    return false;
}
// **********************************************************************
//  void init(void)
//  Inicializa os parâmetros globais de OpenGL
//
// **********************************************************************



// **********************************************************************
//  void reshape( int w, int h )
//  trata o redimensionamento da janela OpenGL
//
// **********************************************************************
void reshape( int w, int h )
{
    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Define a area a ser ocupada pela área OpenGL dentro da Janela
    glViewport(0, 0, w, h);

    // Define os limites lógicos da área OpenGL dentro da Janela
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glOrtho(0,10,0,10,0,1);

}

void DesenhaTiroJogador(int i)
{

    if(tirosJogador[i].visivel)
    {
        float xr, yr, rang;
        rang = -((float)tirosJogador[i].ang*M_PI)/180;
        glPushMatrix();
        {
            glColor3f(tirosJogador[i].cor.r, tirosJogador[i].cor.g, tirosJogador[i].cor.b);
            glTranslatef(tirosJogador[i].x, tirosJogador[i].y, 0);
            glRotated(tirosJogador[i].ang,0,0,1);
            glTranslated(0,tirosJogador[i].movim,0);
            xr = (tirosJogador[i].movim * sin(rang));
            yr = (tirosJogador[i].movim * cos(rang));
            cout << contaTirosJogador << endl;
            tirosJogador[i].x += xr;
            tirosJogador[i].y += yr;
            glPointSize(7);
            glBegin(GL_POINTS);
                glVertex2f(0,0);
            glEnd();
        }
        glPopMatrix();
        if(tirosJogador[i].x >= 1200.0f || tirosJogador[i].x <= 0.0f || tirosJogador[i].y >= 800.0f || tirosJogador[i].y <= 0.0f)
        {
            cout << "oi" << endl;
            tirosJogador[i].visivel = false;
            contaTirosJogador--;
        }
        int j;
        for(j = 0; j < 8; j++)
        {
            if(inimigos[j].visivel == 1)
            {
                if(colisao(inimigos[j], tirosJogador[i]))
                {
                    inimigos[j].visivel = 0;
                    tirosJogador[i].visivel = false;
                }
            }

        }


    }

}

void DesenhaPersonagem(Personagem p)
{
    for(int i = 0; i < p.altura; i++)
    {
        for(int j = 0; j < p.largura; j++)
        {
            int cor;
            cor = p.desenho[i][j];
            glColor3f(p.cores[cor-1].r, p.cores[cor-1].g, p.cores[cor-1].b);

            int x = ((j+1 - p.largura/2) *5);
            int y = ((p.altura - i-1) * 5);

            glPointSize(5);
            glBegin(GL_POINTS);
                glVertex2f(x, y);
            glEnd();
        }
    }
}

void DesenhaJogador()
{
    if(jogador.visivel)
    {
        glPushMatrix();
        {
            glTranslatef(jogador.x,jogador.y,0);
            glRotatef(jogador.ang,0,0,1);
            DesenhaPersonagem(jogador);
        }
        glPopMatrix();
    }
}

void DesenhaInimigo(int in)
{
    float t;
    Ponto p0, p1, p2;
    if(inimigos[in].visivel)
    {
        t = inimigos[in].t;
        if(t <= 1.0f)
        {
            p0 = inimigos[in].p0;
            p1 = inimigos[in].p1;
            p2 = inimigos[in].p2;
        }
        else
        {
            inimigos[in].t = 0.0f;
            p0.x = inimigos[in].x;
            p0.y = inimigos[in].y;
            p1.x = rand() % 1200;
            p1.y = rand() % 800;
            p2.x = rand() % 1200;
            p2.y = rand() % 800;
            inimigos[in].p0 = p0;
            inimigos[in].p1 = p1;
            inimigos[in].p2 = p2;
        }

        inimigos[in].x = (pow((1-t), 2) * p0.x) + (2 * (1-t)*t*p1.x) + (pow(t,2)*p2.x);
        inimigos[in].y = (pow((1-t), 2) * p0.y) + (2 * (1-t)*t*p1.y) + (pow(t,2)*p2.y);
        inimigos[in].x_max = inimigos[in].x + 34.0f;
        inimigos[in].x_min = inimigos[in].x - 25.0f;
        inimigos[in].y_max = inimigos[in].y + 39.0f;
        inimigos[in].y_min = inimigos[in].y;

        inimigos[in].t += 0.2 * dtime;
        glPushMatrix();
        {
            glTranslatef(inimigos[in].x,inimigos[in].y,0);
            DesenhaPersonagem(inimigos[in]);
        }
        glPopMatrix();
    }
}


// **********************************************************************
//  void display( void )
//
// **********************************************************************
void display( void )
{
    int i;

	// Limpa a tela coma cor de fundo
	glClear(GL_COLOR_BUFFER_BIT);

    // Define os limites lógicos da área OpenGL dentro da Janela
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glOrtho(0,1200,0,800,0,1);//0 a 10 em x, 0 a 10 em y e z 0 a 1 (desconsiderado)
    //glColor3f(1,1,1);
    //DesenhaEixos();
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	// Coloque aqui as chamadas das rotinas que desenha os objetos
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	glLineWidth(3);
	glColor3f(1,0,0);

	srand(time(NULL));

	DesenhaJogador();
	DesenhaInimigo(0);
	DesenhaInimigo(1);
	DesenhaInimigo(2);
	DesenhaInimigo(3);
	DesenhaInimigo(4);
	DesenhaInimigo(5);
	DesenhaInimigo(6);
	DesenhaInimigo(7);

	for(i = 0; i < 10; i++)
    {
        if(tirosJogador[i].visivel)
        {
            DesenhaTiroJogador(i);
        }
    }


    //glLoadIdentity();

        /*glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    glTranslatef((currentX + (width * .5)), (currentY + (height * .5)), 0);
    glRotatef(currentAngleOfRotation, 0.0, 0.0, 1.0);
    glTranslatef(-(currentX + (width * .5)), -(currentY + (height * .5)), 0);
    drawRect(currentX, currentY, width, height);
    glPopMatrix();
    glFlush();*/

	/*glLineWidth(3);
	glColor3f(0,1,0);
	glBegin(GL_LINES);
	  glVertex2f(5,5);
	  glVertex2f(10,0);
	glEnd();*/

	glutSwapBuffers();
}


// **********************************************************************
//  void keyboard ( unsigned char key, int x, int y )
//
// **********************************************************************

void keyboard ( unsigned char key, int x, int y )
{
    float xr, yr, rang;
	switch ( key )
	{
		case 27:        // Termina o programa qdo
			exit ( 0 );   // a tecla ESC for pressionada
			break;
        case ' ':
            if(contaTirosJogador < 10)
            {
                tirosJogador[contaTirosJogador].inimigo = false;
                tirosJogador[contaTirosJogador].visivel = true;
                tirosJogador[contaTirosJogador].x = jogador.x;
                tirosJogador[contaTirosJogador].y = jogador.y;
                tirosJogador[contaTirosJogador].ang = jogador.ang;
                tirosJogador[contaTirosJogador].cor.id = 3;
                tirosJogador[contaTirosJogador].cor.r = 0;
                tirosJogador[contaTirosJogador].cor.g = 255;
                tirosJogador[contaTirosJogador].cor.b = 0;
                tirosJogador[contaTirosJogador].movim = 1;
                DesenhaTiroJogador(contaTirosJogador);
                contaTirosJogador++;

            }
            break;
        case 'a':
            jogador.ang+=10;
            break;
        case 'd':
            jogador.ang-=10;
            break;
        case 'w':
            rang = -((float)jogador.ang*M_PI)/180;
            xr = (mov * sin(rang));
            yr = (mov * cos(rang));
            if(jogador.x + xr > (1200 - jogador.altura*5) || (jogador.x + xr < jogador.altura*5))
            {
            }
            else{
                jogador.x += xr;
                jogador.x_max += xr;
                jogador.x_min += xr;
            }

            if((jogador.y + yr > (800 - jogador.altura*5)) || (jogador.y + yr < jogador.altura*5))
            {
            }
            else{
                jogador.y += yr;
                jogador.y_max += yr;
                jogador.y_min += yr;
            }
            break;
        case 's':
            rang = -((float)jogador.ang*M_PI)/180;
            xr = (mov * sin(rang));
            yr = (mov * cos(rang));
            if((jogador.x - xr > (1200 - jogador.altura*5)) || (jogador.x - xr < jogador.altura*5))
            {
            }
            else{
                jogador.x -= xr;
                jogador.x_max -= xr;
                jogador.x_min -= xr;
            }

            if((jogador.y - yr > (800-jogador.altura*5)) || (jogador.y - yr < jogador.altura*5))
            {
            }
            else{
                jogador.y -= yr;
                jogador.y_max -= yr;
                jogador.y_min -= yr;
            }
            break;
		default:
			break;
	}
}


// **********************************************************************
//  void arrow_keys ( int a_keys, int x, int y )
//
//
// **********************************************************************
void arrow_keys ( int a_keys, int x, int y )
{
	switch ( a_keys )
	{
		case GLUT_KEY_UP:       // Se pressionar UP
			glutFullScreen ( ); // Vai para Full Screen
			break;
	    case GLUT_KEY_DOWN:     // Se pressionar UP
								// Reposiciona a janela
            glutPositionWindow (50,50);
			glutReshapeWindow ( 700, 500 );
			break;
		default:
			break;
	}
}

void CarregaPersonagens()
{
    const char* files[] = {"jogador.txt", "inimigo1.txt", "inimigo2.txt", "inimigo3.txt", "inimigo4.txt"};
    Personagem pers;
    int i, j, aux;
    string lixo;
    for(i = 0; i < 5; i++)
    {
        arquivo.open(files[i]);
        if(!arquivo)
        {
            cout << "Erro ao abrir arquivo " << files[i] << endl;
            exit(1);
        }else
        {
            cout << "Arquivo " << files[i] << " aberto com sucesso" << endl;
        }

        getline(arquivo, lixo);
        int nCores;
        Cor *cores;
        arquivo >> nCores;
        cores = new Cor[nCores];
        for(j = 0; j < nCores; j++)
        {
            arquivo >> cores[j].id;
            arquivo >> cores[j].r;
            arquivo >> cores[j].g;
            arquivo >> cores[j].b;
        }
        pers.cores = cores;
        getline(arquivo, lixo);
        getline(arquivo, lixo);
        int altura;
        int largura;
        int desenho[8][12];
        arquivo >> altura;
        arquivo >> largura;
        pers.altura = altura;
        pers.largura = largura;
        for(int i = 0; i < altura; i++)
        {
            for(int j = 0; j < largura; j++)
            {
                arquivo >> pers.desenho[i][j];
            }
        }
        pers.x = 0;
        pers.y = 0;
        pers.visivel = 1;
        pers.ang = 0;
        pers.t = 2.0f;
        Ponto p0, p1, p2;
        p0.x = p1.x = p2.x = 0.0f;
        p0.x = p1.x = p2.x = 0.0f;
        pers.p0 = p0;
        pers.p1 = p1;
        pers.p2 = p2;
        pers.x_min = pers.x - 25.0f;
        pers.x_max = pers.x + 34.0f;
        pers.y_min = pers.y;
        pers.y_max = pers.y + 39.0f;
        arquivo.close();

        if(i == 0)
        {
            jogador = pers;
        }
        else if(i == 1)
        {
            inimigos[0] = inimigos[1] = pers;
        }
        else if(i == 2)
        {
            inimigos[2] = inimigos[3] = pers;
        }
        else if(i == 3)
        {
            inimigos[4] = inimigos[5] = pers;
        }
        else
        {
            inimigos[6] = inimigos[7] = pers;
        }
    }
}



void init(void)
{
    srand(time(NULL));
    //int r;
	// Define a cor do fundo da tela (AZUL)
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    CarregaPersonagens();
    float numeros_x[9];
    float numeros_y[9];
    int i;
    for(i = 0; i < 9; i++)
    {
        numeros_x[i] = -1.0f;
        numeros_y[i] = -1.0f;
    }
    numeros_x[0] = 600;
    numeros_y[0] = 400;
    bool deu = false;
    int x, y;
    int cont = 1;
    int cont2 = 0;
    while(deu == false && cont < 9)
    {
        x = float(rand() % 1150);
        y = float(rand() % 750);
        cont2 = 0;
        for(i = 0; i < 9; i++)
        {
            if(colisao(x,y,numeros_x[i],numeros_y[i]) == false)
            {
                cont2++;
            }
        }
        if(cont2 == 9)
        {
            numeros_x[cont] = x;
            numeros_y[cont] = y;
            cont++;
        }
        if(cont == 9)
        {
            deu = true;
        }
    }
    jogador.x = numeros_x[0];
    jogador.y = numeros_y[0];
    jogador.x_max = numeros_x[0]+34.0f;
    jogador.x_min = numeros_x[0]-25.0f;
    jogador.y_max = numeros_y[0]+39.0f;
    jogador.y_min = numeros_y[0];

    inimigos[0].x = numeros_x[1];
    inimigos[0].y = numeros_y[1];
    inimigos[0].x_max = numeros_x[1]+34.0f;
    inimigos[0].x_min = numeros_x[1]-25.0f;
    inimigos[0].y_max = numeros_y[1]+39.0f;
    inimigos[0].y_min = numeros_y[1];

    inimigos[1].x = numeros_x[2];
    inimigos[1].y = numeros_y[2];
    inimigos[1].x_max = numeros_x[2]+34.0f;
    inimigos[1].x_min = numeros_x[2]-25.0f;
    inimigos[1].y_max = numeros_y[2]+39.0f;
    inimigos[1].y_min = numeros_y[2];

    inimigos[2].x = numeros_x[3];
    inimigos[2].y = numeros_y[3];
    inimigos[2].x_max = numeros_x[3]+34.0f;
    inimigos[2].x_min = numeros_x[3]-25.0f;
    inimigos[2].y_max = numeros_y[3]+39.0f;
    inimigos[2].y_min = numeros_y[3];

    inimigos[3].x = numeros_x[4];
    inimigos[3].y = numeros_y[4];
    inimigos[3].x_max = numeros_x[4]+34.0f;
    inimigos[3].x_min = numeros_x[4]-25.0f;
    inimigos[3].y_max = numeros_y[4]+39.0f;
    inimigos[3].y_min = numeros_y[4];

    inimigos[4].x = numeros_x[5];
    inimigos[4].y = numeros_y[5];
    inimigos[4].x_max = numeros_x[5]+34.0f;
    inimigos[4].x_min = numeros_x[5]-25.0f;
    inimigos[4].y_max = numeros_y[5]+39.0f;
    inimigos[4].y_min = numeros_y[5];

    inimigos[5].x = numeros_x[6];
    inimigos[5].y = numeros_y[6];
    inimigos[5].x_max = numeros_x[6]+34.0f;
    inimigos[5].x_min = numeros_x[6]-25.0f;
    inimigos[5].y_max = numeros_y[6]+39.0f;
    inimigos[5].y_min = numeros_y[6];

    inimigos[6].x = numeros_x[7];
    inimigos[6].y = numeros_y[7];
    inimigos[6].x_max = numeros_x[7]+34.0f;
    inimigos[6].x_min = numeros_x[7]-25.0f;
    inimigos[6].y_max = numeros_y[7]+39.0f;
    inimigos[6].y_min = numeros_y[7];

    inimigos[7].x = numeros_x[8];
    inimigos[7].y = numeros_y[8];
    inimigos[7].x_max = numeros_x[8]+34.0f;
    inimigos[7].x_min = numeros_x[8]-25.0f;
    inimigos[7].y_max = numeros_y[8]+39.0f;
    inimigos[7].y_min = numeros_y[8];
    //r = LoadTXT (name.c_str());

    //if (!r) exit(1); // Erro na carga da imagem
    //else cout << ("Imagem carregada!\n");
}
// **********************************************************************
//  void main ( int argc, char** argv )
//
//
// **********************************************************************
int  main ( int argc, char** argv )
{
    glutInit            ( &argc, argv );
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB );
    glutInitWindowPosition (0,0);

    // Define o tamanho inicial da janela grafica do programa
    glutInitWindowSize  ( 650, 500);

    // Cria a janela na tela, definindo o nome da
    // que aparecera na barra de título da janela.
    glutCreateWindow    ( "Primeiro Programa em OpenGL" );

    // executa algumas inicializações
    init();

    // Define que o tratador de evento para
    // o redesenho da tela. A funcao "display"
    // será chamada automaticamente quando
    // for necessário redesenhar a janela
    glutDisplayFunc ( display );
    glutIdleFunc(animate);

    // Define que o tratador de evento para
    // o redimensionamento da janela. A funcao "reshape"
    // será chamada automaticamente quando
    // o usuário alterar o tamanho da janela
    glutReshapeFunc ( reshape );

    // Define que o tratador de evento para
    // as teclas. A funcao "keyboard"
    // será chamada automaticamente sempre
    // o usuário pressionar uma tecla comum
    glutKeyboardFunc ( keyboard );

    // Define que o tratador de evento para
    // as teclas especiais(F1, F2,... ALT-A,
    // ALT-B, Teclas de Seta, ...).
    // A funcao "arrow_keys" será chamada
    // automaticamente sempre o usuário
    // pressionar uma tecla especial
    glutSpecialFunc ( arrow_keys );

    // inicia o tratamento dos eventos
    glutMainLoop ( );

    return 0;
}
