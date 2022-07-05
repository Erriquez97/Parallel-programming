#pragma once
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
    Mat imgGray = (bgr[0] + bgr[1] + bgr[2]) / 3;
    *image = imgGray;
}

void blurFilter(Mat *imgGrey)
{
    Mat imgSmooth = imgGrey->clone();
    //   blur(*imgGrey, imgSmooth, Size(3, 3));
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
                    value = value + (uchar)imgGrey->at<uchar>(I, J); // reason of uchar: CV_8U is unsigned 8bit/pixel - ie a pixel can have values 0-255, this is the normal range for most image and video formats.
                }
            }
            imgSmooth.at<uchar>(i, j) = value / count;
        }
    }
    *imgGrey = imgSmooth;
    // auto musecBlur = std::chrono::duration_cast<std::chrono::microseconds>(stopBlur - startBlur).count();
    // blurtotal += musecBlur;
}

int checkMotion(Mat *image, Mat *firstFrame)
{
    if (!image->empty())
    {
        Mat diffImage;
        // greyFilter(image);
        // blurFilter(image);
        absdiff(*firstFrame, *image, diffImage);
        float numberDifferentPixels = countNonZero(diffImage); // PIXEL UGUALI A 0 SIGNIFICA CHE SONO UGUALI ALL'IMMAGINE COMPARATA
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

void takeImage(Mat *firstframe, bool *endVideo, queue<Mat> *queueImages, condition_variable *condVar, mutex *queueLock)
{
    int localresult = 0;
    while (true)
    {
        Mat image;
        bool enter = false;
        unique_lock<mutex> l(*queueLock);
        while (queueImages->empty() && !*endVideo)
        {
            condVar->wait(l);  // quando aspetto rilascio il lock e quando vengo notificato riacquisisco il lock
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
