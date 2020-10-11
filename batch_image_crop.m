addpath 'C:\Users\kesha\Pictures\fl_zee_stack\';
folderPath = 'C:\Users\kesha\Pictures\fl_zee_stack';
filePattern = fullfile(folderPath, '*.tif');
images = dir(filePattern);

N = length(images);     % number of fimages
I = imread(images(1).name);  % crop one image for reference
[x, rect] = imcrop(I);

for i = 1:N
    I = imread(images(i).name);
    I = imcrop(I,rect);
    %I = downsample(I, 2);
    [filepath,name,ext] = fileparts(images(i).name);
    name = strcat(name, '_cropped.tif');
    I = double(I);
    imwrite(I, name, 'tif');
end

disp('done!');
