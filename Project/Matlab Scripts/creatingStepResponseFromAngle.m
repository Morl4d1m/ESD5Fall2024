angleData = readtable("C:\Users\Christian Lykke\Documents\MATLAB\vehicleTurningDataTopViewPerfectManipulated.xlsx");

% Extract time (column 1), angle (column 2) and difference in angle (column
% 3)
timeSteps = table2array(angleData(1:end, 1));         % Extract time (Column 1) as an array
angleDegrees = table2array(angleData(1:end, 2)); % Extract angle in degrees (Column 2) as an array
angleDifference = table2array(angleData(1:end, 3));
averageAngleDifference10 = table2array(angleData(1:end, 8));
averageAngleDifference20 = table2array(angleData(1:end, 9));

% Plot data
figure(222);
plot(timeSteps, angleDifference, '-o');
xlabel('Time (s)');
ylabel('Angle Difference (degrees)');
title('Angle Difference vs. Time');

% Set x-axis tick interval to every 0.1 seconds
xticks(min(timeSteps):0.1:max(timeSteps));

% Enable grid for better visualization
grid on;

s=tf('s')
g=exp(-4.988*s)*(0.043/(0.075*s+1));
figure(333);
step(g);

figure(333);
plot(timeSteps,angleDegrees);
xlabel('Time (s)');
ylabel('Angle (degrees)');
title('Angle Over Time at 0.7 km/h Difference');
xticks(min(timeSteps):0.1668:max(timeSteps));
xlim([0 11.488]);
xticks(0:0.25:max(timeSteps));
grid on;


figure(444);
plot(timeSteps,angleDifference);
hold on;
step(g,'g-');
hold off;
hold on;
plot(timeSteps,averageAngleDifference10, 'y-','LineWidth', 1.5);
hold off;
hold on;
plot(timeSteps,averageAngleDifference20, 'r-','LineWidth', 1.5);
hold off;
%hold on;
%mdl =fitlm(timeSteps,angleDifference);
%plot(timeSteps, predict(mdl, timeSteps), 'r-', 'LineWidth', 1.5);
%hold off;
xlabel('Time (s)');
ylabel('Angle Difference (degrees)');
title('Angle Difference Over Time at 0.7 km/h Difference');
xticks(min(timeSteps):0.1668:max(timeSteps));
xlim([0 11.488]);
grid on;