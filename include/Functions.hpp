#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <thread>
#include <ff/ff.hpp>
#include <iostream>
#include <condition_variable>

using namespace std;
using namespace cv;

atomic<int> differentFrames;
float minthreshold = 0.7;

void greyFilter(Mat *image)
{
    Mat bgr[3];
    split(*image, bgr);
    *image = (bgr[0] + bgr[1] + bgr[2]) / 3;
}

void blurFilter(Mat *imgGrey)
{
    Mat imgSmooth = imgGrey->clone();
    for (int i = 0; i < imgGrey->rows; i++)
    {
        for (int j = 0; j < imgGrey->cols; j++)
        {
            short count = 0;
            int value = 0;
            for (int I = max(i - 1, 0); I < min(i + 2, imgGrey->rows); ++I)
            {
                for (int J = max(j - 1, 0); J < min(j + 2, imgGrey->cols); ++J)
                {
                    count++;
                    value = value + (uchar)imgGrey->at<uchar>(I, J);
                }
            }
            imgSmooth.at<uchar>(i, j) = value / count;
        }
    }
    *imgGrey = imgSmooth;
}

int checkMotion(Mat *image, Mat *firstFrame)
{
    if (!image->empty())
    {
        Mat diffImage;
        absdiff(*firstFrame, *image, diffImage);
        float numberDifferentPixels = countNonZero(diffImage); // Pixel equals 0 it means there is no difference between the 2 pixels compared
        float size = diffImage.rows * diffImage.cols;
        float result = numberDifferentPixels / size;
        return result > minthreshold;
    }
    return 0;
}

void applyFilters(Mat *image)
{
    greyFilter(image);
    blurFilter(image);
}

void readImage(Mat *firstframe, bool *endVideo, queue<Mat> *queueImages, condition_variable *condVar, mutex *queueLock)
{
    int localresult = 0;
    while (true)
    {
        Mat image;
        bool enter = false;
        unique_lock<mutex> l(*queueLock);
        while (queueImages->empty() && !*endVideo)
        {
            condVar->wait(l);
        }
        if (queueImages->empty() && *endVideo)
        {
            condVar->notify_one();
            break;
        }
        image = queueImages->front();
        queueImages->pop();
        enter = true;
        l.unlock();
        if (enter && !image.empty())
        {
            applyFilters(&image);
            localresult += checkMotion(&image, firstframe);
        }
    }
    differentFrames += localresult;
}
