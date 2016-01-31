//
//  Envelope.cpp
//  Visuals
//
//  Created by Marinos Koutsomichalis on 3/7/13.
//
//

#include "Envelope.h"


void Envelope::setup(float duration_, float attackTime_, float decayTime_) {
    duration = duration_ ;
	attackTime = attackTime_ ;
    decayTime = decayTime_ ;
    start = ci::app::getElapsedSeconds();
    done = false;
}

float Envelope::getVal() {
    float output;
    
    float currentTime = ci::app::getElapsedSeconds();
    float difference = currentTime - start;
    
    if (difference <= 0.0f) {output = 0.0f; return output;}
    if (difference >= duration ) {output = 0.0f; done = true; return output;}
    if (difference <= attackTime) {output = difference / attackTime; return output;}
    if (difference > attackTime) {
        output = (difference - attackTime) / decayTime;
        output = 1 - output;
        return output;
    }
    
    return 0.0f;
}
