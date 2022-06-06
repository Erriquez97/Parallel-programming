#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <thread>
using namespace cv;
using namespace std;

std::chrono::system_clock::time_point start;
std::chrono::system_clock::time_point stop;
mutex blocco;
mutex queueLock;
float minthreshold = 0.6;
int differentFrames = 0;
Mat firstFrame;
queue<Mat> queueImages;
bool endVideo = false;
bool fine = true;

Mat calcGrey(Mat image)
{
    Mat bgr[3];
    split(image, bgr);
    Mat imgGray = (bgr[0] + bgr[1] + bgr[2]) / 3;
    return imgGray;
}

Mat calcSmooth(Mat imgGrey)
{
    Mat imgSmooth = imgGrey.clone();
    for (int i = 0; i < imgGrey.rows; i++)
    {
        for (int j = 0; j < imgGrey.cols; j++)
        {
            short contatore = 0;
            int value = 0;
            for (int I = max(i - 1, 0); I < min(i + 2, imgGrey.rows); ++I)
            {
                for (int J = max(j - 1, 0); J < min(j + 2, imgGrey.cols); ++J)
                {
                    contatore++;
                    value = value + (uchar)imgGrey.at<uchar>(I, J); // reason of uchar: CV_8U is unsigned 8bit/pixel - ie a pixel can have values 0-255, this is the normal range for most image and video formats.
                }
            }
            imgSmooth.at<uchar>(i, j) = value / contatore;
        }
    }

    return imgSmooth;
}

void calcImg(Mat image, Mat firstFrame, float threshold)
{
    Mat diffImage;
    image = calcGrey(image);
    image = calcSmooth(image);
    absdiff(firstFrame, image, diffImage);
    float numberDifferentPixels = countNonZero(diffImage); // PIXEL UGUALI A 0 SIGNIFICA CHE SONO UGUALI ALL'IMMAGINE COMPARATA
    float grandezza = diffImage.rows * diffImage.cols;
    float risultato = numberDifferentPixels / grandezza;
    if (risultato > threshold)
    {
        blocco.lock();
        differentFrames++;
        blocco.unlock();
    }
}

void takeImage()
{
    while (true)
    {
        Mat image;
        bool enter = false;
        if (queueImages.empty() && endVideo)
        {
            break;
        }

        queueLock.lock();

        if (!queueImages.empty())
        {
            image = queueImages.front();
            queueImages.pop();
            enter = true;
        }
        queueLock.unlock();
        if (enter && !image.empty())
            calcImg(image, firstFrame, minthreshold);
    }
}

int main(int argc, char **argv)
{
    start = std::chrono::system_clock::now();
    string path = "../Resources/VideoTest2.mp4";
    VideoCapture cap(path);
    
    int h3[3][3] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};
    int actualFrame = 0;
    int cores = stoi(argv[1]);
    vector<thread> listThreads;

    if (cores >=1)
    {
        for (int i = 0; i < cores; i++)
        {
            listThreads.push_back(thread(takeImage));
        }
    }

    while (cap.isOpened() && !endVideo)
    {
        Mat image;
        cap.read(image);
        if (actualFrame == 0)
        {
            firstFrame = image.clone();
            firstFrame = calcGrey(firstFrame);
            firstFrame = calcSmooth(firstFrame);
        }

        if (cores >= 1)
        {
            queueImages.push(image);
        }else if(!image.empty()){
            calcImg(image,firstFrame,minthreshold);
        }
        char key = waitKey(1);

        if (!image.empty())
        {
            // imshow("Display video",image);
        }
        else
        {
            endVideo = true;
        }

        actualFrame++;
        if (key == 'q')
            return 0;
    }
    if (cores >= 1)
    {
        for (int i = 0; i < cores; i++)
        {
            listThreads[i].join();
        }
    }

    cout << "Numero di frame diversi: " << differentFrames << endl;
    stop = std::chrono::system_clock::now();
    auto musec = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
    cout << "Milliseconds: " << musec << endl;

    return 0;
}