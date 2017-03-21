%Loads the data from the text file into the workspace
fid=fopen('2DMonte.txt');

%Extracts the data from the file.
dataFile = fscanf(fid,'%g,%g,%g\n',[3 Inf]);
dataFile = dataFile';
mRatio = dataFile(:,1);
mChirp = dataFile(:,2);
distance = dataFile(:,3);

%Makes a figure featuring an (ma,mb) scatter and a histogram.
%Also now features a nice colourmap.
%It only really looks good when you maximise the plot window.
%This first block makes the scatter.
subplot(2,2,1)
samp = scatter(mRatio,mChirp,'.');
%xlim([min(mRatio) max(mRatio)]);
%ylim([min(mChirp) max(mChirp)]);
xlabel('Mass Ratio')
ylabel('Chirp Mass / kg')
title('Data Point Plot')
hold on
legend([samp],{'(Chirp Mass,Mass Ratio) Samples'})
hold off;

%This makes the histogram.
subplot(2,2,2)
hist3(dataFile(:,1:2),[20 20])
xlabel('Mass Ratio')
ylabel('Chirp Mass / kg')
zlabel('Sample Density')
title('Posterior Histogram')
set(get(gca,'child'),'FaceColor','interp','CDataMode','auto')

%This extracts some data from the histogram. n is the number of counts per
%bin and C is the mass value of the bin centers. M is the number of counts
%of the highest bin in each column of n, and I is the index of each of said
%bins within its column of n. N is then the number of counts in the 
%absolute highest bin, and Imb is its index (ie. which column of n it lies 
%in). Ima is then the row in which it lies.
subplot(2,2,3)
[n,C] = hist3(dataFile(:,1:2),[20 20]);
[M,I] = max(n);
[N,Imb] = max(M);
Ima = I(Imb);

%Finds the (m1,m2) combination of the highest histogram bar.
mChirpMax = C{1}(Ima);
mRatioMax = C{2}(Imb);
DispmaMax = sprintf('The best value of chirp mass is %e kg',mChirpMax);
DispmbMax = sprintf('The best value of mass ratio is %e',mRatioMax);
disp(DispmaMax);
disp(DispmbMax);

%This plots the density of the histogram as a flat colourmap.
n1 = n';
n1(size(n,1) + 1, size(n,2) + 1) = 0;
xb = linspace(min(mRatio),max(mRatio),size(n,1)+1);
yb = linspace(min(mChirp),max(mChirp),size(n,1)+1);
h = pcolor(xb,yb,n1);
h.ZData = ones(size(n1)) * -max(max(n));
colormap(jet)
hold on
xlabel('Mass Ratio')
ylabel('Chirp Mass / kg')
title('Histogram Density Plot')
hold off

%Makes a kernel density plot of the data
subplot(2,2,4)
ksdensity(dataFile(:,1:2))
xlabel('Mass Ratio')
ylabel('Chirp Mass / kg')
zlabel('p')

fclose(fid);

%Some of the code is lifted from the MATLAB documentation on hist3

%%

%Loads the data from the text file into the workspace
fid=fopen('2DMonte.txt');

%Extracts the data from the file.
dataFile = fscanf(fid,'%g,%g,%g\n',[3 Inf]);
dataFile = dataFile';
mRatio = dataFile(:,1);
mChirp = dataFile(:,2);
distance = dataFile(:,3);

%Makes a figure featuring an (ma,mb) scatter and a histogram.
%Also now features a nice colourmap.
%It only really looks good when you maximise the plot window.
%This first block makes the scatter.
subplot(2,2,1)
samp = scatter(distance,mChirp,'.');
%xlim([min(mRatio) max(mRatio)]);
%ylim([min(mChirp) max(mChirp)]);
xlabel('Distance / m')
ylabel('Chirp Mass / kg')
title('Data Point Plot')
hold on
legend([samp],{'(Chirp Mass,Distance) Samples'})
hold off;

%This makes the histogram.
subplot(2,2,2)
hist3(dataFile(:,2:3),[20 20])
xlabel('Distance / m')
ylabel('Chirp Mass / kg')
zlabel('Sample Density')
title('Posterior Histogram')
set(get(gca,'child'),'FaceColor','interp','CDataMode','auto')

%This extracts some data from the histogram. n is the number of counts per
%bin and C is the mass value of the bin centers. M is the number of counts
%of the highest bin in each column of n, and I is the index of each of said
%bins within its column of n. N is then the number of counts in the 
%absolute highest bin, and Imb is its index (ie. which column of n it lies 
%in). Ima is then the row in which it lies.
subplot(2,2,3)
[n,C] = hist3(dataFile(:,2:3),[20 20]);
[M,I] = max(n);
[N,Imb] = max(M);
Ima = I(Imb);

%Finds the (m1,m2) combination of the highest histogram bar.
mChirpMax = C{1}(Ima);
mRatioMax = C{2}(Imb);
DispmaMax = sprintf('The best value of chirp mass is %e kg',mChirpMax);
DispmbMax = sprintf('The best value of mass ratio is %e',mRatioMax);
disp(DispmaMax);
disp(DispmbMax);

%This plots the density of the histogram as a flat colourmap.
n1 = n';
n1(size(n,1) + 1, size(n,2) + 1) = 0;
xb = linspace(min(distance),max(distance),size(n,1)+1);
yb = linspace(min(mChirp),max(mChirp),size(n,1)+1);
h = pcolor(xb,yb,n1);
h.ZData = ones(size(n1)) * -max(max(n));
colormap(jet)
hold on
xlabel('Distance / m')
ylabel('Chirp Mass / kg')
title('Histogram Density Plot')
hold off

%Makes a kernel density plot of the data
subplot(2,2,4)
ksdensity(dataFile(:,2:3))
xlabel('Distance / m')
ylabel('Chirp Mass / kg')
zlabel('p')

fclose(fid);

%Some of the code is lifted from the MATLAB documentation on hist3
