% 2D Image Test
filepath = 'C:\Users\kesha\Documents\3D_Fiber_Extraction\Images\small_cropped_brain\FT_bandpass\f0_t0_i0_ch0_c0_r0_z653_mhilbert_cropped_20_2_060.tif';
imgMatrix = imread(filepath);
%imgMatrix = rgb2gray(imgMatrix); Only for colored images
[rows, cols, numberOfColorChannels] = size(imgMatrix);

height = rows;
width = cols;

% Mean subtract the image
mean  = mean2(imgMatrix);
imgMatrix = imsubtract(imgMatrix, mean);

% Filters binray image by area of object
minArea = (height * width) / 250;
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

imgArray = zeros(1, rows*cols);

count = 1;

for element = imgVector
  imgArray(count) = element;
  count = count + 1;
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
  fibers = FiberFinder(1, rows, cols, imgArray, 5, 15, 7, 2, 20);
  %fibers = FiberFinder(1, height, width, imgArray, 10, 5, 2, 1, 10);
toc

I = imread(filepath);
imshow(I)
hold on

fiberColor = [0 1 0];

% Extract x, y coordinates from the fibers
xCoords = [];
yCoords = [];
coordType = 0; % x = 0, y = 1, z = 2
numFibers = 0;

for element = fibers

  if element == -1

    if ~isempty(xCoords)

      % Generate linear regression
      n = max(size(xCoords));
      X = transpose(xCoords);
      X = [ones(n, 1) X];

      b = regress(transpose(yCoords), X);
      yFit = b(1) + b(2)*xCoords;

      plot(xCoords, yFit, 'color', fiberColor)
      numFibers = numFibers + 1;
    end

    xCoords = [];
    yCoords = [];
    coordType = 0;
    continue
  end

  if coordType == 0
    xCoords = [xCoords element];
  end

  if coordType == 1
    yCoords = [yCoords element];
  end

  coordType = coordType + 1;
  coordType = mod(coordType, 3);
end

n = length(xCoords);
X = transpose(xCoords);
X = [ones(n, 1) X];


b = regress(transpose(yCoords), X);
yFit = b(1) + b(2)*xCoords;

numFibers = numFibers + 1;
plot(xCoords, yFit, 'color', fiberColor)
disp('Nubmer of Fibers: ')
disp(numFibers)
