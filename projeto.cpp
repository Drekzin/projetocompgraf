#include <cmath>
#include <GL/glut.h>
#include <GL/glu.h>
#include <cstdlib>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Variáveis para controle das articulações
GLfloat ombro = 90.0f, cotovelo = 0.0f, mao = 0.0f;
GLuint texturacabo, texturaesfera, texturabase, texturacabeca, texturachao, texturaparede,texturateto,texturabase2;

// Variáveis para controlar a direção do movimento
int direcaoOmbro = 1; // 1 para aumentar, -1 para diminuir
int direcaoCotovelo = 1;
int direcaoMao = 1;


// Variáveis da câmera
float anguloX = 0.0f, anguloY = 0.0f;
float distancia = 10.0f;
int ultimaPosX = -1, ultimaPosY = -1;
bool botaoEsquerdoPressionado = false;
bool animacaoAtiva = false;
void atualiza(int valor);
void configurarLuz();

GLfloat intensidadeLuz = 1.0f;

// Função para carregar textura
void carregarTextura(const char* nomeArquivo, GLuint& idTextura) {
    int largura, altura, canais;
    unsigned char* dados = stbi_load(nomeArquivo, &largura, &altura, &canais, 0);
    if (!dados) {
        fprintf(stderr, "Erro ao carregar textura %s\n", nomeArquivo);
        exit(1);
    }

    glGenTextures(1, &idTextura);
    glBindTexture(GL_TEXTURE_2D, idTextura);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, (canais == 4) ? GL_RGBA : GL_RGB,
                 largura, altura, 0, (canais == 4) ? GL_RGBA : GL_RGB,
                 GL_UNSIGNED_BYTE, dados);
    stbi_image_free(dados);
}

// Configuração de luz
void configurarLuz() {
    glEnable(GL_LIGHTING);    // Ativa o sistema de iluminação
    glEnable(GL_LIGHT0);      // Ativa a luz 0 (direcional)
    

    GLfloat luzAmbiente[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat luzDifusa[] = {intensidadeLuz, intensidadeLuz, intensidadeLuz, 1.0f};
    GLfloat luzEspecular[] = {intensidadeLuz, intensidadeLuz, intensidadeLuz, 1.0f};
    GLfloat posicaoLuz[] = {-10.0f, 5.0f, 10.0f, 1.0f}; // Luz direcional

    glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa);
    glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular);
    glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz);

    glEnable(GL_COLOR_MATERIAL); // Permite que cores definam propriedades de materiais
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH); // Ou altere para GL_FLAT se necessário

    
    glutPostRedisplay();
}



void atualiza(int valor) {
    if (animacaoAtiva) {
        // Atualiza os ângulos para criar movimento dentro dos intervalos definidos

        // Movimento do ombro (30 a 150)
        ombro += direcaoOmbro * 1.5f;
        if (ombro >= 140.0f || ombro <= 40.0f) {
            direcaoOmbro *= -1; // Inverte a direção
        }

        // Movimento do cotovelo (-90 a 90)
        cotovelo += direcaoCotovelo * 1.5f;
        if (cotovelo >= 60.0f || cotovelo <= -60.0f) {
            direcaoCotovelo *= -1; // Inverte a direção
        }

        // Movimento da mão (-90 a 90)
        mao += direcaoMao * 0.8f;
        if (mao >= 45.0f || mao <= -45.0f) {
            direcaoMao *= -1; // Inverte a direção
        }

        glutPostRedisplay(); // Solicita a renderização novamente
        glutTimerFunc(16, atualiza, 0); // Chama a função novamente em ~16ms (60 FPS)
    }
}

// Função para lidar com as opções do menu
void menu(int opcao) {
    switch (opcao) {
        case 1: // Iniciar animação
            if (!animacaoAtiva) {
                animacaoAtiva = true;
                glutTimerFunc(16, atualiza, 0);
            }
            break;
        case 2: // Pausar animação
            animacaoAtiva = false;
            break;
        case 3: // Reiniciar posições
            ombro = 90.0f;
            cotovelo = 0.0f;
            mao = 0.0f;
            glutPostRedisplay();
            break;
        case 4: // Iluminação fraca
            intensidadeLuz = 0.3f;
            configurarLuz();
            break;
        case 5: // Iluminação média
            intensidadeLuz = 0.7f;
            configurarLuz();
            break;
        case 6: // Iluminação forte
            intensidadeLuz = 1.0f;
            configurarLuz();
            break;
    }
     glutPostRedisplay();
}


void inicializa() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    carregarTextura("texturas/wood.jpg", texturacabo);
    carregarTextura("texturas/metalplate1.jpg", texturaesfera);
    carregarTextura("texturas/concrete.jpg", texturabase);
    carregarTextura("texturas/bluemetal.jpg",texturacabeca);
    carregarTextura("texturas/woodfloor.jpg",texturachao);
    carregarTextura("texturas/darkbrickwall.jpg",texturaparede);
    carregarTextura("texturas/teto.jpg",texturateto);
    carregarTextura("texturas/madeira.png",texturabase2);

    configurarLuz();
}

void desenhaCilindroComTampas(GLfloat raio, GLfloat altura, GLuint textura) {
    glBindTexture(GL_TEXTURE_2D, textura); // Vincula a textura passada como parâmetro
    GLUquadric* cilindro = gluNewQuadric();
    gluQuadricTexture(cilindro, GL_TRUE);
    gluQuadricDrawStyle(cilindro, GLU_FILL);

    // Cilindro
    gluCylinder(cilindro, raio, raio, altura, 30, 30);

    // Tampo inferior
    glPushMatrix();
    glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
    gluDisk(cilindro, 0.0f, raio, 30, 1);
    glPopMatrix();

    // Tampo superior
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, altura);
    gluDisk(cilindro, 0.0f, raio, 30, 1);
    glPopMatrix();

    gluDeleteQuadric(cilindro);
}

void desenhaBase() {
    glPushMatrix();
    glTranslatef(0.0f, -1.0f, 0.0f); // Posiciona a base no chão
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // Alinha o cilindro na direção Y
    desenhaCilindroComTampas(0.5f, 1.0f, texturabase); // Passa a textura de metal
    glPopMatrix();
}

void desenhaChao() {
    glBindTexture(GL_TEXTURE_2D, texturachao); // Usa a textura desejada (grama ou concreto)
    GLfloat tamanho = 20.0f; // Tamanho do chão
    GLfloat altura = -1.0f; // Posiciona o chão logo abaixo da base do braço

    glBegin(GL_QUADS);
    // Configurando as coordenadas de textura para repetir
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-tamanho, altura, -tamanho);
    glTexCoord2f(10.0f, 0.0f); glVertex3f(tamanho, altura, -tamanho);
    glTexCoord2f(10.0f, 10.0f); glVertex3f(tamanho, altura, tamanho);
    glTexCoord2f(0.0f, 10.0f); glVertex3f(-tamanho, altura, tamanho);
    glEnd();
}



void desenhaCuboTexturizado(GLfloat largura, GLfloat altura, GLfloat profundidade) {
    glBindTexture(GL_TEXTURE_2D, texturacabeca); // Vincula a textura de madeira
    glBegin(GL_QUADS);

    // Frente
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-largura, -altura, profundidade);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(largura, -altura, profundidade);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(largura, altura, profundidade);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-largura, altura, profundidade);

    // Trás
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-largura, -altura, -profundidade);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(largura, -altura, -profundidade);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(largura, altura, -profundidade);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-largura, altura, -profundidade);

    // Topo
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-largura, altura, profundidade);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(largura, altura, profundidade);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(largura, altura, -profundidade);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-largura, altura, -profundidade);

    // Base
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-largura, -altura, profundidade);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(largura, -altura, profundidade);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(largura, -altura, -profundidade);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-largura, -altura, -profundidade);

    // Esquerda
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-largura, -altura, -profundidade);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-largura, -altura, profundidade);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-largura, altura, profundidade);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-largura, altura, -profundidade);

    // Direita
    glTexCoord2f(0.0f, 0.0f); glVertex3f(largura, -altura, -profundidade);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(largura, -altura, profundidade);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(largura, altura, profundidade);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(largura, altura, -profundidade);

    glEnd();
}


void desenhaEsfera(GLfloat raio) {
    glBindTexture(GL_TEXTURE_2D, texturaesfera);
    GLUquadric* esfera = gluNewQuadric();
    gluQuadricTexture(esfera, GL_TRUE);
    gluQuadricDrawStyle(esfera, GLU_FILL);
    gluSphere(esfera, raio, 30, 30);
    gluDeleteQuadric(esfera);
}

void desenhaCilindrosAdicionais() {
    GLfloat raio = 0.5f;  // Raio dos cilindros adicionais
    GLfloat altura = 1.0f; // Altura dos cilindros adicionais
    GLfloat distancia = 5.0f; // Distância da base do braço (6 unidades)

    // Posições para os cilindros adicionais (em relação à base do braço)
    GLfloat posicoes[2][3] = {  // Reduzimos de 3 para 2 cilindros
        {distancia, 0.0f, 0.0f},   // Primeiro cilindro
        {-distancia, 0.0f, 0.0f}   // Segundo cilindro
    };

    // Desenha os cilindros adicionais nas posições especificadas
    for (int i = 0; i < 2; i++) {  // Loop agora vai até 2
        glPushMatrix();
        glTranslatef(posicoes[i][0], posicoes[i][1], posicoes[i][2]); // Posiciona cada cilindro
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f); // Rotaciona o cilindro deitado para ficar de pé
        desenhaCilindroComTampas(raio, altura, texturabase2); // Aplica a textura da base
        glPopMatrix();
    }
}




void desenhaBraco() {
    desenhaBase();

    // Articulação do ombro
    glPushMatrix();  
    glRotatef(ombro, 0.0f, 0.0f, 1.0f); // Rotação do ombro
    glTranslatef(0.0f, 0.0f, 0.0f);     // Esfera está no centro da rotação
    desenhaEsfera(0.3f); // Esfera do ombro

    // Primeiro braço
    glTranslatef(0.0f, 0.0f, 0.0f);    // Translada para o centro do braço
    glPushMatrix();                     
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f); // Alinha o cilindro ao eixo X
    desenhaCilindroComTampas(0.15f, 2.0f, texturacabo); // Primeiro segmento do braço
    glPopMatrix();

    // Articulação do cotovelo
    glTranslatef(2.0f, 0.0f, 0.0f); // Move para o final do primeiro braço
    glRotatef(cotovelo, 0.0f, 0.0f, 1.0f); // Rotação do cotovelo
    desenhaEsfera(0.3f); // Esfera do cotovelo

    // Segundo braço
    glTranslatef(0.0f, 0.0f, 0.0f);    // Translada para o centro do braço
    glPushMatrix();                     
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f); // Alinha o cilindro ao eixo X
    desenhaCilindroComTampas(0.15f, 2.0f, texturacabo); // Segundo segmento do braço
    glPopMatrix();

    // Articulação da mão
    glTranslatef(2.0f, 0.0f, 0.0f); // Move para o final do segundo braço
    glRotatef(mao, 0.0f, 0.0f, 1.0f); // Rotação da mão
    desenhaEsfera(0.3f); // Esfera da mão

    // Martelo
    glPushMatrix(); 
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f); // Alinha o cabo do martelo ao braço
    desenhaCilindroComTampas(0.15f, 2.0f, texturacabo); // Cabo do martelo

    glTranslatef(0.0f, 0.0f, 1.5f); // Move para o final do cabo
    glPushMatrix();
    glScalef(0.5f, 1.0f, 0.6f); // Escala para formar a cabeça do martelo
    desenhaCuboTexturizado(1.0f, 1.0f, 1.0f); // Cabeça do martelo como um cubo
    glPopMatrix();

    glPopMatrix(); // Restaura o estado após o martelo

    glPopMatrix(); // Restaura o estado do ombro
}

void desenhaSala() {
    GLfloat tamanho = 20.0f; // Tamanho da sala
    GLfloat altura = 10.0f;  // Altura das paredes

    // Configura a textura para as paredes e teto
    glBindTexture(GL_TEXTURE_2D, texturaparede);

    glBegin(GL_QUADS);

    // Parede da frente
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-tamanho, -1.0f, tamanho);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(tamanho, -1.0f, tamanho);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(tamanho, altura, tamanho);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-tamanho, altura, tamanho);

    // Parede de trás
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-tamanho, -1.0f, -tamanho);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(tamanho, -1.0f, -tamanho);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(tamanho, altura, -tamanho);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-tamanho, altura, -tamanho);

    // Parede da esquerda
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-tamanho, -1.0f, -tamanho);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-tamanho, -1.0f, tamanho);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-tamanho, altura, tamanho);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-tamanho, altura, -tamanho);

    // Parede da direita
    glTexCoord2f(0.0f, 0.0f); glVertex3f(tamanho, -1.0f, -tamanho);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(tamanho, -1.0f, tamanho);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(tamanho, altura, tamanho);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(tamanho, altura, -tamanho);
    glEnd();

    // Teto
    glBindTexture(GL_TEXTURE_2D, texturateto);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-tamanho, altura, -tamanho);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(tamanho, altura, -tamanho);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(tamanho, altura, tamanho);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-tamanho, altura, tamanho);

    glEnd();
}


void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Configuração da câmera com base nas interações do mouse
    glTranslatef(0.0f, 0.0f, -distancia);
    glRotatef(anguloX, 1.0f, 0.0f, 0.0f);
    glRotatef(anguloY, 0.0f, 1.0f, 0.0f);

     // Atualiza a posição da luz após as transformações da câmera
     GLfloat posicaoLuz[] = {-10.0f, 5.0f, 10.0f, 1.0f};// Luz direcional
    glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz);


    // Renderiza a cena
    desenhaSala();
    desenhaChao();
    desenhaBraco();
    desenhaCilindrosAdicionais();

    glutSwapBuffers();
}



void teclado(unsigned char tecla, int x, int y) {
    // Controle de movimentação da câmera e braço
    switch (tecla) {


        case 'q': // Ativa/desativa a animação
        animacaoAtiva = !animacaoAtiva; // Alterna o estado da animação
        if (animacaoAtiva) {
            glutTimerFunc(0, atualiza, 0); // Inicia a animação imediatamente
        }
        break;

        // Controle da iluminação
        case 'p':
        case 'P': // Aumenta a intensidade
            intensidadeLuz += 0.1f;
            if (intensidadeLuz > 3.0f) intensidadeLuz = 3.0f;
            configurarLuz();
            break;
        case 'o':
        case 'O': // Diminui a intensidade
            intensidadeLuz -= 0.1f;
            if (intensidadeLuz < 0.0f) intensidadeLuz = 0.0f;
            configurarLuz();
            break;
        // Movimentação da câmera
        case 'w': // Para cima
            anguloX -= 5.0f;
            break;
        case 's': // Para baixo
            anguloX += 5.0f;
            break;
        case 'a': // Para a esquerda
            anguloY -= 5.0f;
            break;
        case 'd': // Para a direita
            anguloY += 5.0f;
            break;

        // Controle das articulações
        case 'z':
        case 'Z':
            if (ombro < 150.0f) ombro += 5.0f;
            break;
        case 'x':
        case 'X':
            if (ombro > 30.0f) ombro -= 5.0f;
            break;
        case 'c':
        case 'C':
            if (cotovelo < 90.0f) cotovelo += 5.0f;
            break;
        case 'v':
        case 'V':
            if (cotovelo > -90.0f) cotovelo -= 5.0f;
            break;
        case 'b':
        case 'B':
            if (mao < 90.0f) mao += 5.0f;
            break;
        case 'n':
        case 'N':
            if (mao > -90.0f) mao -= 5.0f;
            break;

        case 27: // ESC
            exit(0);
    }

    glutPostRedisplay();
}


void mouse(int button, int state, int x, int y) {
    // Scroll up (button 3) e scroll down (button 4)
    if ((button == 3) || (button == 4)) { 
        if (state == GLUT_UP) return; // Ignora eventos GLUT_UP redundantes

        if (button == 3) { // Scroll up (zoom in)
            distancia -= 0.5f;
            if (distancia < 2.0f) distancia = 2.0f; // Limite mínimo
        } else if (button == 4) { // Scroll down (zoom out)
            distancia += 0.5f;
            if (distancia > 50.0f) distancia = 50.0f; // Limite máximo
        }
    } else { 
        // Eventos normais do botão do mouse (opcional, se necessário)
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            botaoEsquerdoPressionado = true;
            ultimaPosX = x;
            ultimaPosY = y;
        } else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
            botaoEsquerdoPressionado = false;
        }
    }

    glutPostRedisplay();
}


void movimentoMouse(int x, int y) {
    if (botaoEsquerdoPressionado) {
        anguloY += (x - ultimaPosX) * 0.2f;
        anguloX += (y - ultimaPosY) * 0.2f;
        ultimaPosX = x;
        ultimaPosY = y;
    }
    glutPostRedisplay();
}

void scrollMouse(int botao, int direcao, int x, int y) {
    if (direcao > 0) { // Scroll para cima (zoom in)
        distancia -= 0.5f;
        if (distancia < 2.0f) distancia = 2.0f; // Limite mínimo
    } else { // Scroll para baixo (zoom out)
        distancia += 0.5f;
        if (distancia > 50.0f) distancia = 50.0f; // Limite máximo
    }
    glutPostRedisplay();
}


void reshape(int largura, int altura) {
    glViewport(0, 0, largura, altura);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(65.0, (float)largura / (float)altura, 1.0, 50.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Braco Articulado com Camera Interativa");

    inicializa();

    glutDisplayFunc(display);

    // Criar o submenu para animação
    int submenuAnimacao = glutCreateMenu(menu);
    glutAddMenuEntry("Iniciar animacao", 1);
    glutAddMenuEntry("Pausar animacao", 2);

    // Criar o submenu para resetar o braço
    int submenuReset = glutCreateMenu(menu);
    glutAddMenuEntry("Reiniciar posicoes", 3);

    // Criar o menu principal e adicionar os submenus
    glutCreateMenu(menu);
    glutAddSubMenu("Animacao", submenuAnimacao);
    glutAddSubMenu("Resetar braco", submenuReset);
    glutAddMenuEntry("Iluminacao fraca", 4);
    glutAddMenuEntry("Iluminacao media", 5);
    glutAddMenuEntry("Iluminacao forte", 6);

    // Associar o menu ao botão direito do mouse
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutReshapeFunc(reshape);
    glutKeyboardFunc(teclado);
    glutMouseFunc(mouse);
    glutMotionFunc(movimentoMouse);
    
    glutMainLoop();
    return 0;
}
