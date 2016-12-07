#include <cv.h>
#include <highgui.h>
#include <ctype.h>

int
main (int argc, char **argv)
{
  CvCapture *capture = 0;
  IplImage *frame = 0;
  int c;

  // (1)指定された番号のカメラに対するキャプチャ構造体を作成する
  capture = cvCreateCameraCapture(1);

  // (2)表示用ウィンドウをの初期化
  cvNamedWindow ("Capture", CV_WINDOW_AUTOSIZE);

  while (1) {
    // (3)カメラから画像をキャプチャする
    frame = cvQueryFrame (capture);
    // (4) カメラ画像の表示
    cvShowImage ("Capture", frame);
    // (5) 2msecだけキー入力を待つ
    c = cvWaitKey (2);
    if (c == '\x1b') // Escキー
      break;
  }

  cvReleaseCapture (&capture);
  cvDestroyWindow ("Capture");

  return 0;
}
