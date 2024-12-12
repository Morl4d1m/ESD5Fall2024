% Load Video
videoFile = 'C:\Users\Christian Lykke\Documents\Skole\Aalborg Universitet\ESD5\Project\Videoer\vehicleSpinningLeftMaxSpeedForMatlab.mp4';
video = VideoReader(videoFile);

% Initialize Point Tracker
pointTracker = vision.PointTracker('MaxBidirectionalError', 2);

% Read the first frame
firstFrame = readFrame(video);
imshow(firstFrame);
title('Select Rear Corners of the Vehicle');
% Manually select two points on the rear of the vehicle
[x, y] = ginput(2);  % Select two points on the rear of the vehicle

% Initialize the point tracker with selected points
points = [x, y];
initialize(pointTracker, points, firstFrame);

% Parameters for angle calculation
initialDirection = [0, 1];  % This represents a horizontal line (x-axis)

% Initialize array to store angle data
angles = [];
timestamps = [];
frameNumber = 1;

% Process Video Frame-by-Frame
while hasFrame(video)
    % Read the next frame
    frame = readFrame(video);
    
    % Track the points
    [trackedPoints, validity] = pointTracker(frame);
    
    if all(validity)
        % Calculate the vector between the two points
        vehicleVector = trackedPoints(2, :) - trackedPoints(1, :);
        
        % Calculate the angle with respect to the initial horizontal direction
        angle = atan2d(vehicleVector(2), vehicleVector(1)) - atan2d(initialDirection(2), initialDirection(1));
        
        % Normalize the angle to the range [-180, 180]
        angle = mod(angle + 180, 360) - 180;
        
        % Store angle and timestamp
        angles = [angles; angle];
        timestamps = [timestamps; frameNumber / video.FrameRate];
        
        % Optional: Display the tracked points and orientation angle
        imshow(frame); hold on;
        plot(trackedPoints(:,1), trackedPoints(:,2), 'ro', 'MarkerSize', 5);
        quiver(trackedPoints(1,1), trackedPoints(1,2), vehicleVector(1), vehicleVector(2), 0, 'g', 'LineWidth', 2);
        title(['Angle: ', num2str(angle, '%.2f'), '°']);
        drawnow;
        
        % Increment frame number
        frameNumber = frameNumber + 1;
    else
        warning('Lost track of points in frame %d.', frameNumber);
        break;
    end
end

% Release the point tracker
release(pointTracker);

% Plot the Angle Over Time
figure;
plot(timestamps, angles, 'LineWidth', 1.5);
xlabel('Time (s)');
ylabel('Angle (degrees)');
title('Vehicle Turning Angle Over Time');
grid on;

% Create a table with the timestamps and angles
dataTable = table(timestamps, angles, 'VariableNames', {'Timestamp_s', 'Angle_deg'});

% Save the data as a CSV file
writetable(dataTable, 'vehicleSpinningLeftMaxSpeedData.csv');

% Optionally, save the data as a MAT file for easier reloading in MATLAB
save('vehicleSpinningLeftMaxSpeedData.mat', 'dataTable');