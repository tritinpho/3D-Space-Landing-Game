#pragma once
#include "ofMain.h"
#include "box.h"
#include "ray.h"


class TreeNode {
public:
	Box box;
	vector<int> points;
	vector<TreeNode> children;
	bool intersects = false;
};

class Octree {
public:
	
	Octree();
	void create(const ofMesh & mesh);
	void subdivide(const ofMesh &, TreeNode & node, int numLevels, int level);
	bool intersect(const Ray &, TreeNode & node, TreeNode & nodeRtn);
	bool intersect(const ofVec3f &, TreeNode & node, TreeNode & nodeRtn, int);
	void draw(TreeNode & node, int numLevels, int level);
	void draw(int numLevels, int level) {
		draw(root, numLevels, level);
	}
	static void drawBox(const Box &box);
	static Box meshBounds(const ofMesh &);
	int getMeshPointsInBox(const ofMesh &mesh, const vector<int> & points, Box & box, vector<int> & pointsRtn);
	void subDivideBox8(const Box &b, vector<Box> & boxList);
	void clear(TreeNode & node);
	void reset(TreeNode & node);
	float getAltitude() { return altitude; };

	ofMesh mesh;
	TreeNode root;
	float altitude;
};