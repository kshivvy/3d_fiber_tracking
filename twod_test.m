% 2D Image Test
%imgMatrix = imread('C:\Users\kesha\Documents\ctFIRE_v2.0Beta_TestImages\2B_D9_crop1.tif');
imgMatrix = imread('C:\Users\kesha\Documents\3D_curvelet_analysis_mex\Images\f0_t0_i0_ch0_c53_r21_z0_mSLIM.tif');
[rows, cols, numberOfColorChannels] = size(imgMatrix);
imgVector = reshape(imgMatrix.',1,[]); % Flattens the 2D array by row major order

imgArray = zeros(1, rows*cols);

count = 1;

for element = imgVector
  imgArray(count) = element;
  count = count + 1;
end

%fibers = FiberFinder(1, rows, cols, imgArray, 0.15, 4.5, 2, 10, 0);
fibers = FiberFinder(1, rows, cols, imgArray, 0.45, 4.5, 2, 10, 0);

% Determine number of fibers and points/fiber for polyline matrix
numPoints = 0;
numFibers = 0;
maxNumberPointsInFiber = 0;
currentNumberPointsInFiber = 0;

for element = fibers
  if element == -1
    numFibers = numFibers + 1;
    if currentNumberPointsInFiber/3 > maxNumberPointsInFiber
      maxNumberPointsInFiber = currentNumberPointsInFiber/3;
    end
    currentNumberPointsInFiber = 0;
    continue;
  end
  numPoints = numPoints + 1;
  currentNumberPointsInFiber = currentNumberPointsInFiber + 1;
end

polylineMatrix = zeros(numFibers, maxNumberPointsInFiber*2);

% Extract x, y, z coordinates from the fibers
numPoints = 1;
currentFiberNumber = 0;
pointIndex = 0;

for element = fibers

  if element == -1
    pointIndex = 0;

    if numPoints < (maxNumberPointsInFiber) && numPoints > 1
      for i = numPoints:(maxNumberPointsInFiber)
        polylineMatrix(currentFiberNumber, (i-1)*2 + 1) = polylineMatrix(currentFiberNumber, (i-1)*2 - 1);
        polylineMatrix(currentFiberNumber, (i)*2) = polylineMatrix(currentFiberNumber, (i)*2 - 2);
      end
    end

    currentFiberNumber = currentFiberNumber + 1;

    numPoints = 1;
    continue;
  end


  if pointIndex == 0
    %element
    %numPoints
    %currentFiberNumber
    polylineMatrix(currentFiberNumber, (numPoints-1)*2 + 1) = element;
  end

  if pointIndex == 1
    %element
    %numPoints
    %currentFiberNumber + 1
    polylineMatrix(currentFiberNumber, (numPoints)*2) = element;
  end

  if pointIndex == 2
    numPoints = numPoints + 1;
  end

  pointIndex = pointIndex + 1;
  pointIndex = mod(pointIndex, 3);

end

if numPoints < (maxNumberPointsInFiber) && numPoints > 1
  for i = numPoints:(maxNumberPointsInFiber)
    polylineMatrix(currentFiberNumber, (i-1)*2 + 1) = polylineMatrix(currentFiberNumber, (i-1)*2 - 1);
    polylineMatrix(currentFiberNumber, (i)*2) = polylineMatrix(currentFiberNumber, (i)*2 - 2);
  end
end

I = imread('C:\Users\kesha\Documents\3D_curvelet_analysis_mex\Images\f0_t0_i0_ch0_c53_r21_z0_mSLIM.tif');
RGB = insertShape(I,'Line',polylineMatrix,'LineWidth',1);

%hold on
%for k = 1:size(polylineMatrix,1)
%    vec = polylineMatrix(k,:);
%    RGB = insertShape(I,'Line', vec,'LineWidth',1, 'Color', 'green');
%end
%hold off

imshow(RGB)
