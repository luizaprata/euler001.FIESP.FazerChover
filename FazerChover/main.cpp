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
    Freenect::FreenectDevice(_ctx, _index), m_buffer_depth(FREENECT_DEPTH_11BIT), m_buffer_rgb(FREENECT_VIDEO_RGB), m_gamma(2048), m_new_rgb_frame(false), m_new_depth_frame(false), depthMat(Size(640, 480), CV_16UC1), rgbMat(Size(640, 480), CV_8UC3, Scalar(0)), ownMat(Size(640, 480), CV_8UC3, Scalar(0))
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

int randInt(int min = 0, int max = 1) {
    return (rand()%max) + min;
}

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(1024, 768), "Fazer Chover");
    window.setFramerateLimit(30);
    
    // Set the Icon
    sf::Image icon;
    if (!icon.loadFromFile(resourcePath() + "icon.png")) {
        return -1;
    }
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    
    // set up AnimatedSprite
    sf::Vector2f hidePosition(400,400);
    sf::Vector2f maskPosition(34,258);
    sf::Vector2f pivot(-31,-56);
    std::vector<sf::Vector2f> positionVec(4);
    positionVec.at(0) = sf::Vector2f(34,164);
    positionVec.at(1) = sf::Vector2f(90,164);
    positionVec.at(2) = sf::Vector2f(133,164);
    positionVec.at(3) = sf::Vector2f(195,164);
    
    ///SPECIAL EFFECTS
    AnimatedSprite specialEffect = AnimatedSprite(sf::seconds(0.2), true, false);
    specialEffect.setPosition(maskPosition);
    specialEffect.setLooped(false);
    
    //urubu
    sf::Texture textureUrubu;
    if (!textureUrubu.loadFromFile(resourcePath() + "urubu.png")) {
        std::cout << "fail load texture urubu.png" << std::endl;
        return -1;
    }
    Animation urubu;
    urubu.setSpriteSheet(textureUrubu);
    urubu.addFrame(sf::IntRect(222,850,222,170));
    urubu.addFrame(sf::IntRect(0,850,222,170));
    urubu.addFrame(sf::IntRect(222,680,222,170));
    urubu.addFrame(sf::IntRect(0,850,222,170));
    urubu.addFrame(sf::IntRect(222,680,222,170));
    urubu.addFrame(sf::IntRect(0,850,222,170));
    urubu.addFrame(sf::IntRect(222,680,222,170));
    urubu.addFrame(sf::IntRect(0,850,222,170));
    urubu.addFrame(sf::IntRect(222,680,222,170));
    urubu.addFrame(sf::IntRect(0,680,222,170));
    urubu.addFrame(sf::IntRect(222,510,222,170));
    urubu.addFrame(sf::IntRect(0,850,222,170));
    urubu.addFrame(sf::IntRect(0,510,222,170));
    urubu.addFrame(sf::IntRect(222,340,222,170));
    urubu.addFrame(sf::IntRect(0,340,222,170));
    urubu.addFrame(sf::IntRect(222,170,222,170));
    urubu.addFrame(sf::IntRect(0,170,222,170));
    urubu.addFrame(sf::IntRect(222,0,222,170));
    urubu.addFrame(sf::IntRect(0,0,222,170));
    urubu.addFrame(sf::IntRect(222,680,222,170));
    urubu.addFrame(sf::IntRect(0,850,222,170));
    urubu.addFrame(sf::IntRect(222,680,222,170));
    urubu.addFrame(sf::IntRect(0,850,222,170));
    urubu.addFrame(sf::IntRect(222,680,222,170));
    urubu.addFrame(sf::IntRect(0,850,222,170));
    urubu.addFrame(sf::IntRect(222,680,222,170));
    
    
    // animation feno
    sf::Texture textureFeno;
    if (!textureFeno.loadFromFile(resourcePath() + "feno.png")) {
        std::cout << "fail load texture feno" << std::endl;
        return -1;
    }
    Animation feno;
    feno.setSpriteSheet(textureFeno);
    feno.addFrame(sf::IntRect(222,510,222,170));
    feno.addFrame(sf::IntRect(0,510,222,170));
    feno.addFrame(sf::IntRect(222,340,222,170));
    feno.addFrame(sf::IntRect(0,340,222,170));
    feno.addFrame(sf::IntRect(222,170,222,170));
    feno.addFrame(sf::IntRect(0,170,222,170));
    feno.addFrame(sf::IntRect(222,0,222,170));
    feno.addFrame(sf::IntRect(0,0,222,170));
    feno.addFrame(sf::IntRect(222,510,222,170));
    
    // animation cenario
    sf::Texture textureCenario;
    if (!textureCenario.loadFromFile(resourcePath() + "cenario.png")) {
        std::cout << "fail load texture cenario" << std::endl;
        return -1;
    }
    Animation cenario;
    cenario.setSpriteSheet(textureCenario);
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(0,0,222,170));
    cenario.addFrame(sf::IntRect(222,0,222,170));
    cenario.addFrame(sf::IntRect(222,0,222,170));
    cenario.addFrame(sf::IntRect(444,0,222,170));
    cenario.addFrame(sf::IntRect(444,0,222,170));
    cenario.addFrame(sf::IntRect(666,0,222,170));
    cenario.addFrame(sf::IntRect(666,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(1110,0,222,170));
    cenario.addFrame(sf::IntRect(1332,0,222,170));
    cenario.addFrame(sf::IntRect(1554,0,222,170));
    cenario.addFrame(sf::IntRect(1776,0,222,170));
    cenario.addFrame(sf::IntRect(0,170,222,170));
    cenario.addFrame(sf::IntRect(222,170,222,170));
    cenario.addFrame(sf::IntRect(444,170,222,170));
    cenario.addFrame(sf::IntRect(666,170,222,170));
    cenario.addFrame(sf::IntRect(888,170,222,170));
    cenario.addFrame(sf::IntRect(1110,170,222,170));
    cenario.addFrame(sf::IntRect(1332,170,222,170));
    cenario.addFrame(sf::IntRect(1554,170,222,170));
    cenario.addFrame(sf::IntRect(1776,170,222,170));
    cenario.addFrame(sf::IntRect(0,340,222,170));
    cenario.addFrame(sf::IntRect(222,340,222,170));
    cenario.addFrame(sf::IntRect(444,340,222,170));
    cenario.addFrame(sf::IntRect(666,340,222,170));
    cenario.addFrame(sf::IntRect(888,340,222,170));
    cenario.addFrame(sf::IntRect(1110,340,222,170));
    cenario.addFrame(sf::IntRect(1332,340,222,170));
    cenario.addFrame(sf::IntRect(1554,340,222,170));
    cenario.addFrame(sf::IntRect(1776,340,222,170));
    cenario.addFrame(sf::IntRect(0,510,222,170));
    cenario.addFrame(sf::IntRect(222,510,222,170));
    cenario.addFrame(sf::IntRect(444,510,222,170));
    cenario.addFrame(sf::IntRect(666,510,222,170));
    cenario.addFrame(sf::IntRect(888,510,222,170));
    cenario.addFrame(sf::IntRect(1110,510,222,170));
    cenario.addFrame(sf::IntRect(1332,510,222,170));
    cenario.addFrame(sf::IntRect(1554,510,222,170));
    cenario.addFrame(sf::IntRect(1776,510,222,170));
    cenario.addFrame(sf::IntRect(0,680,222,170));
    cenario.addFrame(sf::IntRect(222,680,222,170));
    cenario.addFrame(sf::IntRect(444,680,222,170));
    cenario.addFrame(sf::IntRect(666,680,222,170));
    cenario.addFrame(sf::IntRect(888,680,222,170));
    cenario.addFrame(sf::IntRect(1110,680,222,170));
    cenario.addFrame(sf::IntRect(1332,680,222,170));
    cenario.addFrame(sf::IntRect(1554,680,222,170));
    cenario.addFrame(sf::IntRect(1776,680,222,170));
    cenario.addFrame(sf::IntRect(0,850,222,170));
    cenario.addFrame(sf::IntRect(222,850,222,170));
    cenario.addFrame(sf::IntRect(444,850,222,170));
    cenario.addFrame(sf::IntRect(666,850,222,170));
    cenario.addFrame(sf::IntRect(888,850,222,170));
    cenario.addFrame(sf::IntRect(1110,850,222,170));
    cenario.addFrame(sf::IntRect(1332,850,222,170));
    cenario.addFrame(sf::IntRect(1554,850,222,170));
    cenario.addFrame(sf::IntRect(1776,850,222,170));
    cenario.addFrame(sf::IntRect(0,1020,222,170));
    cenario.addFrame(sf::IntRect(222,1020,222,170));
    cenario.addFrame(sf::IntRect(444,1020,222,170));
    cenario.addFrame(sf::IntRect(666,1020,222,170));
    cenario.addFrame(sf::IntRect(888,1020,222,170));
    cenario.addFrame(sf::IntRect(1110,1020,222,170));
    cenario.addFrame(sf::IntRect(1332,1020,222,170));
    cenario.addFrame(sf::IntRect(1554,1020,222,170));
    cenario.addFrame(sf::IntRect(1776,1020,222,170));
    cenario.addFrame(sf::IntRect(0,1190,222,170));
    cenario.addFrame(sf::IntRect(222,1190,222,170));
    cenario.addFrame(sf::IntRect(222,1190,222,170));
    cenario.addFrame(sf::IntRect(222,1190,222,170));
    cenario.addFrame(sf::IntRect(222,1190,222,170));
    cenario.addFrame(sf::IntRect(222,1190,222,170));
    cenario.addFrame(sf::IntRect(222,1190,222,170));
    cenario.addFrame(sf::IntRect(222,1190,222,170));
    cenario.addFrame(sf::IntRect(222,1190,222,170));
    cenario.addFrame(sf::IntRect(222,1190,222,170));
    cenario.addFrame(sf::IntRect(222,1190,222,170));
    cenario.addFrame(sf::IntRect(222,1190,222,170));
    cenario.addFrame(sf::IntRect(222,1190,222,170));
    AnimatedSprite cenarioAnimatedSprite = AnimatedSprite(sf::seconds(0.2), true, false);
    cenarioAnimatedSprite.setPosition(maskPosition);
    cenarioAnimatedSprite.setLooped(false);
    
    // animation characters
    sf::Texture textureCharacters;
    if (!textureCharacters.loadFromFile(resourcePath() + "characters.png")) {
        std::cout << "fail load texture characters.png" << std::endl;
        return -1;
    }
    
    std::vector<AnimatedSprite> currentAnimatedSpriteVec(4);
    currentAnimatedSpriteVec.at(0) = AnimatedSprite(sf::seconds(0.2), true, true);
    currentAnimatedSpriteVec.at(1) = AnimatedSprite(sf::seconds(0.2), true, true);
    currentAnimatedSpriteVec.at(2) = AnimatedSprite(sf::seconds(0.2), true, true);
    currentAnimatedSpriteVec.at(3) = AnimatedSprite(sf::seconds(0.2), true, true);
    currentAnimatedSpriteVec.at(0).setPosition(hidePosition);
    currentAnimatedSpriteVec.at(1).setPosition(hidePosition);
    currentAnimatedSpriteVec.at(2).setPosition(hidePosition);
    currentAnimatedSpriteVec.at(3).setPosition(hidePosition);
    
    sf::Clock frameClock;
    sf::Time frameTime;
    
    std::vector<Animation> animations(0);
    Animation char01;
    char01.setSpriteSheet(textureCharacters);
    char01.addFrame(sf::IntRect(201,620,62,57));
    char01.addFrame(sf::IntRect(134,620,62,57));
    char01.addFrame(sf::IntRect(67,620,62,57));
    char01.addFrame(sf::IntRect(0,620,62,57));
    animations.push_back(char01);
    
    Animation char02;
    char02.setSpriteSheet(textureCharacters);
    char02.addFrame(sf::IntRect(402,558,62,57));
    char02.addFrame(sf::IntRect(335,558,62,57));
    char02.addFrame(sf::IntRect(268,558,62,57));
    char02.addFrame(sf::IntRect(201,558,62,57));
    animations.push_back(char02);
    
    Animation char03;
    char03.setSpriteSheet(textureCharacters);
    char03.addFrame(sf::IntRect(134,558,62,57));
    char03.addFrame(sf::IntRect(67,558,62,57));
    char03.addFrame(sf::IntRect(0,558,62,57));
    char03.addFrame(sf::IntRect(402,496,62,57));
    animations.push_back(char03);
    
    Animation char04;
    char04.setSpriteSheet(textureCharacters);
    char04.addFrame(sf::IntRect(335,496,62,57));
    char04.addFrame(sf::IntRect(268,496,62,57));
    char04.addFrame(sf::IntRect(201,496,62,57));
    char04.addFrame(sf::IntRect(134,496,62,57));
    animations.push_back(char04);
    
    Animation char05;
    char05.setSpriteSheet(textureCharacters);
    char05.addFrame(sf::IntRect(67,496,62,57));
    char05.addFrame(sf::IntRect(0,496,62,57));
    char05.addFrame(sf::IntRect(402,434,62,57));
    char05.addFrame(sf::IntRect(335,434,62,57));
    animations.push_back(char05);
    
    Animation char06;
    char06.setSpriteSheet(textureCharacters);
    char06.addFrame(sf::IntRect(268,434,62,57));
    char06.addFrame(sf::IntRect(201,434,62,57));
    char06.addFrame(sf::IntRect(134,434,62,57));
    char06.addFrame(sf::IntRect(67,434,62,57));
    animations.push_back(char06);
    
    Animation char07;
    char07.setSpriteSheet(textureCharacters);
    char07.addFrame(sf::IntRect(0,434,62,57));
    char07.addFrame(sf::IntRect(402,372,62,57));
    char07.addFrame(sf::IntRect(335,372,62,57));
    char07.addFrame(sf::IntRect(268,372,62,57));
    animations.push_back(char07);
    
    Animation char08;
    char08.setSpriteSheet(textureCharacters);
    char08.addFrame(sf::IntRect(201,372,62,57));
    char08.addFrame(sf::IntRect(134,372,62,57));
    char08.addFrame(sf::IntRect(67,372,62,57));
    char08.addFrame(sf::IntRect(0,372,62,57));
    animations.push_back(char08);
    
    Animation char09;
    char09.setSpriteSheet(textureCharacters);
    char09.addFrame(sf::IntRect(402,310,62,57));
    char09.addFrame(sf::IntRect(335,310,62,57));
    char09.addFrame(sf::IntRect(268,310,62,57));
    char09.addFrame(sf::IntRect(201,310,62,57));
    animations.push_back(char09);
    
    Animation char10;
    char10.setSpriteSheet(textureCharacters);
    char10.addFrame(sf::IntRect(134,310,62,57));
    char10.addFrame(sf::IntRect(67,310,62,57));
    char10.addFrame(sf::IntRect(0,310,62,57));
    char10.addFrame(sf::IntRect(402,248,62,57));
    animations.push_back(char10);
    
    Animation char11;
    char11.setSpriteSheet(textureCharacters);
    char11.addFrame(sf::IntRect(335,248,62,57));
    char11.addFrame(sf::IntRect(268,248,62,57));
    char11.addFrame(sf::IntRect(201,248,62,57));
    char11.addFrame(sf::IntRect(134,248,62,57));
    animations.push_back(char11);
    
    Animation char12;
    char12.setSpriteSheet(textureCharacters);
    char12.addFrame(sf::IntRect(67,248,62,57));
    char12.addFrame(sf::IntRect(0,248,62,57));
    char12.addFrame(sf::IntRect(402,186,62,57));
    char12.addFrame(sf::IntRect(335,186,62,57));
    animations.push_back(char12);
    
    Animation char13;
    char13.setSpriteSheet(textureCharacters);
    char13.addFrame(sf::IntRect(268,186,62,57));
    char13.addFrame(sf::IntRect(201,186,62,57));
    char13.addFrame(sf::IntRect(134,186,62,57));
    char13.addFrame(sf::IntRect(67,186,62,57));
    animations.push_back(char13);

    Animation char14;
    char14.setSpriteSheet(textureCharacters);
    char14.addFrame(sf::IntRect(0,186,62,57));
    char14.addFrame(sf::IntRect(402,124,62,57));
    char14.addFrame(sf::IntRect(335,124,62,57));
    char14.addFrame(sf::IntRect(268,124,62,57));
    animations.push_back(char14);
    
    Animation charAvestruz;
    charAvestruz.setSpriteSheet(textureCharacters);
    charAvestruz.addFrame(sf::IntRect(201,124,62,57));
    charAvestruz.addFrame(sf::IntRect(134,124,62,57));
    animations.push_back(charAvestruz);

    Animation charCao;
    charCao.setSpriteSheet(textureCharacters);
    charCao.addFrame(sf::IntRect(67,124,62,57));
    charCao.addFrame(sf::IntRect(0,124,62,57));
    charCao.addFrame(sf::IntRect(402,62,62,57));
    animations.push_back(charCao);

    Animation charCoruja;
    charCoruja.setSpriteSheet(textureCharacters);
    charCoruja.addFrame(sf::IntRect(335,62,62,57));
    charCoruja.addFrame(sf::IntRect(268,62,62,57));
    animations.push_back(charCoruja);

    Animation charGato;
    charGato.setSpriteSheet(textureCharacters);
    charGato.addFrame(sf::IntRect(201,62,62,57));
    charGato.addFrame(sf::IntRect(134,62,62,57));
    animations.push_back(charGato);

    Animation charGorila;
    charGorila.setSpriteSheet(textureCharacters);
    charGorila.addFrame(sf::IntRect(67,62,62,57));
    charGorila.addFrame(sf::IntRect(0,62,62,57));
    animations.push_back(charGorila);

    Animation charJacare;
    charJacare.setSpriteSheet(textureCharacters);
    charJacare.addFrame(sf::IntRect(402,0,62,57));
    charJacare.addFrame(sf::IntRect(335,0,62,57));
    animations.push_back(charJacare);

    Animation charUrso;
    charUrso.setSpriteSheet(textureCharacters);
    charUrso.addFrame(sf::IntRect(268,0,62,57));
    charUrso.addFrame(sf::IntRect(201,0,62,57));
    animations.push_back(charUrso);
    
    Animation charVaca;
    charVaca.setSpriteSheet(textureCharacters);
    charVaca.addFrame(sf::IntRect(134,0,62,57));
    charVaca.addFrame(sf::IntRect(67,0,62,57));
    charVaca.addFrame(sf::IntRect(0,0,62,57));
    animations.push_back(charVaca);
    
    std::random_shuffle ( animations.begin(), animations.end() );
    int countAnimation = 0;
    
    std::vector<Animation> currentAnimationVec(4);
    
    
    
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
    double lackProgress = 0.05;
    double lackProgressCount = 0;
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
                if (perc > lackProgress){
                    lackProgressCount = 0;
                    progress += perc;
                    cenarioAnimatedSprite.setPlayReverse(false);
                    cenarioAnimatedSprite.play(cenario);
                    if (cenarioAnimatedSprite.getCurrentFrame() == 60){
                        specialEffect.restart();
                        specialEffect.play(feno);
                    }
                } else {
                    lackProgressCount += perc;
                    if (lackProgressCount > 1){
                        cenarioAnimatedSprite.setPlayReverse(true);
                    }
                    if (lackProgressCount < 3){
                        int idx = std::round(lackProgressCount);
                        currentAnimatedSpriteVec.at(idx).setPosition(hidePosition);
                        currentAnimatedSpriteVec.at(idx).stop();
                        std::cout  << " idx: " << idx << " / "<< lackProgress << std::endl;
                    }
                    
                    if (lackProgressCount > 10 and cenarioAnimatedSprite.getCurrentFrame() < 70) {
                        lackProgressCount = 1;
                        specialEffect.restart();
                        specialEffect.play(urubu);
                    }
                    
                }
                
                //std::cout  << " progress:" << progress << " - perc:" << perc <<  " - lackProgressCount:" << lackProgressCount <<  " - currentFrame:" << cenarioAnimatedSprite.getCurrentFrame() <<  " -- " << cenarioAnimatedSprite.isPlaying() <<  " - specialEffect:" <<  specialEffect.getCurrentFrame() << std::endl;
                
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
        frameTime = frameClock.restart();

        //cenario
        cenarioAnimatedSprite.update(frameTime);
        window.draw(cenarioAnimatedSprite);
        
        //special effects
        specialEffect.update(frameTime);
        window.draw(specialEffect);
        
        //personagens
        for(int i = 0; i<4; i++) {
            if (currentAnimatedSpriteVec.at(i).update(frameTime)){
                currentAnimatedSpriteVec.at(i).setMaxIteration(randInt(4,10));
                currentAnimatedSpriteVec.at(i).setPosition(maskPosition+positionVec.at(i)+pivot);
                currentAnimationVec.at(i) = animations.at(countAnimation);
                currentAnimatedSpriteVec.at(i).play(currentAnimationVec.at(i));
                countAnimation++;
            }
            if (countAnimation >= animations.size()) {
                countAnimation = 0;
            }
            window.draw(currentAnimatedSpriteVec.at(i));
            
        }
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
                        
                    // ESC // exit
                    case sf::Keyboard::Escape:
                        window.close();
                        break;
                    
                    //show/hide mask'
                    case sf::Keyboard::M:
                        break;
                    
                    //snapshot
                    case sf::Keyboard::T:
                        file.str("");
                        file.clear();
                        file << filename << i_snap << suffix;
                        cv::imwrite(file.str(), rgbMat);
                        i_snap++;
                        break;
                    
                    // reset
                    case sf::Keyboard::R:
                        progress = 0;
                        break;
                    
                    //'d' has been pressed. this will debug mode
                    case sf::Keyboard::D:
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
                    
                    // UP
                    case sf::Keyboard::Up:
                        freenect_angle++;
                        if (freenect_angle > 30) {
                            freenect_angle = 30;
                        }
                        break;
                    
                    // down
                    case sf::Keyboard::Down:
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
