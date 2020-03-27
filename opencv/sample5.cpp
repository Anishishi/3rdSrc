#include <opencv2/opencv.hpp>
#include <stdio.h>

//#include <time.h>

int size_of_mosaic = 0;
int videoflag=0, typeofface=0, loop_flag=1;

int main(int argc, char *argv[])
{
    // 1. load classifier
    std::string cascadeName = "/usr/share/opencv/haarcascades/haarcascade_frontalface_alt.xml"; //Haar-like
    cv::CascadeClassifier cascade;
    if (!cascade.load(cascadeName)){
        printf("ERROR: cascadeFile not found\n");
        return -1;
    }

    // 2. initialize VideoCapture
    cv::Mat frame, input, pic;
    cv::VideoCapture cap;
    cap.open(videoflag);
    cap >> frame;

    // 3. prepare window and trackbar
    cv::namedWindow("result", 1);
    cv::createTrackbar("size", "result", &size_of_mosaic, 30, 0);

    double scale = 4.0;
    cv::Mat gray, smallImg(cv::saturate_cast<int>(frame.rows / scale),
                           cv::saturate_cast<int>(frame.cols / scale), CV_8UC1);

    //clock_t start = clock();
    char *preset_file = "fruits.jpg";
    pic = cv::imread(preset_file, 1);
    while(loop_flag){

        // 4. capture frame
        cap >> frame;
        //convert to gray scale
        cv::cvtColor(frame, gray, CV_BGR2GRAY);

        // 5. scale-down the image
        cv::resize(gray, smallImg, smallImg.size(), 0, 0, cv::INTER_LINEAR);
        cv::equalizeHist(smallImg, smallImg);

        // 6. detect face using Haar-classifier
        std::vector<cv::Rect> faces;
        ///multi-scale face searching
        // image, size, scale, num, flag, smallest rect

        cascade.detectMultiScale(smallImg, faces,
                                 1.1,
                                 2,
                                 CV_HAAR_SCALE_IMAGE,
                                 cv::Size(30, 30));

        // 7. mosaic(pixelate) face-region
        /* if(faces.size()>0){
            clock_t end = clock();
            const double time = static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000.0;
            printf("%lf[ms]\n", time);
            break;
        } */

        for (int i = 0; i < faces.size(); i++)
        {
            cv::Point center;
            int radius;
            center.x = cv::saturate_cast<int>((faces[i].x + faces[i].width * 0.5) * scale);
            center.y = cv::saturate_cast<int>((faces[i].y + faces[i].height * 0.5) * scale);
            radius = cv::saturate_cast<int>((faces[i].width + faces[i].height) * 0.25 * scale);
            //mosaic
            if (size_of_mosaic < 1)
                size_of_mosaic = 1;
            cv::Rect roi_rect(center.x - radius, center.y - radius, radius * 2, radius * 2);
            if(typeofface==0){
                cv::Mat mosaic = frame(roi_rect);
                cv::Mat tmp;
                cv::resize(mosaic, tmp, cv::Size(radius / size_of_mosaic, radius / size_of_mosaic), 0, 0);
                cv::resize(tmp, mosaic, cv::Size(radius * 2, radius * 2), 0, 0, CV_INTER_NN);
            }else if(typeofface==1){
                input = pic.clone();
                cv::Mat changeFace = frame(roi_rect);
                cv::resize(input, input, changeFace.size());
                input.copyTo(changeFace);
            }
        }

        // 8. show mosaiced image to window
        cv::imshow("result", frame);

        int key = cv::waitKey(10);
        switch (key){
        case 'Q':
        case 'q':
            loop_flag=0;
            break;
        case 'a':
            typeofface = 0;
            break;
        case 'b':
            typeofface = 1;
            break;
        case 10:
            loop_flag = 0;
            cv::imwrite("1_5output.jpg", frame);
            break;
        }
    }
    return 0;
}