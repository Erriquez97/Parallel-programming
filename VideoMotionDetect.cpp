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
    Mat firstFrame;
    Mat diffImage;
    float threshold = 30.0f;
    Mat bgr[3];
    int h3[3][3] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};
    start = std::chrono::system_clock::now();
    int fff = 0;
    cap.read(firstFrame);

    while (true)
    {
        cap.read(image);
        split(image, bgr);                            // Divides a multi-channel array into several single-channel arrays.
        Mat imgGray = (bgr[0] + bgr[1] + bgr[2]) / 3; // matrice con un solo colore, che è quello grigio
        imgSmooth = imgGray.clone();

        blur(imgGray, imgSmooth, Size(6, 6));
        if (fff == 0)
        {
            firstFrame = imgSmooth.clone();
        }

        // for (int i = 0; i < imgGray.rows; i++)
        // {
        //     for (int j = 0; j < imgGray.cols; j++)
        //     {
        //         int contatore = 0;
        //         int value =0;
        //         for (int I = max(i - 1, 0); I < min(i + 2, imgGray.rows); ++I)
        //         {
        //             for (int J = max(j - 1, 0); J < min(j + 2, imgGray.cols); ++J)
        //             {
        //                 contatore++;
        //                 Vec3b bgrPixel = imgGray.at<Vec3b>(I, J);
        //                 value = value + (int) bgrPixel[0]; // prendo solo il primo perché essendo l'immagine grigia tutti e 3 i canali hanno lo stesso valore
        //             }
        //         }
        //         imgSmooth.at<Vec3b>(i,j)[0]= value/contatore;
        //         imgSmooth.at<Vec3b>(i,j)[1]= value/contatore;
        //         imgSmooth.at<Vec3b>(i,j)[2]= value/contatore;

        //     }

        // }

        absdiff(firstFrame, imgSmooth, diffImage);
        float numberDifferentPixels = countNonZero(diffImage); // PIXEL UGUALI A 0 SIGNIFICA CHE SONO UGUALI ALL'IMMAGINE COMPARATA
        float grandezza = diffImage.rows * diffImage.cols;
        float risultato= numberDifferentPixels /grandezza;
        cout << "NUMERO DI PIXEL DIVERSI: " << numberDifferentPixels << endl;
        cout << "GRANDEZZA FRAME: " << grandezza <<endl;
        cout << "PERCENNTUALE DIVERSA : " << risultato *100 << endl;

        // int numPixel=0;
        // int numPixel0=0;
        // for(int i=0; i< diffImage.rows; i++){
        //     for(int j=0; j<diffImage.cols; j++){
        //         if(diffImage.at<int>(i,j)==0){
        //             numPixel0++;
        //         }
        //         numPixel++;
        //         // cout << "  NUMERO DI PIXEL: " << (ushort) diffImage.at<Vec3b>(i,j)[0] <<endl;

        //     }
        // }
        // cout << "NUMERO DI PIXEL: " << numPixel <<endl;
        // cout << "NUMERO DI PIXEL DIVERSI: "<< numPixel0 <<endl;

        // cout << "VALORE SMOOTH: " << (int)imgSmooth.at<Vec3b>(23, 23)[0] << endl;
        // cout << "VALORE GRIGIA: " << (int)imgGray.at<Vec3b>(23, 23)[0] << endl;
        fff++;
        imshow("Display Video", diffImage);
        char key = waitKey(100);
        if (key == 'q')
            return 0;
    }
    stop = std::chrono::system_clock::now();
    auto musec = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();
    cout << "Microseconds: " << musec << endl;
    return 0;
}