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

float mov = 5;
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
    int visivel;
    Cor cor;
} Tiro;

Personagem inimigos[8];
Personagem jogador;


Tiro tirosInimigos[8];
Tiro tirosJogador[10];

int contaTirosJogador = 0;
int contaTirosInimigos = 0;

float dtime;
int pps = 5; //pixels por segundo



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
    srand(time(NULL));
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

        inimigos[in].t += 0.3 * dtime;
        glPushMatrix();
        {
            glTranslatef(inimigos[in].x,inimigos[in].y,0);
            DesenhaPersonagem(inimigos[0]);
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

	glPushMatrix();
        glTranslatef(100, 100, 0);
        DesenhaPersonagem(inimigos[0]);
    glPopMatrix();

	DesenhaJogador();
	DesenhaInimigo(0);


    glLoadIdentity();

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
        case 'a':
            jogador.ang+=5;
            break;
        case 'd':
            jogador.ang-=5;
            break;
        case 'w':
            rang = -((float)jogador.ang*M_PI)/180;
            cout << jogador.ang << endl;
            xr = (mov * sin(rang));
            yr = (mov * cos(rang));
            if((jogador.x + xr > 1150) || (jogador.x + xr < 35))
            {
                cout << "POS AGR SIM " << jogador.x << endl;
            }
            else{
                cout << "POS AGR " << jogador.x << endl;
                jogador.x += xr;
            }

            if((jogador.y + yr > 750) || (jogador.y + yr < 35))
            {
                cout << "POS AGR SIM " << jogador.y << endl;
            }
            else{
                cout << "POS AGR " << jogador.y << endl;
                jogador.y += yr;
            }
            break;
        case 's':
            rang = -((float)jogador.ang*M_PI)/180;
            cout << jogador.ang << endl;
            xr = (mov * sin(rang));
            yr = (mov * cos(rang));
            if((jogador.x - xr > 1200) || (jogador.x - xr < 0))
            {
                cout << "POS AGR SIM " << jogador.x << endl;
            }
            else{
                cout << "POS AGR " << jogador.x << endl;
                jogador.x -= xr;
            }

            if((jogador.y - yr > 800) || (jogador.y - yr < 0))
            {
                cout << "POS AGR SIM " << jogador.y << endl;
            }
            else{
                cout << "POS AGR " << jogador.y << endl;
                jogador.y -= yr;
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
        cout << lixo << endl;
        int nCores;
        Cor *cores;
        arquivo >> nCores;
        cout << nCores << endl;
        cores = new Cor[nCores];
        for(j = 0; j < nCores; j++)
        {
            arquivo >> cores[j].id;
            cout << cores[j].id << endl;
            arquivo >> cores[j].r;
            cout << cores[j].r << endl;
            arquivo >> cores[j].g;
            cout << cores[j].g << endl;
            arquivo >> cores[j].b;
            cout << cores[j].b << endl;
        }
        pers.cores = cores;
        getline(arquivo, lixo);
        getline(arquivo, lixo);
        cout << lixo << endl;
        int altura;
        int largura;
        int desenho[8][12];
        arquivo >> altura;
        cout << altura << endl;
        arquivo >> largura;
        cout << largura << endl;
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
    //int r;
	// Define a cor do fundo da tela (AZUL)
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    CarregaPersonagens();
    jogador.x = 600;
    jogador.y = 400;
    inimigos[0].x = 200;
    inimigos[0].y = 300;
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
