% Parametre
N = 9;                  % Filterlængde
fs = 18000;             % Samplefrekvens i Hz
fc = 5000;              % Cutoff-frekvens i Hz
wc = 2*pi*fc/fs;        % Normaliseret cutoff-frekvens

% Frekvensrespons H_d[k]
k = 0:N-1;
omega_k = 2*pi*k/N;     % Diskrete frekvenspunkter
H_d = double(omega_k <= wc); % Lavpasfilter-frekvensrespons

% Beregn impulssvaret via IDFT
h = ifft(H_d, 'symmetric');

% Sikring af lineær fase (symmetri omkring midten)
h = fftshift(h); % Flyt nulpunkt til midten for symmetri

% Amplitude- og faseplot
[H, omega] = freqz(h, 1, 1024, fs); % Frekvensrespons (amplitude og fase)

% Plot impulssvaret
figure(4);
subplot(3, 1, 1);
stem(0:N-1, h, 'filled');
xlabel('n'); ylabel('h[n]');
title('Impulssvar for FIR-filter');
grid on;

% Plot amplituderespons
figure(4);
subplot(3, 1, 2);
plot(omega, abs(H), 'LineWidth', 1.5);
xlabel('Frekvens (Hz)'); ylabel('|H(e^{j\omega})|');
title('Amplituderespons');
grid on;

% Plot fase
figure(4);
subplot(3, 1, 3);
plot(omega, angle(H), 'LineWidth', 1.5);
xlabel('Frekvens (Hz)'); ylabel('Fase (radianer)');
title('Faserespons');
yticks(-pi:(pi/2):pi);
grid on;