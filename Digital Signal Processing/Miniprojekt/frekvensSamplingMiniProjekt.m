% Parametre
N = 9;                  % Filterlængde
fs = 8000;              % Samplefrekvens i Hz
frequencies = [0, 750, 1000, 1500, fs/2]; % Frekvenser i Hz
gains_dB = [0, -1, -3, -10, -40];         % Gain i dB (krav)

% Konverter gain fra dB til lineær skala
gains_linear = 10.^(gains_dB / 20);

% Normér frekvenserne til [0, 1] (Nyquist-normalisering)
f_norm = frequencies / (fs / 2);

% Design impulssvaret med `fir2`
h = fir2(N-1, f_norm, gains_linear);

% Beregn frekvensrespons til visualisering
[H, omega] = freqz(h, 1, 1024, 'whole', fs);

% Beregn frekvensrespons i dB
H_dB = 20 * log10(abs(H));

% Plot impulssvaret
figure(6);
subplot(2, 2, 1);
stem(0:N-1, h, 'filled');
xlabel('n'); ylabel('h[n]');
title('Impulsrespons for FIR-filter');
grid on;

% Plot amplituderespons (lineær skala)
figure(6);
subplot(2, 2, 2);
plot(omega, abs(H), 'LineWidth', 1.5);
xlabel('Frekvens (Hz)'); ylabel('|H(e^{j\omega})|');
title('Amplituderespons (lineær skala)');
grid on;
xlim([0 8000]);

% Plot amplituderespons (dB skala)
figure(6);
subplot(2, 2, 3);
plot(omega, H_dB, 'LineWidth', 1.5);
xlabel('Frekvens (Hz)'); ylabel('|H(e^{j\omega})| (dB)');
title('Amplituderespons (dB-skala)');
ylim([-40 5]); % Begræns y-aksen til at fokusere på relevante værdier
grid on;
xlim([0 8000]);

% Plot faserespons
figure(6);
subplot(2, 2, 4);
plot(omega, angle(H), 'LineWidth', 1.5);
xlabel('Frekvens (Hz)'); ylabel('Fase (radianer)');
title('Faserespons');
grid on;
xlim([0 8000]);

