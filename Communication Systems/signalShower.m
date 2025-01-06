%% Signal Viewer Script
%% Input Parameters
% Enter the signal as a mathematical function or a predefined vector
% Example: @(t) sin(2*pi*10*t) for a 10 Hz sine wave
signal_function = @(t) sin(2*pi*4000*t) + cos(2*pi*8000*t); % Define the signal

% Specify the total time duration for the signal
signal_duration = 2; % Total signal duration in seconds

% Sampling frequency (Hz)
sampling_frequency = 16000; % Samples per second

% Time window to view the signal
view_start_time = 0; % Start time of the view window (seconds)
view_end_time = 0.001; % End time of the view window (seconds) (hvis du kun får en blå blok, så gør sampling tid mindre

%% Signal Generation
% Create a time vector for the full signal
t = 0:1/sampling_frequency:signal_duration; % Time vector

% Generate the signal using the input function
y = signal_function(t);

%% Select Signal for Viewing
% Define indices for the view window
view_indices = (t >= view_start_time) & (t <= view_end_time);

% Extract the time and signal for the specified view window
view_time = t(view_indices);
view_signal = y(view_indices);

%% Plot the Signal
figure(1);
plot(view_time, view_signal, 'b', 'LineWidth', 1.5);
grid on;

title('Selected Portion of the Signal');
xlabel('Time (s)');
ylabel('Amplitude');
legend('Signal');
