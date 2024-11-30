% Parametre til frekvenssamplingsfilter
N = 35;                  % Filterlængde
fs = 8000;               % Samplefrekvens i Hz
frequencies = [0, 750, 1000, 1500, fs/2]; % Frekvenser i Hz
gains_dB = [0, -1, -3, -10, -40];         % Gain i dB (krav)

% Konverter gain fra dB til lineær skala
gains_linear = 10.^(gains_dB / 20);

% Normér frekvenserne til [0, 1] (Nyquist-normalisering)
f_norm = frequencies / (fs / 2);

% Design impulsresponsen med `fir2`
h = fir2(N-1, f_norm, gains_linear);

% Skab et grundsignal til test
duration = 0.05; % Signalets varighed (kort for overskuelighed)
x_noisy = generateNoisySignal(fs, duration);

% Benyt filteret på signalet
y_filtered = conv(x_noisy, h, 'same'); % Fold signalet med filteret uden at forlænge signalet

% Plot langs tid
t = (0:length(x_noisy)-1) / fs;

% Plot det støjende signal i tid
figure(15);
subplot(4,1,1);
plot(t, x_noisy);
title('Signal med grundfrekvens på 500 Hz forurenet med højfrekvent støj');
xlabel('Tid (s)');
ylabel('Amplitude');
grid on;

% Plot det filtrerede signal i tid
figure(15);
subplot(4,1,2);
plot(t, y_filtered);
title('Filtreret signal');
xlabel('Tid (s)');
ylabel('Amplitude');
grid on;

% FFT af det forurenede signal
N_fft = 1024; % FFT længde for god opløsning
f_axis = (0:N_fft-1) * fs / N_fft; % Frekvensakse
X_noisy = fft(x_noisy, N_fft);
figure(15);
subplot(4,1,3);
plot(f_axis(1:N_fft/2), 20*log10(abs(X_noisy(1:N_fft/2))));
title('FFT af det forurenede signal');
xlabel('Frekvens (Hz)');
ylabel('Amplitude (dB)');
yticks([-40:10:60]);
grid on;

% FFT af det filtrerede signal
Y_filtered = fft(y_filtered, N_fft);
figure(15);
subplot(4,1,4);
plot(f_axis(1:N_fft/2), 20*log10(abs(Y_filtered(1:N_fft/2))));
title('FFT af det filtrerede signal');
xlabel('Frekvens (Hz)');
ylabel('Amplitude (dB)');
yticks([-40:10:60]);
grid on;

% Plot amplituderespons (dB skala langs 0 til 2*pi)
figure(16);
plot(omega, H_dB, 'LineWidth', 1.5);
hold on;
xline(750, 'r--', 'DisplayName', '-1dB Ønsket');
yline(-1,'r--', 'DisplayName', '-1dB Ønsket');
xline(1000, 'g--', 'DisplayName', '3dB Knæk');
yline(-3,'g--', 'DisplayName', '3dB Knæk');
xline(1500, 'b--', 'DisplayName', '-10dB Max');
yline(-10,'b--', 'DisplayName', '-10dB Max');
xline(1750, 'm--');
xline(2000, 'm--');
xline(2500, 'm--');
xline(3000, 'm--');
hold off;
xlabel('Frekvens (Hz)'); ylabel('|H(e^{j\omega})| (dB)');
title('Amplituderespons (dB-skala)');
ylim([-40 5]); % Begræns y-aksen til at fokusere på relevante værdier
grid on;
xlim([0 fs/2]);

% Skab et støjende signal vha den her
function x_noisy = generateNoisySignal(fs, duration)
    % Parametre for signalet
    f_passband = 500;             % Frekvens i pasbåndet
    amplitude_passband = 1;       % Amplitude for pasbåndssignalet
    
    % Generer tidsvektor
    t = 0:1/fs:duration - 1/fs;
    
    % Skab pasbåndskomponenten (ren 500 Hz sinusbølge)
    signal_passband = amplitude_passband * sin(2 * pi * f_passband * t);
    
    % Skab højfrekvent støj (frekvenser over 1500 Hz)
    high_freqs = [1750, 2000, 2500, 3000]; % Støjfrekvenser i stopbåndet
    noise = 0; % Initialiser støj
    
    for f_noise = high_freqs
        noise = noise + 0.5 * sin(2 * pi * f_noise * t); % Tilføj hver højfrekvente komponent
    end
    
    % Kombiner signal og støj
    x_noisy = signal_passband + noise;
end
