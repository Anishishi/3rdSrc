#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <vector>
#include <math.h>

#define IN_VIDEO_FILE "input00.mp4"             //00~03,1人目、10~13,2人目(2球目11はプロフィールに被る)
int offset_x = 0, offset_y = 0, getposmode = 0; //動画によって位置が異なる, 場所を確認するために画像として保存する

bool endflag = 0;
int stop = -1;
int xl_rightbox = 430 + offset_x, xr_rightbox = 580 + offset_x; //右バッターボックスx座標左,x右
int xl_leftbox = 0 + offset_x, xr_leftbox = 345 + offset_x;     //左バッター
int boxUpper = 20 + offset_y, boxLower = 230 + offset_y;        //バッターの上下の閾値

using namespace cv;
using namespace std;

Rect rightBox(Point(xl_rightbox, boxUpper),
              Point(xr_rightbox, boxLower));                                      //バッターボックス上の白線が入らない位置
Rect homebaseBox(Point(xr_leftbox, boxLower), Point(xl_rightbox, boxLower + 40)); //homebase付近、左右のバッターボックスの間の領域
Mat to_batter, flagImg, preframe, frame, gray_face;
Mat to_homebase;
Mat tmp;
vector<Rect> faces, people;
Point mostLeftknee = Point(1000000, 0), homebaseLeft, homebaseRight, ballcenter;
Point bb;                                 //判定に使われるボールの位置
vector<Point> PreBallcenter;              //ボールが検知されてからのフレームごとでボールの中心を格納していく
int beltpos = 0, ballradius, ballcnt = 0; //ベルトの高さ、ボールの半径、上のvectorのcount用
bool ballok = 0, tracedone = 0;           //ボールが手元から離れたのを検知したか否か

void die(string str); //失敗したら終了させる
void ZoneHeight();    //ストライクゾーン高低を導出
void ZoneWidth();     //ストライクゾーン幅を導出
bool getpitcher();    //ピッチャーを検出
bool getBall();       //投じられたボールを検出
bool traceBall();     //ボールのトラッキング
bool judgeSB();       //ストライクボールを判定

int main(int argc, char *argv[])
{
    //ストライクボール表示用の画像を読み込む
    Mat strikeimg, ballimg;
    strikeimg = imread("strike.jpg", 1);
    ballimg = imread("ball.jpg", 1);

    // 1. prepare VideoCapture Object
    VideoCapture cap;
    string input_index;
    if (argc == 2)
    {
        input_index = argv[1];
    }
    else if (argc == 1)
    {
        input_index = IN_VIDEO_FILE;
    }
    else
    {
        die("the number of input video is not proper.");
    }
    cap.open(input_index);

    // 2. prepare VideoWriter Object
    Mat gray_img, flagImg;
    bool pitcherDetected = false, isstrike, judgedone = false;
    bool done = false;

    namedWindow("video", 1);

    if (!cap.isOpened())
    {
        die("no input video");
    }
    else
    {
        cap >> frame; //1フレーム目のみ以下の処理を行う
        if (frame.empty())
        {
            die("no remain frame");
        }
        to_batter = frame(rightBox).clone();      //右バッターボックスだけを取り出す
        to_homebase = frame(homebaseBox).clone(); //ホームベース付近だけを取り出す

        if(getposmode){//バッターボックスとホームベースの抽出領域確認用
            imwrite("batterbox.jpg", to_batter);
            imwrite("homebase.jpg", to_homebase);
            die("detect the positions of frames.");
        }

        ZoneHeight(); //バッターボックスの高低を検出する
        ZoneWidth();  //バッターボックスの左右を検出する

        bool loop_flag = true;
        while (loop_flag)
        {
            if (stop == -1 && endflag == 0)
            {
                cap >> frame;
            }
            if (frame.empty())
            {
                frame = preframe;
                endflag = 1;
                stop = 1;
            }
            else
            {
                preframe = frame.clone();
            }
            flagImg = frame.clone();

            if (!pitcherDetected)
            {
                pitcherDetected = getpitcher(); //ピッチャーの検出
            }
            if (ballok && stop == -1 && !tracedone)
            {
                tracedone = traceBall(); //ボールを追跡する
            }

            if (tracedone)
            {
                isstrike = judgeSB(); //ストライクボール判定
                judgedone = true;
            }
            if (judgedone)
            {
                namedWindow("judge");
                circle(flagImg, bb, 2, Scalar(0, 0, 255), 3); //判定に使われたボールの位置
                //sb表示
                if (isstrike)
                {
                    imshow("judge", strikeimg);
                }
                else
                {
                    imshow("judge", ballimg);
                }
                if (!done)
                {
                    stop = 1;
                    done = true;
                }
            }

            if (!ballok && pitcherDetected)
            {
                ballok = getBall(); //ピッチャーの手元から離れたボールを検出
            }

            if (ballok)
            {
                circle(flagImg, ballcenter,
                       ballradius, Scalar(0, 0, 255), 3); //初めて検出されたボールを描画
                for (int i = 0; i < PreBallcenter.size() - 1; i++)
                {
                    line(flagImg, PreBallcenter[i], PreBallcenter[i + 1], Scalar(0, 255, 255), 2, 8);
                }
            }

            rectangle(flagImg,
                      Point(xr_leftbox + homebaseLeft.x,
                            boxUpper + (faces[0].y + faces[0].height + beltpos) / 2),
                      Point(xr_leftbox + homebaseRight.x,
                            boxUpper + mostLeftknee.y),
                      Scalar(0, 255, 122), 2); //ストライクゾーンを描画

            imshow("output", flagImg);
            imshow("video", frame);

            // 5. process according to input key
            int k = waitKey(20);
            switch (k)
            {
            case 'q':
            case 'Q':
                loop_flag = false;
                printf("interrupted.\n");
                break;
            case 10:
                imwrite("frame_src.jpg", frame);
                imwrite("ZoneBallTrace.jpg", flagImg);
                loop_flag = false;
                printf("successfully saved.\n");
                break;
            case 's':
                if (endflag == 1)
                {
                    loop_flag = false;
                    printf("safely done.\n");
                }
                stop *= -1;
                break;
            }
        }
    }
    return 0;
}

void die(string str)
{
    printf("%s\n", str.c_str());
    exit(1); //errorがあったらプログラム終了
}

void ZoneHeight()
{
    tmp = to_batter.clone();
    cvtColor(tmp, flagImg, CV_BGR2GRAY);
    threshold(flagImg, flagImg, 160, 255, THRESH_BINARY); //閾値160で2値画像に変換
    threshold(flagImg, flagImg, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    vector<vector<Point>> contours;
    findContours(flagImg, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE); //輪郭を抽出

    vector<vector<Point>> contours_subset;

    for (int i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours.at(i));
        if (area > 1000 && area < 5000)
        {
            contours_subset.push_back(contours.at(i)); //面積がこの値の間にあるものだけを抽出
        }
    }
    Mat mask = Mat::zeros(tmp.rows, tmp.cols, CV_8UC1);
    if (contours_subset.empty())
    {
        die("no contours in zoneheight");
    }
    drawContours(mask, contours_subset, -1, Scalar(255), -1);
    Mat result_lower = mask.clone(); //下半身のユニフォームが白いところのみを取り出せる
    Size s = result_lower.size();

    bool pre = 0;
    int cnt;
    for (int h = 0; h < s.height; h++)
    { //ベルトの位置と膝の位置を探索する
        uchar *ptr1;
        cnt = 0, pre = 0;
        ptr1 = result_lower.ptr<uchar>(h);
        for (int w = 0; w < s.width; w++)
        { //x方向に見ていきベルトのあるところは白黒白黒となっている。膝は足の中で一番x座標が小さい
            if (*ptr1 == 255)
            {
                if (pre == 0 && !beltpos)
                {
                    cnt++;
                    pre = 1;
                }
                if (mostLeftknee.x > w)
                {
                    mostLeftknee = Point(w, h);
                }
            }
            else if (pre == 1 && !beltpos)
            {
                cnt++;
                pre = 0;
            }
            ptr1++;
        }
        if (cnt >= 2)
        {
            beltpos = h;
        }
    }
    if (beltpos == 0)
    {
        die("the height of belt is not detected.");
    }
    if (mostLeftknee == Point(1000000, 0))
    {
        die("batter's knee is not detecetd.");
    }

    double scale = 4.0;
    string cascadeName = "/usr/share/opencv/haarcascades/haarcascade_profileface.xml"; //Haar-like横顔検出
    cvtColor(to_batter, gray_face, CV_BGR2GRAY);
    equalizeHist(gray_face, gray_face);
    CascadeClassifier cascade;
    if (!cascade.load(cascadeName))
    {
        die("cascadeFile not found");
    }
    cascade.detectMultiScale(gray_face, faces, 1.01, 3, CASCADE_SCALE_IMAGE, Size(1, 1)); //顔の一番したが方であると言える
    if (faces.size() == 0)
    {
        die("batter's face is not detected.");
    }
    /*line(to_batter, Point(0,beltpos), Point(200, beltpos), Scalar(0, 255, 100), 2);
    rectangle(to_batter, faces[0], Scalar(0, 255, 100), 2);
    imwrite("face.jpg", to_batter);
    imwrite("bw_lower.jpg", result_lower);
    imwrite("lower_gray.jpg", flagImg);*/
}

void ZoneWidth()
{
    Mat grayhomebase, edgehomebase;
    cvtColor(to_homebase, grayhomebase, CV_BGR2GRAY);
    threshold(grayhomebase, grayhomebase, 180, 255, THRESH_BINARY);                   //閾値180で2値画像に変換
    threshold(grayhomebase, grayhomebase, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU); //

    vector<vector<Point>> contours, contourssubset;
    findContours(grayhomebase, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE); //輪郭を抽出
    int maxdist = 0, useedge = 100000, ansx0, ansx1;
    for (int i = 0; i < contours.size(); i++)
    { //輪郭の中で横に長いものがベースである
        int small = 1000, large = 0;
        for (int j = 0; j < contours[i].size(); j++)
        {
            small = std::min(small, contours[i][j].x);
            large = std::max(large, contours[i][j].x);
        }
        if (large - small > maxdist)
        {
            useedge = i;
            maxdist = large - small;
            ansx0 = small;
            ansx1 = large;
        }
    }

    if (contours.empty())
    {
        die("no contours in zonewidth.");
    }
    contourssubset.push_back(contours[useedge]);
    Mat mask = Mat::zeros(grayhomebase.rows, grayhomebase.cols, CV_8UC1);
    drawContours(mask, contourssubset, -1, Scalar(255), -1);

    homebaseLeft = Point(ansx0, contourssubset[0][0].y);
    homebaseRight = Point(ansx1, contourssubset[0][0].y);

    /*imwrite("homebase_pro.jpg", to_homebase);
    imwrite("bw_homebase.jpg", grayhomebase);
    imwrite("homebase.jpg", mask);*/
}

bool getpitcher()
{                      //ピッチャーの検出、その成否をboolで表す
    HOGDescriptor hog; //hog特徴とSVMによる人検出
    Mat graypeople;
    cvtColor(frame, graypeople, CV_BGR2GRAY);
    hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
    hog.detectMultiScale(graypeople, people);
    if (people.size() == 0)
    {
        return 0;
    }
    int MLpersonPos = 1000, MLpersonFlag = 10;
    for (int i = 0; i < people.size(); i++)
    { //x座標を見てあげることでピッチャーを取り出す
        Rect r = people[i];
        if (r.x < MLpersonPos)
        {
            MLpersonPos = r.x;
            MLpersonFlag = i;
        }
    }
    people[0] = people[MLpersonFlag]; //pitcherをpeople[0]に格納
    if (people[0].x > 200)
    {
        return 0; //x座標が大きく右方に存在していたらピッチャーでない
    }
    int x_center = people[0].x + people[0].width / 2;
    people[0].x = x_center - (int)(people[0].height / 2 * 1.2);
    people[0].width = (int)(people[0].height * 1.2); //ボールの探索領域を横幅が高さの1.2倍となるように拡大
    return 1;
}

bool getBall() //ボールの検出、その成否をboolで表す
{
    Mat pitchbox, graypitch;
    pitchbox = frame(people[0]).clone();
    GaussianBlur(pitchbox, pitchbox, Size(5, 5), 0, 0, 0);
    cvtColor(pitchbox, graypitch, CV_BGR2GRAY);
    threshold(graypitch, graypitch, 200, 255, THRESH_BINARY);                   //閾値160で2値画像に変換
    threshold(graypitch, graypitch, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU); //白黒にする、ボールは白い
    vector<Vec3f> circles;
    HoughCircles(graypitch, circles, CV_HOUGH_GRADIENT, 1, 10, 100, 6, 1, 5); //hough変換による円の検知
    Mat cpPitch;
    cpPitch = pitchbox.clone();
    int punish = 0; //ボールである可能性のあるものからボールでないと思われるものを以下で除く

    for (auto it = circles.begin(); it < circles.end(); it++)
    {
        bool flag = true;
        ballradius = (*it)[2];
        ballcenter = Point((*it)[0], (*it)[1]);
        int xx = ballcenter.x, yy = ballcenter.y;
        for (int i = -1; i <= 1; i++)
        {
            for (int j = -1; j <= 1; j++)
            {
                //近傍が黒い、または、少し遠くが白かったら手から離れたボールではない
                if (graypitch.at<unsigned char>(xx + i, yy + j) == 0 ||
                    (j != 0 && graypitch.at<unsigned char>(xx + i, yy + j * 6) == 255))
                {
                    flag = false;
                }
            }
        }
        if (people[0].x + people[0].width / 3 < xx && //体の近くにあったらボールではない
            xx < people[0].x + people[0].width / 3 * 2)
        {
            flag = false;
        }
        if (flag == true)
        {
            break; //すべての条件をクリアしたらボール
        }
        else
        {
            punish++;
        }
    }
    //namedWindow("graypitch");
    //imshow("graypitch", graypitch);
    if (circles.size() - punish >= 1) //ボールが存在する場合
    {
        ballcenter += people[0].tl();
        PreBallcenter.push_back(ballcenter);
        return 1;
    }
    return 0;
}

bool traceBall()
{
    Rect nextrange(PreBallcenter[ballcnt].x, PreBallcenter[ballcnt].y - 20, 30, 40);
    Mat nextimg = frame(nextrange).clone(); //次のフレームでボールが存在する可能性のある領域
    Mat graynext, gaussianimg;
    GaussianBlur(nextimg, gaussianimg, Size(3, 3), 0, 0, 0);
    cvtColor(gaussianimg, graynext, CV_BGR2GRAY);
    threshold(graynext, graynext, 200, 255, THRESH_BINARY);                   //閾値160で2値画像に変換
    threshold(graynext, graynext, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU); //白黒にする、ボールは白い
    vector<Vec3f> cir;
    HoughCircles(graynext, cir, CV_HOUGH_GRADIENT, 1, 10, 100, 3, 1, 5); //hough変換による円の検知、パラメータは緩めに
    if (cir.size() == 0)
    {
        return 1; //ボールがミットに収まり、検知終了
    }

    for (auto it = cir.begin(); it < cir.end(); it++)
    {
        Point bc = Point((*it)[0], (*it)[1]);
        //移動した距離をハミング距離として計算して適当な距離動いたものを残す
        int distance = bc.x + bc.y;
        //printf("%d\n", distance);
        if (distance > 50)
        {
            return 1;
        }
    }

    int nextballradius = cir[0][2];
    Point nextballcenter = Point(cir[0][0], cir[0][1]);
    circle(nextimg, nextballcenter,
           nextballradius, Scalar(255, 0, 30), 4);

    nextballcenter += nextrange.tl();
    PreBallcenter.push_back(nextballcenter); //ボールの場所を記録
    ballcnt++;

    //namedWindow("nextimg");
    //imshow("nextimg", nextimg);
    //imshow("bw", graynext);

    return 0;
}

bool judgeSB()
{ //ストライクのとき1, ボールのとき0
    int framelength = PreBallcenter.size();
    //ピッチャーの手から離れるボールとホームベースとの距離は約16m、ホームベースとキャッチャーの距離は約1m
    int distframenum = (int)(framelength * 16 / 17.0);
    bb = PreBallcenter[distframenum]; //判定されるボールの位置、ゾーンにボールがかかればいい
    if (bb.x + 2 >= xr_leftbox + homebaseLeft.x && bb.x - 2 <= xr_leftbox + homebaseRight.x &&
        bb.y - 2 <= boxUpper + mostLeftknee.y &&
        bb.y + 2 >= boxUpper + (faces[0].y + faces[0].height + beltpos) / 2)
    {
        return 1;
    }
    return 0;
}