
// by Marinos Koutsomichalis - March 2013, Brussels.

// this application is only to be used by its creator for the purpose of his media art installation 'Subception'
// this application renders the 2D graphics for projection and also interacts with supercollider via OSC

// osc
#define IN_PORT 3000                // the port to listen to OSC messages
#define SEND_PORT 57120             // the port to send OSC messages (note this is not the same as the port from which the messages are sent.. this is randomly selected on runtime)
#define HOST "localhost"            // the host that runs supercollider code
// strobo
#define STROBO_ON_PROB 20           // the probabiblity to have strobo (/3000)
#define STROBO_PROB 0.4             // the frequency of strobing
#define STROBO_WHITE_PROB 0.4       // the probability of strobing being white
#define STROBO_MIN_DURATION 30     // in frames
#define STROBO_MAX_DURATION 150     // in frames
// rectangles
#define RECTANGLES_OFF_PROB 0       // the probabiblity NOT to have rectangles
#define MAX_RECTANGLES_SPLITS 2     // the maximum possibles splits in the projection
#define RECTANGLES_OFF_MIN_DURATION 60 // in frames
#define RECTANGLES_OFF_MAX_DURATION 240 // in frames
// shadows
#define SHADOWS_ON_PROB 10          // the probabiblity to have shadows (/3000)
#define SHADOWS_MIN_DURATION 200   // in frames
#define SHADOWS_MAX_DURATION 580  // in frames
// to do -> add defines for the coordinates in the screen
// translates
#define STROBO_TRANSL_PROB 0.02          // /1.0
#define STROBO_TRANSL_RECT_PROB 0.02     // /1.0
#define SHADOWS_TRANSL_PROB 0.02          // /1.0
#define SHADOWS_TRANSL_RECT_PROB 0.03     // /1.0


#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "OscListener.h"
#include "OscSender.h"
#include "Rectangles.h"
#include "Shadows.h"
#include "Strobo.h"

// ------------------------------------------------------------------------------------------------
class VisualsApp : public cinder::app::AppBasic {
  public:
    void prepareSettings( Settings*);
	void setup();
	void update();
	void draw();
    void keyDown(ci::app::KeyEvent);
private:
    cinder::osc::Listener listener;     // osc Listener for listening to supercollider
    cinder::osc::Sender sender;         // osc Sender for sending data to supercollider
    RectangleFactory rectangles;        // rectangle factory spawns color rectangles according to the frequencies it receives from supercollider
    Shadows shadow;                     // Shadows creative a generative pattern with lines aperiodically
    Strobo strobo;                      // Strobo creates b or w flashes occasionally
    bool shadowsOn, rectanglesOn, stroboOn;       // flags to control the apperarence of elements
    unsigned int shadowsCounter, rectanglesCounter, stroboCounter;  // counters to control the duration of the various elements
    void receiveOSC_rectangle();      // a function to receive osc messages and spawn rectangles
    void translate();                 // a function to take care of all translations
};

// ------------------------------------------------------------------------------------------------
void VisualsApp::prepareSettings( Settings* settings) {
    settings->setWindowSize(1280, 768);
    settings->setFrameRate( 60.0f );
};

// ------------------------------------------------------------------------------------------------
void VisualsApp::setup(){
    // graphics settings
    ci::gl::enableAlphaBlending();
    
    // osc settings
    listener.setup(IN_PORT);
    sender.setup(HOST, SEND_PORT);
    
    // setup the various graphics objects
    rectangles.setup();
    strobo.setup(STROBO_PROB, STROBO_WHITE_PROB);
    shadow.setup(1,0, 640,500);
    
    // initialize the various flags
    rectanglesOn = true;
    shadowsOn = false;
    stroboOn = false;
    
}

// ------------------------------------------------------------------------------------------------
void VisualsApp::update(){
    
    try {
        // here we decide if we want rectrangles and if yes listen to messages from SC to create them algorithmically
        if (rectanglesOn) {
            if (ci::Rand::randInt(4000) < RECTANGLES_OFF_PROB) {
                rectanglesOn = false;
                rectanglesCounter = ci::Rand::randInt(RECTANGLES_OFF_MIN_DURATION, RECTANGLES_OFF_MAX_DURATION);
            } else {
                receiveOSC_rectangle();
                rectangles.update();
            }
        } else {
            if (rectanglesCounter > 0) {
                rectanglesCounter -= 1;
            } else if (rectanglesCounter == 0) {
                rectanglesOn = true;
            }
        }
        
        // decide if we want shadows and calculate them also send data to SC
        if (not shadowsOn) {
            if (ci::Rand::randInt(4000) < SHADOWS_ON_PROB) {
                shadowsOn = true;
                shadowsCounter = ci::Rand::randInt(SHADOWS_MIN_DURATION,SHADOWS_MAX_DURATION);
                // notify SC
                cinder::osc::Message message;
                message.setAddress("/shadows/duration");
                message.addIntArg(shadowsCounter);
                message.setRemoteEndpoint(HOST, SEND_PORT);
                sender.sendMessage(message);
            }
        } else {
            if (shadowsCounter > 0) {
                shadowsCounter -= 1;
                shadow.update(640,530);
                // send data to supercollider
                cinder::osc::Message message;
                message = shadow.getData();
                message.setAddress("/shadows/freqs");
                message.setRemoteEndpoint(HOST, SEND_PORT);
                sender.sendMessage(message);
            } else if (shadowsCounter == 0) {
                shadowsOn = false;
            }
        }
        
        // decide if we want strobo and calculate them
        if (not stroboOn) {
            if (ci::Rand::randInt(4000) < STROBO_ON_PROB) {
                stroboOn = true;
                stroboCounter = ci::Rand::randInt(STROBO_MIN_DURATION,STROBO_MAX_DURATION);
            }
        } else {
            if (stroboCounter > 0) {
                stroboCounter -= 1;
                strobo.update();
            } else if (stroboCounter == 0) {
                stroboOn = false;
            }
        }
        
        // translate algorithmically
        translate();
        
    } catch (...) {
        ci::app::console() << "Something went wrong in the update callback.." << std::endl;
    }
}

// ------------------------------------------------------------------------------------------------
void VisualsApp::receiveOSC_rectangle() {
    while (listener.hasWaitingMessages()) {
        cinder::osc::Message message;
        listener.getNextMessage(&message);
        float freq = 0;
        float duration = 0;
        if (message.getAddress() == "/spawn") {
            if (message.getArgType(0) == ci::osc::TYPE_FLOAT) {
                try {
                    duration = message.getArgAsFloat(0);
                }
                catch (...) {
                    cinder::app::console() << "Exception reading argument as Float" << std::endl;
                }
            } else if (message.getArgType(0) == ci::osc::TYPE_INT32) {
                try {
                    duration = (float) message.getArgAsInt32(0);
                }
                catch (...) {
                    cinder::app::console() << "Exception reading argument as int32" << std::endl;
                }
            }
            if (message.getArgType(1) == ci::osc::TYPE_FLOAT) {
                try {
                    freq = message.getArgAsFloat(1);
                }
                catch (...) {
                    cinder::app::console() << "Exception reading argument as Float" << std::endl;
                }
            } else if (message.getArgType(1) == ci::osc::TYPE_INT32) {
                try {
                    freq = (float) message.getArgAsInt32(1);
                }
                catch (...) {
                    cinder::app::console() << "Exception reading argument as int32" << std::endl;
                }
            }
            unsigned short levels = ci::Rand::randInt(1, MAX_RECTANGLES_SPLITS + 1);
            rectangles.spawn(levels, duration, freq);
        }
    }
}

// ------------------------------------------------------------------------------------------------
void VisualsApp::translate() {
    if (stroboOn) {
        if (ci::Rand::randFloat(1.0f) < STROBO_TRANSL_PROB) {
            char r = ci::Rand::randInt(0,3);
            strobo.translate(r);
        }
        if ((ci::Rand::randFloat(1.0f) < STROBO_TRANSL_RECT_PROB) && rectanglesOn) {
            char r = ci::Rand::randInt(0,3);
            rectangles.translate(r);
        }
    } else {
        strobo.translate(0);
        rectangles.translate(0);
    }
    if (shadowsOn) {
        if (ci::Rand::randFloat(1.0f) < SHADOWS_TRANSL_PROB) {
            char r = ci::Rand::randInt(0,3);
            shadow.translate(r);
        }
        if ((ci::Rand::randFloat(1.0f) < SHADOWS_TRANSL_RECT_PROB) && rectanglesOn) {
            char r = ci::Rand::randInt(0,3);
            rectangles.translate(r);
        }
    } else {
        shadow.translate(0);
        rectangles.translate(0);
    }
}

// ------------------------------------------------------------------------------------------------
void VisualsApp::keyDown(ci::app::KeyEvent event) {
    if (event.getCode() == 27) quit(); // quit on escape
}

// ------------------------------------------------------------------------------------------------
void VisualsApp::draw(){
    try {
        ci::gl::clear( ci::Color( 0, 0, 0 ) );
        if (rectanglesOn) {rectangles.draw();}
        if (stroboOn) {strobo.draw();}
        if (shadowsOn) {shadow.draw();}
    } catch (...) {
        ci::app::console() << "Something went wrong in the draw callback.." <<std::endl;
    }
}

// ------------------------------------------------------------------------------------------------
CINDER_APP_BASIC( VisualsApp, cinder::app::RendererGl )
