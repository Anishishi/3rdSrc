#include <opencv2/opencv.hpp>
#include <stdio.h>

#define FLAG 1 // (0: direct access / 1: built-in function)

char *preset_file = "fruits.jpg";

void convertColorToGray(cv::Mat &input, cv::Mat &processed);
void gaussianFilter(cv::Mat &input, cv::Mat &processed);
void edgeDetection(cv::Mat &input, cv::Mat &processed);

int main(int argc, char *argv[])
{
  char *input_file;
  //1. prepare Mat objects for input-image and output-image
  cv::Mat input, processed;

  if(argc==2){
      input_file = argv[1];
  }
  else{
      input_file = preset_file;
  }

  // 2. read an image from the specified file
  input = cv::imread(input_file, 1);
  if(input.empty()){
      fprintf(stderr, "cannot open %s\n", input_file);
      exit(0);
  }

  //convertColorToGray(input, processed);
  //gaussianFilter(input, processed);//平滑化
  edgeDetection(input, processed);//エッジ抽出

  //5. create windows
  cv::namedWindow("original image", 1);
  cv::namedWindow("processed image", 1);

  //6. who images
  cv::imshow("original image", input);
  cv::imshow("processed image", processed);

  //7. wait key input
  cv::waitKey(0);

  //8. save the processed result
  cv::imwrite("1_1EdgeDetectionBB.jpg", processed);

  return 0;
}


void convertColorToGray(cv::Mat &input, cv::Mat &processed)
{
#if FLAG // use built-in function

  //4. convert color to gray
  cv::Mat temp;
  std::vector<cv::Mat> planes;
  cv::cvtColor(input, temp, CV_BGR2YCrCb);
  cv::split(temp, planes);
  processed = planes[0];

#else

  // 3. create Mat for output-image
  cv::Size s = input.size();
  processed.create(s, CV_8UC1);

  for(int j = 0; j < s.height; j++){
    uchar *ptr1, *ptr2;
    ptr1 = input.ptr<uchar>(j);
    ptr2 = processed.ptr<uchar>(j);

    //4. convert color to gray
    for(int i = 0; i < s.width; i++){
      double y = 0.114 * ((double)ptr1[0]) + 0.587 * (double)ptr1[1] + 0.229 * (double)ptr1[2];

      if(y > 255) y = 255;
      if(y < 0) y = 0;

      *ptr2 = (uchar)y;
      ptr1 += 3;
      ptr2++;
    }
  }
#endif
}

//課題1.1.1
void gaussianFilter(cv::Mat &input, cv::Mat &processed)
{
#if FLAG // use built-in function

  cv::Size s=input.size();
  cv::GaussianBlur(input, processed, cv::Size(11, 11), 0, 0);

#else

  exit(1);//is not defined

#endif
}

//課題1.1.2
void edgeDetection(cv::Mat &input, cv::Mat &processed)
{
#if FLAG // use built-in function

  cv::Size s=input.size();
  cv::Sobel(input, processed, CV_8U, 0, 1);

#else

  exit(1);//is not defined

#endif
}