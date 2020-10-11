#include "FiberFinder.h"
#include "mex.h"

// Forces all couts to go to MATLAB terminal window.
// Source: https://www.mathworks.com/matlabcentral/answers/132527-in-mex-files-where-does-output-to-stdout-and-stderr-go
class mystream : public std::streambuf
{
protected:
virtual std::streamsize xsputn(const char *s, std::streamsize n) { mexPrintf("%.*s", n, s); return n; }
virtual int overflow(int c=EOF) { if (c != EOF) { mexPrintf("%.1s", &c); } return 1; }
};
class scoped_redirect_cout
{
public:
  scoped_redirect_cout() { old_buf = std::cout.rdbuf(); std::cout.rdbuf(&mout); }
  ~scoped_redirect_cout() { std::cout.rdbuf(old_buf); }
private:
  mystream mout;
  std::streambuf *old_buf;
};
static scoped_redirect_cout mycout_redirect;

// Constructor
FiberFinder::FiberFinder(int depth, int height, int width, double *** image,
  double binaryThreshold, double nucleationPointThreshold, double localMaximumPointThreshold,
  int lmpBoxRadius, double danglingFiberLength) {

  this->width = width;
  this->height = height;
  this->depth = depth;
  this->binaryThreshold = binaryThreshold;
  this->nucleationPointThreshold = nucleationPointThreshold;
  this->localMaximumPointThreshold = localMaximumPointThreshold;
  this->lmpBoxRadius = lmpBoxRadius;
  this->danglingFiberLength = danglingFiberLength;
  this->originalImage = image;

  //this->maxFiberLength = (width + height + depth) / 3;

  this->maxFiberLength = width;

  if (height > this->maxFiberLength) {
    this->maxFiberLength = height;
  }

  if (depth > this->maxFiberLength) {
    this->maxFiberLength = depth;
  }

  //this->maxDFSDepth = 2;

  //this->maxFiberLength = 50;
  this->maxDFSDepth = this->maxFiberLength / lmpBoxRadius;
  //this->maxDFSDepth = 9;

  this->modifiedImage  = new double ** [this->depth];
  for (int i = 0; i < this->depth; i++) {
    this->modifiedImage[i] = new double * [this->height];

    for (int j = 0; j < this->height; j++) {
      this->modifiedImage[i][j] = new double [this->width];
    }
  }

  //cout << "Allocated modifed image 3D double ***" << endl;

  for (int i = 0; i < this->depth; i++){
      for (int j = 0; j < this->height; j++) {
        for (int k = 0; k < this->width; k++) {
          this->modifiedImage[i][j][k] = this->originalImage[i][j][k];
      }
    }
  }

  // Initialize map declaring if a pixel is already in a fiber.
  for (int i = 0; i < this->depth; i++){
    for (int j = 0; j < this->height; j++) {
      for (int k = 0; k < this->width; k++) {
        this->pointInFiber[{i, j, k}] = false;
      }
    }
  }

}

// Destructor
FiberFinder::~FiberFinder() {
  for (int i = 0; i < this->depth; i++) {
    for (int j = 0; j < this->height; j++) {
      delete [] modifiedImage[i][j];
    }
    delete [] modifiedImage[i];
  }

  delete [] modifiedImage;
}

void FiberFinder::generateFibers() {
  binarizeImage();

  mexPrintf("Binarized Image.\n");
  mexEvalString("drawnow;");

  calculateForegroundToBackgroundDistances();

  mexPrintf("Calculated foreground to background pixel distances.\n");
  mexEvalString("drawnow;");

  pickNucleationPoints();

  mexPrintf("Picked nucleation points.\n");
  mexEvalString("drawnow;");

  //printNucleationPoints();

  buildFibersFromNucleationPoints();

  mexPrintf("Generated fibers.\n");
  mexEvalString("drawnow;");
}

void FiberFinder::binarizeImage() {

  long numForegroundPixels = 0;
  long numBackgroundPixels = 0;

  for (int i = 0; i < this->depth; i++){
    for (int j = 0; j < this->height; j++) {
      for (int k = 0; k < this->width; k++) {
        if (this->originalImage[i][j][k] >= this->binaryThreshold) {
          this->modifiedImage[i][j][k] = 1.0;
          numForegroundPixels += 1;
        } else {
          this->modifiedImage[i][j][k] = 0.0;
          numBackgroundPixels += 1;
        }
      }
    }
  }

}

void FiberFinder::calculateForegroundToBackgroundDistances() {
  double avgForegroundToBackgroundDistance = 0.0;
  double numForegroundPixels = 0.0;

  for (int i = 0; i < this->depth; i++) {
    for (int j = 0; j < this->height; j++) {
      for (int k = 0; k < this->width; k++) {

        if (this->modifiedImage[i][j][k] >= 1) {
          this->modifiedImage[i][j][k] = calculateCurrentForegroundToBackgroundDistance(i, j, k);

          if (modifiedImage[i][j][k] != this->depth && modifiedImage[i][j][k] != this->height && modifiedImage[i][j][k] != this->width && modifiedImage[i][j][k] > 0) {
            avgForegroundToBackgroundDistance += modifiedImage[i][j][k];
            numForegroundPixels += 1.0;
          }
        }

      }
    }
  }

  avgForegroundToBackgroundDistance = avgForegroundToBackgroundDistance / numForegroundPixels;
  cout << "avgForegroundToBackgroundDistance: " << avgForegroundToBackgroundDistance << endl;

  // Account for the non-complete foreground to background distance
  for (int i = 0; i < this->depth; i++) {
    for (int j = 0; j < this->height; j++) {
      for (int k = 0; k < this->width; k++) {
        if (modifiedImage[i][j][k] == this->depth || modifiedImage[i][j][k] == this->height || modifiedImage[i][j][k] == this->width) {
            modifiedImage[i][j][k] = this->nucleationPointThreshold;
        }
      }
    }
  }

}

// i = depth, j = current height, k = current width
double FiberFinder::calculateCurrentForegroundToBackgroundDistance(int i, int j, int k) {

  // The maximum initial minimumDistance is the max(width, height, depth) of the image
  double minimumDistance = this->height;
  if (this->width > minimumDistance) {
    minimumDistance = this->width;
  }
  if (this->depth > minimumDistance) {
    minimumDistance = this->depth;
  }

// Less robust way of finding the minimum distance.
/**
  for (int w = 0; w < k; w++) {
    if (modifiedImage[i][j][w] <= 0 && abs(w - k) < minimumDistance) {
      minimumDistance = abs(w - k);
    }
  }

  for (int w = k + 1; w < this->width; w++) {
    if (modifiedImage[i][j][w] <= 0 && abs(w - k) < minimumDistance) {
      minimumDistance = abs(w - k);
    }
  }

  for (int h = 0; h < j; h++) {
    if (modifiedImage[i][h][k] <= 0 && abs(h - j) < minimumDistance) {
      minimumDistance = abs(h - j);
    }
  }

  for (int h = j + 1; h < this->height; h++) {
    if (modifiedImage[i][h][k] <= 0 && abs(h - j) < minimumDistance) {
      minimumDistance = abs(h - j);
    }
  }

  for (int d = 0; d < i; d++) {
    if (modifiedImage[d][j][k] <= 0 && abs(d - i) < minimumDistance) {
      minimumDistance = abs(d - i);
    }
  }

  for (int d = i + 1; d < this->depth; d++) {
    if (modifiedImage[d][j][k] <= 0 && abs(d - i) < minimumDistance) {
      minimumDistance = abs(d - i);
    }
  }
*/
  // Consider images in a radiusToSearchIn pixel box radius
  //int radiusToSearchIn = (width + height + depth) / 3 / 10;
  //cout << "radiusToSearchIn: " << radiusToSearchIn << endl;
  int radiusToSearchIn = this->nucleationPointThreshold;

  if (radiusToSearchIn > (width + height + depth) / 3 / 10) {
    radiusToSearchIn = (width + height + depth) / 3 / 10;
  }

  if (radiusToSearchIn > 30) {
    radiusToSearchIn = 30;
  }

  //Reduced brute force minimum distance finder.
  int minD = fmax(i - radiusToSearchIn, 0);
  int maxD = fmin(i + radiusToSearchIn, this->depth - 1);

  int minH = fmax(j - radiusToSearchIn, 0);
  int maxH = fmin(j + radiusToSearchIn, this->height - 1);

  int minW = fmax(k - radiusToSearchIn, 0);
  int maxW = fmin(k + radiusToSearchIn, this->width - 1);

  for (int d = minD; d < maxD; d++) {
    for (int h = minH; h < maxH; h++) {
      for (int w = minW; w < maxW; w++) {

        if (modifiedImage[d][h][w] <= 0) {
            double currentDistance = sqrt(pow(d - i, 2) + pow(h - j, 2) + pow(w - k, 2));
            if (currentDistance < minimumDistance) {
              minimumDistance = currentDistance;
            }
        }

      }
    }
  }

  return minimumDistance;
}

void FiberFinder::pickNucleationPoints() {
  for (int i = 0; i < this->depth; i++){
    for (int j = 0; j < this->height; j++) {
      for (int k = 0; k < this->width; k++) {
        if (this->modifiedImage[i][j][k] >= this->nucleationPointThreshold) {
          this->nucleationPoints.push_back({i, j, k});
        }
      }
    }
  }
}

// TODO: Consider a max DFS depth.
void FiberFinder::buildFibersFromNucleationPoints() {

  for (int np = 0; np < nucleationPoints.size(); np++) {

    if (np % 1000 == 0){
      cout << "Building out nucleation point " + to_string(np + 1) + " of " + to_string(nucleationPoints.size()) << endl;
      mexEvalString("drawnow;");
    }

    //Don't go through a nucleation point if it is already within a fiber.
    /**
    bool currentNucleationPointInFiber = false;
    for (int i = 0; i < this->fibers.size(); i++) {
      for (int j = 0; j < this->fibers[i].size(); j++) {
        int x = this->fibers[i][j][0];
        int y = this->fibers[i][j][1];
        int z = this->fibers[i][j][2];
        if (nucleationPoints[np][0] == x && nucleationPoints[np][1] == y && nucleationPoints[np][2] == z) {
          currentNucleationPointInFiber = true;
          break;
        }
      }
    }

    if (currentNucleationPointInFiber) {
      continue;
    }
    */

    if (this->pointInFiber[{nucleationPoints[np][0], nucleationPoints[np][1], nucleationPoints[np][2]}]) {
      continue;
    }

    vector< vector <int> > fiber;
    fiber.push_back(nucleationPoints[np]);

    stack< vector <int> > pointsToExpand;
    pointsToExpand.push(nucleationPoints[np]);

    // Explored set
    map< vector <int>, bool > visited;

    // Initialize explored set
    for (int i = 0; i < this->depth; i++){
      for (int j = 0; j < this->height; j++) {
        for (int k = 0; k < this->width; k++) {
          visited[{i, j, k}] = false;

          // Prevents rexploring the image multiple times.
          if (this->pointInFiber[{i, j, k}]) {
            visited[{i, j, k}] = true;
          }

        }
      }
    }

    //cout << "Created and initialized explored set." << endl;
    //mexEvalString("drawnow;");

    //cout << "Current Nucleation Point: " << endl;
    //cout << "(" + to_string(nucleationPoints[np][0]) + ", " + to_string(nucleationPoints[np][1]) + ", " + to_string(nucleationPoints[np][2]) + ")" << endl;

    vector <int> currentNucleationPoint = nucleationPoints[np];

    map< vector <int>, int> dfsExpansionDepth;
    map< vector <int>, string> dfsExpansionDirection;
    map< vector <int>, vector <int>> parent;

    parent[currentNucleationPoint] = currentNucleationPoint;
    dfsExpansionDepth[currentNucleationPoint] = 0;
    dfsExpansionDirection[currentNucleationPoint] = "";

    // Essentially a DFS on each nulceation point, searching for the rest of the fiber.
    while (pointsToExpand.size() > 0) {
      vector<int> point = pointsToExpand.top();
      pointsToExpand.pop();
      visited[point] = true;

      if (dfsExpansionDepth[point] > this->maxDFSDepth){
        continue;
      }

      //cout << "Current stack pointsToExpand point: " << endl;
      //cout << "(" + to_string(point[0]) + ", " + to_string(point[1]) + ", " + to_string(point[2]) + ")" << endl;

      int minD = fmax(point[0] - this->lmpBoxRadius, 0);
      int maxD = fmin(point[0] + this->lmpBoxRadius, this->depth - 1);

      int minH = fmax(point[1] - this->lmpBoxRadius, 0);
      int maxH = fmin(point[1] + this->lmpBoxRadius, this->height - 1);

      int minW = fmax(point[2] - this->lmpBoxRadius, 0);
      int maxW = fmin(point[2] + this->lmpBoxRadius, this->width - 1);

      // cout << "minD: " + to_string(minD) << endl;
      // cout << "maxD: " + to_string(maxD) << endl;
      //
      // cout << "minH: " + to_string(minH) << endl;
      // cout << "maxH: " + to_string(maxH) << endl;
      //
      // cout << "minW: " + to_string(minW) << endl;
      // cout << "maxW: " + to_string(maxW) << endl;

      for (int h = minH; h <= maxH; h++) {
        for (int w = minW; w <= maxW; w++) {
          //cout << "(minD, h, w): (" + to_string(minD) + ", " + to_string(h) + ", " + to_string(w) + "). modifiedImage[minD][h][w] = " + to_string(modifiedImage[minD][h][w]) << endl;
          //cout << "(maxD, h, w): (" + to_string(maxD) + ", " + to_string(h) + ", " + to_string(w) + "). modifiedImage[maxD][h][w] = " + to_string(modifiedImage[maxD][h][w]) << endl;
          if (modifiedImage[minD][h][w] >= this->localMaximumPointThreshold && visited[{minD, h, w}] == false) {

            double distance = sqrt(pow(currentNucleationPoint[0] - minD, 2) + pow(currentNucleationPoint[1] - h, 2) + pow(currentNucleationPoint[2] - w, 2));

            if (distance <= this->maxFiberLength) {
              fiber.push_back({minD, h, w});
              pointsToExpand.push({minD, h, w});
              visited[{minD, h, w}] = true;
              pointInFiber[{minD, h, w}] = true;
              parent[{minD, h, w}] = point;
              dfsExpansionDepth[{minD, h, w}] = dfsExpansionDepth[point] + 1;
            }
          }

          if (modifiedImage[maxD][h][w] >= this->localMaximumPointThreshold && visited[{maxD, h, w}] == false) {

            double distance = sqrt(pow(currentNucleationPoint[0] - maxD, 2) + pow(currentNucleationPoint[1] - h, 2) + pow(currentNucleationPoint[2] - w, 2));

            if (distance <= this->maxFiberLength) {
              fiber.push_back({maxD, h, w});
              pointsToExpand.push({maxD, h, w});
              visited[{maxD, h, w}] = true;
              pointInFiber[{maxD, h, w}] = true;
              parent[{maxD, h, w}] = point;
              dfsExpansionDepth[{maxD, h, w}] = dfsExpansionDepth[point] + 1;
            }
          }
        }
      }

      for (int d = minD; d <= maxD; d++) {
        for (int w = minW; w <= maxW; w++) {
          //cout << "(d, minH, w): (" + to_string(d) + ", " + to_string(minH) + ", " + to_string(w) + "). modifiedImage[d][minH][w] = " + to_string(modifiedImage[d][minH][w]) << endl;
          //cout << "(d, maxH, w): (" + to_string(d) + ", " + to_string(maxH) + ", " + to_string(w) + "). modifiedImage[d][maxH][w] = " + to_string(modifiedImage[d][maxH][w]) << endl;
          if (modifiedImage[d][minH][w] >= this->localMaximumPointThreshold  && visited[{d, minH, w}] == false) {

            double distance = sqrt(pow(currentNucleationPoint[0] - d, 2) + pow(currentNucleationPoint[1] - minH, 2) + pow(currentNucleationPoint[2] - w, 2));

            if (distance <= this->maxFiberLength) {
              fiber.push_back({d, minH, w});
              pointsToExpand.push({d, minH, w});
              visited[{d, minH, w}] = true;
              pointInFiber[{d, minH, w}] = true;
              parent[{d, minH, w}] = point;
              dfsExpansionDepth[{d, minH, w}] = dfsExpansionDepth[point] + 1;
            }

          }

          if (modifiedImage[d][maxH][w] >= this->localMaximumPointThreshold && visited[{d, maxH, w}] == false) {

            double distance = sqrt(pow(currentNucleationPoint[0] - d, 2) + pow(currentNucleationPoint[1] - maxH, 2) + pow(currentNucleationPoint[2] - w, 2));

            if (distance <= this->maxFiberLength) {
              fiber.push_back({d, maxH, w});
              pointsToExpand.push({d, maxH, w});
              visited[{d, maxH, w}] = true;
              pointInFiber[{d, maxH, w}] = true;
              parent[{d, maxH, w}] = point;
              dfsExpansionDepth[{d, maxH, w}] = dfsExpansionDepth[point] + 1;
            }
          }
        }
      }

      for (int d = minD; d <= maxD; d++) {
        for (int h = minH; h <= maxH; h++) {
          //cout << "(d, h, minW): (" + to_string(d) + ", " + to_string(h) + ", " + to_string(minW) + "). modifiedImage[d][h][minW] = " + to_string(modifiedImage[d][h][minW]) << endl;
          //cout << "(d, h, maxW): (" + to_string(d) + ", " + to_string(h) + ", " + to_string(maxW) + "). modifiedImage[d][h][maxW] = " + to_string(modifiedImage[d][h][minW]) << endl;

          if (modifiedImage[d][h][minW] >= this->localMaximumPointThreshold && visited[{d, h, minW}] == false) {

            double distance = sqrt(pow(currentNucleationPoint[0] - d, 2) + pow(currentNucleationPoint[1] - h, 2) + pow(currentNucleationPoint[2] - minW, 2));

            if (distance <= this->maxFiberLength) {
              fiber.push_back({d, h, minW});
              pointsToExpand.push({d, h, minW});
              visited[{d, h, minW}] = true;
              pointInFiber[{d, h, minW}] = true;
              parent[{d, h, minW}] = point;
              dfsExpansionDepth[{d, h, minW}] = dfsExpansionDepth[point] + 1;
            }
          }

          if (modifiedImage[d][h][maxW] >= this->localMaximumPointThreshold  && visited[{d, h, maxW}] == false) {

            double distance = sqrt(pow(currentNucleationPoint[0] - d, 2) + pow(currentNucleationPoint[1] - h, 2) + pow(currentNucleationPoint[2] - maxW, 2));

            if (distance <= this->maxFiberLength) {
              fiber.push_back({d, h, maxW});
              pointsToExpand.push({d, h, maxW});
              visited[{d, h, maxW}] = true;
              pointInFiber[{d, h, maxW}] = true;
              parent[{d, h, maxW}] = point;
              dfsExpansionDepth[{d, h, maxW}] = dfsExpansionDepth[point];
            }
          }
        }
      }

    }

    //cout << "Completed DFS" << endl;
    //mexEvalString("drawnow;");

    if (isValidFiber(fiber)) {
      this->fibers.push_back(fiber);
    }
  }

}

// Removes dangling fibers
// TODO: Implement different tangent angle to remove dangling fiber.
// TODO: Join similar fibers?
bool FiberFinder::isValidFiber(vector< vector <int> > fiber) {

  double distance = 0.0;
  vector<int> currentPoint = fiber[0];

  for (int i = 1; i < fiber.size(); i++) {
    distance += sqrt(pow(currentPoint[0] - fiber[i][0], 2) + pow(currentPoint[1] - fiber[i][1], 2) + pow(currentPoint[2] - fiber[i][2], 2));
    currentPoint = fiber[i];

    if (distance >= this->danglingFiberLength) {
      return true;
    }
  }

  return false;
}

void FiberFinder::printModifiedImage(){
  cout << "Modified Image:" << endl;

  for (int i = 0; i < this->depth; i++) {
    for (int j = 0; j < this->height; j++) {
      for (int k = 0; k < this->width; k++) {
        cout << fixed << setprecision(2) << this->modifiedImage[i][j][k] << " ";
      }
      cout << '\n';
    }
    cout << '\n';
  }
}

void FiberFinder::printOriginalImage(){
  cout << "Original Image: " << endl;

  for (int i = 0; i < this->depth; i++) {
    for (int j = 0; j < this->height; j++) {
      for (int k = 0; k < this->width; k++) {
        cout << fixed << setprecision(2) << this->originalImage[i][j][k] << " ";
      }
      cout << '\n';
    }
    cout << '\n';
  }
}

void FiberFinder::printNucleationPoints() {
  // cout << "Nucleation Points: " << endl;
  for (int i = 0; i < nucleationPoints.size(); i++) {

    cout << "Nucleation Point " + to_string(i) << endl;
    cout << "(" + to_string(nucleationPoints[i][0]) + ", " + to_string(nucleationPoints[i][1]) + ", " + to_string(nucleationPoints[i][2]) + ")" + "\n" << endl;
  }
  cout << " " << endl;
}

void FiberFinder::printFibers() {
  cout << "Fibers: "<< endl;
  for (int i = 0; i < this->fibers.size(); i++) {
    cout << "Fiber " + to_string(i + 1) + ": " << endl;

    for (int j = 0; j < this->fibers[i].size(); j++) {
      cout << "(" + to_string(this->fibers[i][j][0]) + ", " + to_string(this->fibers[i][j][1]) + ", " + to_string(this->fibers[i][j][2]) + ")" << endl;
    }
    cout << " " << endl;
  }
}

vector< vector < vector <int> > > FiberFinder::getFibers() {
  //printFibers();

  // Sorts the points within each fiber
  for (int i = 0; i < this->fibers.size(); i++) {
    vector< vector <int> > currentFiber = fibers[i];
    for (int j = 0; j < currentFiber.size(); j++) {
      std::reverse(currentFiber[j].begin(),currentFiber[j].end());
    }
    std::sort(currentFiber.begin(), currentFiber.end(), std::less<vector<int>>());
    fibers[i] = currentFiber;
  }
  //printFibers();
  return this->fibers;
}

// Creates a FiberFinder object
static vector< vector < vector <int> > > mexcpp(int width, int height, int depth, double * image,
  double binaryThreshold, double nucleationPointThreshold, double localMaximumPointThreshold,
  int lmpBoxRadius, double danglingFiberLength) {

  cout << "Entering mexcpp() function." << endl;

  double *** boxImage;

  boxImage = new double ** [depth];

  for (int i = 0; i < depth; i++) {
    boxImage[i] = new double * [height];

    for (int j = 0; j < height; j++) {
      boxImage[i][j] = new double [width];
      // cout << "(i, j): " + to_string(i) + ", " + to_string(j) + ")" << endl;
    }
  }

  cout << "Allocated memory for 3D image matrix." << endl;

  for (int i = 0; i < depth; i++) {
   for (int j = 0; j < height; j++) {
     for (int k = 0; k < width; k++) {
       // cout << "(i, j, k): (" + to_string(i) + ", " + to_string(j) + ", " + to_string(k) + ")" << endl;
       int index = i * (width * height) + j * width + k;
        // cout << "index: " + to_string(index) << endl;
        // cout << "image[index]: " + to_string(image[index]) << endl;
       //cout << "" << endl;
       boxImage[i][j][k]  = (double) image[index]; // access [depth][height][width]
      }
    }
  }

  cout << "Transferred the inputted 1D matrix image into a 3D image. " << endl;
  mexPrintf("Creating an instance of FiberFinder class.\n");
  mexEvalString("drawnow;");

  FiberFinder ff(depth, height, width, boxImage, binaryThreshold, nucleationPointThreshold, localMaximumPointThreshold, lmpBoxRadius, danglingFiberLength);

  // cout << "Created instance of FiberFinder class with mex parameters." << endl;

  ff.generateFibers();

  //ff.printOriginalImage();
  //ff.printModifiedImage();
  // ff.printNucleationPoints();
  //ff.printFibers();

  // Clean up the allocated 3D array.
  for (int i = 0; i < depth; i++) {
    for (int j = 0; j < height; j++) {
      delete [] boxImage[i][j];
    }
    delete [] boxImage[i];
  }

  delete [] boxImage;

  return ff.getFibers();

  // vector< vector< vector< int> >> blanks;
  // return blanks;
}

// Mex gateway function
void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) {

  // cout << "Entering mexFunction()" << endl;
  mexPrintf("Entering mexFunction()\n");
  mexEvalString("drawnow;");

  if(nrhs != 9) {
      mexErrMsgIdAndTxt("MATLAB:mexcpp:nargin",
                        "MEXCPP requires nine input arguments.");
  }
  if(nlhs != 1) {
      mexErrMsgIdAndTxt("MATLAB:mexcpp:nargout",
                        "MEXCPP requires one output arguments.");
  }

  auto depth = (int) (*mxGetPr(prhs[0]));
  auto height = (int) (*mxGetPr(prhs[1]));
  auto width = (int) (*mxGetPr(prhs[2]));
  auto * image = (double *) mxGetPr(prhs[3]);
  auto binaryThreshold = (double) (*mxGetPr(prhs[4]));
  auto nucleationPointThreshold = (double) (*mxGetPr(prhs[5]));
  auto localMaximumPointThreshold = (double) (*mxGetPr(prhs[6]));
  auto lmpBoxRadius = (int) (*mxGetPr(prhs[7]));
  auto danglingFiberLength =  (double) (*mxGetPr(prhs[8]));

  // Processing the return value into a 1D array.
  // std::array<<int,3>

  // cout << "Calling mexcpp() function" << endl;

  vector< vector < vector <int> > > fibers = (mexcpp(width, height, depth, image, binaryThreshold, nucleationPointThreshold,
     localMaximumPointThreshold, lmpBoxRadius, danglingFiberLength));

  int numValues = 0;

  for (int i = 0; i < fibers.size(); i++) {
    for (int j = 0; j < fibers[i].size(); j++) {
      for (int k = 0; k < 3; k++) {
        numValues += 1;
      }
    }
  }

  double * values = new double [numValues + fibers.size()];
  //^ switch to std::vector<double> my_vector
  // my_vector.data()

  int index = 0;

  // Creates a 1D representation of jagged 3D fibers. A -1 is used to seperate fibers.
  //mexPrintf(fibers.size());
  //mexEvalString("drawnow;");

  for (int i = 0; i < fibers.size(); i++) {
    values[index] = -1;
    index++;
    for (int j = 0; j < fibers[i].size(); j++) {
      for (int k = 0; k < 3; k++) {
        values[index] = fibers[i][j][k];
        index++;
      }
    }
  }

  plhs[0] = mxCreateDoubleMatrix(1, numValues + fibers.size(), mxREAL);
  // STD::COPY
  memcpy(mxGetPr(plhs[0]), values, (numValues + fibers.size()) * sizeof(double));
  // delete [] values;

}
