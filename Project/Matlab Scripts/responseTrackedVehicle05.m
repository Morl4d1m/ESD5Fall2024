% Columns: [Frame, Timestamp, X Position, X Velocity, X Acceleration, Current]
data = [];  % This will store all frames' data

% Define parameters
fps = 60;  % Frames per second
time_step = 1 / fps;  % Time interval between frames

% Positions in meters - estimated by eye - first sample is last frame
% before amperes are drawn from power supply
positions = [0.0, 0.0, 0.0, 0.0, 0.001, 0.001, 0.002, 0.003, 0.004, 0.006, 0.008, 0.01, 0.011, 0.013, 0.015, 0.016, 0.017, 0.018, 0.02, 0.021, 0.022, 0.024, 0.026, 0.028, 0.03, 0.033, 0.036, 0.038, 0.04, 0.042, 0.045, 0.047, 0.049, 0.052, 0.055, 0.057, 0.059, 0.062, 0.064, 0.066, 0.069, 0.072, 0.074, 0.076, 0.078, 0.081, 0.083, 0.086, 0.088, 0.09, 0.092, 0.094, 0.096, 0.099, 0.101, 0.103, 0.106, 0.108, 0.11, 0.113, 0.115, 0.117, 0.119, 0.12, 0.123, 0.126, 0.128, 0.13, 0.132, 0.135, 0.137, 0.139, 0.142, 0.144, 0.146, 0.149, 0.151, 0.153, 0.155, 0.158, 0.16, 0.162, 0.164, 0.167, 0.170, 0.173, 0.176, 0.180, 0.182, 0.185, 0.189, 0.191, 0.194, 0.197, 0.200, 0.202, 0.205, 0.209, 0.212, 0.216];

% Current in amps
current = [0.0, 0.627, 0.627, 0.627, 0.627, 0.627, 0.627, 0.627, 0.627, 0.627, 0.627, 0.627, 0.627, 0.627, 0.627, 0.627, 0.627, 0.627, 0.627, 0.627, 0.627, 0.889, 0.799, 0.799, 0.799, 0.799, 0.799, 0.799, 0.799, 0.799, 0.799, 0.799, 0.799, 0.799, 0.799, 0.799, 0.799, 0.799, 0.799, 0.799, 0.799, 0.799, 0.869, 0.869, 0.869, 0.869, 0.869, 0.869, 0.869, 0.869, 0.869, 0.869, 0.869, 0.869, 0.869, 0.869, 0.869, 0.869, 0.869, 0.869, 0.869, 0.869, 0.862, 0.862, 0.862, 0.862, 0.862, 0.862, 0.862, 0.862, 0.862, 0.862, 0.862, 0.862, 0.862, 0.862, 0.862, 0.862, 0.862, 0.862, 0.862, 0.862, 0.862, 0.843, 0.843, 0.843, 0.843, 0.843, 0.843, 0.843, 0.843, 0.843, 0.843, 0.843, 0.843, 0.843, 0.843, 0.843, 0.898, 0.898];

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
save('trackedVehicleResponseData05.mat', 'data_table');


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
stepResponse05kmh = tfest(data_id, 1);  % 1st order transfer function - can be changed changing value of np (1)

% Display the transfer function
disp('Estimated Transfer Function:');
disp(stepResponse05kmh);

% Plot of the step response of the transfer function
%step(stepResponse05kmh);
%xlim([0,2]); % If a dot is added at 1.65, the resulting amplitude is 0.217, which is 1 mm
%title('Step Response of the Estimated Transfer Function');
%grid on;

% Calculate the mean of all velocities when nominal speed is reached
% (approx 15 frames in)
mean_velocity = mean(data_table.X_Velocity_m_s(15:end));

% Display the result
disp(['Mean Velocity: ', num2str(mean_velocity), ' m/s']);

s=tf('s')
g=exp(-0.05*s)*11*(0.0125/(0.15*s+1));
figure(3);
step(g);

figure(1);
plot(timestamps,velocities);

figure(2);
plot(timestamps,velocities);
hold on;
step(g,'g-');
hold off;
xlabel('Time (s)');
ylabel('Velocity (m)');
title('Velocity Over Time at 0.5 km/h');
xlim([0 1.7]);
grid on;

figure(3);
plot(timestamps,positions);
xlabel('Time (s)');
ylabel('Positions (m)');
title('Position Over Time at 0.5 km/h');
set(gca, 'XTick', 0:0.1:2); % Set grid ticks
set(gca, 'YTick', 0:0.01:2);
xlim([0 1.7]);
grid on;