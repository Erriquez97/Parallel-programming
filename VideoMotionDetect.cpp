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





class sharedQueue{


};

chrono::system_clock::time_point start;
chrono::system_clock::time_point stop;
mutex queueLock;
float minthreshold = 0.6;
atomic<int> differentFrames;
Mat firstFrame;
queue<Mat> queueImages;
bool endVideo = false;
condition_variable condVar;

string path = "../Resources/VideoTest2.mp4";
VideoCapture cap(path);
int h3[3][3] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};
int actualFrame = 0;
vector<thread> listThreads;

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
    // blur(imgGrey, imgSmooth, Size(3, 3));
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

int calcImg(Mat &image, Mat &firstFrame)
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
        return 1;
    }
    else
        return 0;
}

void takeImage()
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
            localresult += calcImg(image, firstFrame);
    }
    differentFrames += localresult;
}

class emitter: public ff_monode_t <Mat>{

public:
    emitter(){}

    Mat *svc(Mat * image)
    {
        int contatore=0;
        while (cap.isOpened() && !endVideo)
        {
            Mat image;
            cap.read(image);
            if (image.empty())
            {
                cout << "EMPTY" << endl;
                endVideo = true;
                return(EOS);
            }
            contatore++;
            cout<< "CONTATORE: "<< contatore <<endl;
            ff_send_out(&image);
        }
        return (EOS);
    }
};

class calculate : public ff_node_t <Mat>{

public:
    calculate(){}
    Mat *svc(Mat * image){
        cout << "GOT IT " <<endl;
        // differentFrames+= calcImg(image,firstFrame);
        return(GO_ON);
   }
};

int main(int argc, char **argv)
{
    start = std::chrono::system_clock::now();
    int cores = stoi(argv[1]);
    int ff = stoi(argv[2]);

    if (cores >= 1 && ff == 0) // COMPUTAZIONE PARALLELA
    {
        for (int i = 0; i < cores; i++)
        {
            listThreads.push_back(thread(takeImage));
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
            else
            {
                unique_lock<mutex> l(queueLock);
                queueImages.push(image);
                l.unlock();
                condVar.notify_one();
            }
            char key = waitKey(1);
            actualFrame++;

            if (key == 'q')
                return 0;
        }
        for (int i = 0; i < cores; i++)
        {
            listThreads[i].join();
        }
    }
    else
    {
        if (ff == 1) // COMPUTAZIONE CON FASTFLOW
        {
            emitter s1;
            calculate s2;
            Mat image;
            cap.read(image);
            firstFrame = image.clone();
            firstFrame = calcGrey(firstFrame);
            firstFrame = calcSmooth(firstFrame);

            ff_Pipe<> pipe(s1,s2);
            pipe.run_and_wait_end();
            cout<< "DifferentFrames: " << differentFrames <<endl;



            // farm di fastflow
        }
        else // COMPUTAZIONE SEQUENZIALE
        {
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
                differentFrames += calcImg(image, firstFrame);
                char key = waitKey(1);
                actualFrame++;
                if (key == 'q')
                    return 0;
            }
        }
    }
    stop = std::chrono::system_clock::now();
    auto musec = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
    cout << "Numero frame diversi: " << differentFrames << endl;
    cout << "Milliseconds: " << musec << endl;
    return 0;
}

// Da fare: modificare funzione blur, vedere fastflow