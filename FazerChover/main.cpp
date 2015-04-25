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

void setCurrentPivot(Animation* animation,sf::Vector2f* pivot) {
    *pivot = animation->getPivot();
    int size = animation->getSize();
    pivot->x /= size;
    pivot->y /= size;
    pivot->x = -std::round(pivot->x/2);
    pivot->y = -std::round(pivot->y);
    
}

int randInt(int min = 0, int max = 1) {
    return (rand()%max) + min;
}


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
    
    
    
    // set up AnimatedSprite
    
    sf::Vector2f maskPosition(34,258);
    
    std::vector<sf::Vector2f> positionVec(4);
    positionVec.at(0) = sf::Vector2f(34,164);
    positionVec.at(1) = sf::Vector2f(90,164);
    positionVec.at(2) = sf::Vector2f(133,164);
    positionVec.at(3) = sf::Vector2f(195,164);
    
    sf::Texture textureDesert;
    if (!textureDesert.loadFromFile(resourcePath() + "bg_deserto.png")) {
        std::cout << "fail load texture deserto" << std::endl;
        return -1;
    }
    
    sf::Sprite bgSprite(textureDesert);
    bgSprite.setPosition(maskPosition);
    
    
    // animation characters
    sf::Texture textureCharacters;
    if (!textureCharacters.loadFromFile(resourcePath() + "characters.png")) {
        std::cout << "fail load texture deserto" << std::endl;
        return -1;
    }
    
    
    std::vector<Animation> currentPivotVec(4);
    std::vector<AnimatedSprite> currentAnimatedSpriteVec(4);
    currentAnimatedSpriteVec.at(0) = AnimatedSprite(sf::seconds(0.2), true, false);
    currentAnimatedSpriteVec.at(1) = AnimatedSprite(sf::seconds(0.2), true, false);
    currentAnimatedSpriteVec.at(2) = AnimatedSprite(sf::seconds(0.2), true, false);
    currentAnimatedSpriteVec.at(3) = AnimatedSprite(sf::seconds(0.2), true, false);
    
    sf::Clock frameClock;
    sf::Time frameTime;
    
    std::vector<Animation> animations(0);
    Animation char01;
    char01.setSpriteSheet(textureCharacters);
    char01.addFrame(sf::IntRect(112,440,38,52));
    char01.addFrame(sf::IntRect(61,440,41,57));
    char01.addFrame(sf::IntRect(10,440,41,57));
    char01.addFrame(sf::IntRect(450,378,38,52));
    animations.push_back(char01);
    Animation char02;
    char02.setSpriteSheet(textureCharacters);
    char02.addFrame(sf::IntRect(402,378,38,52));
    char02.addFrame(sf::IntRect(351,378,41,45));
    char02.addFrame(sf::IntRect(300,378,41,47));
    char02.addFrame(sf::IntRect(252,378,38,52));
    animations.push_back(char02);
    Animation char03;
    char03.setSpriteSheet(textureCharacters);
    char03.addFrame(sf::IntRect(204,378,38,49));
    char03.addFrame(sf::IntRect(156,378,38,49));
    char03.addFrame(sf::IntRect(108,378,38,49));
    char03.addFrame(sf::IntRect(60,378,38,49));
    animations.push_back(char03);
    Animation char04;
    char04.setSpriteSheet(textureCharacters);
    char04.addFrame(sf::IntRect(10,378,40,48));
    char04.addFrame(sf::IntRect(449,314,42,54));
    char04.addFrame(sf::IntRect(399,314,40,54));
    char04.addFrame(sf::IntRect(349,314,40,48));
    animations.push_back(char04);
    Animation char05;
    char05.setSpriteSheet(textureCharacters);
    char05.addFrame(sf::IntRect(301,314,38,52));
    char05.addFrame(sf::IntRect(253,314,38,52));
    char05.addFrame(sf::IntRect(205,314,38,48));
    char05.addFrame(sf::IntRect(157,314,38,48));
    animations.push_back(char05);
    Animation char06;
    char06.setSpriteSheet(textureCharacters);
    char06.addFrame(sf::IntRect(109,314,38,52));
    char06.addFrame(sf::IntRect(61,314,38,52));
    char06.addFrame(sf::IntRect(10,314,41,48));
    char06.addFrame(sf::IntRect(456,252,41,48));
    animations.push_back(char06);
    Animation char07;
    char07.setSpriteSheet(textureCharacters);
    char07.addFrame(sf::IntRect(408,252,38,49));
    char07.addFrame(sf::IntRect(360,252,38,49));
    char07.addFrame(sf::IntRect(312,252,38,49));
    char07.addFrame(sf::IntRect(264,252,38,49));
    animations.push_back(char07);
    Animation char08;
    char08.setSpriteSheet(textureCharacters);
    char08.addFrame(sf::IntRect(214,252,40,48));
    char08.addFrame(sf::IntRect(164,252,40,48));
    char08.addFrame(sf::IntRect(111,252,43,48));
    char08.addFrame(sf::IntRect(58,252,43,48));
    animations.push_back(char08);
    Animation char09;
    char09.setSpriteSheet(textureCharacters);
    char09.addFrame(sf::IntRect(10,252,38,52));
    char09.addFrame(sf::IntRect(459,190,38,52));
    char09.addFrame(sf::IntRect(411,190,38,52));
    char09.addFrame(sf::IntRect(363,190,38,52));
    animations.push_back(char09);
    Animation char10;
    char10.setSpriteSheet(textureCharacters);
    char10.addFrame(sf::IntRect(312,190,41,52));
    char10.addFrame(sf::IntRect(261,190,41,52));
    char10.addFrame(sf::IntRect(210,190,41,52));
    char10.addFrame(sf::IntRect(159,190,41,52));
    animations.push_back(char10);
    Animation char11;
    char11.setSpriteSheet(textureCharacters);
    char11.addFrame(sf::IntRect(111,190,38,52));
    char11.addFrame(sf::IntRect(63,190,38,52));
    char11.addFrame(sf::IntRect(10,190,43,50));
    char11.addFrame(sf::IntRect(447,127,43,50));
    animations.push_back(char11);
    Animation char12;
    char12.setSpriteSheet(textureCharacters);
    char12.addFrame(sf::IntRect(399,127,38,52));
    char12.addFrame(sf::IntRect(351,127,38,52));
    char12.addFrame(sf::IntRect(303,127,38,47));
    char12.addFrame(sf::IntRect(255,127,38,47));
    animations.push_back(char12);
    Animation char13;
    char13.setSpriteSheet(textureCharacters);
    char13.addFrame(sf::IntRect(207,127,38,49));
    char13.addFrame(sf::IntRect(159,127,38,49));
    char13.addFrame(sf::IntRect(111,127,38,53));
    char13.addFrame(sf::IntRect(63,127,38,53));
    animations.push_back(char13);
    Animation char14;
    char14.setSpriteSheet(textureCharacters);
    char14.addFrame(sf::IntRect(10,127,43,48));
    char14.addFrame(sf::IntRect(417,63,43,48));
    char14.addFrame(sf::IntRect(364,63,43,54));
    char14.addFrame(sf::IntRect(311,63,43,54));
    animations.push_back(char14);
    Animation charAvestruz;
    charAvestruz.setSpriteSheet(textureCharacters);
    charAvestruz.addFrame(sf::IntRect(272,63,29,39));
    charAvestruz.addFrame(sf::IntRect(231,63,31,29));
    animations.push_back(charAvestruz);
    Animation charCao;
    charCao.setSpriteSheet(textureCharacters);
    charCao.addFrame(sf::IntRect(192,63,29,23));
    charCao.addFrame(sf::IntRect(148,63,34,23));
    charCao.addFrame(sf::IntRect(94,63,44,23));
    animations.push_back(charCao);
    Animation charCoruja;
    charCoruja.setSpriteSheet(textureCharacters);
    charCoruja.addFrame(sf::IntRect(60,63,24,30));
    charCoruja.addFrame(sf::IntRect(10,63,40,32));
    animations.push_back(charCoruja);
    Animation charGato;
    charGato.setSpriteSheet(textureCharacters);
    charGato.addFrame(sf::IntRect(480,10,17,25));
    charGato.addFrame(sf::IntRect(452,10,18,29));
    animations.push_back(charGato);
    Animation charGorila;
    charGorila.setSpriteSheet(textureCharacters);
    charGorila.addFrame(sf::IntRect(405,10,37,33));
    charGorila.addFrame(sf::IntRect(358,10,37,38));
    animations.push_back(charGorila);
    Animation charJacare;
    charJacare.setSpriteSheet(textureCharacters);
    charJacare.addFrame(sf::IntRect(308,10,40,39));
    charJacare.addFrame(sf::IntRect(258,10,40,43));
    animations.push_back(charJacare);
    Animation charUrso;
    charUrso.setSpriteSheet(textureCharacters);
    charUrso.addFrame(sf::IntRect(212,10,36,37));
    charUrso.addFrame(sf::IntRect(166,10,36,41));
    animations.push_back(charUrso);
    Animation charVaca;
    charVaca.setSpriteSheet(textureCharacters);
    charVaca.addFrame(sf::IntRect(111,10,45,23));
    charVaca.addFrame(sf::IntRect(65,10,36,37));
    charVaca.addFrame(sf::IntRect(10,10,45,23));
    animations.push_back(charVaca);
    
    std::vector<Animation> currentAnimationVec(4);
    bool once(true);
    
    
    
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
    
    int nextChange = 0;
    
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
        
        //draw all
        window.draw(bgSprite);
        
        //animation
        frameTime = frameClock.restart();
        for(int i = 0; i<4; i++) {
//            sf::Vector2f pivot(0,0);
            if (once){
                currentAnimationVec.at(i) = animations.at(randInt(0,animations.size()));
                currentAnimatedSpriteVec.at(i).setPosition(maskPosition+positionVec.at(i));
            }
            currentAnimatedSpriteVec.at(i).play(currentAnimationVec.at(i));
            if (currentAnimatedSpriteVec.at(i).update(frameTime)){
                currentAnimationVec.at(i) = animations.at(randInt(0,animations.size()));
                currentAnimatedSpriteVec.at(i).setPosition(maskPosition+positionVec.at(i));
                currentAnimatedSpriteVec.at(i).setMaxIteration(randInt(2,10));
            }
            window.draw(currentAnimatedSpriteVec.at(i));
            
        }
        
        once = false;
        
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
