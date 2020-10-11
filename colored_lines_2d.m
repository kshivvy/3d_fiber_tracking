% 2D Image Test
%imgMatrix = imread('C:\Users\kesha\Documents\ctFIRE_v2.0Beta_TestImages\2B_D9_crop1.tif');
imgMatrix = imread('C:\Users\kesha\Pictures\Jorge_brain\10x-ctrl-ac.tif');
[rows, cols, numberOfColorChannels] = size(imgMatrix);
imgVector = reshape(imgMatrix.',1,[]); % Flattens the 2D array by row major order

imgArray = zeros(1, rows*cols);

count = 1;

for element = imgVector
  imgArray(count) = element;
  count = count + 1;
end

tic
  fibers = FiberFinder(1, rows, cols, imgArray, 0.10, 5, 13, 2, 0);
toc

I = imread('C:\Users\kesha\Pictures\Jorge_brain\10x-ctrl-ac.tif');
imshow(I)
hold on


% Extract x, y coordinates from the fibers
xCoords = [];
yCoords = [];
coordType = 0; % x = 0, y = 1, z = 2

for element = fibers

  if element == -1
    plot(xCoords, yCoords, 'color', rand(1, 3))
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
