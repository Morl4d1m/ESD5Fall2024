angleDataRight = readtable("C:\Users\Christian Lykke\Documents\MATLAB\vehicleSpinningRightDataBest.xlsx");

% Extract time (column 1), angle (column 2) and difference in angle (column
% 3)
timeStepsRight = table2array(angleDataRight(1:509, 1));         % Extract time as an array
angleDegreesRight = table2array(angleDataRight(1:509, 4)); % Extract angle in degrees as an array
angleDifferenceRight = table2array(angleDataRight(1:509, 3));

% Plot data
%figure(222);
%plot(timeSteps, angleDifference, '-o');
%xlabel('Time (s)');
%ylabel('Angle Difference (degrees)');
%title('Angle Difference vs. Time');

% Set x-axis tick interval to every 0.1 seconds
%xticks(min(timeSteps):0.1:max(timeSteps));

% Enable grid for better visualization
%grid on;

s=tf('s')
g=exp(-4.988*s)*exp(-0.12*s)*(2.38/(1.5*s+1));
%figure(333);
%step(g);

figure(33333);
plot(timeStepsRight,angleDegreesRight);
xlabel('Time (s)');
ylabel('Angle (degrees)');
title('Angle Over Time Rotating Right');
xticks(min(timeStepsRight):0.1668:max(timeStepsRight));
yticks(-10:10:380);
%xlim([0 11.488]);
%xticks(0:0.25:max(timeSteps));
grid on;


figure(44444);
plot(timeStepsRight,angleDifferenceRight);
hold on;
step(g,'g-');
hold off;
%hold on;
%mdl =fitlm(timeSteps,angleDifference);
%plot(timeSteps, predict(mdl, timeSteps), 'r-', 'LineWidth', 1.5);
%hold off;
xlabel('Time (s)');
ylabel('Angle Difference (degrees)');
title('Angle Difference Over Time Rotating Right');
%xticks(min(timeSteps):0.1668:max(timeSteps));
xticks(0:0.5:20);
yticks(-1.5:0.1:7);
xlim([0 20]);
grid on;

angleDataLeft = readtable("C:\Users\Christian Lykke\Documents\MATLAB\vehicleSpinningLeftData260degree.xlsx");

% Extract time (column 1), angle (column 2) and difference in angle (column
% 3)
timeStepsLeft = table2array(angleDataLeft(1:488, 1));         % Extract time as an array
angleDegreesLeft = table2array(angleDataLeft(1:488, 4)); % Extract angle in degrees as an array
angleDifferenceLeft = table2array(angleDataLeft(1:488, 3));

% Plot data
%figure(222);
%plot(timeSteps, angleDifference, '-o');
%xlabel('Time (s)');
%ylabel('Angle Difference (degrees)');
%title('Angle Difference vs. Time');

% Set x-axis tick interval to every 0.1 seconds
%xticks(min(timeSteps):0.1:max(timeSteps));

% Enable grid for better visualization
%grid on;

s=tf('s')
g=exp(-4.988*s)*exp(-0.12*s)*(2.54/(1.2*s+1));
%figure(333);
%step(g);

figure(333333);
plot(timeStepsLeft,angleDegreesLeft);
xlabel('Time (s)');
ylabel('Angle (degrees)');
title('Angle Over Time Rotating Left');
xticks(min(timeStepsLeft):0.1668:max(timeStepsLeft));
yticks(-10:10:380);
%xlim([0 20]);
%xticks(0:0.25:max(timeSteps));
grid on;


figure(444444);
plot(timeStepsLeft,angleDifferenceLeft);
hold on;
step(g,'g-');
hold off;
%hold on;
%mdl =fitlm(timeSteps,angleDifference);
%plot(timeSteps, predict(mdl, timeSteps), 'r-', 'LineWidth', 1.5);
%hold off;
xlabel('Time (s)');
ylabel('Angle Difference (degrees)');
title('Angle Difference Over Time Rotating Left');
%xticks(min(timeSteps):0.1668:max(timeSteps));
xticks(0:0.5:20);
yticks(-1.5:0.1:7);
xlim([0 20]);
grid on;

figure(5555);
plot(timeStepsLeft,angleDegreesLeft,'LineWidth', 1.5);
hold on;
plot(timeStepsRight,angleDegreesRight,'LineWidth', 1.5);
hold off;
xlabel('Time (s)');
ylabel('Angle Difference (degrees)');
title('Angle Difference Over Time Rotating Left Compared With Rotating Right');
xticks(0:0.25:20);
yticks(-10:5:370);
%xlim([0 20]);
ylim([-5 285]);
grid on;
