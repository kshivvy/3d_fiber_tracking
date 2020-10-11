% To compile, open in MATLAB.
% mex FiberFinder.cpp
% main

% One fiber down the middle
square1 = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 2.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0];
square2 = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 2.0, 2.0, 2.0, 0.0, 0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0];
square3 = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 2.0, 4.0, 3.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0];
square4 = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 2.0, 2.0, 2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0];
square5 = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 2.0, 2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0];

testImage1 = [square1, square2, square3, square4, square5];


% Two fibers along the edges
square6  = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0];
square7  = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0];
square8  = [0.0, 0.0, 0.0, 0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 0.0, 3.0, 3.0, 0.0, 0.0, 4.0, 3.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0];
square9  = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 4.0, 3.0, 0.0, 0.0, 4.0, 4.0, 0.0, 0.0, 0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 0.0, 0.0];
square10 = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 4.0, 3.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0];

testImage2 = [square6, square7, square8, square9, square10];


% A planar fiber
square11 = [0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0];
square12 = [0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0];
square13 = [0.0 0.0 0.0 0.0 0.0 3.0 3.0 5.0 3.0 3.0 3.0 3.0 5.0 3.0 3.0 3.0 3.0 5.0 3.0 3.0 0.0 0.0 0.0 0.0 0.0];
square14 = [0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0];
square15 = [0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0];

testImage3 = [square11, square12, square13, square14, square15];

% Edge case
testImage4 = 1:125;

% Case where width = 5, height  = 4, and depth = 3
square16 = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 2.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0];
square17 = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 2.0, 2.0, 2.0, 0.0, 0.0, 0.0, 2.0, 0.0, 0.0];
square18 = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 2.0, 4.0, 3.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0];

testImage5 = [square16, square17, square18];



% Original C++ Class signature

%FiberFinder(int width, int height, int depth, double *** image,
%  double binaryThreshold, double nucleationPointThreshold, double localMaximumPointThreshold,
%  int lmpBoxRadius, double danglingFiberLength);

% Parameters: Ideal values differ for images
% int width:                         image size in "x" direction
% int height:                        image size in "y" direction
% int depth:                         image size in "z" direction
% double * image:                    Will be converted into a double ***, is a 1D representation of the image
% double binaryThreshold:            pixel intensity threshold distinguishing a background vs foreground pixel
% double nucleationPointThreshold:   pixel intensity threshold for a nucleation point, aka fiber "center"
% double localMaximumPointThreshold: pixel intensity threshold for a local max nucleation point, aka fiber "center"
% int lmpBoxRadius:                  box radius in which to search for local maximum points (LMPs), given a nucleation point
% double danglingFiberLength:        fiber length (in size relative to number of pixels) at which to not consider a fiber.

% Return Value:
% 1D mxMatrix fibers:              1D Representaiton of vector of fibers, with each fiber being a vector of points. Each fiber is seperated by a -1.

%fibers = FiberFinder(5, 5, 5, testImage1, 1.2, 1.2, 1, 1, 2.5)
%fibers = FiberFinder(5, 5, 5, testImage2, 1.2, 1.2, 1, 1, 2.5)
%fibers = FiberFinder(5, 5, 5, testImage3, 1.2, 1, 1, 1, 2.5)
%fibers = FiberFinder(5, 5, 5, testImage4, 1, 1, 1, 1, 2.5)
% fibers = FiberFinder(3, 4, 5, testImage5, 1.2, 1.2, 1, 1, 2.5)

%testImage6 = zeros(1, 3125760);
%fibers = FiberFinder(1, 1760, 1776, testImage6, 1, 1, 1, 1, 2.5)

% [imgMatrix, cmap] = imread('C:\Users\kesha\Documents\3D_curvelet_analysis_mex\Images\f0_t0_i0_ch0_c53_r21_z0_mSLIM.tif');
imgMatrix = imread('C:\Users\kesha\Documents\3D_curvelet_analysis_mex\Images\f0_t0_i0_ch0_c53_r21_z0_mSLIM.tif');
% imshow(imgMatrix)
imgVector = reshape(imgMatrix.',1,[]); % Flattens the 2D array by row major order

imgArray = zeros(1, 3125760);

count = 1;

for element = imgVector
  imgArray(count) = element;
  count = count + 1;
end

% Img Matrix is 1760 x 1776 (rows x cols)
% lmp threshold is 0.2???, lmpBoxRadius = 250, danglingFiberLength = 30 as per Steom et al and CurveAlign;

disp('Calling FiberFinder function from .m file.')
fibers = FiberFinder(1, 1760, 1776, imgArray, 0.20, 5, 2, 10, 0);

%fibers = FiberFinder(5, 5, 5, testImage2, 1.2, 1.2, 1, 1, 2.5);

numPoints = 0;
numFibers = 0;
for element = fibers
  if element == -1
    numFibers = numFibers + 1;
    continue;
  end
  numPoints = numPoints + 1;
end


disp('Number of Fibers Identified:')
disp(numFibers)

x = zeros(1, numPoints);
y = zeros(1, numPoints);
z = zeros(1, numPoints);

numPoints = 1;
pointIndex = 0;

for element = fibers

  if element == -1
    pointIndex = 0;
    continue;
  end

  if pointIndex == 0
    z(numPoints) = element;
  end

  if pointIndex == 1
    y(numPoints) = element;
  end

  if pointIndex == 2
    x(numPoints) = element;
    numPoints = numPoints + 1;
  end

  pointIndex = pointIndex + 1;
  pointIndex = mod(pointIndex, 3);

end

%s = scatter3(x, y, z, '.')
%set(gca,'XLim',[0 5],'YLim',[0 5],'ZLim',[0 5])
scatter(x, y, '.');
set(gca,'XLim',[0 1800],'YLim',[0 1800])

disp('Finished main')

% "C:\Program Files\MATLAB\R2018a\bin\matlab.exe" -nodisplay -nosplash -nodesktop -r "run('C:\Users\kesha\Documents\3D_curvelet_analysis_mex\main.m');"
