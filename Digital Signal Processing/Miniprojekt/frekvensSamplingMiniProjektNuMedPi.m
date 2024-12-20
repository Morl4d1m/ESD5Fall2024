% Parametre
N = 91;                  % Filterlængde
fs = 8000;              % Samplefrekvens i Hz
frequencies = [0, 750, 1000, 1500, fs/2]; % Frekvenser i Hz
gains_dB = [0, -1, -3, -10, -40];         % Gain i dB (krav)

% Konverter gain fra dB til lineær skala
gains_linear = 10.^(gains_dB / 20);

% Normér frekvenserne til [0, 1] (Nyquist-normalisering)
f_norm = frequencies / (fs / 2);

% Design impulssvaret med `fir2`
h = fir2(N-1, f_norm, gains_linear);

% Beregn frekvensrespons fra 0 til 2*pi (hele området)
[H, omega] = freqz(h, 1, 1024, 'whole', fs);
H_dB = 20 * log10(abs(H));

% Plot impulsresponsen
figure(7);
subplot(3, 2, 1);
stem(0:N-1, h, 'filled');
xlabel('n'); ylabel('h[n]');
title('Impulsrespons for FIR-filter');
grid on;

% Plot amplituderespons (lineær skala)
figure(7);
subplot(3, 2, 2);
plot(omega, abs(H), 'LineWidth', 1.5);
xlabel('Frekvens (Hz)'); ylabel('|H(e^{j\omega})|');
title('Amplituderespons (lineær skala)');
grid on;
xlim([0 fs]);

% Plot amplituderespons (dB skala)
figure(7);
subplot(3, 2, 3);
plot(omega, H_dB, 'LineWidth', 1.5);
xlabel('Frekvens (Hz)'); ylabel('|H(e^{j\omega})| (dB)');
title('Amplituderespons (dB-skala)');
ylim([-40 5]); % Begræns y-aksen til at fokusere på relevante værdier
grid on;
xlim([0 fs]);

% Plot faserespons
figure(7);
subplot(3, 2, 4);
plot(omega, angle(H), 'LineWidth', 1.5);
xlabel('Frekvens (Hz)'); ylabel('Fase (radianer)');
title('Faserespons');
grid on;
xlim([0 fs]);

% Plot amplituderespons (dB skala langs 0 til 2*pi)
figure(7);
subplot(3, 2, 5);
plot(omega * 2 * pi / fs, H_dB, 'LineWidth', 1.5);
xlabel('\omega (rad/sample)'); ylabel('|H(e^{j\omega})| (dB)');
title('Amplituderespons (dB skala, 0 til 2\pi)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
ylim([-40 5]);
xlim([0 2*pi]);
grid on;

% Plot faserespons (0 til 2*pi)
figure(7);
subplot(3, 2, 6);
plot(omega * 2 * pi / fs, angle(H), 'LineWidth', 1.5);
xlabel('\omega (rad/sample)'); ylabel('Fase (radianer)');
title('Faserespons (0 til 2\pi)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
xlim([0 2*pi]);
grid on;


%opgave 3/punkt 3
% Parametre
N = 3;                  % Filterlængde
fs = 8000;              % Samplefrekvens i Hz
frequencies = [0, 750, 1000, 1500, fs/2]; % Frekvenser i Hz
gains_dB = [0, -1, -3, -10, -40];         % Gain i dB (krav)

% Konverter gain fra dB til lineær skala
gains_linear = 10.^(gains_dB / 20);

% Normér frekvenserne til [0, 1] (Nyquist-normalisering)
f_norm = frequencies / (fs / 2);

% Design impulssvaret med `fir2`
h = fir2(N-1, f_norm, gains_linear);

% Beregn frekvensrespons fra 0 til 2*pi (hele området)
[H, omega] = freqz(h, 1, 1024, 'whole', fs);
H_dB = 20 * log10(abs(H));

% Plot impulsresponsen
figure(8);
subplot(3, 2, 1);
stem(0:N-1, h, 'filled');
xlabel('n'); ylabel('h[n]');
title('Impulsrespons for FIR-filter');
grid on;

% Plot amplituderespons (dB skala langs 0 til 2*pi)
figure(8);
subplot(3, 2, 2);
plot(omega * 2 * pi / fs, H_dB, 'LineWidth', 1.5);
xlabel('\omega (rad/sample)'); ylabel('|H(e^{j\omega})| (dB)');
title('Amplituderespons (dB skala, 0 til 2\pi)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
ylim([-40 5]);
xlim([0 2*pi]);
grid on;

% Parametre
N = 5;                  % Filterlængde
fs = 8000;              % Samplefrekvens i Hz
frequencies = [0, 750, 1000, 1500, fs/2]; % Frekvenser i Hz
gains_dB = [0, -1, -3, -10, -40];         % Gain i dB (krav)

% Konverter gain fra dB til lineær skala
gains_linear = 10.^(gains_dB / 20);

% Normér frekvenserne til [0, 1] (Nyquist-normalisering)
f_norm = frequencies / (fs / 2);

% Design impulssvaret med `fir2`
h = fir2(N-1, f_norm, gains_linear);

% Beregn frekvensrespons fra 0 til 2*pi (hele området)
[H, omega] = freqz(h, 1, 1024, 'whole', fs);
H_dB = 20 * log10(abs(H));

% Plot impulsresponsen
figure(8);
subplot(3, 2, 3);
stem(0:N-1, h, 'filled');
xlabel('n'); ylabel('h[n]');
title('Impulsrespons for FIR-filter');
grid on;

% Plot amplituderespons (dB skala langs 0 til 2*pi)
figure(8);
subplot(3, 2, 4);
plot(omega * 2 * pi / fs, H_dB, 'LineWidth', 1.5);
xlabel('\omega (rad/sample)'); ylabel('|H(e^{j\omega})| (dB)');
title('Amplituderespons (dB skala, 0 til 2\pi)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
ylim([-40 5]);
xlim([0 2*pi]);
grid on;


% Parametre
N = 7;                  % Filterlængde
fs = 8000;              % Samplefrekvens i Hz
frequencies = [0, 750, 1000, 1500, fs/2]; % Frekvenser i Hz
gains_dB = [0, -1, -3, -10, -40];         % Gain i dB (krav)

% Konverter gain fra dB til lineær skala
gains_linear = 10.^(gains_dB / 20);

% Normér frekvenserne til [0, 1] (Nyquist-normalisering)
f_norm = frequencies / (fs / 2);

% Design impulssvaret med `fir2`
h = fir2(N-1, f_norm, gains_linear);

% Beregn frekvensrespons fra 0 til 2*pi (hele området)
[H, omega] = freqz(h, 1, 1024, 'whole', fs);
H_dB = 20 * log10(abs(H));

% Plot impulsresponsen
figure(8);
subplot(3, 2, 5);
stem(0:N-1, h, 'filled');
xlabel('n'); ylabel('h[n]');
title('Impulsrespons for FIR-filter');
grid on;

% Plot amplituderespons (dB skala langs 0 til 2*pi)
figure(8);
subplot(3, 2, 6);
plot(omega * 2 * pi / fs, H_dB, 'LineWidth', 1.5);
xlabel('\omega (rad/sample)'); ylabel('|H(e^{j\omega})| (dB)');
title('Amplituderespons (dB skala, 0 til 2\pi)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
ylim([-40 5]);
xlim([0 2*pi]);
grid on;

%Plot en hel masse amplituderesponse:
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

% Beregn frekvensrespons fra 0 til 2*pi (hele området)
[H, omega] = freqz(h, 1, 1024, 'whole', fs);
H_dB = 20 * log10(abs(H));

% Plot amplituderespons (dB skala langs 0 til 2*pi)
figure(9);
subplot(4, 4, 1);
plot(omega * 2 * pi / fs, H_dB, 'LineWidth', 1.5);
xlabel('\omega (rad/sample)'); ylabel('|H(e^{j\omega})| (dB)');
title('Amplituderespons (dB skala, 0 til 2\pi)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
ylim([-40 5]);
xlim([0 2*pi]);
grid on;

N=11;                 % Filterlængde
fs = 8000;              % Samplefrekvens i Hz
frequencies = [0, 750, 1000, 1500, fs/2]; % Frekvenser i Hz
gains_dB = [0, -1, -3, -10, -40];         % Gain i dB (krav)

% Konverter gain fra dB til lineær skala
gains_linear = 10.^(gains_dB / 20);

% Normér frekvenserne til [0, 1] (Nyquist-normalisering)
f_norm = frequencies / (fs / 2);

% Design impulssvaret med `fir2`
h = fir2(N-1, f_norm, gains_linear);

% Beregn frekvensrespons fra 0 til 2*pi (hele området)
[H, omega] = freqz(h, 1, 1024, 'whole', fs);
H_dB = 20 * log10(abs(H));

% Plot amplituderespons (dB skala langs 0 til 2*pi)
figure(9);
subplot(4, 4, 2);
plot(omega * 2 * pi / fs, H_dB, 'LineWidth', 1.5);
xlabel('\omega (rad/sample)'); ylabel('|H(e^{j\omega})| (dB)');
title('Amplituderespons (dB skala, 0 til 2\pi)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
ylim([-40 5]);
xlim([0 2*pi]);
grid on;

N=13;                 % Filterlængde
fs = 8000;              % Samplefrekvens i Hz
frequencies = [0, 750, 1000, 1500, fs/2]; % Frekvenser i Hz
gains_dB = [0, -1, -3, -10, -40];         % Gain i dB (krav)

% Konverter gain fra dB til lineær skala
gains_linear = 10.^(gains_dB / 20);

% Normér frekvenserne til [0, 1] (Nyquist-normalisering)
f_norm = frequencies / (fs / 2);

% Design impulssvaret med `fir2`
h = fir2(N-1, f_norm, gains_linear);

% Beregn frekvensrespons fra 0 til 2*pi (hele området)
[H, omega] = freqz(h, 1, 1024, 'whole', fs);
H_dB = 20 * log10(abs(H));

% Plot amplituderespons (dB skala langs 0 til 2*pi)
figure(9);
subplot(4, 4, 3);
plot(omega * 2 * pi / fs, H_dB, 'LineWidth', 1.5);
xlabel('\omega (rad/sample)'); ylabel('|H(e^{j\omega})| (dB)');
title('Amplituderespons (dB skala, 0 til 2\pi)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
ylim([-40 5]);
xlim([0 2*pi]);
grid on;

N=15;                 % Filterlængde
fs = 8000;              % Samplefrekvens i Hz
frequencies = [0, 750, 1000, 1500, fs/2]; % Frekvenser i Hz
gains_dB = [0, -1, -3, -10, -40];         % Gain i dB (krav)

% Konverter gain fra dB til lineær skala
gains_linear = 10.^(gains_dB / 20);

% Normér frekvenserne til [0, 1] (Nyquist-normalisering)
f_norm = frequencies / (fs / 2);

% Design impulssvaret med `fir2`
h = fir2(N-1, f_norm, gains_linear);

% Beregn frekvensrespons fra 0 til 2*pi (hele området)
[H, omega] = freqz(h, 1, 1024, 'whole', fs);
H_dB = 20 * log10(abs(H));
% Plot amplituderespons (dB skala langs 0 til 2*pi)
figure(9);
subplot(4, 4, 4);
plot(omega * 2 * pi / fs, H_dB, 'LineWidth', 1.5);
xlabel('\omega (rad/sample)'); ylabel('|H(e^{j\omega})| (dB)');
title('Amplituderespons (dB skala, 0 til 2\pi)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
ylim([-40 5]);
xlim([0 2*pi]);
grid on;

N=17;                 % Filterlængde
fs = 8000;              % Samplefrekvens i Hz
frequencies = [0, 750, 1000, 1500, fs/2]; % Frekvenser i Hz
gains_dB = [0, -1, -3, -10, -40];         % Gain i dB (krav)

% Konverter gain fra dB til lineær skala
gains_linear = 10.^(gains_dB / 20);

% Normér frekvenserne til [0, 1] (Nyquist-normalisering)
f_norm = frequencies / (fs / 2);

% Design impulssvaret med `fir2`
h = fir2(N-1, f_norm, gains_linear);

% Beregn frekvensrespons fra 0 til 2*pi (hele området)
[H, omega] = freqz(h, 1, 1024, 'whole', fs);
H_dB = 20 * log10(abs(H));
% Plot amplituderespons (dB skala langs 0 til 2*pi)
figure(9);
subplot(4, 4, 5);
plot(omega * 2 * pi / fs, H_dB, 'LineWidth', 1.5);
xlabel('\omega (rad/sample)'); ylabel('|H(e^{j\omega})| (dB)');
title('Amplituderespons (dB skala, 0 til 2\pi)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
ylim([-40 5]);
xlim([0 2*pi]);
grid on;

N=19;                 % Filterlængde
fs = 8000;              % Samplefrekvens i Hz
frequencies = [0, 750, 1000, 1500, fs/2]; % Frekvenser i Hz
gains_dB = [0, -1, -3, -10, -40];         % Gain i dB (krav)

% Konverter gain fra dB til lineær skala
gains_linear = 10.^(gains_dB / 20);

% Normér frekvenserne til [0, 1] (Nyquist-normalisering)
f_norm = frequencies / (fs / 2);

% Design impulssvaret med `fir2`
h = fir2(N-1, f_norm, gains_linear);

% Beregn frekvensrespons fra 0 til 2*pi (hele området)
[H, omega] = freqz(h, 1, 1024, 'whole', fs);
H_dB = 20 * log10(abs(H));
% Plot amplituderespons (dB skala langs 0 til 2*pi)
figure(9);
subplot(4, 4, 6);
plot(omega * 2 * pi / fs, H_dB, 'LineWidth', 1.5);
xlabel('\omega (rad/sample)'); ylabel('|H(e^{j\omega})| (dB)');
title('Amplituderespons (dB skala, 0 til 2\pi)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
ylim([-40 5]);
xlim([0 2*pi]);
grid on;

N=21;                 % Filterlængde
fs = 8000;              % Samplefrekvens i Hz
frequencies = [0, 750, 1000, 1500, fs/2]; % Frekvenser i Hz
gains_dB = [0, -1, -3, -10, -40];         % Gain i dB (krav)

% Konverter gain fra dB til lineær skala
gains_linear = 10.^(gains_dB / 20);

% Normér frekvenserne til [0, 1] (Nyquist-normalisering)
f_norm = frequencies / (fs / 2);

% Design impulssvaret med `fir2`
h = fir2(N-1, f_norm, gains_linear);

% Beregn frekvensrespons fra 0 til 2*pi (hele området)
[H, omega] = freqz(h, 1, 1024, 'whole', fs);
H_dB = 20 * log10(abs(H));
% Plot amplituderespons (dB skala langs 0 til 2*pi)
figure(9);
subplot(4, 4, 7);
plot(omega * 2 * pi / fs, H_dB, 'LineWidth', 1.5);
xlabel('\omega (rad/sample)'); ylabel('|H(e^{j\omega})| (dB)');
title('Amplituderespons (dB skala, 0 til 2\pi)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
ylim([-40 5]);
xlim([0 2*pi]);
grid on;

N=23;                 % Filterlængde
fs = 8000;              % Samplefrekvens i Hz
frequencies = [0, 750, 1000, 1500, fs/2]; % Frekvenser i Hz
gains_dB = [0, -1, -3, -10, -40];         % Gain i dB (krav)

% Konverter gain fra dB til lineær skala
gains_linear = 10.^(gains_dB / 20);

% Normér frekvenserne til [0, 1] (Nyquist-normalisering)
f_norm = frequencies / (fs / 2);

% Design impulssvaret med `fir2`
h = fir2(N-1, f_norm, gains_linear);

% Beregn frekvensrespons fra 0 til 2*pi (hele området)
[H, omega] = freqz(h, 1, 1024, 'whole', fs);
H_dB = 20 * log10(abs(H));
% Plot amplituderespons (dB skala langs 0 til 2*pi)
figure(9);
subplot(4, 4, 8);
plot(omega * 2 * pi / fs, H_dB, 'LineWidth', 1.5);
xlabel('\omega (rad/sample)'); ylabel('|H(e^{j\omega})| (dB)');
title('Amplituderespons (dB skala, 0 til 2\pi)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
ylim([-40 5]);
xlim([0 2*pi]);
grid on;

N=25;                 % Filterlængde
fs = 8000;              % Samplefrekvens i Hz
frequencies = [0, 750, 1000, 1500, fs/2]; % Frekvenser i Hz
gains_dB = [0, -1, -3, -10, -40];         % Gain i dB (krav)

% Konverter gain fra dB til lineær skala
gains_linear = 10.^(gains_dB / 20);

% Normér frekvenserne til [0, 1] (Nyquist-normalisering)
f_norm = frequencies / (fs / 2);

% Design impulssvaret med `fir2`
h = fir2(N-1, f_norm, gains_linear);

% Beregn frekvensrespons fra 0 til 2*pi (hele området)
[H, omega] = freqz(h, 1, 1024, 'whole', fs);
H_dB = 20 * log10(abs(H));
% Plot amplituderespons (dB skala langs 0 til 2*pi)
figure(9);
subplot(4, 4, 9);
plot(omega * 2 * pi / fs, H_dB, 'LineWidth', 1.5);
xlabel('\omega (rad/sample)'); ylabel('|H(e^{j\omega})| (dB)');
title('Amplituderespons (dB skala, 0 til 2\pi)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
ylim([-40 5]);
xlim([0 2*pi]);
grid on;

N=27;                 % Filterlængde
fs = 8000;              % Samplefrekvens i Hz
frequencies = [0, 750, 1000, 1500, fs/2]; % Frekvenser i Hz
gains_dB = [0, -1, -3, -10, -40];         % Gain i dB (krav)

% Konverter gain fra dB til lineær skala
gains_linear = 10.^(gains_dB / 20);

% Normér frekvenserne til [0, 1] (Nyquist-normalisering)
f_norm = frequencies / (fs / 2);

% Design impulssvaret med `fir2`
h = fir2(N-1, f_norm, gains_linear);

% Beregn frekvensrespons fra 0 til 2*pi (hele området)
[H, omega] = freqz(h, 1, 1024, 'whole', fs);
H_dB = 20 * log10(abs(H));
% Plot amplituderespons (dB skala langs 0 til 2*pi)
figure(9);
subplot(4, 4, 10);
plot(omega * 2 * pi / fs, H_dB, 'LineWidth', 1.5);
xlabel('\omega (rad/sample)'); ylabel('|H(e^{j\omega})| (dB)');
title('Amplituderespons (dB skala, 0 til 2\pi)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
ylim([-40 5]);
xlim([0 2*pi]);
grid on;

N=29;                 % Filterlængde
fs = 8000;              % Samplefrekvens i Hz
frequencies = [0, 750, 1000, 1500, fs/2]; % Frekvenser i Hz
gains_dB = [0, -1, -3, -10, -40];         % Gain i dB (krav)

% Konverter gain fra dB til lineær skala
gains_linear = 10.^(gains_dB / 20);

% Normér frekvenserne til [0, 1] (Nyquist-normalisering)
f_norm = frequencies / (fs / 2);

% Design impulssvaret med `fir2`
h = fir2(N-1, f_norm, gains_linear);

% Beregn frekvensrespons fra 0 til 2*pi (hele området)
[H, omega] = freqz(h, 1, 1024, 'whole', fs);
H_dB = 20 * log10(abs(H));
% Plot amplituderespons (dB skala langs 0 til 2*pi)
figure(9);
subplot(4, 4, 11);
plot(omega * 2 * pi / fs, H_dB, 'LineWidth', 1.5);
xlabel('\omega (rad/sample)'); ylabel('|H(e^{j\omega})| (dB)');
title('Amplituderespons (dB skala, 0 til 2\pi)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
ylim([-40 5]);
xlim([0 2*pi]);
grid on;

N=29;                 % Filterlængde
fs = 8000;              % Samplefrekvens i Hz
frequencies = [0, 750, 1000, 1500, fs/2]; % Frekvenser i Hz
gains_dB = [0, -1, -3, -10, -40];         % Gain i dB (krav)

% Konverter gain fra dB til lineær skala
gains_linear = 10.^(gains_dB / 20);

% Normér frekvenserne til [0, 1] (Nyquist-normalisering)
f_norm = frequencies / (fs / 2);

% Design impulssvaret med `fir2`
h = fir2(N-1, f_norm, gains_linear);

% Beregn frekvensrespons fra 0 til 2*pi (hele området)
[H, omega] = freqz(h, 1, 1024, 'whole', fs);
H_dB = 20 * log10(abs(H));
% Plot amplituderespons (dB skala langs 0 til 2*pi)
figure(9);
subplot(4, 4, 12);
plot(omega * 2 * pi / fs, H_dB, 'LineWidth', 1.5);
xlabel('\omega (rad/sample)'); ylabel('|H(e^{j\omega})| (dB)');
title('Amplituderespons (dB skala, 0 til 2\pi)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
ylim([-40 5]);
xlim([0 2*pi]);
grid on;

N=29;                 % Filterlængde
fs = 8000;              % Samplefrekvens i Hz
frequencies = [0, 750, 1000, 1500, fs/2]; % Frekvenser i Hz
gains_dB = [0, -1, -3, -10, -40];         % Gain i dB (krav)

% Konverter gain fra dB til lineær skala
gains_linear = 10.^(gains_dB / 20);

% Normér frekvenserne til [0, 1] (Nyquist-normalisering)
f_norm = frequencies / (fs / 2);

% Design impulssvaret med `fir2`
h = fir2(N-1, f_norm, gains_linear);

% Beregn frekvensrespons fra 0 til 2*pi (hele området)
[H, omega] = freqz(h, 1, 1024, 'whole', fs);
H_dB = 20 * log10(abs(H));
% Plot amplituderespons (dB skala langs 0 til 2*pi)
figure(9);
subplot(4, 4, 13);
plot(omega * 2 * pi / fs, H_dB, 'LineWidth', 1.5);
xlabel('\omega (rad/sample)'); ylabel('|H(e^{j\omega})| (dB)');
title('Amplituderespons (dB skala, 0 til 2\pi)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
ylim([-40 5]);
xlim([0 2*pi]);
grid on;

N=31;                 % Filterlængde
fs = 8000;              % Samplefrekvens i Hz
frequencies = [0, 750, 1000, 1500, fs/2]; % Frekvenser i Hz
gains_dB = [0, -1, -3, -10, -40];         % Gain i dB (krav)

% Konverter gain fra dB til lineær skala
gains_linear = 10.^(gains_dB / 20);

% Normér frekvenserne til [0, 1] (Nyquist-normalisering)
f_norm = frequencies / (fs / 2);

% Design impulssvaret med `fir2`
h = fir2(N-1, f_norm, gains_linear);

% Beregn frekvensrespons fra 0 til 2*pi (hele området)
[H, omega] = freqz(h, 1, 1024, 'whole', fs);
H_dB = 20 * log10(abs(H));
% Plot amplituderespons (dB skala langs 0 til 2*pi)
figure(9);
subplot(4, 4, 14);
plot(omega * 2 * pi / fs, H_dB, 'LineWidth', 1.5);
xlabel('\omega (rad/sample)'); ylabel('|H(e^{j\omega})| (dB)');
title('Amplituderespons (dB skala, 0 til 2\pi)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
ylim([-40 5]);
xlim([0 2*pi]);
grid on;

N=33;                 % Filterlængde
fs = 8000;              % Samplefrekvens i Hz
frequencies = [0, 750, 1000, 1500, fs/2]; % Frekvenser i Hz
gains_dB = [0, -1, -3, -10, -40];         % Gain i dB (krav)

% Konverter gain fra dB til lineær skala
gains_linear = 10.^(gains_dB / 20);

% Normér frekvenserne til [0, 1] (Nyquist-normalisering)
f_norm = frequencies / (fs / 2);

% Design impulssvaret med `fir2`
h = fir2(N-1, f_norm, gains_linear);

% Beregn frekvensrespons fra 0 til 2*pi (hele området)
[H, omega] = freqz(h, 1, 1024, 'whole', fs);
H_dB = 20 * log10(abs(H));
% Plot amplituderespons (dB skala langs 0 til 2*pi)
figure(9);
subplot(4, 4, 15);
plot(omega * 2 * pi / fs, H_dB, 'LineWidth', 1.5);
xlabel('\omega (rad/sample)'); ylabel('|H(e^{j\omega})| (dB)');
title('Amplituderespons (dB skala, 0 til 2\pi)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
ylim([-40 5]);
xlim([0 2*pi]);
grid on;

N=35;                 % Filterlængde
fs = 8000;              % Samplefrekvens i Hz
frequencies = [0, 750, 1000, 1500, fs/2]; % Frekvenser i Hz
gains_dB = [0, -1, -3, -10, -40];         % Gain i dB (krav)

% Konverter gain fra dB til lineær skala
gains_linear = 10.^(gains_dB / 20);

% Normér frekvenserne til [0, 1] (Nyquist-normalisering)
f_norm = frequencies / (fs / 2);

% Design impulssvaret med `fir2`
h = fir2(N-1, f_norm, gains_linear);

% Beregn frekvensrespons fra 0 til 2*pi (hele området)
[H, omega] = freqz(h, 1, 1024, 'whole', fs);
H_dB = 20 * log10(abs(H));
% Plot amplituderespons (dB skala langs 0 til 2*pi)
figure(9);
subplot(4, 4, 16);
plot(omega * 2 * pi / fs, H_dB, 'LineWidth', 1.5);
xlabel('\omega (rad/sample)'); ylabel('|H(e^{j\omega})| (dB)');
title('Amplituderespons (dB skala, 0 til 2\pi)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
ylim([-40 5]);
xlim([0 2*pi]);
grid on;