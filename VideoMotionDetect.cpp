#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <thread>
#include <ff/ff.hpp>
#include <iostream>

using namespace ff;
using namespace cv;
using namespace std;

chrono::system_clock::time_point start;
chrono::system_clock::time_point stop;
mutex queueLock;
float minthreshold = 0.6;
atomic<int> differentFrames;
Mat firstFrame;
queue<Mat> queueImages;
bool endVideo = false;

Mat calcGrey(Mat &image)
{
    Mat bgr[3];
    split(image, bgr);
    Mat imgGray = (bgr[0] + bgr[1] + bgr[2]) / 3;
    return imgGray;
}

Mat calcSmooth(Mat &imgGrey)
{
    Mat imgSmooth = imgGrey.clone();
    //  blur(imgGrey, imgSmooth, Size(3, 3));
    for (int i = 0; i < imgGrey.rows; i++)
    {
        for (int j = 0; j < imgGrey.cols; j++)
        {
            short count = 0;
            int value = 0;
            for (int I = max(i - 1, 0); I < min(i + 2, imgGrey.rows); ++I)
            {
                for (int J = max(j - 1, 0); J < min(j + 2, imgGrey.cols); ++J)
                {
                    count++;
                    value = value + (uchar)imgGrey.at<uchar>(I, J); // reason of uchar: CV_8U is unsigned 8bit/pixel - ie a pixel can have values 0-255, this is the normal range for most image and video formats.
                }
            }
            imgSmooth.at<uchar>(i, j) = value / count;
        }
    }
    return imgSmooth;
}

void calcImg(Mat &image, Mat &firstFrame)
{
    Mat diffImage;
    image = calcGrey(image);
    image = calcSmooth(image);
    absdiff(firstFrame, image, diffImage);
    float numberDifferentPixels = countNonZero(diffImage); // PIXEL UGUALI A 0 SIGNIFICA CHE SONO UGUALI ALL'IMMAGINE COMPARATA
    float size = diffImage.rows * diffImage.cols;
    float result = numberDifferentPixels / size;
    if (result > minthreshold)
    {
        differentFrames++;
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
            calcImg(image, firstFrame);
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
    int ff = stoi(argv[2]);
    vector<thread> listThreads;

    if (cores >= 1 && ff == 0)
    {
        cout << "CORES Creation" << endl;
        for (int i = 0; i < cores; i++)
        {
            listThreads.push_back(thread(takeImage));
        }
    }

    while (cap.isOpened() && !endVideo)
    {
        Mat image;
        cap.read(image);

        if (image.empty())
        {
            endVideo = true;
            break;
        }
        if (actualFrame == 0)
        {
            firstFrame = image.clone();
            firstFrame = calcGrey(firstFrame);
            firstFrame = calcSmooth(firstFrame);
        }

        if (cores >= 1 && ff == 0)
        {
            cout << "Entra nei cores" <<endl;
            queueImages.push(image);
        }
        else
        {
            if (ff == 1)
            {
                cout << "Entra in fastflow" << endl;

            }
            else
            {
                cout << "sequenziale"  <<endl;
                calcImg(image, firstFrame);
            }
        }
        char key = waitKey(1);

        actualFrame++;
        if (key == 'q')
            return 0;
    }

    if (cores >= 1 && ff == 0)
    {
        for (int i = 0; i < cores; i++)
        {
            listThreads[i].join();
        }
    }

    stop = std::chrono::system_clock::now();

    cout << "Numero di frame diversi: " << differentFrames << endl;
    cout << "numero totale di frames: " << actualFrame << endl;

    auto musec = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
    cout << "Milliseconds: " << musec << endl;

    return 0;
}