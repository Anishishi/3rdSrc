#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>

#define WINDOW_X (500)
#define WINDOW_Y (500)
#define WINDOW_NAME "test3"

void init_GL(int argc, char *argv[]);
void init();
void set_callback_functions();

void glut_display();
void glut_keyboard(unsigned char key, int x, int y);
void glut_mouse(int button, int state, int x, int y);
void glut_motion(int x, int y);

void draw_pyramid();

// グローバル変数
double g_angle1 = 0.0;
double g_angle2 = 0.0;
double g_distance = 20.0;
bool g_isLeftButtonOn = false;
bool g_isRightButtonOn = false;
int task1 = -1;
int task2 = -1;
int task3 = -1;
int task4 = -1;

int main(int argc, char *argv[]){
    /* OpenGLの初期化 */
    init_GL(argc, argv);

    /* このプログラム特有の初期化 */
    init();

    /* コールバック関数の登録 */
    set_callback_functions();

    /* メインループ */
    glutMainLoop();

    return 0;
}

void init_GL(int argc, char *argv[]){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_X, WINDOW_Y);
    glutCreateWindow(WINDOW_NAME);
}

void init(){

    glClearColor(0.0, 0.0, 0.0, 0.0); // 背景の塗りつぶし色を指定
}

void set_callback_functions(){
    glutDisplayFunc(glut_display);
    glutKeyboardFunc(glut_keyboard);
    glutMouseFunc(glut_mouse);
    glutMotionFunc(glut_motion);
    glutPassiveMotionFunc(glut_motion);
}

void glut_keyboard(unsigned char key, int x, int y){
    switch (key){
    case 'q':
    case 'Q':
    case '\033':
        exit(0);
    case '1':
        task1 = -task1;
        break;
    case '2':
        task2 = -task2;
        break;
    case '3':
        task3 = -task3;
        break;
    case '4':
        task4 = -task4;
        break;
    }
    glutPostRedisplay();
}

void glut_mouse(int button, int state, int x, int y){
    if (button == GLUT_LEFT_BUTTON){
        if (state == GLUT_UP){
            g_isLeftButtonOn = false;
        }else if (state == GLUT_DOWN){
            g_isLeftButtonOn = true;
        }
    }

    if (button == GLUT_RIGHT_BUTTON){
        if (state == GLUT_UP){
            g_isRightButtonOn = false;
        }else if (state == GLUT_DOWN){
            g_isRightButtonOn = true;
        }
    }
}

void glut_motion(int x, int y){
    static int px = -1, py = -1;
    if (g_isLeftButtonOn == true){
        if (px >= 0 && py >= 0){
            g_angle1 += (double)-(x - px) / 20;
            g_angle2 += (double)(y - py) / 20;
        }
        px = x;
        py = y;
    }else if (g_isRightButtonOn == true){
        if (px >= 0 && py >= 0){
            g_distance += (double)(y - py) / 20;
        }
        px = x;
        py = y;
    }else{
        px = -1;
        py = -1;
    }
    glutPostRedisplay();
}

void glut_display(){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30.0, 1.0, 0.1, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    if (cos(g_angle2) > 0){
        gluLookAt(g_distance * cos(g_angle2) * sin(g_angle1),
                  g_distance * sin(g_angle2),
                  g_distance * cos(g_angle2) * cos(g_angle1),
                  0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    }else{
        gluLookAt(g_distance * cos(g_angle2) * sin(g_angle1),
                  g_distance * sin(g_angle2),
                  g_distance * cos(g_angle2) * cos(g_angle1),
                  0.0, 0.0, 0.0, 0.0, -1.0, 0.0);}

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    if(task4==-1){
        if(task1==-1) glPushMatrix();
        if(task3==1) glutWireTeapot(1.0);
        if(task2==1){
            glRotatef(-30, 0.0, 0.0, 1.0);//回転
            glTranslatef(-1.0, 3.0, 0.0);//平行移動
        }else if(task2==-1){//デフォルト
            glTranslatef(-1.0, 3.0, 0.0);//平行移動
            glRotatef(-30, 0.0, 0.0, 1.0);//回転
        }

        glColor3f(1.0, 1.0, 1.0);
        if(task3==-1) glutWireTeapot(1.0);
        if(task1==-1)glPopMatrix();

        if(task1==-1)glPushMatrix();
        //この処理をせずにglpush, glpopをしないとポットと四角錐どちらにも同じ処理がなされ、重なる
        glTranslatef(0.0, -2.0, 0.0);
        draw_pyramid();
        if(task1==-1)glPopMatrix();
    }else if(task4==1){
        glPushMatrix();
        glColor3f(0.0, 1.0, 0.0);
        glScalef(2.5, 2.0, 2.5);
        glutSolidCube(1.0);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.0, 2.0, 0.0);
        draw_pyramid();
        glPopMatrix();
    }
    glFlush();
    glDisable(GL_DEPTH_TEST);

    glutSwapBuffers();
}


void draw_pyramid(){
    GLdouble pointO[] = {0.0, 1.0, 0.0};
    GLdouble pointA[] = {1.5, -1.0, 1.5};
    GLdouble pointB[] = {-1.5, -1.0, 1.5};
    GLdouble pointC[] = {-1.5, -1.0, -1.5};
    GLdouble pointD[] = {1.5, -1.0, -1.5};

    glColor3d(1.0, 0.0, 0.0);
    glBegin(GL_TRIANGLES);
    glVertex3dv(pointO);
    glVertex3dv(pointA);
    glVertex3dv(pointB);
    glEnd();

    glColor3d(1.0, 1.0, 0.0);
    glBegin(GL_TRIANGLES);
    glVertex3dv(pointO);
    glVertex3dv(pointB);
    glVertex3dv(pointC);
    glEnd();

    glColor3d(0.0, 1.0, 1.0);
    glBegin(GL_TRIANGLES);
    glVertex3dv(pointO);
    glVertex3dv(pointC);
    glVertex3dv(pointD);
    glEnd();

    glColor3d(1.0, 0.0, 1.0);
    glBegin(GL_TRIANGLES);
    glVertex3dv(pointO);
    glVertex3dv(pointD);
    glVertex3dv(pointA);
    glEnd();

    glColor3d(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex3dv(pointA);
    glVertex3dv(pointB);
    glVertex3dv(pointC);
    glVertex3dv(pointD);
    glEnd();
}