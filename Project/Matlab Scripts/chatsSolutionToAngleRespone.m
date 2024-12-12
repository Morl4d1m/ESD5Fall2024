angleData = readtable("C:\Users\Christian Lykke\Documents\MATLAB\vehicleTurningDataTopViewPerfectManipulated.xlsx");

% Extract time (column 1), angle (column 2) and difference in angle (column
% 3)
timeSteps = table2array(angleData(1:end, 1));         % Extract time (Column 1) as an array
angleDegrees = table2array(angleData(1:end, 2)); % Extract angle in degrees (Column 2) as an array
angleDifference = table2array(angleData(1:end, 3));

% Identify steady-state turning rate
steady_state_rate = 0.04 / 0.1668; % degrees per second

% Find time constant (tau) by finding time to reach 63% of steady-state rate
% Let's assume the angle data has already been processed to find this value
target_angle = 0.63 * steady_state_rate * (timeSteps(end) - 5); % Adjust by 5 seconds
index_tau = find(angleDegrees >= target_angle, 1);
tau = timeSteps(index_tau) - 5; % time constant after the 5-second mark

% Define transfer function
K = steady_state_rate; % steady-state gain
sys = tf(K, [tau, 1]);

% Plot the step response to verify
step(sys);
title('Step Response of Vehicle Turning');
xlabel('Time (s)');
ylabel('Turning Rate (degrees per second)');

% Save the estimated transfer function parameters for further analysis
save('vehicle_turning_transfer_function.mat', 'sys', 'tau', 'K');
