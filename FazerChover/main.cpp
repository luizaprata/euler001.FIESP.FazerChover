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
    window.setFramerateLimit(10);
    
    // Set the Icon
    sf::Image icon;
    if (!icon.loadFromFile(resourcePath() + "icon.png")) {
        return -1;
    }
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    
    //animated sprites
    std::vector<AnimatedSprite*> animatedSpriteSheets(0);
    
    // set up AnimatedSprite
    sf::Vector2f hidePosition(400,800);
    sf::Vector2f maskPosition(34,258);
    sf::Vector2f pivot(-31,-56);
    std::vector<sf::Vector2f> positionVec(4);
    positionVec.at(0) = sf::Vector2f(133,164);
    positionVec.at(1) = sf::Vector2f(90,164);
    positionVec.at(2) = sf::Vector2f(195,164);
    positionVec.at(3) = sf::Vector2f(34,164);
    
    
    
    double FPS = 0.2;
    double FRAME_START_ANIM_CHARS = 14;
    
    
    
    
    ///SPECIAL EFFECTS
    AnimatedSprite specialEffect = AnimatedSprite(sf::seconds(FPS), true, false);
    specialEffect.setPosition(maskPosition);
    specialEffect.setLooped(false);
    animatedSpriteSheets.push_back(&specialEffect);
    
    
    AnimatedSprite startAnimated = AnimatedSprite(sf::seconds(FPS), true, false);
    startAnimated.setPosition(maskPosition);
    startAnimated.setLooped(false);
    animatedSpriteSheets.push_back(&startAnimated);
    
    //end
    sf::Texture textureStart;
    if (!textureStart.loadFromFile(resourcePath() + "start.png")) {
        std::cout << "fail load texture start.png" << std::endl;
        return -1;
    }
    Animation start;
    start.setSpriteSheet(textureStart);
    start.addFrame(sf::IntRect(0,0,222,170));
    start.addFrame(sf::IntRect(0,0,222,170));
    start.addFrame(sf::IntRect(0,0,222,170));
    start.addFrame(sf::IntRect(222,0,222,170));
    start.addFrame(sf::IntRect(444,0,222,170));
    start.addFrame(sf::IntRect(666,0,222,170));
    start.addFrame(sf::IntRect(888,0,222,170));
    start.addFrame(sf::IntRect(1110,0,222,170));
    start.addFrame(sf::IntRect(1332,0,222,170));
    start.addFrame(sf::IntRect(1554,0,222,170));
    start.addFrame(sf::IntRect(1776,0,222,170));
    start.addFrame(sf::IntRect(0,170,222,170));
    start.addFrame(sf::IntRect(222,170,222,170));
    start.addFrame(sf::IntRect(444,170,222,170));
    start.addFrame(sf::IntRect(666,170,222,170));
    start.addFrame(sf::IntRect(888,170,222,170));
    start.addFrame(sf::IntRect(1110,170,222,170));
    start.addFrame(sf::IntRect(1332,170,222,170));
    start.addFrame(sf::IntRect(1554,170,222,170));
    start.addFrame(sf::IntRect(1776,170,222,170));
    start.addFrame(sf::IntRect(0,340,222,170));
    start.addFrame(sf::IntRect(222,340,222,170));
    start.addFrame(sf::IntRect(444,340,222,170));
    start.addFrame(sf::IntRect(666,340,222,170));
    start.addFrame(sf::IntRect(888,340,222,170));
    start.addFrame(sf::IntRect(1110,340,222,170));
    start.addFrame(sf::IntRect(1332,340,222,170));
    start.addFrame(sf::IntRect(1554,340,222,170));
    start.addFrame(sf::IntRect(1776,340,222,170));
    start.addFrame(sf::IntRect(0,510,222,170));
    start.addFrame(sf::IntRect(222,510,222,170));
    start.addFrame(sf::IntRect(0,0,222,170));
    start.addFrame(sf::IntRect(0,0,222,170));
    start.addFrame(sf::IntRect(0,0,222,170));
    start.addFrame(sf::IntRect(0,0,222,170));
    start.addFrame(sf::IntRect(0,0,222,170));
    start.addFrame(sf::IntRect(0,0,222,170));
    start.addFrame(sf::IntRect(0,0,222,170));
    start.addFrame(sf::IntRect(0,0,222,170));
    start.addFrame(sf::IntRect(444,510,222,170));
    start.addFrame(sf::IntRect(0,0,222,170));
    start.addFrame(sf::IntRect(444,510,222,170));
    start.addFrame(sf::IntRect(0,0,222,170));
    start.addFrame(sf::IntRect(0,0,222,170));
    start.addFrame(sf::IntRect(0,0,222,170));
    start.addFrame(sf::IntRect(0,0,222,170));
    start.addFrame(sf::IntRect(0,0,222,170));
    start.addFrame(sf::IntRect(0,0,222,170));
    start.addFrame(sf::IntRect(444,510,222,170));
    start.addFrame(sf::IntRect(0,0,222,170));
    start.addFrame(sf::IntRect(444,510,222,170));
    start.addFrame(sf::IntRect(0,0,222,170));
    start.addFrame(sf::IntRect(0,0,222,170));
    start.addFrame(sf::IntRect(0,0,222,170));
    start.addFrame(sf::IntRect(0,0,222,170));
    start.addFrame(sf::IntRect(0,0,222,170));
    start.addFrame(sf::IntRect(0,0,222,170));
    start.addFrame(sf::IntRect(0,0,222,170));
    start.addFrame(sf::IntRect(0,0,222,170));
    start.addFrame(sf::IntRect(666,510,222,170));
    start.addFrame(sf::IntRect(888,510,222,170));
    start.addFrame(sf::IntRect(1110,510,222,170));
    start.addFrame(sf::IntRect(1332,510,222,170));
    start.addFrame(sf::IntRect(1554,510,222,170));
    start.addFrame(sf::IntRect(1776,510,222,170));
    start.addFrame(sf::IntRect(0,680,222,170));
    start.addFrame(sf::IntRect(222,680,222,170));
    start.addFrame(sf::IntRect(222,680,222,170));
    start.addFrame(sf::IntRect(222,680,222,170));
    start.addFrame(sf::IntRect(222,680,222,170));
    start.addFrame(sf::IntRect(444,680,222,170));
    start.addFrame(sf::IntRect(666,680,222,170));
    start.addFrame(sf::IntRect(888,680,222,170));
    start.addFrame(sf::IntRect(1110,680,222,170));
    start.addFrame(sf::IntRect(1332,680,222,170));
    start.addFrame(sf::IntRect(1554,680,222,170));
    start.addFrame(sf::IntRect(1776,680,222,170));
    start.addFrame(sf::IntRect(0,850,222,170));
    start.addFrame(sf::IntRect(222,850,222,170));
    start.addFrame(sf::IntRect(444,850,222,170));
    start.addFrame(sf::IntRect(666,850,222,170));
    start.addFrame(sf::IntRect(888,850,222,170));
    start.addFrame(sf::IntRect(1110,850,222,170));
    start.addFrame(sf::IntRect(888,850,222,170));
    start.addFrame(sf::IntRect(1110,850,222,170));
    start.addFrame(sf::IntRect(888,850,222,170));
    start.addFrame(sf::IntRect(1332,850,222,170));
    start.addFrame(sf::IntRect(1332,850,222,170));
    start.addFrame(sf::IntRect(1554,850,222,170));
    start.addFrame(sf::IntRect(1776,850,222,170));
    start.addFrame(sf::IntRect(0,1020,222,170));
    start.addFrame(sf::IntRect(222,1020,222,170));
    start.addFrame(sf::IntRect(444,1020,222,170));
    start.addFrame(sf::IntRect(666,1020,222,170));
    start.addFrame(sf::IntRect(888,1020,222,170));
    start.addFrame(sf::IntRect(1110,1020,222,170));
    start.addFrame(sf::IntRect(1332,1020,222,170));
    start.addFrame(sf::IntRect(1554,1020,222,170));
    start.addFrame(sf::IntRect(1776,1020,222,170));
    start.addFrame(sf::IntRect(222,680,222,170));
    start.addFrame(sf::IntRect(222,680,222,170));
    start.addFrame(sf::IntRect(222,680,222,170));
    start.addFrame(sf::IntRect(222,680,222,170));
    start.addFrame(sf::IntRect(222,680,222,170));
    start.addFrame(sf::IntRect(222,680,222,170));
    start.addFrame(sf::IntRect(222,680,222,170));
    start.addFrame(sf::IntRect(0,1190,222,170));
    start.addFrame(sf::IntRect(222,1190,222,170));
    start.addFrame(sf::IntRect(444,1190,222,170));
    start.addFrame(sf::IntRect(666,1190,222,170));
    start.addFrame(sf::IntRect(888,1190,222,170));
    start.addFrame(sf::IntRect(1110,1190,222,170));
    start.addFrame(sf::IntRect(1332,1190,222,170));

    
    //end
    sf::Texture textureEnd;
    if (!textureEnd.loadFromFile(resourcePath() + "end.png")) {
        std::cout << "fail load texture end.png" << std::endl;
        return -1;
    }
    Animation end;
    end.setSpriteSheet(textureEnd);
    end.addFrame(sf::IntRect(0,0,222,177));
    end.addFrame(sf::IntRect(222,0,222,177));
    end.addFrame(sf::IntRect(444,0,222,177));
    end.addFrame(sf::IntRect(666,0,222,177));
    end.addFrame(sf::IntRect(888,0,222,177));
    end.addFrame(sf::IntRect(1110,0,222,177));
    end.addFrame(sf::IntRect(1332,0,222,177));
    end.addFrame(sf::IntRect(1554,0,222,177));
    end.addFrame(sf::IntRect(1776,0,222,177));
    end.addFrame(sf::IntRect(0,177,222,177));
    end.addFrame(sf::IntRect(222,177,222,177));
    end.addFrame(sf::IntRect(444,177,222,177));
    end.addFrame(sf::IntRect(444,177,222,177));
    end.addFrame(sf::IntRect(666,177,222,177));
    end.addFrame(sf::IntRect(666,177,222,177));
    end.addFrame(sf::IntRect(444,177,222,177));
    end.addFrame(sf::IntRect(444,177,222,177));
    end.addFrame(sf::IntRect(666,177,222,177));
    end.addFrame(sf::IntRect(666,177,222,177));
    end.addFrame(sf::IntRect(444,177,222,177));
    end.addFrame(sf::IntRect(444,177,222,177));
    end.addFrame(sf::IntRect(888,177,222,177));
    end.addFrame(sf::IntRect(1110,177,222,177));
    end.addFrame(sf::IntRect(1332,177,222,177));
    end.addFrame(sf::IntRect(1554,177,222,177));
    end.addFrame(sf::IntRect(1776,177,222,177));
    end.addFrame(sf::IntRect(0,354,222,177));
    end.addFrame(sf::IntRect(222,354,222,177));
    end.addFrame(sf::IntRect(444,354,222,177));
    end.addFrame(sf::IntRect(666,354,222,177));
    end.addFrame(sf::IntRect(888,354,222,177));
    end.addFrame(sf::IntRect(1110,354,222,177));
    end.addFrame(sf::IntRect(1332,354,222,177));
    end.addFrame(sf::IntRect(1554,354,222,177));
    end.addFrame(sf::IntRect(1776,354,222,177));
    end.addFrame(sf::IntRect(0,531,222,177));
    end.addFrame(sf::IntRect(222,531,222,177));
    end.addFrame(sf::IntRect(444,531,222,177));
    end.addFrame(sf::IntRect(666,531,222,177));
    end.addFrame(sf::IntRect(888,531,222,177));
    end.addFrame(sf::IntRect(1110,531,222,177));
    end.addFrame(sf::IntRect(1332,531,222,177));
    end.addFrame(sf::IntRect(1332,531,222,177));
    end.addFrame(sf::IntRect(1554,531,222,177));
    end.addFrame(sf::IntRect(1776,531,222,177));
    end.addFrame(sf::IntRect(0,708,222,177));
    end.addFrame(sf::IntRect(222,708,222,177));
    end.addFrame(sf::IntRect(444,708,222,177));
    end.addFrame(sf::IntRect(666,708,222,177));
    end.addFrame(sf::IntRect(888,708,222,177));
    end.addFrame(sf::IntRect(1110,708,222,177));
    end.addFrame(sf::IntRect(1332,708,222,177));
    end.addFrame(sf::IntRect(1554,708,222,177));
    end.addFrame(sf::IntRect(1554,708,222,177));
    end.addFrame(sf::IntRect(1776,708,222,177));
    end.addFrame(sf::IntRect(0,885,222,177));
    end.addFrame(sf::IntRect(222,885,222,177));
    end.addFrame(sf::IntRect(444,885,222,177));
    end.addFrame(sf::IntRect(666,885,222,177));
    end.addFrame(sf::IntRect(888,885,222,177));
    end.addFrame(sf::IntRect(1110,885,222,177));
    end.addFrame(sf::IntRect(1332,885,222,177));
    end.addFrame(sf::IntRect(1554,885,222,177));
    end.addFrame(sf::IntRect(1776,885,222,177));
    end.addFrame(sf::IntRect(0,1062,222,177));
    end.addFrame(sf::IntRect(222,1062,222,177));
    end.addFrame(sf::IntRect(444,1062,222,177));
    end.addFrame(sf::IntRect(666,1062,222,177));
    end.addFrame(sf::IntRect(888,1062,222,177));
    end.addFrame(sf::IntRect(1110,1062,222,177));
    end.addFrame(sf::IntRect(1332,1062,222,177));
    end.addFrame(sf::IntRect(0,0,222,177));
    end.addFrame(sf::IntRect(0,0,222,177));
    end.addFrame(sf::IntRect(0,0,222,177));
    end.addFrame(sf::IntRect(0,0,222,177));

    
    
    
    //trovao
    sf::Texture textureTrovao;
    if (!textureTrovao.loadFromFile(resourcePath() + "trovao.png")) {
        std::cout << "fail load texture trovao.png" << std::endl;
        return -1;
    }
    Animation trovao;
    trovao.setSpriteSheet(textureTrovao);
    trovao.addFrame(sf::IntRect(0,0,222,170));
    trovao.addFrame(sf::IntRect(222,0,222,170));
    trovao.addFrame(sf::IntRect(0,170,222,170));
    trovao.addFrame(sf::IntRect(222,0,222,170));
    trovao.addFrame(sf::IntRect(222,170,222,170));
    trovao.addFrame(sf::IntRect(222,0,222,170));
    trovao.addFrame(sf::IntRect(0,340,222,170));
    trovao.addFrame(sf::IntRect(222,0,222,170));
    
    AnimatedSprite thunderEffect = AnimatedSprite(sf::seconds(FPS*0.5), true, false);
    thunderEffect.setPosition(maskPosition);
    thunderEffect.setLooped(false);
    animatedSpriteSheets.push_back(&thunderEffect);
    
    
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
    cenario.addFrame(sf::IntRect(222,0,222,170));
    cenario.addFrame(sf::IntRect(444,0,222,170));
    cenario.addFrame(sf::IntRect(666,0,222,170));
    cenario.addFrame(sf::IntRect(888,0,222,170));
    cenario.addFrame(sf::IntRect(1110,0,222,170));
    cenario.addFrame(sf::IntRect(1332,0,222,170));
    cenario.addFrame(sf::IntRect(1554,0,222,170));
    cenario.addFrame(sf::IntRect(1776,0,222,170));
    cenario.addFrame(sf::IntRect(1998,0,222,170));
    cenario.addFrame(sf::IntRect(2220,0,222,170));
    cenario.addFrame(sf::IntRect(2220,0,222,170));
    cenario.addFrame(sf::IntRect(2220,0,222,170));
    cenario.addFrame(sf::IntRect(2220,0,222,170));
    cenario.addFrame(sf::IntRect(2442,0,222,170));
    cenario.addFrame(sf::IntRect(2220,0,222,170));
    cenario.addFrame(sf::IntRect(2442,0,222,170));
    cenario.addFrame(sf::IntRect(2664,0,222,170));
    cenario.addFrame(sf::IntRect(2886,0,222,170));
    cenario.addFrame(sf::IntRect(3108,0,222,170));
    cenario.addFrame(sf::IntRect(3330,0,222,170));
    cenario.addFrame(sf::IntRect(3552,0,222,170));
    cenario.addFrame(sf::IntRect(3774,0,222,170));
    cenario.addFrame(sf::IntRect(0,170,222,170));
    cenario.addFrame(sf::IntRect(222,170,222,170));
    cenario.addFrame(sf::IntRect(444,170,222,170));
    cenario.addFrame(sf::IntRect(666,170,222,170));
    cenario.addFrame(sf::IntRect(888,170,222,170));
    cenario.addFrame(sf::IntRect(666,170,222,170));
    cenario.addFrame(sf::IntRect(888,170,222,170));
    cenario.addFrame(sf::IntRect(1110,170,222,170));
    cenario.addFrame(sf::IntRect(1332,170,222,170));
    cenario.addFrame(sf::IntRect(1554,170,222,170));
    cenario.addFrame(sf::IntRect(1776,170,222,170));
    cenario.addFrame(sf::IntRect(1998,170,222,170));
    cenario.addFrame(sf::IntRect(2220,170,222,170));
    cenario.addFrame(sf::IntRect(2442,170,222,170));
    cenario.addFrame(sf::IntRect(2664,170,222,170));
    cenario.addFrame(sf::IntRect(2886,170,222,170));
    cenario.addFrame(sf::IntRect(3108,170,222,170));
    cenario.addFrame(sf::IntRect(3330,170,222,170));
    cenario.addFrame(sf::IntRect(3552,170,222,170));
    cenario.addFrame(sf::IntRect(3774,170,222,170));
    cenario.addFrame(sf::IntRect(0,340,222,170));
    cenario.addFrame(sf::IntRect(222,340,222,170));
    cenario.addFrame(sf::IntRect(444,340,222,170));
    cenario.addFrame(sf::IntRect(666,340,222,170));
    cenario.addFrame(sf::IntRect(888,340,222,170));
    cenario.addFrame(sf::IntRect(1110,340,222,170));
    cenario.addFrame(sf::IntRect(1332,340,222,170));
    cenario.addFrame(sf::IntRect(1554,340,222,170));
    cenario.addFrame(sf::IntRect(1776,340,222,170));
    cenario.addFrame(sf::IntRect(1998,340,222,170));
    cenario.addFrame(sf::IntRect(2220,340,222,170));
    cenario.addFrame(sf::IntRect(2442,340,222,170));
    cenario.addFrame(sf::IntRect(2664,340,222,170));
    cenario.addFrame(sf::IntRect(2886,340,222,170));
    cenario.addFrame(sf::IntRect(3108,340,222,170));
    cenario.addFrame(sf::IntRect(3330,340,222,170));
    cenario.addFrame(sf::IntRect(3552,340,222,170));
    cenario.addFrame(sf::IntRect(3774,340,222,170));
    cenario.addFrame(sf::IntRect(0,510,222,170));
    cenario.addFrame(sf::IntRect(222,510,222,170));
    cenario.addFrame(sf::IntRect(444,510,222,170));
    cenario.addFrame(sf::IntRect(666,510,222,170));
    cenario.addFrame(sf::IntRect(888,510,222,170));
    cenario.addFrame(sf::IntRect(1110,510,222,170));
    cenario.addFrame(sf::IntRect(1332,510,222,170));
    cenario.addFrame(sf::IntRect(1554,510,222,170));
    cenario.addFrame(sf::IntRect(1776,510,222,170));
    cenario.addFrame(sf::IntRect(1998,510,222,170));
    cenario.addFrame(sf::IntRect(2220,510,222,170));
    cenario.addFrame(sf::IntRect(2442,510,222,170));
    cenario.addFrame(sf::IntRect(2664,510,222,170));
    cenario.addFrame(sf::IntRect(2886,510,222,170));
    cenario.addFrame(sf::IntRect(3108,510,222,170));
    cenario.addFrame(sf::IntRect(3330,510,222,170));
    cenario.addFrame(sf::IntRect(3552,510,222,170));
    cenario.addFrame(sf::IntRect(3774,510,222,170));
    cenario.addFrame(sf::IntRect(0,680,222,170));
    cenario.addFrame(sf::IntRect(222,680,222,170));
    cenario.addFrame(sf::IntRect(444,680,222,170));
    cenario.addFrame(sf::IntRect(666,680,222,170));
    cenario.addFrame(sf::IntRect(888,680,222,170));
    cenario.addFrame(sf::IntRect(1110,680,222,170));
    cenario.addFrame(sf::IntRect(1332,680,222,170));
    cenario.addFrame(sf::IntRect(1554,680,222,170));
    cenario.addFrame(sf::IntRect(1776,680,222,170));
    cenario.addFrame(sf::IntRect(1998,680,222,170));
    cenario.addFrame(sf::IntRect(2220,680,222,170));
    cenario.addFrame(sf::IntRect(2442,680,222,170));
    cenario.addFrame(sf::IntRect(2664,680,222,170));
    cenario.addFrame(sf::IntRect(2886,680,222,170));
    cenario.addFrame(sf::IntRect(3108,680,222,170));
    cenario.addFrame(sf::IntRect(3330,680,222,170));
    cenario.addFrame(sf::IntRect(3552,680,222,170));
    cenario.addFrame(sf::IntRect(3774,680,222,170));
    cenario.addFrame(sf::IntRect(0,850,222,170));
    cenario.addFrame(sf::IntRect(222,850,222,170));
    cenario.addFrame(sf::IntRect(444,850,222,170));
    cenario.addFrame(sf::IntRect(666,850,222,170));
    cenario.addFrame(sf::IntRect(888,850,222,170));
    cenario.addFrame(sf::IntRect(1110,850,222,170));
    cenario.addFrame(sf::IntRect(1332,850,222,170));
    cenario.addFrame(sf::IntRect(1554,850,222,170));
    cenario.addFrame(sf::IntRect(1776,850,222,170));
    cenario.addFrame(sf::IntRect(1998,850,222,170));
    cenario.addFrame(sf::IntRect(2220,850,222,170));
    cenario.addFrame(sf::IntRect(2442,850,222,170));
    cenario.addFrame(sf::IntRect(2664,850,222,170));
    cenario.addFrame(sf::IntRect(2886,850,222,170));
    cenario.addFrame(sf::IntRect(3108,850,222,170));
    cenario.addFrame(sf::IntRect(3330,850,222,170));
    cenario.addFrame(sf::IntRect(3552,850,222,170));
    cenario.addFrame(sf::IntRect(3774,850,222,170));
    cenario.addFrame(sf::IntRect(0,1020,222,170));
    cenario.addFrame(sf::IntRect(222,1020,222,170));
    cenario.addFrame(sf::IntRect(444,1020,222,170));
    cenario.addFrame(sf::IntRect(666,1020,222,170));
    cenario.addFrame(sf::IntRect(888,1020,222,170));
    cenario.addFrame(sf::IntRect(1110,1020,222,170));
    cenario.addFrame(sf::IntRect(1332,1020,222,170));
    cenario.addFrame(sf::IntRect(1554,1020,222,170));
    cenario.addFrame(sf::IntRect(1776,1020,222,170));
    cenario.addFrame(sf::IntRect(1998,1020,222,170));
    cenario.addFrame(sf::IntRect(2220,1020,222,170));
    cenario.addFrame(sf::IntRect(2442,1020,222,170));
    cenario.addFrame(sf::IntRect(2664,1020,222,170));
    cenario.addFrame(sf::IntRect(2886,1020,222,170));
    cenario.addFrame(sf::IntRect(3108,1020,222,170));
    cenario.addFrame(sf::IntRect(3330,1020,222,170));
    cenario.addFrame(sf::IntRect(3552,1020,222,170));
    cenario.addFrame(sf::IntRect(3774,1020,222,170));
    cenario.addFrame(sf::IntRect(0,1190,222,170));
    cenario.addFrame(sf::IntRect(222,1190,222,170));
    cenario.addFrame(sf::IntRect(444,1190,222,170));
    cenario.addFrame(sf::IntRect(666,1190,222,170));
    cenario.addFrame(sf::IntRect(888,1190,222,170));
    cenario.addFrame(sf::IntRect(1110,1190,222,170));
    cenario.addFrame(sf::IntRect(1332,1190,222,170));
    cenario.addFrame(sf::IntRect(1554,1190,222,170));
    cenario.addFrame(sf::IntRect(1776,1190,222,170));
    cenario.addFrame(sf::IntRect(1998,1190,222,170));
    cenario.addFrame(sf::IntRect(2220,1190,222,170));
    cenario.addFrame(sf::IntRect(2442,1190,222,170));
    cenario.addFrame(sf::IntRect(2664,1190,222,170));
    cenario.addFrame(sf::IntRect(2886,1190,222,170));
    cenario.addFrame(sf::IntRect(3108,1190,222,170));
    cenario.addFrame(sf::IntRect(3330,1190,222,170));
    cenario.addFrame(sf::IntRect(3552,1190,222,170));
    cenario.addFrame(sf::IntRect(3774,1190,222,170));
    cenario.addFrame(sf::IntRect(0,1360,222,170));
    cenario.addFrame(sf::IntRect(222,1360,222,170));
    cenario.addFrame(sf::IntRect(444,1360,222,170));
    cenario.addFrame(sf::IntRect(666,1360,222,170));
    cenario.addFrame(sf::IntRect(888,1360,222,170));
    cenario.addFrame(sf::IntRect(1110,1360,222,170));
    cenario.addFrame(sf::IntRect(1332,1360,222,170));
    cenario.addFrame(sf::IntRect(1554,1360,222,170));
    cenario.addFrame(sf::IntRect(1776,1360,222,170));
    cenario.addFrame(sf::IntRect(1998,1360,222,170));
    cenario.addFrame(sf::IntRect(2220,1360,222,170));
    cenario.addFrame(sf::IntRect(2442,1360,222,170));
    cenario.addFrame(sf::IntRect(2664,1360,222,170));
    cenario.addFrame(sf::IntRect(2886,1360,222,170));
    cenario.addFrame(sf::IntRect(3108,1360,222,170));
    cenario.addFrame(sf::IntRect(3330,1360,222,170));
    cenario.addFrame(sf::IntRect(3552,1360,222,170));
    cenario.addFrame(sf::IntRect(3774,1360,222,170));
    cenario.addFrame(sf::IntRect(0,1530,222,170));
    cenario.addFrame(sf::IntRect(222,1530,222,170));
    cenario.addFrame(sf::IntRect(444,1530,222,170));
    cenario.addFrame(sf::IntRect(666,1530,222,170));
    cenario.addFrame(sf::IntRect(888,1530,222,170));
    cenario.addFrame(sf::IntRect(1110,1530,222,170));
    cenario.addFrame(sf::IntRect(1332,1530,222,170));
    cenario.addFrame(sf::IntRect(1554,1530,222,170));
    cenario.addFrame(sf::IntRect(1776,1530,222,170));
    cenario.addFrame(sf::IntRect(1998,1530,222,170));
    cenario.addFrame(sf::IntRect(2220,1530,222,170));
    cenario.addFrame(sf::IntRect(2442,1530,222,170));
    cenario.addFrame(sf::IntRect(2664,1530,222,170));
    cenario.addFrame(sf::IntRect(2886,1530,222,170));
    cenario.addFrame(sf::IntRect(3108,1530,222,170));
    cenario.addFrame(sf::IntRect(3330,1530,222,170));
    cenario.addFrame(sf::IntRect(3552,1530,222,170));
    cenario.addFrame(sf::IntRect(3774,1530,222,170));
    cenario.addFrame(sf::IntRect(0,1700,222,170));
    cenario.addFrame(sf::IntRect(222,1700,222,170));
    cenario.addFrame(sf::IntRect(444,1700,222,170));
    cenario.addFrame(sf::IntRect(666,1700,222,170));
    cenario.addFrame(sf::IntRect(888,1700,222,170));
    cenario.addFrame(sf::IntRect(1110,1700,222,170));
    cenario.addFrame(sf::IntRect(1332,1700,222,170));
    cenario.addFrame(sf::IntRect(1554,1700,222,170));
    cenario.addFrame(sf::IntRect(1776,1700,222,170));
    cenario.addFrame(sf::IntRect(1998,1700,222,170));
    cenario.addFrame(sf::IntRect(2220,1700,222,170));
    cenario.addFrame(sf::IntRect(2442,1700,222,170));
    cenario.addFrame(sf::IntRect(2664,1700,222,170));
    cenario.addFrame(sf::IntRect(2886,1700,222,170));
    cenario.addFrame(sf::IntRect(3108,1700,222,170));
    cenario.addFrame(sf::IntRect(3330,1700,222,170));
    cenario.addFrame(sf::IntRect(3552,1700,222,170));
    cenario.addFrame(sf::IntRect(3774,1700,222,170));
    cenario.addFrame(sf::IntRect(0,1870,222,170));
    cenario.addFrame(sf::IntRect(222,1870,222,170));
    cenario.addFrame(sf::IntRect(444,1870,222,170));
    cenario.addFrame(sf::IntRect(666,1870,222,170));
    cenario.addFrame(sf::IntRect(888,1870,222,170));
    cenario.addFrame(sf::IntRect(1110,1870,222,170));
    cenario.addFrame(sf::IntRect(1332,1870,222,170));
    cenario.addFrame(sf::IntRect(1554,1870,222,170));
    cenario.addFrame(sf::IntRect(1776,1870,222,170));
    cenario.addFrame(sf::IntRect(1998,1870,222,170));
    cenario.addFrame(sf::IntRect(2220,1870,222,170));
    cenario.addFrame(sf::IntRect(2442,1870,222,170));
    cenario.addFrame(sf::IntRect(2664,1870,222,170));
    cenario.addFrame(sf::IntRect(2886,1870,222,170));
    cenario.addFrame(sf::IntRect(3108,1870,222,170));
    cenario.addFrame(sf::IntRect(3330,1870,222,170));
    cenario.addFrame(sf::IntRect(3552,1870,222,170));
    cenario.addFrame(sf::IntRect(3774,1870,222,170));
    cenario.addFrame(sf::IntRect(0,2040,222,170));
    cenario.addFrame(sf::IntRect(222,2040,222,170));
    cenario.addFrame(sf::IntRect(444,2040,222,170));
    cenario.addFrame(sf::IntRect(666,2040,222,170));
    cenario.addFrame(sf::IntRect(888,2040,222,170));
    cenario.addFrame(sf::IntRect(1110,2040,222,170));
    cenario.addFrame(sf::IntRect(1332,2040,222,170));
    cenario.addFrame(sf::IntRect(1554,2040,222,170));
    cenario.addFrame(sf::IntRect(1776,2040,222,170));
    cenario.addFrame(sf::IntRect(1998,2040,222,170));
    cenario.addFrame(sf::IntRect(2220,2040,222,170));
    cenario.addFrame(sf::IntRect(2442,2040,222,170));
    cenario.addFrame(sf::IntRect(2664,2040,222,170));
    cenario.addFrame(sf::IntRect(2886,2040,222,170));
    cenario.addFrame(sf::IntRect(3108,2040,222,170));
    cenario.addFrame(sf::IntRect(3330,2040,222,170));
    cenario.addFrame(sf::IntRect(3552,2040,222,170));
    cenario.addFrame(sf::IntRect(3774,2040,222,170));
    cenario.addFrame(sf::IntRect(0,2210,222,170));
    cenario.addFrame(sf::IntRect(222,2210,222,170));
    cenario.addFrame(sf::IntRect(444,2210,222,170));
    cenario.addFrame(sf::IntRect(666,2210,222,170));
    cenario.addFrame(sf::IntRect(888,2210,222,170));
    cenario.addFrame(sf::IntRect(1110,2210,222,170));
    cenario.addFrame(sf::IntRect(1332,2210,222,170));
    cenario.addFrame(sf::IntRect(1554,2210,222,170));
    cenario.addFrame(sf::IntRect(1776,2210,222,170));
    cenario.addFrame(sf::IntRect(1998,2210,222,170));
    cenario.addFrame(sf::IntRect(2220,2210,222,170));
    cenario.addFrame(sf::IntRect(2442,2210,222,170));
    cenario.addFrame(sf::IntRect(2664,2210,222,170));
    cenario.addFrame(sf::IntRect(2886,2210,222,170));
    cenario.addFrame(sf::IntRect(3108,2210,222,170));
    cenario.addFrame(sf::IntRect(3330,2210,222,170));
    cenario.addFrame(sf::IntRect(3552,2210,222,170));
    cenario.addFrame(sf::IntRect(3774,2210,222,170));
    cenario.addFrame(sf::IntRect(0,2380,222,170));
    cenario.addFrame(sf::IntRect(222,2380,222,170));
    cenario.addFrame(sf::IntRect(444,2380,222,170));
    cenario.addFrame(sf::IntRect(666,2380,222,170));
    cenario.addFrame(sf::IntRect(888,2380,222,170));
    cenario.addFrame(sf::IntRect(1110,2380,222,170));
    cenario.addFrame(sf::IntRect(1332,2380,222,170));
    cenario.addFrame(sf::IntRect(1554,2380,222,170));
    cenario.addFrame(sf::IntRect(1776,2380,222,170));
    cenario.addFrame(sf::IntRect(1998,2380,222,170));
    cenario.addFrame(sf::IntRect(2220,2380,222,170));
    cenario.addFrame(sf::IntRect(2442,2380,222,170));
    cenario.addFrame(sf::IntRect(2664,2380,222,170));
    cenario.addFrame(sf::IntRect(2886,2380,222,170));
    cenario.addFrame(sf::IntRect(3108,2380,222,170));
    cenario.addFrame(sf::IntRect(3330,2380,222,170));
    cenario.addFrame(sf::IntRect(3552,2380,222,170));
    cenario.addFrame(sf::IntRect(3774,2380,222,170));
    cenario.addFrame(sf::IntRect(0,2550,222,170));
    cenario.addFrame(sf::IntRect(222,2550,222,170));
    cenario.addFrame(sf::IntRect(444,2550,222,170));
    cenario.addFrame(sf::IntRect(666,2550,222,170));
    cenario.addFrame(sf::IntRect(888,2550,222,170));
    cenario.addFrame(sf::IntRect(1110,2550,222,170));
    cenario.addFrame(sf::IntRect(1332,2550,222,170));
    cenario.addFrame(sf::IntRect(1554,2550,222,170));
    cenario.addFrame(sf::IntRect(1554,2550,222,170));
    cenario.addFrame(sf::IntRect(1776,2550,222,170));
    cenario.addFrame(sf::IntRect(1998,2550,222,170));
    cenario.addFrame(sf::IntRect(2220,2550,222,170));
    cenario.addFrame(sf::IntRect(2442,2550,222,170));
    cenario.addFrame(sf::IntRect(2664,2550,222,170));
    cenario.addFrame(sf::IntRect(2886,2550,222,170));
    cenario.addFrame(sf::IntRect(3108,2550,222,170));
    cenario.addFrame(sf::IntRect(3108,2550,222,170));
    cenario.addFrame(sf::IntRect(3108,2550,222,170));


    AnimatedSprite cenarioAnimatedSprite = AnimatedSprite(sf::seconds(FPS), true, false);
    cenarioAnimatedSprite.setPosition(maskPosition);
    cenarioAnimatedSprite.setLooped(false);
    
    // animation characters
    sf::Texture textureCharacters;
    if (!textureCharacters.loadFromFile(resourcePath() + "characters.png")) {
        std::cout << "fail load texture characters.png" << std::endl;
        return -1;
    }
    
    std::vector<AnimatedSprite> currentAnimatedSpriteVec(4);
    currentAnimatedSpriteVec.at(0) = AnimatedSprite(sf::seconds(FPS), true, true);
    currentAnimatedSpriteVec.at(1) = AnimatedSprite(sf::seconds(FPS), true, true);
    currentAnimatedSpriteVec.at(2) = AnimatedSprite(sf::seconds(FPS), true, true);
    currentAnimatedSpriteVec.at(3) = AnimatedSprite(sf::seconds(FPS), true, true);
    currentAnimatedSpriteVec.at(0).setPosition(hidePosition);
    currentAnimatedSpriteVec.at(1).setPosition(hidePosition);
    currentAnimatedSpriteVec.at(2).setPosition(hidePosition);
    currentAnimatedSpriteVec.at(3).setPosition(hidePosition);
    animatedSpriteSheets.push_back(&currentAnimatedSpriteVec.at(0));
    animatedSpriteSheets.push_back(&currentAnimatedSpriteVec.at(1));
    animatedSpriteSheets.push_back(&currentAnimatedSpriteVec.at(2));
    animatedSpriteSheets.push_back(&currentAnimatedSpriteVec.at(3));
    
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
    int countChar = 0;
    
    std::vector<Animation> currentAnimationVec(4);
    
    
    
    //kinect
    Freenect::Freenect freenect;
    MyFreenectDevice& device = freenect.createDevice<MyFreenectDevice>(0);
    device.startVideo();
    device.startDepth();
    
    //keyboard control
    std::ostringstream file;
    string filename("snapshot");
    string suffix(".bmp");
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
    double lackProgress = 0.1;
    double lackProgressCount = 0;
    double w = 640;
    
    bool newAnimation = true;
    bool newSnap = true;
    
    int nextChange = 0;
    int idx = 0;
    
    bool reachedEnd = false;
    
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
            for(i = 0; i < rows; ++i){
                for( j = 0; j < cols; ++j ){
                    if (differenceImage.at<uchar>(i,j) > 0){
                        totalWhite++;
                    }
                }
            }
            
            if (totalWhite >= 0) {
                perc = cols*rows;
                perc = totalWhite/perc;
                reachedEnd = cenarioAnimatedSprite.getCurrentFrame() > 293;
                
                if (!reachedEnd && perc > lackProgress){ //forward
                    lackProgressCount = 0;
                    progress += 0.25;
                    
                    idx = ((int)progress % 4);
                    //std::cout  << "+ " << progress << "\tidx: " << idx << std::endl;
                    if (cenarioAnimatedSprite.getCurrentFrame() > FRAME_START_ANIM_CHARS){
                        if (newAnimation && idx < 4){
                            if (countAnimation+countChar >= animations.size()) {
                                countAnimation = 0;
                            }
                            currentAnimatedSpriteVec.at(countChar).restart();
                            currentAnimatedSpriteVec.at(countChar).play(animations.at(countAnimation+countChar));
                            currentAnimatedSpriteVec.at(countChar).setPosition(maskPosition+positionVec.at(countChar)+pivot);
                            countChar++;
                            if (countChar >= currentAnimatedSpriteVec.size()){
                                newAnimation = false;
                            }
                        }
                        if (!newAnimation && !currentAnimatedSpriteVec.at(idx).isPlaying() and idx < 4){
                            currentAnimatedSpriteVec.at(idx).play(animations.at(countAnimation+idx));
                            currentAnimatedSpriteVec.at(idx).setPosition(maskPosition+positionVec.at(idx)+pivot);
                        
                        }
                    }
                    
                    cenarioAnimatedSprite.setPlayReverse(false);
                    cenarioAnimatedSprite.play(cenario);
                    
                    if (cenarioAnimatedSprite.getCurrentFrame() == 15 && !thunderEffect.isPlaying()) {
                        thunderEffect.restart();
                        thunderEffect.setLooped(false);
                        thunderEffect.play(trovao);
                        thunderEffect.setPosition(maskPosition);
                    }
                    
                    if (newSnap && cenarioAnimatedSprite.getCurrentFrame() == 20) {
                        newSnap = false;
                        file.str("");
                        file.clear();
                        auto time = std::chrono::system_clock::now();
                        auto since_epoch = time.time_since_epoch();
                        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(since_epoch);
                        long now = millis.count();
                        file << "/Users/luizaprata/Desktop/snapshot/"<< filename << now << suffix;
                        cv::imwrite(file.str(), rgbMat);
                        
                        std::cout << file.str() << std::endl;
                        
                    }
                    
                    
                    if (cenarioAnimatedSprite.getCurrentFrame() > 150 && !specialEffect.isPlaying()) {
                        specialEffect.restart();
                        specialEffect.setLooped(true);
                        specialEffect.play(end);
                        specialEffect.setPosition(maskPosition);
                    }
                    
                } else { //back
                    progress = 0;
                    lackProgressCount += 0.25;
                    if (lackProgressCount > 1 && !reachedEnd){
                        cenarioAnimatedSprite.setPlayReverse(true);
                    }
                    
                    //sumindo aos poucos
                    idx = ((int)lackProgressCount % 4);
                    //std::cout  << "- " << lackProgressCount << "\tidx:" << idx  << std::endl;
                    if (idx<4) {
                        if (lackProgressCount > 4 || cenarioAnimatedSprite.getCurrentFrame() < FRAME_START_ANIM_CHARS)
                        {
                            currentAnimatedSpriteVec.at(idx).setPosition(hidePosition);
                            
                            if (!reachedEnd && cenarioAnimatedSprite.getCurrentFrame() < 250){
                                specialEffect.stop();
                                specialEffect.setPosition(hidePosition);
                            }
                            
                        }
                        else if (currentAnimatedSpriteVec.at(idx).isPlaying())
                        {
                            currentAnimatedSpriteVec.at(idx).stop();
                        }
                    }
                }
                
                //std::cout << "------frame:" << cenarioAnimatedSprite.getCurrentFrame() << "newSnap: " << newSnap<< std::endl;
                
                if (perc == 1){
                    cenarioAnimatedSprite.stop();
                }
                
                
                if (cenarioAnimatedSprite.getCurrentFrame() < FRAME_START_ANIM_CHARS) {
                    if (!startAnimated.isPlaying()){
                        startAnimated.restart();
                        startAnimated.setLooped(true);
                        startAnimated.play(start);
                        startAnimated.setPosition(maskPosition);
                    }
                    
                } else {
                    startAnimated.stop();
                    startAnimated.setPosition(hidePosition);
                }
                
                
                if (!thunderEffect.isPlaying()){
                    thunderEffect.setPosition(hidePosition);
                }
                
                //std::cout  << "progress:" << progress << "\nperc:" << perc <<  "\ncurrentFrame:" << cenarioAnimatedSprite.getCurrentFrame() <<  " -- " << cenarioAnimatedSprite.isPlaying() << std::endl;
                
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
        
        thunderEffect.update(frameTime);
        window.draw(thunderEffect);
        
        startAnimated.update(frameTime);
        window.draw(startAnimated);
        
        
        //personagens
        for(int i = 0; i<4; i++) {
            currentAnimatedSpriteVec.at(i).update(frameTime);
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
                    
                    // reset
                    case sf::Keyboard::R:
                        std::cout << "restart" << animatedSpriteSheets.size() << std::endl;
                        countAnimation += 4;
                        countChar = 0;
                        newAnimation = true;
                        newSnap = true;
                        for(i = 0; i < animatedSpriteSheets.size(); i++){
                            animatedSpriteSheets.at(i)->setPosition(hidePosition);
                            animatedSpriteSheets.at(i)->stop();
                            
                        }
                        progress = 0;
                        reachedEnd = false;
                        cenarioAnimatedSprite.restart();
                        cenarioAnimatedSprite.play(cenario);
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
