//Chris Vo
//CS 134
//Final Project


#include "ofApp.h"

//--------------------------------------------------------------
// setup scene, lighting, state and load geometry
//
void ofApp::setup(){

	alt = 0;
	impulse = new ImpulseForce();
	thruster = new ThrusterForce();
	bWireframe = false;
	bDisplayPoints = false;
	bAltKeyDown = false;
	bCtrlKeyDown = false;
	bLanderLoaded = false;

	rocket.loadSound("sounds/rocket.mp3");
	cam.setDistance(10);
	cam.setNearClip(.1);
	cam.setFov(65.5);   // approx equivalent to 28mm in 35mm format

	light.setup();
	light.enable();
	light.setDirectional();
	light.setAreaLight(1000, 1000);
	light.setPosition(ofVec3f(500, 100, 500));
	light.rotate(90, ofVec3f(1, 0, 0));
	light.setSpotlightCutOff(1500);
	light.setDiffuseColor(ofColor::orange);

	topCam.setNearClip(.1);
	topCam.setFov(65.5);   
	topCam.setPosition(0, 10, 0);
	topCam.lookAt(glm::vec3(0, 0, 0));

	// set current camera;
	//
	theCam = &cam;
	
	ofSetVerticalSync(true);
	ofEnableSmoothing();
	ofEnableDepthTest();

	// load BG image
	//
	bBackgroundLoaded = backgroundImage.load("images/starfield-plain.jpg");

	// setup rudimentary lighting 
	//
	initLightingAndMaterials();

	surface.loadModel("geo/moon-houdini.obj");
	surface.setScaleNormalization(false);
	surface.setRotation(0, 180, 0, 0, 1);

	// load lander model
	//
	if (lander.loadModel("geo/lander.obj")) {
		lander.setScaleNormalization(false);
		lander.setScale(.5, .5, .5);
		lander.setRotation(0, -180, 1, 0, 0);

		bLanderLoaded = true;
	}
	else {
		cout << "Error: Can't load model" << "geo/lander.obj" << endl;
		ofExit(0);
	}

	drift = new ParticleSystem();
	drift->addForce(new TurbulenceForce(ofVec3f(-1,0), ofVec3f(1,0)));
	drift->addForce(thruster);
	drift->addForce(impulse);

	GravityForce *gravityForce = new GravityForce(ofVec3f(0, -10, 0));

	exhaust.sys->addForce(gravityForce);

	exhaust.setVelocity(ofVec3f(0, 1, 0));
	exhaust.setOneShot(true);
	exhaust.setGroupSize(50);
	exhaust.setParticleRadius(0.02);
	exhaust.setLifespan(1);
	exhaust.setRate(0.02);
	exhaust.setEmitterType(DiscEmitter);
	Particle particle;
	particle.lifespan = 999;
	drift->add(particle);

	octree.create(surface.getMesh(0));
	octree.subdivide(surface.getMesh(0), octree.root, numLevels, 1);

	cam.enableMouseInput();
}

void ofApp::update() {

	exhaust.update();
	drift->update();
	if(!(drift->particles.empty()))
		lander.setPosition(drift->particles[0].position.x + 100, drift->particles[0].position.y + 100, drift->particles[0].position.z + 100);

	exhaust.setPosition(lander.getPosition());

	if (surfaceView) {
		cam.setPosition(ofVec3f(lander.getPosition().x, lander.getPosition().y, lander.getPosition().z));
		cam.lookAt(ofVec3f(lander.getPosition().x, 0, lander.getPosition().z));
	}

	if (windowView)
		cam.setPosition(ofVec3f(lander.getPosition().x, lander.getPosition().y + 3, lander.getPosition().z));

	if (fixedView) {
		cam.setPosition(lander.getPosition().x + 10, lander.getPosition().y, lander.getPosition().z);
		cam.lookAt(lander.getPosition());
	}

	alt = octree.getAltitude() * (float)lander.getPosition().y;
	intersect = octree.intersect(lander.getPosition(), octree.root, node, 1);

	if (intersect) {
		impulse->apply(-60 * drift->particles[0].velocity);
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	//	ofBackgroundGradient(ofColor(20), ofColor(0));   // pick your own backgroujnd
	//	ofBackground(ofColor::black);
	if (bBackgroundLoaded) {
		ofPushMatrix();
		ofDisableDepthTest();
		ofSetColor(50, 50, 50);
		ofScale(2, 2);
		backgroundImage.draw(-200, -100);
		ofEnableDepthTest();
		ofPopMatrix();
	}

	theCam->begin();
	ofPushMatrix();
	if (bWireframe) {                    // wireframe mode  (include axis)
		ofDisableLighting();
		ofSetColor(ofColor::slateGray);
		surface.drawWireframe();
		if (bLanderLoaded) {
			lander.drawWireframe();
		}
	}
	else {
		ofEnableLighting();              // shaded mode
		surface.drawFaces();
		if (bLanderLoaded) {
			lander.drawFaces();

		}
	}

	ofFill();
	ofSetColor(ofColor::yellow);
	ofDrawSphere(ofVec3f(500, 100, 500) , 50);
	 
	ofNoFill();
	if(drawFlag)
		octree.draw(octree.root, numLevels, 1);

	exhaust.draw();
	ofPopMatrix();
	theCam->end();

	// draw screen data
	//
	string str;
	str += "Frame Rate: " + std::to_string(ofGetFrameRate());
	ofSetColor(ofColor::white);
	ofDrawBitmapString(str, ofGetWindowWidth() - 170, 75);

	string str2;
	str2 = "Altitude: " + std::to_string(alt);
	ofDrawBitmapString(str2, ofGetWindowWidth() - 350, 75);
}


// Draw an XYZ axis in RGB at world (0,0,0) for reference.
//
void ofApp::drawAxis(ofVec3f location) {

	ofPushMatrix();
	ofTranslate(location);

	ofSetLineWidth(1.0);

	// X Axis
	ofSetColor(ofColor(255, 0, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(1, 0, 0));
	

	// Y Axis
	ofSetColor(ofColor(0, 255, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 1, 0));

	// Z Axis
	ofSetColor(ofColor(0, 0, 255));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 0, 1));

	ofPopMatrix();
}


void ofApp::keyPressed(int key) {
	Particle particle;

	switch (key) {
	case 'a':
		thruster->set(ofVec3f(-1, 0, 0));
		break;
	case 'w':
		thruster->set(ofVec3f(0, 0, 1));
		break;
	case 's':
		thruster->set(ofVec3f(0, 0, -1));
		break;
	case 'd':
		thruster->set(ofVec3f(1, 0, 0));
		break;
	case 'q':
		thruster->set(ofVec3f(0, 1, 0));
		break;
	case 'e':
		thruster->set(ofVec3f(0, -1, 0));
		break;
	case 'c':
		if (cam.getMouseInputEnabled()) cam.disableMouseInput();
		else cam.enableMouseInput();
		break;
	case 'F':
	case 'f':
		cam.setPosition(lander.getPosition());
		break;
	case 'H':
	case 'h':
		if (windowView)
			windowView = false;
		else
			windowView = true;
		break;
	case 'P':
	case 'p':
		if (fixedView)
			fixedView = false;
		else
			fixedView = true;
		break;
	case 'r':
		lander.setPosition(0, 0, 0);
		cam.reset();
		break;
	case 't':
		if (drawFlag)
			drawFlag = false;
		else
			drawFlag = true;
		break;
	case 'u':
		if (surfaceView)
			surfaceView = false;
		else
			surfaceView = true;
		break;
	case 'v':
		togglePointsDisplay();
		break;
	case 'V':
		toggleWireframeMode();
		break;
	case OF_KEY_F1:
		theCam = &cam;
		break;
	case OF_KEY_F3:
		theCam = &topCam;
		break;
	case OF_KEY_ALT:
		cam.enableMouseInput();
		bAltKeyDown = true;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = true;
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_DEL:
		break;
	case OF_KEY_UP:
		cam.move(ofVec3f(0, 1, 0));
		break;
	case OF_KEY_DOWN:
		cam.move(ofVec3f(0, -1, 0));
		break;
	case OF_KEY_LEFT:
		cam.move(ofVec3f(-1, 0, 0));
		break;
	case OF_KEY_RIGHT:
		cam.move(ofVec3f(1, 0, 0));
		break;
	case ',':
		cam.move(ofVec3f(0, 0, 1));
		break;
	case '/':
		cam.move(ofVec3f(0, 0, -1));
		break;
	case ' ':
		exhaust.start();
		thruster->add(ofVec3f(0,1,0));
		break;
	default:
		break;
	}
}

void ofApp::toggleWireframeMode() {
	bWireframe = !bWireframe;
}


void ofApp::togglePointsDisplay() {
	bDisplayPoints = !bDisplayPoints;
}

void ofApp::keyReleased(int key) {

	switch (key) {
	case 'a':
		thruster->set(ofVec3f(0, 0, 0));
		break;
	case 'w':
		thruster->set(ofVec3f(0, 0, 0));
		break;
	case 's':
		thruster->set(ofVec3f(0, 0, 0));
		break;
	case 'd':
		thruster->set(ofVec3f(0, 0, 0));
		break;
	case 'q':
		thruster->set(ofVec3f(0, 0, 0));
		break;
	case 'e':
		thruster->set(ofVec3f(0, 0, 0));
		break;
	case ' ':
		rocket.setLoop(true);
		rocket.setSpeed(2);
		rocket.play();
		thruster->add(ofVec3f(0, 0, 0));
		break;
	case OF_KEY_ALT:
		cam.disableMouseInput();
		bAltKeyDown = false;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = false;
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_UP:
	case OF_KEY_DOWN:
	case OF_KEY_LEFT:
	case OF_KEY_RIGHT:
		drift->newDirection = noDirection;
		break;
	default:
		break;

	}
}


//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
}


//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	
	ofVec3f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam.screenToWorld(mouse);
	ofVec3f rayDir = rayPoint - cam.getPosition();
	rayDir.normalize();
	Ray ray = Ray(Vector3(rayPoint.x, rayPoint.y, rayPoint.z),
		Vector3(rayDir.x, rayDir.y, rayDir.z));
	TreeNode result;
	int initial = ofGetSystemTimeMillis();
	cout << octree.root.points.size() << endl;
	octree.intersect(ray, octree.root, result); //tests ray intersection whenever a mouse clicked
	cout << "2 " << octree.root.points.size() << endl;
	
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {


}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}


//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}



//--------------------------------------------------------------
// setup basic ambient lighting in GL  (for now, enable just 1 light)
//
void ofApp::initLightingAndMaterials() {

	static float ambient[] =
	{ .5f, .5f, .5, 1.0f };
	static float diffuse[] =
	{ .7f, .7f, .7f, 1.0f };

	static float position[] =
	{20.0, 20.0, 20.0, 0.0 };

	static float lmodel_ambient[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float lmodel_twoside[] =
	{ GL_TRUE };


	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
//	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
//	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position);


	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
//	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glShadeModel(GL_SMOOTH);
} 

void ofApp::savePicture() {
	ofImage picture;
	picture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
	picture.save("screenshot.png");
	cout << "picture saved" << endl;
}

//--------------------------------------------------------------
//
// support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
//
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
