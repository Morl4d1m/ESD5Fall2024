% Columns: [Frame, Timestamp, X Position, X Velocity, X Acceleration, Current]

data = [];  % This will store all frames' data

% Define parameters
fps = 60;  % Frames per second
time_step = 1 / fps;  % Time interval between frames

% Positions in meters - estimated by eye - first sample is last frame
% before amperes are drawn from power supply
positions = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.001, 0.001, 0.002, 0.003, 0.005, 0.008, 0.012, 0.016, 0.019, 0.023, 0.028, 0.031, 0.035, 0.039, 0.043, 0.048, 0.053, 0.059, 0.064, 0.069, 0.073, 0.076, 0.080, 0.085, 0.090, 0.095, 0.101, 0.106, 0.112, 0.118, 0.124, 0.130, 0.136, 0.142, 0.147, 0.152, 0.157, 0.163, 0.169, 0.175, 0.181, 0.187, 0.193, 0.198, 0.204, 0.210, 0.215, 0.221, 0.226, 0.231, 0.237, 0.242, 0.248, 0.252, 0.257, 0.262, 0.267, 0.273, 0.278, 0.283, 0.289, 0.295, 0.300, 0.306, 0.313, 0.319, 0.324, 0.330, 0.336, 0.343, 0.348, 0.352, 0.358, 0.364, 0.370, 0.376, 0.383, 0.388, 0.393, 0.398, 0.403, 0.407, 0.413, 0.419, 0.425, 0.432, 0.437, 0.442];
%Note that voltage is 4.7 until 0.053, as this is also the first frame of movement. From
%here till 0.157 the voltage is 12.2, before it goes to 13.6 again
%where current is drawn
% Current in amps
current = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.27, 1.27, 1.27, 1.27, 1.27, 1.27, 1.27, 1.27, 1.27, 1.27, 1.27, 1.27, 1.27, 1.27, 1.27, 1.27, 1.27, 1.27, 1.27, 1.27, 1.27, 1.27, 1.27, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.35, 1.35, 1.35, 1.35, 1.35, 1.35, 1.35, 1.35, 1.35, 1.35, 1.35, 1.35, 1.35, 1.35, 1.35];

% F=m*a calculator:
m = 1.736; % Weight in kilos of the tracked vehicle
% a will be derived from x_accel

% K_t calculation
T_stall = 30/100; % 30ncm converted to nm
I_stall = 8.24; % Found on https://www.newcapmaquettes.com/Geared-Motor-50-1-KRICK.html
K_t = T_stall/I_stall;

% Loop through each frame and calculate velocity and acceleration
for frame = 1:length(positions)
    % Current timestamp
    timestamp = (frame - 1) * time_step;
    
    % Current position (X)
    x_pos = positions(frame);
    
    % Calculate velocity (X)
    if frame == 1
        % Assume initial velocity is zero for the first frame
        x_vel = 0;
    else
        x_vel = (positions(frame) - positions(frame - 1)) / time_step;
    end
    
    % Calculate acceleration (X)
    if frame <= 2
        % Assume initial acceleration is zero for the first two frames
        x_accel = 0;
    else
        x_accel = (x_vel - ((positions(frame - 1) - positions(frame - 2)) / time_step)) / time_step;
    end
    
    % Current Amperes
    currentA = current(frame);

    % F=m*a
    Force = m*x_accel;

    % Torque calculation:
    Torque = K_t*currentA;

    % Append the data for this frame
    data = [data; frame, timestamp, x_pos, x_vel, x_accel, currentA, Force, Torque];
end

% Create a table and specify column headers
data_table = array2table(data, 'VariableNames', {'Frame', 'Timestamp_s', 'X_Position_m', 'X_Velocity_m_s', 'X_Acceleration_m_s2', 'Current_A', 'F', 'Torque'});
disp(data_table);
save('trackedVehicleResponseData12.mat', 'data_table');
writetable(data_table);

% Extract the Timestamp and Velocity columns for plotting
timestamps = data_table.Timestamp_s;
velocities = data_table.X_Velocity_m_s;

% Create a plot of Velocity vs. Time
%figure;  % Open a new figure window
%plot(timestamps, velocities, 'b-', 'LineWidth', 1.5);  % Plot with blue line
%xlabel('Time (s)'); 
%ylabel('Velocity (m/s)'); 
%title('Velocity Over Time');  % Plot title
%grid on; 

% Define input and output for transfer function approximation
input = data_table.Current_A;  % Input: Current in Amperes
output = data_table.X_Position_m;  % Output: Position in meters

% Time vector
Ts = 1 / fps;  % Sample time (1/frames per second)
t = (0:length(input)-1) * Ts;  

% Create iddata object
data_id = iddata(output, input, Ts);  

% Estimate transfer function
stepResponse12kmh = tfest(data_id, 1);  % 1st order transfer function - can be changed changing value of np (1)

% Display the transfer function
disp('Estimated Transfer Function:');
disp(stepResponse12kmh);

% Plot of the step response of the transfer function
step(stepResponse12kmh);
%xlim([0,2]); % If a dot is added at 1.65, the resulting amplitude is 0.385, which is 59 mm off
title('Step Response of the Estimated Transfer Function');
grid on;

% Calculate the mean of all velocities when nominal speed is reached
% (approx 60 frames in)
mean_velocity = mean(data_table.X_Velocity_m_s(60:end));

% Display the result
disp(['Mean Velocity: ', num2str(mean_velocity), ' m/s']);

figure(11);
plot(timestamps,velocities);
%hold on;
%step(stepResponse12kmh);
%hold off;
xlabel('Time (s)');
ylabel('Velocity (m)');
title('Velocity Over Time');
grid on;

figure(12);
plot(timestamps,positions);
xlabel('Time (s)');
ylabel('Position (m)'); 
title('Position Over Time');
grid on;

s=tf('s')
g=exp(-0.2*s)*53*(0.0062/(0.15*s+1)); %Bemærk Kirsten havde valgt tau til 0.2
figure(3);
step(g);

figure(13);
plot(timestamps,velocities);
hold on;
step(g,'g-');
hold off;
xlabel('Time (s)');
ylabel('Velocity (m)');
title('Velocity Over Time at 1.2 km/h');
xlim([0 1.7])
grid on;

figure(33);
plot(timestamps,positions);
xlabel('Time (s)');
ylabel('Positions (m)');
title('Position Over Time at 1.2 km/h');
set(gca, 'XTick', 0:0.1:2); % Set grid ticks
set(gca, 'YTick', 0:0.01:2);
xlim([0 1.7]);
grid on;