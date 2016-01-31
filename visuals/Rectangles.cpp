//
//  Rectangles.cpp
//  Visuals
//
//  Created by Marinos Koutsomichalis on 3/6/13.
//
//

#include "Rectangles.h"

void Rectangle::setup(unsigned short levels_, float duration_, float frequency_) {
    levels = levels_;
    duration = duration_;
    frequency = frequency_;
    done = false;
    perlin = ci::Perlin();
    envelope.setup(duration, duration/2, duration/2);
    calculateRects();
    calculateColors();
}

void Rectangle::calculateRects() {
    float size = cinder::app::getWindowWidth() / levels;
    for (int i = 0; i<levels; ++i) {
        ci::Rectf r = ci::Rectf(i*size, 0, (i+1)*size, cinder::app::getWindowHeight());
        try {
            rects.push_back(r);
        } catch (...) {
            ci::app::console() << "EXception pushing_back rect (Rectangle Class)" << std::endl;
        }
    }
}

void Rectangle::calculateColors() {
    for (int i = 0; i<levels; ++i) {
        try {
            colors.push_back(ci::ColorA(0,0,0,0));
        } catch (...) {
            ci::app::console() << "I can't see any problem but..." << std::endl;
        }
    }
}

void Rectangle::updateColors() {
    float noise = perlin.fBm(duration);
    noise = (noise + 1) / 2;
    float hue, weightedHue;
    float alpha = envelope.getVal() * 0.8;
    if (frequency < 75) {
        hue = ((frequency-40)/35) * envelope.getVal();
        weightedHue = hue / levels;
        for (int i = 0; i<colors.size(); ++i) {
            float r,g,b;
            r = hue;
            g = weightedHue * i;
            b = noise;
            try {
                colors[i] = ci::ColorA(r,g,b,alpha);
            } catch (...) {
                ci::app::console() << "I can't see any problem but..." << std::endl;
            }
        }
    } else if (frequency < 6000) {
        hue = ((frequency-4000)/2000) * envelope.getVal();
        weightedHue = hue / levels;
        for (int i = 0; i<colors.size(); ++i) {
            float r,g,b;
            r = weightedHue * i;
            g = hue;
            b = noise;
            try {
                colors[i] = ci::ColorA(r,g,b,alpha);
            } catch (...) {
                ci::app::console() << "I can't see any problem but..." << std::endl;
            }
        }
    } else {
        hue = ((frequency-6000)/7000) * envelope.getVal();
        weightedHue = hue / levels;
        for (int i = 0; i<colors.size(); ++i) {
            float r,g,b;
            r = noise;
            g = weightedHue * i;
            b = hue;
            try {
                colors[i] = ci::ColorA(r,g,b,alpha);
            } catch (...) {
                ci::app::console() << "I can't see any problem but..." << std::endl;
            }
        }
    }
}

void Rectangle::update() {
    if (envelope.isDone()) {
        if (not done) {
            done = true;
        }
    } else {
        try {
            updateColors();
        } catch (...) {
            ci::app::console() << "maybe here ??? " << std::endl;
        }
    }
}

void Rectangle::draw() {
    for (std::vector<ci::Rectf>::iterator it = rects.begin() ; it != rects.end(); ++it) {
        try {
            glColor4f(colors[std::distance(rects.begin(), it)]);
        } catch (...) {
            ci::app::console() << "Exception attempting to change color" << std::endl;
        }
        ci::gl::drawSolidRect(*it);
    }
}


// rectangle Factory

void RectangleFactory::spawn(unsigned short levels, float duration, float frequency) {
    Rectangle rect;
    rect.setup(levels, duration, frequency);
    rectangles.push_back(rect);
}

void RectangleFactory::update() {
    for (std::vector<Rectangle>::iterator rect = rectangles.begin() ; rect != rectangles.end(); ++rect) {
        try {
            rect->update();
        } catch (...) {
            ci::app::console() << "exception updating rectangle" << std::endl;
        }
    }
    for(int i=0; i<rectangles.size(); ++i) {
        if (rectangles[i].isDone() == true) {
            try {
                rectangles.erase(rectangles.begin()+i);
            } catch (...) {
                ci::app::console() << "I hope it fucking works now..." << std::endl;
            }
        }
    }
}

void RectangleFactory::draw() {
    if (translateMode == 1) {
        ci::gl::pushMatrices();
        ci::gl::translate(left);
        for (std::vector<Rectangle>::iterator rect = rectangles.begin() ; rect != rectangles.end(); ++rect)
        {
            try {
                rect->draw();
                glColor4f(0,0,0,1);
            } catch (...) {
                ci::app::console() << "exception drawing rectangle" << std::endl;
            }
        }
        ci::gl::popMatrices();
    } else if (translateMode == 2) {
        ci::gl::pushMatrices();
        ci::gl::translate(right);
        for (std::vector<Rectangle>::iterator rect = rectangles.begin() ; rect != rectangles.end(); ++rect)
        {
            try {
                rect->draw();
                glColor4f(0,0,0,1);
            } catch (...) {
                ci::app::console() << "exception drawing rectangle" << std::endl;
            }
        }
        ci::gl::popMatrices();
    } else {
        for (std::vector<Rectangle>::iterator rect = rectangles.begin() ; rect != rectangles.end(); ++rect)
        {
            try {
                rect->draw();
                glColor4f(0,0,0,1);
            } catch (...) {
                ci::app::console() << "exception drawing rectangle" << std::endl;
            }
        }
    }
}



