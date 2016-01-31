//
//  Envelope.h
//  Visuals
//
//  Created by Marinos Koutsomichalis on 3/7/13.
//
//

#ifndef __Visuals__Envelope__
#define __Visuals__Envelope__

#include "cinder/app/AppBasic.h"

class Envelope {
public:
	void setup(float duration, float attackTime, float decayTime); // arguments should be in seconds
	float getVal();
    bool isDone() const;
private:
	float attackTime, decayTime;
    float duration;
    float start; // starting time
    bool done;
};

inline bool Envelope::isDone() const {
    return done;
}


#endif /* defined(__Visuals__Envelope__) */
