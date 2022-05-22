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
    Mat bgr[3];

    start = std::chrono::system_clock::now();

    while (true)
    {
        cap.read(image);
        split(image,bgr);
        // Mat bigCube(image.rows,image.cols , CV_8U, Scalar::all(0));   // crea una matrice che è tutta nera, infatti alla fine fa scalar all 0, cioè applica a tutte le celle il valore 0 che è il nero
        Mat greyColor = (bgr[0]+bgr[1]+bgr[2])/3; // matrice con un solo colore, che è quello grigio
        Mat container[3] = {greyColor,greyColor,greyColor}; // in questo modo creiamo una matrice container che ha 3 canali(colori) che hanno lo stesso valore
        merge(container,3,imgGray); // in quessto modo andiamo a creare l'immagine imgGray che è formata dalle matrici che sono in container
        // imgSmooth=imgGray.clone();
        
        for (int i = 0; i < image.rows; i++)
        {
            for (int j = 0; j < image.cols; j++)
            {
                 int contatore = 0;
                for (int I = max(i - 1, 0); I < min(i + 2, image.rows); ++I)
                {
                    for (int J = max(j - 1, 0); J < min(j + 2, image.cols); ++J)
                    {
                    }
                }  
            }
        }
        // cvtColor(image,imgGray,COLOR_BGR2GRAY);
        imshow("Display Video", imgGray);
        char key= waitKey(1);
        if( key == 'q')
            return 0;
    }
    stop = std::chrono::system_clock::now();
    auto musec = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();
    cout << "Microseconds: " << musec << endl;

    return 0;
}