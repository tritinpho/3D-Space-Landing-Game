//  Kevin M. Smith - Basic Octree Class - CS134/235 4/18/18
//


#include "Octree.h"

Octree::Octree() {
	root.intersects = true;
	altitude = 1;
}

// draw Octree (recursively)
//
void Octree::draw(TreeNode & node, int numLevels, int level) {
	//if (level > numLevels) return;

	//to differ each level
	switch (level) {
	case 1:
		ofSetColor(ofColor::blue);
		break;
	case 2:
		ofSetColor(ofColor::red);
		break;
	case 3:
		ofSetColor(ofColor::green);
		break;
	case 4:
		ofSetColor(ofColor::cyan);
		break;
	case 5:
		ofSetColor(ofColor::brown);
		break;
	case 6:
		ofSetColor(ofColor::purple);
		break;
	case 7:
		ofSetColor(ofColor::orange);
		break;
	case 8:
		ofSetColor(ofColor::pink);
		break;
	default:
		ofSetColor(ofColor::white);
		break;
	}

	if (node.points.size() == 1) { //if the ray selection intersects a node(s), draw it
		drawBox(node.box);
		return;
	}
	//level++;
	for (int i = 0; i < node.children.size(); i++) { //recursively call the draw method for a current node's children
		draw(node.children[i], numLevels, level);
	}
}

//sets all the nodes in the Octree to false to "undraw" them
void Octree::clear(TreeNode & node) {
	node.intersects = false;
	for (int i = 0; i < node.children.size(); i++)
		clear(node.children[i]);
}

//sets all the nodes in the Octree to true to draw every node in the Octree
void Octree::reset(TreeNode & node) {
	for (int i = 0; i < node.children.size(); i++)
		reset(node.children[i]);
}


//draw a box from a "Box" class  
//
void Octree::drawBox(const Box &box) {
	Vector3 min = box.parameters[0];
	Vector3 max = box.parameters[1];
	Vector3 size = max - min;
	Vector3 center = size / 2 + min;
	ofVec3f p = ofVec3f(center.x(), center.y(), center.z());
	float w = size.x();
	float h = size.y();
	float d = size.z();
	ofDrawBox(p, w, h, d);
}

// return a Mesh Bounding Box for the entire Mesh
//
Box Octree::meshBounds(const ofMesh & mesh) {
	int n = mesh.getNumVertices();
	ofVec3f v = mesh.getVertex(0);
	ofVec3f max = v;
	ofVec3f min = v;
	for (int i = 1; i < n; i++) {
		ofVec3f v = mesh.getVertex(i);

		if (v.x > max.x) max.x = v.x;
		else if (v.x < min.x) min.x = v.x;

		if (v.y > max.y) max.y = v.y;
		else if (v.y < min.y) min.y = v.y;

		if (v.z > max.z) max.z = v.z;
		else if (v.z < min.z) min.z = v.z;
	}
	cout << "vertices: " << n << endl;
	//	cout << "min: " << min << "max: " << max << endl;
	return Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
}

// getMeshPointsInBox:  return an array of indices to points in mesh that are contained 
//                      inside the Box.  Return count of points found;
//
int Octree::getMeshPointsInBox(const ofMesh & mesh, const vector<int>& points,
	Box & box, vector<int> & pointsRtn)
{
	int count = 0;
	for (int i = 0; i < points.size(); i++) {
		ofVec3f v = mesh.getVertex(points[i]);
		if (box.inside(Vector3(v.x, v.y, v.z))) {
			count++;
			pointsRtn.push_back(points[i]);
		}
	}
	return count;
}



//  Subdivide a Box into eight(8) equal size boxes, return them in boxList;
//
void Octree::subDivideBox8(const Box &box, vector<Box> & boxList) {
	Vector3 min = box.parameters[0];
	Vector3 max = box.parameters[1];
	Vector3 size = max - min;
	Vector3 center = size / 2 + min;
	float xdist = (max.x() - min.x()) / 2;
	float ydist = (max.y() - min.y()) / 2;
	float zdist = (max.z() - min.z()) / 2;
	Vector3 h = Vector3(0, ydist, 0);

	//  generate ground floor
	//
	Box b[8];
	b[0] = Box(min, center);
	b[1] = Box(b[0].min() + Vector3(xdist, 0, 0), b[0].max() + Vector3(xdist, 0, 0));
	b[2] = Box(b[1].min() + Vector3(0, 0, zdist), b[1].max() + Vector3(0, 0, zdist));
	b[3] = Box(b[2].min() + Vector3(-xdist, 0, 0), b[2].max() + Vector3(-xdist, 0, 0));

	boxList.clear();
	for (int i = 0; i < 4; i++)
		boxList.push_back(b[i]);

	// generate second story
	//
	for (int i = 4; i < 8; i++) {
		b[i] = Box(b[i - 4].min() + h, b[i - 4].max() + h);
		boxList.push_back(b[i]);
	}
}

void Octree::create(const ofMesh & geo) {
	// initialize octree structure
	//
	root.box = meshBounds(geo);
	vector<int> indices;
	for (int i = 0; i < geo.getNumVertices(); i++)
		indices.push_back(i);

	getMeshPointsInBox(geo, indices, root.box, root.points);
	cout << root.points.size() << endl;
}

void Octree::subdivide(const ofMesh & mesh, TreeNode & node, int numLevels, int level) {
	if (node.points.size() > 2) { //a condition to stop recursion if a specified level is reached
		vector<Box> temp1;
		TreeNode temp2;
		subDivideBox8(node.box, temp1); //first subdivide the box into 8 smaller boxes
		for (int i = 0; i < temp1.size(); i++) {
			temp2.box = temp1[i];

			getMeshPointsInBox(mesh, node.points, temp2.box, temp2.points); //checks if children's points correspond to parent's points

			if (temp2.points.size() > 0) //a child node will become the child node of the parent if there's corresponding points
				node.children.push_back(temp2);
		}

		node.points.clear();
		level++;
		for (int i = 0; i < node.children.size(); i++) {
			if (node.children[i].points.size() != 1) { //call recursive function as long as the child's number of points is not 1
				node.children[i].intersects = true;
				subdivide(mesh, node.children[i], numLevels, level);
			}
		}
	}
}

bool Octree::intersect(const Ray &ray, TreeNode & node, TreeNode & nodeRtn) {
	if (node.box.intersect(ray, -1000, 1000)) {
		if (node.points.size() == 1) { //if there's a node with one point
			nodeRtn = node;
			cout << "intersect" << endl;
			return true;
		}
		node.intersects = true;
		for (int i = 0; i < node.children.size(); i++) {
			if (intersect(ray, node.children[i], nodeRtn)) { //if it reached to a box with one point, recursively return true
				node.intersects = true;
				return true;
			}
		}
	}

	return false;
}

bool Octree::intersect(const ofVec3f & v, TreeNode & node, TreeNode & nodeRtn, int level) {
	if (node.box.inside(Vector3(v.x, v.y, v.z))) {
		altitude = level;
		if (node.points.size() == 1) {
			return true;
		}

		level++;
		for (int i = 0; i < node.children.size(); i++) {
			if (intersect(v, node.children[i], nodeRtn, level))
				return true;
		}
	}

	return false;
}



