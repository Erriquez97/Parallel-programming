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
    emitter(VideoCapture *cap) : cap(cap){}
    Mat *svc(Mat *image)
    {
        while (cap->isOpened())
        {
            Mat *image = new Mat();
            cap->read(*image);
            if (image->empty())
            {
                return (EOS);
            }
            ff_send_out(image);
        }
        return (EOS);
    }
};

class worker : public ff_node_t<Mat>
{
private:
    Mat firstframe;
    int localresult = 0;

public:
    worker(Mat background) : firstframe(background) {}
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
    unsigned short ondemand = atoi(argv[3]);
    string path = (argv[4]);
    vector<thread> listThreads;
    VideoCapture cap(path);
    bool endVideo = false;
    queue<Mat> queueImages;
    condition_variable condVar;
    mutex queueLock;
    Mat background;
    cap.read(background);
    applyFilters(&background);
    int numberFrames = cap.get(CAP_PROP_FRAME_COUNT);

    if (cores >= 1 && ff == 0) // PARALLEL COMPUTATION
    {
        for (unsigned short i = 0; i < cores; i++)
        {
            listThreads.push_back(thread(readImage, &background, &endVideo, &queueImages, &condVar, &queueLock));
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
        }
        for (unsigned short i = 0; i < cores; i++)
        {
            listThreads[i].join();
        }
    }
    else
    {
        if (ff == 1) // FASTFLOW COMPUTATION
        {
            emitter s1(&cap);
            vector<unique_ptr<ff_node>> W;
            for (unsigned short i = 0; i < cores; i++)
            {
                W.push_back(make_unique<worker>(background));
            }
            ff_Farm<Mat> fa(move(W));
            fa.add_emitter(s1);
            fa.remove_collector();
            if (ondemand == 1)
            {
                fa.set_scheduling_ondemand();
            }
            fa.run_and_wait_end();
        }
        else // SEQUENTIAL COMPUTATION
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
            }
        }
    }
    stop = std::chrono::system_clock::now();
    auto musec = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();
    cout << "Completion time: " << musec << endl;
    cout << "Numero frame diversi: " << differentFrames << endl;
    cout << "Numero totale frames: " << numberFrames << endl;

    return 0;
}
