//
//  Strobo.cpp
//  Visuals
//
//  Created by Marinos Koutsomichalis on 3/9/13.
//
//

#include "Strobo.h"


void Strobo::setup(float probability_, float whiteProbability_) {
    probability = probability_;
    whiteProbability = whiteProbability_;
    right= ci::Vec2f(ci::app::getWindowWidth()/2, 0);
    left = ci::Vec2f(-1 * (ci::app::getWindowWidth()/2), 0);
    rectangle = ci::Rectf(0,0,ci::app::getWindowWidth(),ci::app::getWindowHeight());
    translateMode = 0;
}

void Strobo::update() {
    if (ci::Rand::randFloat(0, 1.0f) < probability) {
        if (ci::Rand::randFloat(0, 1.0f) < whiteProbability ) {
            mode = 3;
        } else {
            mode = 2;
        }
    } else {
        mode = 1;
    }
}

void Strobo::draw() {
    if (translateMode == 1) {
        ci::gl::pushMatrices();
        ci::gl::translate(left);
        if (mode==3) {
            ci::gl::color(0,0,0);
            ci::gl::drawSolidRect(rectangle);
        } else if (mode==2) {
            ci::gl::color(1,1,1);
            ci::gl::drawSolidRect(rectangle);
        }
        ci::gl::popMatrices();
    } else if (translateMode == 2) {
        ci::gl::pushMatrices();
        ci::gl::translate(right);
        if (mode==3) {
            ci::gl::color(0,0,0);
            ci::gl::drawSolidRect(rectangle);
        } else if (mode==2) {
            ci::gl::color(1,1,1);
            ci::gl::drawSolidRect(rectangle);
        }
        ci::gl::popMatrices();
    } else {
        if (mode==3) {
            ci::gl::color(0,0,0);
            ci::gl::drawSolidRect(rectangle);
        } else if (mode==2) {
            ci::gl::color(1,1,1);
            ci::gl::drawSolidRect(rectangle);
        }
    }
}


