//
//  Shadows.h
//  Visuals
//
//  Created by Marinos Koutsomichalis on 3/8/13.
//
//

#ifndef __Visuals__Shadows__
#define __Visuals__Shadows__

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Perlin.h"
#include <math.h>
#include "OscSender.h"

static const int MAX_CHILDREN =  4;
static const int MAX_LEVELS = 4;

class Shadows {
public:
    void setup(float level_, float index_, float x, float y);
    void update(float, float);
    void draw();
    cinder::osc::Message getData();
    void translate(char);           // moves the scene to left (1) or right(2) or nowhere    
private:
    void calculateCoordinates(float, float);
    void calculatePoints();
    void calculateColors();
    float level, index, stroke;
    int x, y;
    float x1,y1;
    float angle, angleChange;
    int length, lengthChange;
    std::vector<Shadows> children;
    ci::ColorA color;
    std::vector<ci::Vec2f> points;
    ci::Perlin perlin;
    char translateMode;
    ci::Vec2f left, right;          // translate coordinates
};


// inline functions here

inline void Shadows::translate(char mode_) {
    translateMode = mode_;
}


#endif /* defined(__Visuals__Shadows__) */
