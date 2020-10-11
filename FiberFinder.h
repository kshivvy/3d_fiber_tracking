#include <vector>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <stack>
//#include <unordered_map>
#include <string>
#include <map>
#include <algorithm>

using namespace std;

class FiberFinder {
  private:
    void binarizeImage();
    void calculateForegroundToBackgroundDistances();
    double calculateCurrentForegroundToBackgroundDistance(int i, int j, int k);
    void pickNucleationPoints();
    void buildFibersFromNucleationPoints();
    bool isValidFiber(vector< vector <int> > fiber);

  public:
    FiberFinder(int width, int height, int depth, double *** image,
      double binaryThreshold, double nucleationPointThreshold, double localMaximumPointThreshold,
      int lmpBoxRadius, double danglingFiberLength);

    void printModifiedImage();
    void printOriginalImage();
    void printNucleationPoints();
    void printFibers();
    void generateFibers();
    ~FiberFinder();
    vector< vector < vector <int> > > getFibers();

    int width, height, depth;
    double *** originalImage;
    double *** modifiedImage;
    vector< vector < vector <int> > > fibers;

    vector< vector <int> > nucleationPoints;

    double binaryThreshold;
    double nucleationPointThreshold;
    double localMaximumPointThreshold;
    int lmpBoxRadius;
    double danglingFiberLength;
    double maxFiberLength;
    int maxDFSDepth;

    map< vector <int>, bool > pointInFiber;

};
