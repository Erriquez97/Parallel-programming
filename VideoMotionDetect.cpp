#include <stdio.h>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

int main(int argc, char** argv )
{

    string path = "../Resources/VideoTest.mkv";
    VideoCapture cap(path);
    Mat image;
    while (true)
    {
        cap.read(image);
        imshow("Display Image", image);
        waitKey(10);
    }
    

     namedWindow("Display Image", WINDOW_AUTOSIZE );
    // serve per non far chiudere la finestra dell'immagine, con 0 significa che non si chiude mai fino a quando non la chiudo io premendo su x
    return 0;

}