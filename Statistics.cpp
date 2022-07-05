#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <thread>
#include <ff/ff.hpp>
#include <iostream>
#include <condition_variable>
#include "Functions.hpp"

using namespace ff;
using namespace cv;
using namespace std;

chrono::system_clock::time_point start;
chrono::system_clock::time_point stop;
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

//  FASTFLOW

class emitter : public ff_monode_t<Mat>
{
private:
    VideoCapture *cap;
    bool *endVideo;

public:
    emitter(VideoCapture *cap, bool *endVideo) : cap(cap), endVideo(endVideo) {}
    Mat *svc(Mat *image)
    {
        while (cap->isOpened() && !*endVideo)
        {
            Mat *image = new Mat();
            cap->read(*image);
            if (image->empty())
            {
                *endVideo = true;
                return (EOS);
            }
            ff_send_out(image);
        }
        return (EOS);
    }
};

class calculate : public ff_node_t<Mat>
{
private:
    Mat firstframe;
    int localresult = 0;

public:
    calculate(Mat background) : firstframe(background) {}
    Mat *svc(Mat *image)
    {
        greyFilter(image);
        blurFilter(image);
        localresult += checkMotion(image, &firstframe);
        return (GO_ON);
    }
    void svc_end()
    {
        differentFrames += localresult;
    }
};

// END FASTFLOW

int main(int argc, char **argv)
{
    unsigned short cores = atoi(argv[1]);
    unsigned short ff = atoi(argv[2]);
    unsigned short showVideo= atoi(argv[3]);
    string path = (argv[4]);
    int actualFrame = 1;
    vector<thread> listThreads;
    VideoCapture cap(path);
    bool endVideo = false;
    queue<Mat> queueImages;
    condition_variable condVar;
    mutex queueLock;

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

    if (cores >= 1 && ff == 0) // COMPUTAZIONE PARALLELA
    {
        for (unsigned short i = 0; i < cores; i++)
        {
            listThreads.push_back(thread(takeImage, &background, &endVideo, &queueImages, &condVar, &queueLock));
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
            startPush = std::chrono::system_clock::now();
            unique_lock<mutex> l(queueLock);
            queueImages.push(image);
            l.unlock();
            condVar.notify_one();
            stopPush = std::chrono::system_clock::now();
            auto musecPush = std::chrono::duration_cast<std::chrono::microseconds>(stopPush - startPush).count();
            resultPush += musecPush;
            actualFrame++;
        }

        for (unsigned short i = 0; i < cores; i++)
        {
            listThreads[i].join();
        }
    }
    else
    {
        if (ff == 1) // COMPUTAZIONE CON FASTFLOW
        {
            emitter s1(&cap, &endVideo);
            vector<unique_ptr<ff_node>> W;
            for (unsigned short i = 0; i < cores; i++)
            {
                W.push_back(make_unique<calculate>(background));
            }
            ff_Farm<Mat> fa(move(W));
            fa.add_emitter(s1);
            fa.remove_collector();
            fa.run_and_wait_end();
            fa.ffStats(cout);
        }
        else // COMPUTAZIONE SEQUENZIALE
        {
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

                if(showVideo==1){
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
                differentFrames+=result;
                actualFrame++;
            }
        }
    }

    averageGrey = resultGrey / actualFrame;
    averageBlur = resultBlur / actualFrame;
    averageMotion = resultMotion / actualFrame;
    averageRead = resultRead / actualFrame;
    averagepush = resultPush / actualFrame;

    if (cores == 0)
    {
        cout << averageRead << endl;
        // cout <<"Average Grey filter: "<< averageGrey << endl;
        // cout <<"Average Blur filter: " << averageBlur << endl;
        // cout <<"Average Motion: " <<averageMotion << endl;
    }
    if (cores >= 1 && ff == 0)
    {
        cout << averagepush << endl;
    }
    // cout << "Numero frame diversi: " << differentFrames << endl;
    // cout << "numero totale frames: " << actualFrame << endl;
    return 0;
}
