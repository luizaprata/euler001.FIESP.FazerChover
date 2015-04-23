//============================================================================
// Name        : MotionDetection.cpp
// Author      : Luiza Prata
// Version     :
// Copyright   : LuizaPSPassos
// Description : freenect+opencv
//============================================================================

#include <iostream>
#include <vector>
#include <pthread.h>

#include "libfreenect/libfreenect.hpp"



#include <SFML/Graphics.hpp>
#include "ResourcePath.hpp"
#include "AnimatedSprite.hpp"

#include <opencv2/opencv.hpp>

using namespace cv;



////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(1024, 768), "Fazer Chover");
    window.setFramerateLimit(10);
    
    // Set the Icon
    sf::Image icon;
    if (!icon.loadFromFile(resourcePath() + "icon.png")) {
        return -1;
    }
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    
    
    
    // Load a sprite to display
    sf::Texture texture;
    if (!texture.loadFromFile(resourcePath() + "bg_deserto.png")) {
        std::cout << "fail load texture deserto" << std::endl;
        return -1;
    }
    sf::Sprite sprite(texture);
    
    Animation walkingAnimationDown;
    walkingAnimationDown.setSpriteSheet(texture);
    walkingAnimationDown.addFrame(sf::IntRect(32, 0, 32, 32));
    walkingAnimationDown.addFrame(sf::IntRect(64, 0, 32, 32));
    walkingAnimationDown.addFrame(sf::IntRect(32, 0, 32, 32));
    walkingAnimationDown.addFrame(sf::IntRect( 0, 0, 32, 32));
    Animation* currentAnimation = &walkingAnimationDown;
    // set up AnimatedSprite
    AnimatedSprite animatedSprite(sf::seconds(0.2), true, false);
    animatedSprite.setPosition(sf::Vector2f(1024,768));
    sf::Clock frameClock;
    
    //kinect
    Freenect::Freenect freenect;
    MyFreenectDevice& device = freenect.createDevice<MyFreenectDevice>(0);
    device.startVideo();
    device.startDepth();
    
    //keyboard control
    std::ostringstream file;
    string filename("snapshot");
    string suffix(".png");
    int i_snap(0);
    int freenect_angle(0);
    int current_freenect_angle(-1);
    
    
    //moviment sensor
    Mat depthMat(Size(640, 480), CV_16UC1);
    Mat depthf(Size(640, 480), CV_8UC1);
    Mat rgbMat(Size(640, 480), CV_8UC3, Scalar(0));
    Mat ownMat(Size(640, 480), CV_8UC3, Scalar(0));
    Mat oldDepth;
    Mat differenceImage;
    int totalWhite = 0;
    int i = 0;
    int j = 0;
    double perc = 0;
    int rows = 0;
    int cols = 0;
    bool debugMode = false;
    double progress = 0;
    double w = 640;
    
    //while
    while (window.isOpen())
    {
        device.getVideo(rgbMat);
        device.getDepth(depthMat);
        depthMat.convertTo(depthf, CV_8UC1, 255.0 / 2024.0);
        if (debugMode) {
            cv::imshow("depth", depthf);
        }
        if (oldDepth.empty() == 0) {
            cv::absdiff(oldDepth, depthf, differenceImage);
            if (debugMode) {
                cv::imshow("diff", differenceImage);
            }
        }
        depthMat.convertTo(oldDepth, CV_8UC1, 255.0 / 2024.0);
        
        //animation
        animatedSprite.play(*currentAnimation);
        
        //contador da diferenca
        if (differenceImage.channels() == 1) {
            rows = differenceImage.rows;
            cols = differenceImage.cols;
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
        
        //std::cout << progress << std::endl;
        
        
        //show barra progresso
        if (debugMode) {
            w = 640*progress/100;
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
        }
        
        // Clear screen
        window.clear();
        
        // Update the window
        window.display();
        
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            
            // Escape pressed: exit
            if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code){
                    case sf::Keyboard::Escape:// ESC // exit
                        window.close();
                        break;
                    
                    case sf::Keyboard::M: 	//show/hide mask'
                        break;
                    
                    case sf::Keyboard::T: 	//snapshot
                        file.str("");
                        file.clear();
                        file << filename << i_snap << suffix;
                        cv::imwrite(file.str(), rgbMat);
                        i_snap++;
                        break;

                    case sf::Keyboard::R: // reset
                        progress = 0;
                        break;
                    
                    case sf::Keyboard::D: //'d' has been pressed. this will debug mode
                        debugMode = !debugMode;
                        if (debugMode == false){
                            std::cout<<"Debug mode disabled."<<std::endl;
                            cvDestroyWindow("rgb");
                            cvDestroyWindow("depth");
                            cvDestroyWindow("diff");
                        } else {
                            std::cout<<"Debug mode enabled."<<std::endl;
                            namedWindow("rgb", CV_WINDOW_AUTOSIZE);
                            namedWindow("depth", CV_WINDOW_AUTOSIZE);
                            namedWindow("diff", CV_WINDOW_AUTOSIZE);
                        }
                        break;
                        
                    case sf::Keyboard::Up: // UP
                        freenect_angle++;
                        if (freenect_angle > 30) {
                            freenect_angle = 30;
                        }
                        break;
                        
                    case sf::Keyboard::Down: // down
                        freenect_angle--;
                        if (freenect_angle < -30) {
                            freenect_angle = -30;
                        }
                        break;
                }
                //cout << "waitKey" << waitKey(10) << endl;
                if (current_freenect_angle != freenect_angle){
                    device.setTiltDegrees(freenect_angle);
                    current_freenect_angle = freenect_angle;
                }
            }
        }
    }
    
    device.stopVideo();
    device.stopDepth();
    return 0;
}
