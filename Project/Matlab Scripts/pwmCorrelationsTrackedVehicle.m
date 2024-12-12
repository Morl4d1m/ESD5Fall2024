% Define arrays for each column
PWM = [11, 17, 24, 29, 37, 44, 50, 53, 56, 58, 60, 64, 71, 99, 99]; % PWM values as percentages
Speed_kmh = [0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9]; % Speed in km/h
Speed_ms = [0.138, 0.167, 0.195, 0.222, 0.250, 0.278, 0.306, 0.333, 0.361, 0.389, 0.417, 0.444, 0.472, 0.500, 0.528]; % Speed in m/s
Amperes_drawn = [0.802, 0.877, 0.957, 0.987, 1.076, 1.143, 1.268, 1.331, 1.386, 1.392, 1.401, 1.426, 1.440, 1.451, 1.488]; % Amperes drawn
Peak_amperes = [0.898, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]; % Peak Amperes (assume 0 where no data)
Voltage = [13.6, 13.6, 13.6, 13.6, 13.6, 13.6, 13.6, 13.6, 13.6, 13.6, 13.6, 13.6, 13.6, 13.6, 14.4]; % Voltage
PWMLeft = [10, 16, 21, 26, 36, 44, 48, 51, 53, 55, 57, 62, 70, 98, 99];
PWMRight = [15, 19, 26, 32, 39, 46, 52, 55, 56, 58, 61, 64, 73, 99, 99];

% Plotting each subsequent column against PWM
figure(20);

% Define the range of PWM ticks for grid lines
PWM_ticks = 0:1:100; % PWM from 0% to 100% with 1% increments
speedTicks = 0:0.1:2;

% Plot Speed (km/h) vs PWM
%subplot(2,1,1);
figure();
plot(PWM, Speed_kmh, '-o');
hold on;
plot(PWMLeft, Speed_kmh, 'r-');
%mdl =fitlm(PWM,Speed_kmh);
%plot(PWM, predict(mdl, PWM'), 'r-', 'LineWidth', 1.5);
hold off;
hold on;
plot(PWMRight, Speed_kmh, 'g-');
hold off;
xlabel('PWM (%)');
ylabel('Speed (km/h)');
title('Speed (km/h) vs PWM');
subtitle('Blue = Both Belts, Red = Left Belt, Green = Right Belt');
grid on;
set(gca, 'XTick', PWM_ticks); % Set grid ticks
set(gca, 'YTick', speedTicks);
xlim([0 100]);

% Plot Speed (m/s) vs PWM
%subplot(3,2,2);
%plot(PWM, Speed_ms, '-o');
%xlabel('PWM (%)');
%ylabel('Speed (m/s)');
%title('Speed (m/s) vs PWM');
%grid on;

% Plot Amperes drawn vs PWM
%subplot(2,1,2);
%plot(PWM, Amperes_drawn, '-o');
%xlabel('PWM (%)');
%ylabel('Amperes Drawn (A)');
%title('Amperes Drawn vs PWM');
%grid on;
%set(gca, 'XTick', PWM_ticks); % Set grid ticks
%xlim([0 100]);

% Adjust layout
%sgtitle('PWM Correlation Plots');
