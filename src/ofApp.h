//Chris Vo
//CS 134
//Final Project
#pragma once

#include "ofMain.h"
#include  "ofxAssimpModelLoader.h"
#include "ParticleSystem.h"
#include "ParticleEmitter.h"
#include "Octree.h"
#include "box.h"
#include "ray.h"


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void drawAxis(ofVec3f);
		void initLightingAndMaterials();
		void savePicture();
		void toggleWireframeMode();
		void togglePointsDisplay();

		ofEasyCam cam;
		ofxAssimpModelLoader lander;
		ofxAssimpModelLoader surface;
		ofImage backgroundImage;
		ofCamera *theCam = NULL;
		ofCamera topCam;

		bool bAltKeyDown;
		bool bCtrlKeyDown;
		bool bWireframe;
		bool bDisplayPoints;
	
		bool bBackgroundLoaded = false;
		bool bLanderLoaded = false;
		Octree octree;
		TreeNode node;
		int numLevels = 5;
		bool surfaceView = false;
		bool windowView = false;
		bool fixedView = false;
		ImpulseForce *impulse;
		ThrusterForce *thruster;
		bool drawFlag = false;
		float alt;
		bool intersect = false;
		ofSoundPlayer rocket;
	
		ParticleSystem *drift; //particle system of one particle to add drift to the model
		ParticleEmitter exhaust;
		ofLight light;
};
