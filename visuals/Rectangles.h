//
//  Rectangles.h
//  Visuals
//
//  Created by Marinos Koutsomichalis on 3/6/13.
//
//

#ifndef __Visuals__Rectangles__
#define __Visuals__Rectangles__

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Perlin.h"
#include "Envelope.h"


class Rectangle {
public:
    void setup(unsigned short levels, float duration, float frequency);
    void update();
    void draw();
    bool isDone() const;
private:
    unsigned short levels;
    float frequency;
    float duration;
    bool done;
    Envelope envelope;
    cinder::Perlin perlin;
    std::vector<ci::ColorA> colors;
    std::vector<ci::Rectf> rects;
    void calculateRects();
    void calculateColors();
    void updateColors();
    
};

// 
class RectangleFactory {
public:
    void setup();
    void update();
    void draw();
    void spawn(unsigned short levels, float duration, float frequency);
    void translate(char);           // moves the scene to left (1) or right(2) or nowhere
private:
    std::vector<Rectangle> rectangles;
    char translateMode;
    ci::Vec2f left, right;          // translate coordinates
};


// inline functions here

inline bool Rectangle::isDone() const {
    return done;
}

inline void RectangleFactory::translate(char mode_) {
    translateMode = mode_;
}

inline void RectangleFactory::setup() {
    right= ci::Vec2f(ci::app::getWindowWidth()/2, 0);
    left = ci::Vec2f(-1 * (ci::app::getWindowWidth()/2), 0);
    translateMode = 0;
}


#endif /* defined(__Visuals__Rectangles__) */


