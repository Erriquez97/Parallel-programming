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
        applyFilters(image);
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
    start = std::chrono::system_clock::now();
    unsigned short cores = atoi(argv[1]);
    unsigned short ff = atoi(argv[2]);
    string path = (argv[3]);
    int actualFrame = 1;
    vector<thread> listThreads;
    VideoCapture cap(path);
    bool endVideo = false;
    queue<Mat> queueImages;
    condition_variable condVar;
    mutex queueLock;
    Mat background;
    cap.read(background);
    applyFilters(&background);

    if (cores >= 1 && ff == 0) // COMPUTAZIONE PARALLELA
    {
        for (unsigned short i = 0; i < cores; i++)
        {
            listThreads.push_back(thread(takeImage, &background, &endVideo, &queueImages, &condVar,&queueLock));
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
            unique_lock<mutex> l(queueLock);
            queueImages.push(image);
            l.unlock();
            condVar.notify_one();
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
        }
        else // COMPUTAZIONE SEQUENZIALE
        {
            while (cap.isOpened() && !endVideo)
            {
                Mat image;
                cap.read(image);
                int result = 0;
                if (image.empty())
                {
                    endVideo = true;
                    break;
                }
                applyFilters(&image);
                differentFrames += checkMotion(&image, &background);
                actualFrame++;
            }
        }
    }
    stop = std::chrono::system_clock::now();
    auto musec = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();
    cout << musec << endl;
    cout << "Numero frame diversi: " << differentFrames << endl;
    cout << "numero totale frames: " << actualFrame << endl;

    // DA FARE è MISURARE POI DA QUANDO FACCIO A PUSH A QUANDO TERMINO DI FARE LA PUSH
    return 0;
}
