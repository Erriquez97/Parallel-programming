#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <thread>
#include <ff/ff.hpp>
#include <iostream>
#include <condition_variable>

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
mutex queueLock;
float minthreshold = 0.7;
atomic<int> differentFrames;
queue<Mat> queueImages;
bool endVideo = false;
condition_variable condVar;
string path = "../Resources/Video720p.mp4";
VideoCapture cap(path);

int greytotal=0;
int timescalledGrey=0;

void greyFilter(Mat *image)
{
    timescalledGrey++;
    startGrey = std::chrono::system_clock::now();
    Mat bgr[3];
    split(*image, bgr);
    Mat imgGray = (bgr[0] + bgr[1] + bgr[2]) / 3;
    *image = imgGray;
    stopGrey = std::chrono::system_clock::now();
    auto musecGrey = std::chrono::duration_cast<std::chrono::microseconds>(stopGrey - startGrey).count();
    greytotal+=musecGrey;
}

void blurFilter(Mat *imgGrey)
{
    startBlur = std::chrono::system_clock::now();
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
    stopBlur = std::chrono::system_clock::now();
}

int checkMotion(Mat *image, Mat *firstFrame)
{
   
    if (!image->empty())
    {
        Mat diffImage;
        greyFilter(image);
        blurFilter(image);
        absdiff(*firstFrame, *image, diffImage);
        float numberDifferentPixels = countNonZero(diffImage); // PIXEL UGUALI A 0 SIGNIFICA CHE SONO UGUALI ALL'IMMAGINE COMPARATA
        float size = diffImage.rows * diffImage.cols;
        float result = numberDifferentPixels / size;
        return result > minthreshold;
    }
   
    return 0;
}

Mat getBackground(Mat *image)
{
    greyFilter(image);
    blurFilter(image);
    return *image;
}

void takeImage(Mat *firstframe)
{
    int localresult = 0;
    while (true)
    {
        Mat image;
        bool enter = false;
        unique_lock<mutex> l(queueLock);
        while (queueImages.empty() && !endVideo)
        {
            condVar.wait(l);
        }
        if (queueImages.empty() && endVideo)
        {
            condVar.notify_one();
            break;
        }
        image = queueImages.front();
        queueImages.pop();
        enter = true;
        l.unlock();
        if (enter && !image.empty())
            localresult += checkMotion(&image, firstframe);
    }
    differentFrames += localresult;
}

//  FASTFLOW

class emitter : public ff_monode_t<Mat>
{

public:
    emitter() {}

    Mat *svc(Mat *imageaa)
    {
        startRead = std::chrono::system_clock::now();
        while (cap.isOpened() && !endVideo)
        {
            Mat *image = new Mat();
            cap.read(*image);
            if (image->empty())
            {
                endVideo = true;
                return (EOS);
            }
            ff_send_out(image);
        }
        stopRead = std::chrono::system_clock::now();
        return (EOS);
    }
};

class calculate : public ff_node_t<Mat>
{
private:
    Mat firstframe;
    int localresult = 0;

public:
    calculate(Mat firstframe) : firstframe(firstframe) {}
    Mat *svc(Mat *image)
    {
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
    int cores = stoi(argv[1]);
    int ff = stoi(argv[2]);
    Mat firstFrame;
    int actualFrame = 0;
    vector<thread> listThreads;

    if (cores >= 1 && ff == 0) // COMPUTAZIONE PARALLELA
    {
        for (int i = 0; i < cores; i++)
        {
            listThreads.push_back(thread(takeImage, &firstFrame));
        }
        startRead = std::chrono::system_clock::now();
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
                firstFrame = getBackground(&image);
            }
            else
            {
                unique_lock<mutex> l(queueLock);
                queueImages.push(image);
                l.unlock();
                condVar.notify_one();
            }
            actualFrame++;
        }
        stopRead = std::chrono::system_clock::now();
        for (int i = 0; i < cores; i++)
        {
            listThreads[i].join();
        }
    }
    else
    {
        if (ff == 1) // COMPUTAZIONE CON FASTFLOW
        {   
            Mat image;
            cap.read(image);
            firstFrame = getBackground(&image);
            emitter s1;
            vector<unique_ptr<ff_node>> W;
            for(int i=0; i<cores;i++){
                W.push_back(make_unique<calculate>(firstFrame));
            }
            ff_Farm<Mat>fa(move(W));
            fa.add_emitter(s1);
            fa.remove_collector();
            fa.run_and_wait_end();
            fa.ffStats(cout);
        }
        else // COMPUTAZIONE SEQUENZIALE
        {
            while (cap.isOpened() && !endVideo)
            {
                Mat image;
                cap.read(image);
                int result=0;
                if (image.empty())
                {
                    endVideo = true;
                    break;
                }
                if (actualFrame == 0)
                {
                    firstFrame = getBackground(&image);
                }
                else
                {
                    startMotion = std::chrono::system_clock::now();
                    result = checkMotion(&image, &firstFrame);
                    stopMotion = std::chrono::system_clock::now();
                    differentFrames+= result;
                }
                // if(result==1){
                //     cout<<"MOTION: "<< actualFrame<<endl;
                // }
                // if(!image.empty()){
                //     imshow("Display video", image);
                // }
                
                // auto musecBlur = std::chrono::duration_cast<std::chrono::microseconds>(stopBlur - startBlur).count();
                // auto musecMotion = std::chrono::duration_cast<std::chrono::microseconds>(stopMotion - startMotion).count();
                // cout << musecGrey << endl;
                // cout << "Time Blur: " << musecBlur << endl;
                // cout << "Time Motion: " << musecMotion << endl;
                actualFrame++; 
                // char key = waitKey(1);
                // if (key == 'q')
                //     return 0;
            }
        }
    }
    int finalGrey= greytotal/timescalledGrey;
    cout << "GREY FINAL AVERAGE: "  << finalGrey<<endl;
    stop = std::chrono::system_clock::now();
    // auto musec = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
    // auto musecRead = std::chrono::duration_cast<std::chrono::milliseconds>(stopRead - startRead).count();
    // cout << "Numero frame diversi: " << differentFrames << endl;
    // cout << "numero totale frames: "  << actualFrame <<endl;
    // cout << "Read Time: " << musecRead << endl;
    // cout << "Completion Time: " << musec << endl;
    return 0;
}
