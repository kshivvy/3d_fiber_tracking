%folderPath = 'C:\Users\kesha\Documents\3D_Fiber_Extraction\Images\cropped_brain\z_limit';
%folderPath = 'C:\Users\kesha\Documents\3D_Fiber_Extraction\Images\fl_zee_stack\mini_cropped';
folderPath = 'C:\Users\kesha\Documents\3D_Fiber_Extraction\Images\fl_zee_stack';
filePattern = fullfile(folderPath, '*.tif');
tifFiles = dir(filePattern);

numPixels = 0;
height = 0;
width = 0;
depth = 0;


for i = 1:length(tifFiles)
  baseFileName = tifFiles(i).name;
  fullFileName = fullfile(folderPath, baseFileName);
  img = imread(fullFileName);
  [rows, columns, numberOfColorChannels] = size(img);
  numPixels = numPixels + rows*columns;
  height = rows;
  width = columns;
  depth = depth + 1;
end

imgArray = zeros(1, numPixels);


index = 1;
for i = 1:length(tifFiles)
  baseFileName = tifFiles(i).name;
  fullFileName = fullfile(folderPath, baseFileName);
  rgbImage = imread(fullFileName);
  imgMatrix = rgb2gray(rgbImage); %Only for colored tiff stacks
  %imgMatrix = rgbImage;

  % Mean subtract each image
  mean  = mean2(imgMatrix);
  imgMatrix = imsubtract(imgMatrix, mean);

  % Filters binray image by area of object
  %minArea = (height * width) / 250;
  minArea = 100;
  maxArea = height * width;
  BW = bwareafilt(logical(imgMatrix), [minArea, maxArea]);

  for row = 1:height
    for col = 1:width
      if BW(row, col) == 0
        imgMatrix(row, col) = 0;
      end
    end
  end

  % Flattens the 2D array by row major order
  imgVector = reshape(imgMatrix.',1,[]);

  for element = imgVector
    imgArray(index) = element;
    index = index + 1;
  end

end


% Parameters: Ideal values differ for images
% int width:                         image size in "x" direction
% int height:                        image size in "y" direction
% int depth:                         image size in "z" direction
% double * image:                    Will be converted into a double ***, is a 1D representation of the image
% double binaryThreshold:            pixel intensity threshold distinguishing a background vs foreground pixel
% double nucleationPointThreshold:   pixel intensity threshold for a nucleation point, aka fiber "center"
% double localMaximumPointThreshold: pixel intensity threshold for a local max nucleation point
% int lmpBoxRadius:                  box radius in which to search for local maximum points (LMPs), given a nucleation point
% double danglingFiberLength:        fiber length (in size relative to number of pixels) at which to not consider a fiber.

tic
  %fibers = FiberFinder(depth, height, width, imgArray, 0.05, 5, 2, 13, 5);
  %fibers = FiberFinder(depth, height, width, imgArray, 10, 5, 2, 1, 5);
  %fibers = FiberFinder(1, rows, cols, imgArray, 5, 15, 7, 2, 20);
  fibers = FiberFinder(depth, height, width, imgArray, 10, 5, 2, 1, 10);
toc

% Extract x, y coordinates from the fibers
xCoords = [];
yCoords = [];
zCoords = [];
coordType = 0; % x = 0, y = 1, z = 2
numFibers = 0;

hold on

for element = fibers

  if element == -1

    if length(xCoords) == 0
      coordType = 0;
      continue;
    end

    numFibers = numFibers + 1;
    P = [xCoords.', yCoords.', zCoords.'];
    k = boundary(P, 1);
    trisurf(k,P(:,1),P(:,2),P(:,3),'Facecolor',rand(1, 3))

    xCoords = [];
    yCoords = [];
    zCoords = [];
    coordType = 0;
    continue;
  end

  if coordType == 0
    xCoords = [xCoords element];
  end

  if coordType == 1
    yCoords = [yCoords element];
  end

  if coordType == 2
    zCoords = [zCoords element];
  end

  coordType = coordType + 1;
  coordType = mod(coordType, 3);
end

P = [xCoords.', yCoords.', zCoords.'];
k = boundary(P);
trisurf(k,P(:,1),P(:,2),P(:,3),'Facecolor',[0 1 0])
axis([0 width 0 height 0 depth])
numFibers = numFibers + 1;
disp('Nubmer of Fibers: ')
disp(numFibers)
rotate3d on
