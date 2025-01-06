% MATLAB Script for 4-PAM Modulation with Matched Colors
clc;
clear;
close all;

%% User Inputs
M = 4; % M-PAM order (e.g., 4 for 4-PAM)
A = [-2, -1, 1, 2]; % Amplitudes corresponding to M-PAM symbols
binary_sequence = '00101101110010'; % Binary sequence to transmit
fs = 1000; % Sampling frequency (samples per second)
fc = 1; % Carrier frequency in Hz
T = 1; % Symbol duration in seconds

% Validate Inputs
if length(A) ~= M
    error('Number of amplitudes must match the M-PAM order.');
end
if mod(length(binary_sequence), log2(M)) ~= 0
    error('Binary sequence length must be a multiple of log2(M).');
end

% Derived Parameters
symbols = cellstr(dec2bin(0:M-1)); % Binary symbols as cell array of strings
t_wave = linspace(0, T, fs); % Time vector for one symbol duration
gT = cos(2 * pi * fc * t_wave); % Carrier signal gT(t)
symbol_map = containers.Map(symbols, A); % Map binary symbols to amplitudes
sequence_length = length(binary_sequence) / log2(M); % Number of symbols

% Predefine Colors for Consistent Symbol Representation
color_map = lines(M); % Generate M unique colors using MATLAB's "lines" colormap

%% Task B: Sequential Signal Waveforms for Each Symbol
figure(1);
hold on;

for i = 1:M
    t_symbol = t_wave + (i-1)*T; % Shift each symbol sequentially in time
    waveform = A(i) * gT; % Modulated waveform for each amplitude
    plot(t_symbol, waveform, 'LineWidth', 1.5, 'Color', color_map(i, :), ...
        'DisplayName', ['Symbol ', symbols{i}]); % Assign a unique color
end

xlabel('Time (s)');
ylabel('Amplitude');
title('Signal Waveforms for 4-PAM (Sequential)');
legend('show');
grid on;
hold off;

%% Task C: Transmission of a Binary Sequence
% Map Binary Sequence to Amplitude Levels
sequence_amplitudes = zeros(1, sequence_length);
sequence_binary = cell(1, sequence_length); % To store binary symbols for the legend

for i = 1:sequence_length
    binary_symbol = binary_sequence((i-1)*log2(M)+1:i*log2(M)); % Extract binary symbol
    sequence_amplitudes(i) = symbol_map(binary_symbol); % Map to amplitude
    sequence_binary{i} = binary_symbol; % Store binary symbol
end

% Time Vector for Entire Transmission
total_time = sequence_length * T; % Total duration of the transmission
t_total = linspace(0, total_time, sequence_length * fs); % Total time vector
carrier = cos(2 * pi * fc * t_total); % Carrier for the entire sequence

% Plot Each Symbol's Contribution (Overlaid) with Matched Colors
figure(2);
hold on;
for i = 1:sequence_length
    start_idx = (i-1)*fs + 1;
    end_idx = i*fs;
    symbol_waveform = zeros(1, length(t_total));
    symbol_waveform(start_idx:end_idx) = sequence_amplitudes(i) * gT;
    
    % Find the color corresponding to the binary symbol
    symbol_idx = find(strcmp(sequence_binary{i}, symbols));
    plot(t_total, carrier .* symbol_waveform, 'LineWidth', 1.2, ...
        'Color', color_map(symbol_idx, :), ... % Use the matched color
        'DisplayName', ['Symbol: ', sequence_binary{i}]); % Binary symbol in legend
end

xlabel('Time (s)');
ylabel('Amplitude');
title('4-PAM Modulated Signal with Binary Symbol Labels');
legend('show', 'Location', 'Best');
grid on;
hold off;

% Display Transmission Duration
disp(['Transmission duration: ', num2str(total_time), ' seconds']);
