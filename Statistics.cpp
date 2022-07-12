#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <iostream>
#include "Functions.hpp"

using namespace cv;
using namespace std;

chrono::system_clock::time_point startRead;
chrono::system_clock::time_point stopRead;
chrono::system_clock::time_point startGrey;
chrono::system_clock::time_point stopGrey;
chrono::system_clock::time_point startBlur;
chrono::system_clock::time_point stopBlur;
chrono::system_clock::time_point startMotion;
chrono::system_clock::time_point stopMotion;
chrono::system_clock::time_point startPush;
chrono::system_clock::time_point stopPush;

int main(int argc, char **argv)
{
    unsigned short showVideo = atoi(argv[1]);
    string path = (argv[2]);
    int actualFrame = 1;

    VideoCapture cap(path);
    bool endVideo = false;

    int resultGrey = 0;
    int averageGrey = 0;

    int resultBlur = 0;
    int averageBlur = 0;

    int resultMotion = 0;
    int averageMotion = 0;

    int resultRead = 0;
    int averageRead = 0;

    int resultPush = 0;
    int averagepush = 0;

    Mat background;
    cap.read(background);
    applyFilters(&background);

    while (cap.isOpened() && !endVideo)
    {
        startRead = std::chrono::system_clock::now();
        Mat image;
        cap.read(image);
        stopRead = std::chrono::system_clock::now();
        auto musecRead = std::chrono::duration_cast<std::chrono::microseconds>(stopRead - startRead).count();
        resultRead += musecRead;

        int result = 0;
        if (image.empty())
        {
            endVideo = true;
            break;
        }
        startGrey = std::chrono::system_clock::now();
        greyFilter(&image);
        stopGrey = std::chrono::system_clock::now();

        auto musecGrey = std::chrono::duration_cast<std::chrono::microseconds>(stopGrey - startGrey).count();
        resultGrey += musecGrey;

        startBlur = std::chrono::system_clock::now();
        blurFilter(&image);
        stopBlur = std::chrono::system_clock::now();

        auto musecBlur = std::chrono::duration_cast<std::chrono::microseconds>(stopBlur - startBlur).count();
        resultBlur += musecBlur;

        startMotion = std::chrono::system_clock::now();
        result += checkMotion(&image, &background);
        stopMotion = std::chrono::system_clock::now();

        auto musecMotion = std::chrono::duration_cast<std::chrono::microseconds>(stopMotion - startMotion).count();
        resultMotion += musecMotion;

        if (showVideo == 1)
        {
            if (result == 1)
            {
                cout << "MOTION: " << actualFrame << endl;
            }

            if (!image.empty())
            {
                imshow("Display video: ", image);
            }

            char key = waitKey(1);
            if (key == 'q')
                return 0;
        }
        differentFrames += result;
        actualFrame++;
    }

    averageGrey = resultGrey / actualFrame;
    averageBlur = resultBlur / actualFrame;
    averageMotion = resultMotion / actualFrame;
    averageRead = resultRead / actualFrame;
    averagepush = resultPush / actualFrame;

    cout << "Average read: " << averageRead << endl;
    cout << "Average Grey filter: " << averageGrey << endl;
    cout << "Average Blur filter: " << averageBlur << endl;
    cout << "Average Motion: " << averageMotion << endl;
    cout << "Numero frame diversi: " << differentFrames << endl;
    cout << "numero totale frames: " << actualFrame << endl;
    return 0;
}
