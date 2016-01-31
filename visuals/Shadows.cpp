//
//  Shadows.cpp
//  Visuals
//
//  Created by Marinos Koutsomichalis on 3/8/13.
//
//

#include "Shadows.h"

void Shadows::setup(float level_, float index_, float x_, float y_){
    right= ci::Vec2f(ci::app::getWindowWidth()/2, 0);
    left = ci::Vec2f(-1 * (ci::app::getWindowWidth()/2), 0);
    translateMode = 0;
    level = level_;
    index = index_;
    stroke = (1/level) * 10;
    length = (1 / level) * ci::Rand::randFloat(1200);
    angle = ci::Rand::randFloat(2 * M_PI);
    lengthChange = ci::Rand::randInt(-50,50);
    angleChange = ci::Rand::randFloat(-3.0f,3.0f);
    for (int i=0; i<4; ++i) {
        ci::Vec2f point = ci::Vec2f(x_,y_);
        points.push_back(point);
    }
    update(ci::app::getWindowWidth() / 2, ci::app::getWindowHeight() / 2);
    if (level < MAX_LEVELS) {
        for (int i=0; i<MAX_CHILDREN; i++) {
            Shadows child;
            child.setup(level + 1, i,x1, y1);
            children.push_back(child);
        }
    }
}

void Shadows::update(float x_, float y_) {
    calculateCoordinates(x_, y_);
    calculatePoints();
    calculateColors();
    for (std::vector<Shadows>::iterator it = children.begin() ; it != children.end(); ++it) {
        it->update(x1,y1);
    }
}

void Shadows::calculateCoordinates(float x_, float y_) {
    x = x_;
    y = y_;
    length = ci::Rand::randFloat(200);
    if (angle > (2 * M_PI)) {
        angle=0;
    } else {
        angle = angle + angleChange;
    }
    length -= lengthChange;
    if (length < 300) { lengthChange *= -1; }
    else if (length > 1000) { lengthChange *= -1; }
    x1 = x + (length * cos(angle));
    y1 = y + (length * sin(angle));
}

void Shadows::calculatePoints() {
    points.clear();
    int random = ci::Rand::randInt(10);
    if (random < 5) {
        points.push_back(ci::Vec2f(x,0));
        points.push_back(ci::Vec2f(x1,y1));
        points.push_back(ci::Vec2f(0,y));
        points.push_back(ci::Vec2f(ci::app::getWindowWidth(),y));
    } else {
        points.push_back(ci::Vec2f(x,0));
        points.push_back(ci::Vec2f(x1,ci::app::getWindowHeight()));
        points.push_back(ci::Vec2f(0,y1));
        points.push_back(ci::Vec2f(x1,y));
    }
}

void Shadows::calculateColors() {
    float r,g,b;
    r = (perlin.fBm(ci::app::getElapsedFrames()) + 1) / 2 ;
    g = (cos(ci::app::getElapsedSeconds()) + 1) / 2 ;
    b = ci::randFloat(1.0f);
    color = ci::ColorA(r,g,b,1);
}

void Shadows::draw() {
    if (translateMode == 1) {
        ci::gl::pushMatrices();
        ci::gl::translate(left);
        if (level > 1) {
            glLineWidth(stroke);
            ci::gl::color(color);
            ci::gl::drawLine(points[0], points[1]);
            ci::gl::drawLine(points[2], points[3]);
        }
        for (std::vector<Shadows>::iterator it = children.begin() ; it != children.end(); ++it) {
            it->draw();
        }
        ci::gl::popMatrices();
    } else if (translateMode == 2) {
        ci::gl::pushMatrices();
        ci::gl::translate(right);
        if (level > 1) {
            glLineWidth(stroke);
            ci::gl::color(color);
            ci::gl::drawLine(points[0], points[1]);
            ci::gl::drawLine(points[2], points[3]);
        }
        for (std::vector<Shadows>::iterator it = children.begin() ; it != children.end(); ++it) {
            it->draw();
        }
        ci::gl::popMatrices();
    } else {
        if (level > 1) {
            glLineWidth(stroke);
            ci::gl::color(color);
            ci::gl::drawLine(points[0], points[1]);
            ci::gl::drawLine(points[2], points[3]);
        }
        for (std::vector<Shadows>::iterator it = children.begin() ; it != children.end(); ++it) {
            it->draw();
        }
    }
}

cinder::osc::Message Shadows::getData() {
        cinder::osc::Message message;
    try {
        message.addFloatArg(x);
        message.addFloatArg(x1);
        message.addFloatArg(y);
        message.addFloatArg(y1);
    } catch (...) {
        ci::app::console() << "Error adding float to Message (Shadows)" << std::endl;
    }
    return message;
}

