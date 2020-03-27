#include <stdio.h>

//クラス宣言
class myclass{
public:
    //メンバ関数(メソッド)
    void set_x(int num);//xの値
    int get_x();//xの値を取得

    //メンバ変数
    int x;
};

//メンバ関数定義
void myclass::set_x(int num){ x = num;}
int myclass::get_x(){ return x;}

int main(){
    myclass a;//myclassのオブジェクトを宣言

    //メンバ関数を使った変数にアクセス
    a.set_x(1);
    printf("x = %d\n", a.get_x());//1が出力される

    //メンバ変数に直接アクセス
    a.x = 2;
    printf("x = %d\n", a.x);//2が出力される

    return 0;
}