clear;
%{
for i=0:999;
    omega(i+1)=pi*i/999;
end;
for i=1:1000;
    v4=sqrt((cos(omega(i))+1)^2+(sin(omega(i)))^2);
    v1=sqrt((cos(omega(i))-0.5224)^2+(sin(omega(i))-0.4524)^2);
    v2=sqrt((cos(omega(i))-0.5224)^2+(sin(omega(i))+0.4524)^2);    
    v3=sqrt((cos(omega(i))-0.4142)^2+(sin(omega(i)))^2);
    h(i)=0.0317*((v4)^3)/(v1*v2*v3);
end;

plot(omega,h,omega(187),0.8913,'+',omega(250),1/sqrt(2),'+',omega(375),0.3162,'+');
%}

% Simple FIR Filter with Linear Phase Response
clear; clc; close all;

% Define the filter coefficients (symmetric for linear phase)
h = [0.1, 0.3, 0.4, 0.3, 0.1];  % Example symmetric FIR filter

% Compute frequency response using freqz
[H, omega] = freqz(h, 1, 1024, 'whole');

% Magnitude and Phase of the frequency response
magnitude = abs(H);
phase = angle(H);

% Plot the magnitude and phase response
figure;

% Plot magnitude response
subplot(2, 1, 1);
plot(omega, magnitude, 'LineWidth', 1.5);
grid on;
xlabel('Frequency (\omega)');
ylabel('Magnitude');
title('FIR Filter: Magnitude Response');

% Plot phase response
subplot(2, 1, 2);
plot(omega, unwrap(phase), 'LineWidth', 1.5);
grid on;
xlabel('Frequency (\omega)');
ylabel('Phase (radians)');
title('FIR Filter: Phase Response');

