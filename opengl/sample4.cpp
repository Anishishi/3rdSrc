#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>

#define WINDOW_X (500)
#define WINDOW_Y (500)
#define WINDOW_NAME "test4"

void init_GL(int argc, char *argv[]);
void init();
void set_callback_functions();

void glut_display();
void glut_keyboard(unsigned char key, int x, int y);
void glut_mouse(int button, int state, int x, int y);
void glut_motion(int x, int y);

void draw_pyramid();
void draw_plane();

// グローバル変数
double g_angle1 = 0.0;
double g_angle2 = 0.0;
double g_angle3 = 0.0;
double g_angle4 = 3.141592 / 4;
double g_distance = 20.0;
bool g_isLeftButtonOn = false;
bool g_isRightButtonOn = false;
float color_variation[4] = {0.0, 1.0, 0.0, 0.0};//法線ベクトルを変える。4つ目はダミー
float light_diff[4] = {1.0, 1.0, 1.0, 1.0};//光源の成分を変える。
int color_flag=0, secondlighton = -1;

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

void reset_parms(){
    color_flag=0;
    color_variation[0]=0.0;
    color_variation[1]=1.0;
    color_variation[2]=0.0;
    for(int i=0; i<4; i++){light_diff[i]=1.0;}
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
    case '0':
        color_flag = 0;
        break;
    case '1':
        color_flag = 1;
        break;
    case '2':
        color_flag = 2;
        break;
    case '3':
        color_flag = 3;
        break;
    case 'a':
        color_variation[color_flag]+=0.1;
        break;
    case 'b':
        color_variation[color_flag]-=0.1;
        break;
    case 'c':
        light_diff[color_flag]+=0.1;
        break;
    case 'd':
        light_diff[color_flag]-=0.1;
        break;
    case 'e':
        secondlighton = -secondlighton;
    case 'r':
        reset_parms();
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
    }else if (button == GLUT_RIGHT_BUTTON){
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
            g_angle3 += (double)(x - px) / 20;
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
                  0.0, 0.0, 0.0, 0.0, 1.0, 0.0);}
    else{
        gluLookAt(g_distance * cos(g_angle2) * sin(g_angle1),
                  g_distance * sin(g_angle2),
                  g_distance * cos(g_angle2) * cos(g_angle1),
                  0.0, 0.0, 0.0, 0.0, -1.0, 0.0);}

    GLfloat lightpos[] = {(float)(5 * cos(g_angle4) * sin(g_angle3)),
                          (float)(5 * sin(g_angle4)),
                          (float)(5 * cos(g_angle4) * cos(g_angle3)),
                          1.0};
    GLfloat lightpos2[] = {(float)(1.0),
                          (float)(1.0),
                          (float)(1.0),
                          1.0};
    GLfloat diffuse[] = {light_diff[0], light_diff[1], light_diff[2], light_diff[3]};
    GLfloat diffuse2[] = {1.0, 1.0, 1.0, 1.0};

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glPushMatrix();
    glTranslatef(5 * cos(g_angle4) * sin(g_angle3), 5 * sin(g_angle4),
                 5 * cos(g_angle4) * cos(g_angle3));
    glutSolidSphere(0.2, 50, 50);
    glPopMatrix();

    if(secondlighton==1){
        glPushMatrix();
        glTranslatef(1.0, 1.0, 1.0);
        glutSolidSphere(0.2, 50, 50);
        glPopMatrix();
    }

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    if(secondlighton==1) glEnable(GL_LIGHTING);
    if(secondlighton==1) glEnable(GL_LIGHT1);
    else if(secondlighton==-1) glDisable(GL_LIGHT1);

    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
    if(secondlighton==1) glLightfv(GL_LIGHT1, GL_POSITION, lightpos2);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    if(secondlighton==1) glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse2);


    glPushMatrix();
    glTranslatef(0.0, -2.0, 0.0);
    draw_plane();
    glPopMatrix();

    glPushMatrix();
    glScalef(1.0, 2.0, 1.0);
    draw_pyramid();
    glPopMatrix();

    glFlush();

    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glutSwapBuffers();
}

void draw_pyramid(){
    GLdouble pointO[] = {0.0, 1.0, 0.0};
    GLdouble pointA[] = {1.5, -1.0, 1.5};
    GLdouble pointB[] = {-1.5, -1.0, 1.5};
    GLdouble pointC[] = {-1.5, -1.0, -1.5};
    GLdouble pointD[] = {1.5, -1.0, -1.5};

    GLfloat facecolor1[] = {1.0, 0.0, 0.0, 0.8};
    GLfloat facecolor2[] = {1.0, 1.0, 0.0, 0.8};

    GLfloat facecolor3[] = {0.0, 1.0, 1.0, 0.8};
    GLfloat facecolor4[] = {1.0, 0.0, 1.0, 0.8};
    GLfloat facecolor5[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat specular[] = {0.0, 0.0, 0.0, 1.0};

    glMaterialfv(GL_FRONT, GL_DIFFUSE, facecolor1);
    glNormal3d(0.0, 0.6, 0.8);
    glBegin(GL_TRIANGLES);
    glVertex3dv(pointO);
    glVertex3dv(pointA);
    glVertex3dv(pointB);
    glEnd();

    glMaterialfv(GL_FRONT, GL_DIFFUSE, facecolor2);
    glNormal3d(-0.8, 0.6, 0.0);
    glBegin(GL_TRIANGLES);
    glVertex3dv(pointO);
    glVertex3dv(pointB);
    glVertex3dv(pointC);
    glEnd();

    glMaterialfv(GL_FRONT, GL_DIFFUSE, facecolor3);
    glNormal3d(0.0, 0.6, -0.8);
    glBegin(GL_TRIANGLES);
    glVertex3dv(pointO);
    glVertex3dv(pointC);
    glVertex3dv(pointD);
    glEnd();

    glMaterialfv(GL_FRONT, GL_DIFFUSE, facecolor4);
    glNormal3d(0.8, 0.6, 0.0);
    glBegin(GL_TRIANGLES);
    glVertex3dv(pointO);
    glVertex3dv(pointD);
    glVertex3dv(pointA);
    glEnd();

    glMaterialfv(GL_FRONT, GL_DIFFUSE, facecolor5);
    glNormal3d(0.0, -1.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3dv(pointA);
    glVertex3dv(pointB);
    glVertex3dv(pointC);
    glVertex3dv(pointD);
    glEnd();
}

void draw_plane(){
    GLfloat facecolor[] = {0.9, 0.9, 0.9, 1.0};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, facecolor);
    glNormal3d(color_variation[0], color_variation[1], color_variation[2]);
    glBegin(GL_QUADS);
    glVertex3d(4.0, 0.0, 4.0);
    glVertex3d(4.0, 0.0, -4.0);
    glVertex3d(-4.0, 0.0, -4.0);
    glVertex3d(-4.0, 0.0, 4.0);
    glEnd();
}