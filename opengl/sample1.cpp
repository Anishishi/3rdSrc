#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>

#define WINDOW_X (500)
#define WINDOW_Y (500)
#define WINDOW_NAME "test1"

void init_GL(int argc, char *argv[]);
void init();
void set_callback_functions();

void glut_display();
void glut_keyboard(unsigned char key, int x, int y);

void draw_square1();
void draw_square2();
void draw_square3();
void draw_hexagon();

//グローバル変数
int g_display_mode = 1;
int vartexNumbers = 2;

int main(int argc, char *argv[]){
	/* OpenGLの初期化 */
	init_GL(argc, argv);

	/* このプログラム特有の初期化 */
	init();

	/* コールバック関数の登録 */
	set_callback_functions();

	/* メインループ */
	glutMainLoop(); //無限ループ。コールバック関数が呼ばれるまでずっと実行される

	return 0;
}

void init_GL(int argc, char *argv[]){
	glutInit(&argc, argv);        // OpenGLの初期化
	glutInitDisplayMode(GLUT_RGBA);      // ディスプレイモードをRGBAモードに設定
	glutInitWindowSize(WINDOW_X, WINDOW_Y);     // ウィンドウサイズを指定
	glutCreateWindow(WINDOW_NAME);      // ウィンドウを生成。表示はされない
}

void init(){
	glClearColor(0.0, 0.0, 0.0, 0.0);       // 背景の塗りつぶし色指定
}

void set_callback_functions(){
	glutDisplayFunc(glut_display);      // ディスプレイに変化が合った時に呼ばれるコールバック関数を登録
	glutKeyboardFunc(glut_keyboard);    // キーボードに変化があったときに呼び出されるコールバック関数の登録
}

// キーボードに変化があったときに呼び出されるコールバック関数
void glut_keyboard(unsigned char key, int x, int y){
	switch(key){

	case 'q':
	case 'Q':
	case '\033': // Escキーのこと
		exit(0);
	case '1':
		g_display_mode = 1;
		break;
	case '2':
		g_display_mode = 2;
		break;
	case '3':
		g_display_mode = 3;
		break;
    case '4':
        g_display_mode = 4;
        break;
    case '5':
        vartexNumbers++;
        break;
    case '6':
        vartexNumbers--;
        break;
	}

	glutPostRedisplay();          // 「ディスプレイのコールバック関数を読んで」と指示する
}

// ディスプレイに変化が合った時に呼び出されるコールバック関数
// ディスプレイに変化があったときはglutPostRedisplay()で表示する
void glut_display(){
	glClear(GL_COLOR_BUFFER_BIT);      // 今まで画面に描かれていたものを消す

	switch(g_display_mode){
	case 1:
		draw_square1();
		break;
	case 2:
		draw_square2();
		break;
	case 3:
		draw_square3();
		break;
    case 4:
        draw_hexagon();
        break;
	}

    glFlush();        // ここで画面を描画する
}

void draw_square1(){
	glBegin(GL_LINE_LOOP);

	glColor3d(1.0, 0.0, 0.0);
    glVertex2d(-0.9, -0.9);
    glVertex2d(0.9, -0.9);
    glVertex2d(0.9, 0.9);
    glVertex2d(-0.9, 0.9);

	glEnd();
}

void draw_square2(){
	glBegin(GL_POLYGON);

	glColor3d(1.0, 0.0, 0.0);
    glVertex2d(-0.9, -0.9);
    glVertex2d(0.9, -0.9);
    glVertex2d(0.9, 0.9);
    glVertex2d(-0.9, 0.9);

	glEnd();
}

void draw_square3(){
	glBegin(GL_POLYGON);

	glColor3d(1.0, 0.0, 0.0);
    glVertex2d(-0.9, -0.9);
	glColor3d(1.0, 1.0, 0.0);
    glVertex2d(0.9, -0.9);
	glColor3d(0.0, 1.0, 1.0);
    glVertex2d(0.9, 0.9);
	glColor3d(0.0, 0.0, 0.0);
    glVertex2d(-0.9, 0.9);

	glEnd();
}

void draw_hexagon(){//5,6をおすことで正多角形の頂点の数を増減させることができる。4を押すと正n角形を描画できる。
    glBegin(GL_POLYGON);

    glColor3d(0.0, 1.0, 0.0);
    for(int i=1; i<=vartexNumbers; i++){
        glVertex2d(cos(2*M_PI/vartexNumbers*i), sin(2*M_PI/vartexNumbers*i));
    }

    glEnd();
}