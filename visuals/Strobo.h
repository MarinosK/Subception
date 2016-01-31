//
//  Strobo.h
//  Visuals
//
//  Created by Marinos Koutsomichalis on 3/9/13.
//
//

// creates random black or white flashes according to probability

#ifndef __Visuals__Strobo__
#define __Visuals__Strobo__

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

class Strobo {
public:
    void setup(float, float);
    void update();
    void draw();
    float getProb() const; // no getWhiteProb cause I don't plan to use it..
    void setProb(float, float);
    void translate(char);           // moves the scene to left (1) or right(2) or nowhere
private:
    char mode;  // choose between off (1), black (2), white (3)
    float probability; // the Probability of strobo to happen
    float whiteProbability; // the probability of it being white
    char translateMode;
    ci::Vec2f left, right;          // translate coordinates
    ci::Rectf rectangle; // a rectangle is used to mute the screen
};

// inlined functions here

inline void Strobo::setProb(float probability_, float whiteProbability_) {
    probability = probability_;
    whiteProbability = whiteProbability_;
}

inline float Strobo::getProb() const {
    return probability;
}

inline void Strobo::translate(char mode_) {
    translateMode = mode_;
}

#endif /* defined(__Visuals__Strobo__) */
