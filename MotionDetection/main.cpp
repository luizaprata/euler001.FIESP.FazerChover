//============================================================================
// Name        : MotionDetection.cpp
// Author      : Luiza Prata
// Version     :
// Copyright   : LuizaPSPassos
// Description : freenect+opencv
//============================================================================

#include "libfreenect/libfreenect.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <pthread.h>
#include <opencv2/opencv.hpp>


using namespace cv;
using namespace std;

class myMutex
{
public:
    myMutex()
    {
        pthread_mutex_init(&m_mutex, NULL);
    }
    void lock()
    {
        pthread_mutex_lock(&m_mutex);
    }
    void unlock()
    {
        pthread_mutex_unlock(&m_mutex);
    }
private:
    pthread_mutex_t m_mutex;
};

class MyFreenectDevice: public Freenect::FreenectDevice
{
public:
    MyFreenectDevice(freenect_context *_ctx, int _index) :
    Freenect::FreenectDevice(_ctx, _index), m_buffer_depth(
                                                           FREENECT_DEPTH_11BIT), m_buffer_rgb(FREENECT_VIDEO_RGB), m_gamma(
                                                                                                                            2048), m_new_rgb_frame(false), m_new_depth_frame(false), depthMat(
                                                                                                                                                                                              Size(640, 480), CV_16UC1), rgbMat(Size(640, 480), CV_8UC3,
                                                                                                                                                                                                                                Scalar(0)), ownMat(Size(640, 480), CV_8UC3, Scalar(0))
    {
        
        for (unsigned int i = 0; i < 2048; i++)
        {
            float v = i / 2048.0;
            v = std::pow(v, 3) * 6;
            m_gamma[i] = v * 6 * 256;
        }
    }
    
    // Do not call directly even in child
    void VideoCallback(void* _rgb, uint32_t timestamp)
    {
        //		std::cout << "RGB callback" << std::endl;
        m_rgb_mutex.lock();
        uint8_t* rgb = static_cast<uint8_t*>(_rgb);
        rgbMat.data = rgb;
        m_new_rgb_frame = true;
        m_rgb_mutex.unlock();
    }
    ;
    
    // Do not call directly even in child
    void DepthCallback(void* _depth, uint32_t timestamp)
    {
        //		std::cout << "Depth callback" << std::endl;
        m_depth_mutex.lock();
        uint16_t* depth = static_cast<uint16_t*>(_depth);
        depthMat.data = (uchar*) depth;
        m_new_depth_frame = true;
        m_depth_mutex.unlock();
    }
    
    bool getVideo(Mat& output)
    {
        m_rgb_mutex.lock();
        if (m_new_rgb_frame)
        {
            cv::cvtColor(rgbMat, output, CV_RGB2BGR);
            m_new_rgb_frame = false;
            m_rgb_mutex.unlock();
            return true;
        }
        else
        {
            m_rgb_mutex.unlock();
            return false;
        }
    }
    
    bool getDepth(Mat& output)
    {
        m_depth_mutex.lock();
        if (m_new_depth_frame)
        {
            depthMat.copyTo(output);
            m_new_depth_frame = false;
            m_depth_mutex.unlock();
            return true;
        }
        else
        {
            m_depth_mutex.unlock();
            return false;
        }
    }
private:
    std::vector<uint8_t> m_buffer_depth;
    std::vector<uint8_t> m_buffer_rgb;
    std::vector<uint16_t> m_gamma;
    Mat depthMat;
    Mat rgbMat;
    Mat ownMat;
    
    
    
    myMutex m_rgb_mutex;
    myMutex m_depth_mutex;
    bool m_new_rgb_frame;
    bool m_new_depth_frame;
};

int main(int argc, char **argv)
{
    bool die(false);
    string filename("snapshot");
    string suffix(".png");
    int i_snap(0);
    int freenect_angle(0);
    int current_freenect_angle(-1);
    
    Mat depthMat(Size(640, 480), CV_16UC1);
    Mat depthf(Size(640, 480), CV_8UC1);
    Mat rgbMat(Size(640, 480), CV_8UC3, Scalar(0));
    Mat ownMat(Size(640, 480), CV_8UC3, Scalar(0));
    
    Mat oldDepth;
    Mat differenceImage;
    
    // The next two lines must be changed as Freenect::Freenect
    // isn't a template but the method createDevice:
    // Freenect::Freenect<MyFreenectDevice> freenect;
    // MyFreenectDevice& device = freenect.createDevice(0);
    // by these two lines:
    
    Freenect::Freenect freenect;
    MyFreenectDevice& device = freenect.createDevice<MyFreenectDevice>(0);
    
    namedWindow("rgb", CV_WINDOW_AUTOSIZE);
    namedWindow("depth", CV_WINDOW_AUTOSIZE);
    namedWindow("diff", CV_WINDOW_AUTOSIZE);
    device.startVideo();
    device.startDepth();
    std::ostringstream file;
    int totalWhite = 0;
    int i = 0;
    int j = 0;
    double perc = 0;
    int rows = 0;
    int cols = 0;
    
    double progress = 0;
    double w = 640;
    
    //	sf::Clock Clock;
    //	while (Clock.GetElapsedTime() < 5.f)
    //	{
    //		std::cout << Clock.GetElapsedTime() << std::endl;
    //		sf::Sleep(0.5f);
    //	}
    //
    //	return 0;
    //
    //
    ////	sf::RenderWindow window(sf::VideoMode(640, 480), "SFML Application");
    ////	sf::CircleShape shape;
    ////	shape.setRadius(40.f);
    ////	shape.setPosition(100.f, 100.f);
    ////	shape.setFillColor(sf::Color::Cyan);
    ////	while (window.isOpen())
    ////	{
    ////		sf::Event event;
    ////		while (window.pollEvent(event))
    ////		{
    ////			if (event.type == sf::Event::Closed)
    ////				window.close();
    ////		}
    ////		window.clear();
    ////		window.draw(shape);
    ////		window.display();
    ////	}
    //	return 0;
    
    while (!die)
    {
        device.getVideo(rgbMat);
        device.getDepth(depthMat);
        
        depthMat.convertTo(depthf, CV_8UC1, 255.0 / 2024.0);
        
        cv::imshow("depth", depthf);
        if (oldDepth.empty() == 0) {
            cv::absdiff(oldDepth, depthf, differenceImage);
            cv::imshow("diff", differenceImage);
        }
        
        rows = differenceImage.rows;
        cols = differenceImage.cols;
        
        if (differenceImage.channels() == 1) {
            totalWhite = 0;
            for( i = 0; i < rows; ++i){
                for( j = 0; j < cols; ++j ){
                    if (differenceImage.at<uchar>(i,j) > 0){
                        totalWhite++;
                    }
                }
            }
            
            if (totalWhite > 0) {
                perc = cols*rows;
                perc = totalWhite/perc;
                progress += perc;
            }
        }
        w = 640*progress/100;
        //cout << progress << " = "<< w <<endl;
        rectangle( rgbMat,
                  Point( 0, 240 ),
                  Point( w, 244),
                  Scalar( 0, 255, 255 ),
                  -1,
                  8 );
        
        if (w > 640){
            progress = 0;
        }
        
        
        cv::imshow("rgb", rgbMat);
        
        depthMat.convertTo(oldDepth, CV_8UC1, 255.0 / 2024.0);
        
        switch (waitKey(10))
        {
            case 27:// ESC // exit
                cvDestroyWindow("rgb");
                cvDestroyWindow("depth");
                die = true;
                break;
            case 116: 	//'t' snapshot
                file.str("");
                file.clear();
                file << filename << i_snap << suffix;
                cv::imwrite(file.str(), rgbMat);
                i_snap++;
                break;
            case 63232: // UP
                cout << "UP" << endl;
                freenect_angle++;
                if (freenect_angle > 30) {
                    freenect_angle = 30;
                }
                break;
            case 114: // r
                progress = 0;
                break;
            case 63233: // down
                freenect_angle--;
                if (freenect_angle < -30) {
                    freenect_angle = -30;
                }
                
                break;
        }
        
        //		cout << "freenect_angle" << waitKey(10) << endl;
        if (current_freenect_angle != freenect_angle){
            device.setTiltDegrees(freenect_angle);
            current_freenect_angle = freenect_angle;
        }
        
    }
    
    
    
    device.stopVideo();
    device.stopDepth();
    return 0;
}
