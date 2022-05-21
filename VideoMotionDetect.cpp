#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <chrono>
using namespace cv;
using namespace std;

std::chrono::system_clock::time_point start;
std::chrono::system_clock::time_point stop;

int main(int argc, char **argv)
{

    string path = "../Resources/VideoTest.mp4";
    VideoCapture cap(path);
    Mat image;
    Mat imgGray;
    Mat imgSmooth;

    start = std::chrono::system_clock::now();

    while (true)
    {
        cap.read(image);

        // imgSmooth=imgGray.clone();
        
        for (int i = 0; i < image.rows; i++)
        {
            for (int j = 0; j < image.cols; j++)
            {
                int value=(image.at<Vec3b>(i,j)[0]+image.at<Vec3b>(i,j)[1]+image.at<Vec3b>(i,j)[2])/3;
                 image.at<Vec3b>(i,j)[0]=value;
                 image.at<Vec3b>(i,j)[1]=value;
                 image.at<Vec3b>(i,j)[2]=value;
                 int contatore = 0;

                for (int I = max(i - 1, 0); I < min(i + 2, image.rows); ++I)
                {
                    for (int J = max(j - 1, 0); J < min(j + 2, image.cols); ++J)
                    {
                        contatore = contatore + image.at<int>(I, J);
                    }
                }
                // Il contatore qui è la somma dei valori dei vicini
                // imgGray.at<Vec3b>(i,j)= contatore;


                
            }
        }

        // cvtColor(image,imgGray,COLOR_BGR2GRAY);
        imshow("Display Video", image);
        waitKey(10);
    }
    stop = std::chrono::system_clock::now();
    auto musec = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();
    cout << "Microseconds: " << musec << endl;

    return 0;
}