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

ifstream inFile;

float ang = 0;
float mov = 0;
float posx = 0;
float posy = 0;



typedef struct{
    float x, y;
    int height,width;
    int form[20][20];
    int visible;
    float move;
}Object;

Object enemy1;

// **********************************************************************
//  void animate ( unsigned char key, int x, int y )
//
//
// **********************************************************************
void animate()
{
    static float dt;
    static float AccumTime=0;

#ifdef _WIN32
    DWORD time_now;
    time_now = GetTickCount();
    dt = (float) (time_now - last_idle_time) / 1000.0;
#else
    // Figure out time elapsed since last call to idle function
    struct timeval time_now;
    gettimeofday(&time_now, NULL);
    dt = (float)(time_now.tv_sec  - last_idle_time.tv_sec) +
    1.0e-6*(time_now.tv_usec - last_idle_time.tv_usec);
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


void DesenhaTriangulo()
{
    glBegin(GL_TRIANGLES);
	    glVertex2f(0,0);
        glVertex2f(0.5,1);
        glVertex2f(1,0);
	glEnd();
}

void DesenhaInimigo()
{
    glBegin(GL_TRIANGLES);
        glVertex2f(0,2);
        glVertex2f(2,2);
        glVertex2f(0,4);
    glEnd();
}

void DesenhaEixos()
{
    glBegin(GL_LINES);
        glVertex2f(0,5);
        glVertex2f(10,5);
        glVertex2f(5,0);
        glVertex2f(5,10);
    glEnd();
}

void MakeImage(Object o){
    int altura = o.height;
    int largura = o.width;
    for(int i = 0; i < altura; i++){ // I e a linha
        for(int j = 0; j < largura; j++){ // J e a coluna
            int color;
            color = o.form[i][j];
            if(color == 1)
                glColor3f(0, 0, 0);
            else if(color == 2)
                glColor3f(255,0,0);
            else if(color == 3)
                glColor3f(0,255,0);
            else if(color == 4)
                glColor3f(255,255,255);

            int x = ((j+1 - largura/2) *7);
            int y = ((altura - i-1) * 7);

            glPointSize(7);
            glBegin(GL_POINTS);
                glVertex2f(x, y);
            glEnd();
        }
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
        glTranslatef(500, 500, 0);
        MakeImage(enemy1);
    glPopMatrix();

	glPushMatrix();
	{
        //glTranslatef(-0.5,-0.5,0);
        glRotatef(ang, 0,0,1);
        glTranslatef(0,mov,0);
        glTranslatef(-0.5,-0.5,0);
        //glRotated(-ang,0,0,1);
        DesenhaTriangulo();
	}
    glPopMatrix();


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

	switch ( key )
	{
		case 27:        // Termina o programa qdo
			exit ( 0 );   // a tecla ESC for pressionada
			break;
        case 'a':
            ang+=10;
            break;
        case 'd':
            ang-=10;
            break;
        case 'w':
            mov+=0.3;
            posx+=0.3;
            posy+=0.3;
            break;
        case 's':
            mov-=0.3;
            posx-=0.3;
            posy-=0.3;
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

void LoadImages()
{
    inFile.open("enemy02.txt");
    if(!inFile){
        cout << "Não consegui abrir o arquivo do inimigo 01" << endl;
        exit(1);
    }else{
        cout << "Arquivo do inimigo 01 aberto" << endl;
    }
    int altura;
    int largura;
    int form[20][20];
    inFile >> altura >> largura;
    enemy1.height= altura;
    enemy1.width= largura;
    cout<<altura<<endl;
    cout<<largura<<endl;
    for(int i = 0; i < altura; i++){ // I e a linha
        for(int j = 0; j < largura; j++){ // J e a coluna
            inFile >> enemy1.form[i][j];
        }
    }
    //objects[ENEMY01] = enemy01;
    //MakeImage(enemy1);
    inFile.close();
}

void init(void)
{
    //int r;
	// Define a cor do fundo da tela (AZUL)
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    LoadImages();
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
