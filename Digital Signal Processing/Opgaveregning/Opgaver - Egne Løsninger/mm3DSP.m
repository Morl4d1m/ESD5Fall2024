% Parametre
omega = linspace(0, pi, 1000); % Digital frekvensområde fra 0 til π (radians/sample)

% Invers bilineær transformation for analog frekvens
T = 10; % Samplingstid
Omega = (2 / T) * tan(omega / 2); % Analog frekvens for T=10

% Alternativt for T=2
T = 2;
Omega = tan(omega / 2); % Analog frekvens for T=2

% Beregning af det analoge frekvensrespons H(jΩ)
s = 1j * Omega; % Laplace-variabel
H_s = 2 ./ ((s + 1) .* (s + 3)); % H(jΩ)
mag_H_s = abs(H_s); % Amplitude af det analoge frekvensrespons

% Beregning af det digitale frekvensrespons H(e^{jω})
z = exp(1j * omega); % Z-transformvariabel
H_z = 2 * (z + 1).^2 ./ ((21 * z - 19) .* (23 * z - 17)); % H(e^{jω})
mag_H_z = abs(H_z); % Amplitude af det digitale frekvensrespons

% Plot af magnituderespons
figure;
plot(omega, mag_H_s, 'b', 'LineWidth', 1.5); % Analog filterrespons
hold on;
plot(omega, mag_H_z, 'r--', 'LineWidth', 1.5); % Digital filterrespons
hold off;

% Plottilpasning
xlabel('Frekvens (rad/sample)');
ylabel('Amplitude');
title('Sammenligning af Magnituderespons');
legend('Analogt filter |H(j\Omega)|', 'Digitalt filter |H(e^{j\omega})|');
grid on;
