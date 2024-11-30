% Parametre til frekvenssamplingsfilter
N = 35;                  % Filterlængde
fs = 8000;              % Samplefrekvens i Hz
frequencies = [0, 750, 1000, 1500, fs/2]; % Frekvenser i Hz
gains_dB = [0, -1, -3, -10, -80];         % Gain i dB (krav)

% Konverter gain fra dB til lineær skala
gains_linear = 10.^(gains_dB / 20);

% Normér frekvenserne til [0, 1] (Nyquist-normalisering)
f_norm = frequencies / (fs / 2);

% Design impulsresponsen med `fir2`
h = fir2(N-1, f_norm, gains_linear);

% Beregn frekvensrespons fra 0 til 2*pi (hele området)
[H, omega] = freqz(h, 1, 1024, 'whole', fs);

% Beregn frekvensrespons i dB
H_dB = 20 * log10(abs(H));

% Plot impulsresponsen
figure(8);
subplot(3, 1, 1);
stem(0:N-1, h, 'filled');
xlabel('n'); ylabel('h[n]');
title('Impulsrespons for FIR-filter (Frekvenssampling)');
grid on;

% Plot amplituderespons (dB skala langs 0 til 2*pi)
figure(8);
subplot(3, 1, 2);
plot(omega * 2 * pi / fs, H_dB, 'LineWidth', 1.5);
hold on;
xline(f_norm(1,2)*pi, 'r--', 'DisplayName', '-1dB Ønsket');
yline(-1,'r--', 'DisplayName', '-1dB Ønsket');
xline(f_norm(1,3)*pi, 'g--', 'DisplayName', '3dB Knæk');
yline(-3,'g--', 'DisplayName', '3dB Knæk');
xline(f_norm(1,4)*pi, 'b--', 'DisplayName', '-10dB Max');
yline(-10,'b--', 'DisplayName', '-10dB Max');
hold off;
xlabel('\omega (rad/sample)'); ylabel('|H(e^{j\omega})| (dB)');
title('Amplituderespons (dB skala, 0 til 2\pi) (Frekvenssampling)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
ylim([-40 5]);
xlim([0 2*pi]);
grid on;

% Plot faserespons (0 til 2*pi)
figure(8);
subplot(3, 1, 3);
plot(omega * 2 * pi / fs, angle(H), 'LineWidth', 1.5);
xlabel('\omega (rad/sample)'); ylabel('Fase (radianer)');
title('Faserespons (0 til 2\pi) (Frekvenssampling)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
xlim([0 2*pi]);
grid on;


%Lowpassing it all
% Design FIR med specificeret filter og type (rektangulær)
h_rec = fir1(N - 1, f_norm(1,3), 'LOW', rectwin(N));

% Frekvensrespons af filteret
[H_rec, omega] = freqz(h_rec, 1, 1024, 'whole', fs);

% Beregn frekvensrespons i dB
H_dB_rec = 20 * log10(abs(H_rec));

% Plot impulsresponsen
figure(9);
subplot(3, 1, 1);
stem(0:N-1, h_rec, 'filled');
xlabel('n'); ylabel('h[n]');
title('Impulsrespons for FIR-filter (Rectangular Window)');
grid on;

% Plot amplituderespons (dB skala langs 0 til 2*pi)
figure(9);
subplot(3, 1, 2);
plot(omega * 2 * pi / fs, H_dB_rec, 'LineWidth', 1.5);
hold on;
xline(f_norm(1,2)*pi, 'r--', 'DisplayName', '-1dB Ønsket');
yline(-1,'r--', 'DisplayName', '-1dB Ønsket');
xline(f_norm(1,3)*pi, 'g--', 'DisplayName', '3dB Knæk');
yline(-3,'g--', 'DisplayName', '3dB Knæk');
xline(f_norm(1,4)*pi, 'b--', 'DisplayName', '-10dB Max');
yline(-10,'b--', 'DisplayName', '-10dB Max');
hold off;
xlabel('\omega (rad/sample)'); ylabel('|H(e^{j\omega})| (dB)');
title('Amplituderespons (dB skala, 0 til 2\pi) (Rectangular Window)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
%ylim([-40 5]);
xlim([0 2*pi]);
grid on;

% Plot faserespons (0 til 2*pi)
figure(9);
subplot(3, 1, 3);
plot(omega * 2 * pi / fs, angle(H_rec), 'LineWidth', 1.5);
xlabel('\omega (rad/sample)'); ylabel('Fase (radianer)');
title('Faserespons (0 til 2\pi) (Rectangular Window)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
xlim([0 2*pi]);
grid on;


% Design FIR med specificeret filter og type (Hamming)
h_hamming = fir1(N - 1, f_norm(1,3), 'LOW', hamming(N));

% Frekvensrespons af filteret
[H_hamming, omega] = freqz(h_hamming, 1, 1024, 'whole', fs);

% Beregn frekvensrespons i dB
H_dB_hamming = 20 * log10(abs(H_hamming));

% Plot impulsresponsen
figure(10);
subplot(3, 1, 1);
stem(0:N-1, h_hamming, 'filled');
xlabel('n'); ylabel('h[n]');
title('Impulsrespons for FIR-filter (Hamming Window)');
grid on;

% Plot amplituderespons (dB skala langs 0 til 2*pi)
figure(10);
subplot(3, 1, 2);
plot(omega * 2 * pi / fs, H_dB_hamming, 'LineWidth', 1.5);
hold on;
xline(f_norm(1,2)*pi, 'r--', 'DisplayName', '-1dB Ønsket');
yline(-1,'r--', 'DisplayName', '-1dB Ønsket');
xline(f_norm(1,3)*pi, 'g--', 'DisplayName', '3dB Knæk');
yline(-3,'g--', 'DisplayName', '3dB Knæk');
xline(f_norm(1,4)*pi, 'b--', 'DisplayName', '-10dB Max');
yline(-10,'b--', 'DisplayName', '-10dB Max');
hold off;
xlabel('\omega (rad/sample)'); ylabel('|H(e^{j\omega})| (dB)');
title('Amplituderespons (dB skala, 0 til 2\pi) (Hamming Window)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
%ylim([-40 5]);
xlim([0 2*pi]);
grid on;

% Plot faserespons (0 til 2*pi)
figure(10);
subplot(3, 1, 3);
plot(omega * 2 * pi / fs, angle(H_hamming), 'LineWidth', 1.5);
xlabel('\omega (rad/sample)'); ylabel('Fase (radianer)');
title('Faserespons (0 til 2\pi) (Hamming Window)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
xlim([0 2*pi]);
grid on;


% Design FIR med specificeret filter og type (Hanning)
h_hanning = fir1(N - 1, f_norm(1,3), 'LOW', hanning(N));

% Frekvensrespons af filteret
[H_hanning, omega] = freqz(h_hanning, 1, 1024, 'whole', fs);

% Beregn frekvensrespons i dB
H_dB_hanning = 20 * log10(abs(H_hanning));

% Plot impulsresponsen
figure(11);
subplot(3, 1, 1);
stem(0:N-1, h_hanning, 'filled');
xlabel('n'); ylabel('h[n]');
title('Impulsrespons for FIR-filter (Hanning Window)');
grid on;

% Plot amplituderespons (dB skala langs 0 til 2*pi)
figure(11);
subplot(3, 1, 2);
plot(omega * 2 * pi / fs, H_dB_hanning, 'LineWidth', 1.5);
hold on;
xline(f_norm(1,2)*pi, 'r--', 'DisplayName', '-1dB Ønsket');
yline(-1,'r--', 'DisplayName', '-1dB Ønsket');
xline(f_norm(1,3)*pi, 'g--', 'DisplayName', '3dB Knæk');
yline(-3,'g--', 'DisplayName', '3dB Knæk');
xline(f_norm(1,4)*pi, 'b--', 'DisplayName', '-10dB Max');
yline(-10,'b--', 'DisplayName', '-10dB Max');
hold off;
xlabel('\omega (rad/sample)'); ylabel('|H(e^{j\omega})| (dB)');
title('Amplituderespons (dB skala, 0 til 2\pi) (Hanning Window)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
%ylim([-40 5]);
xlim([0 2*pi]);
grid on;

% Plot faserespons (0 til 2*pi)
figure(11);
subplot(3, 1, 3);
plot(omega * 2 * pi / fs, angle(H_hanning), 'LineWidth', 1.5);
xlabel('\omega (rad/sample)'); ylabel('Fase (radianer)');
title('Faserespons (0 til 2\pi) (Hanning Window)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
xlim([0 2*pi]);
grid on;


% Design FIR med specificeret filter og type (Blackman)
h_blackman = fir1(N - 1, f_norm(1,3), 'LOW', blackman(N));

% Frekvensrespons af filteret
[H_blackman, omega] = freqz(h_blackman, 1, 1024, 'whole', fs);

% Beregn frekvensrespons i dB
H_dB_blackman = 20 * log10(abs(H_blackman));

% Plot impulsresponsen
figure(12);
subplot(3, 1, 1);
stem(0:N-1, h_blackman, 'filled');
xlabel('n'); ylabel('h[n]');
title('Impulsrespons for FIR-filter (Blackman Window)');
grid on;

% Plot amplituderespons (dB skala langs 0 til 2*pi)
figure(12);
subplot(3, 1, 2);
plot(omega * 2 * pi / fs, H_dB_blackman, 'LineWidth', 1.5);
hold on;
xline(f_norm(1,2)*pi, 'r--', 'DisplayName', '-1dB Ønsket');
yline(-1,'r--', 'DisplayName', '-1dB Ønsket');
xline(f_norm(1,3)*pi, 'g--', 'DisplayName', '3dB Knæk');
yline(-3,'g--', 'DisplayName', '3dB Knæk');
xline(f_norm(1,4)*pi, 'b--', 'DisplayName', '-10dB Max');
yline(-10,'b--', 'DisplayName', '-10dB Max');
hold off;
xlabel('\omega (rad/sample)'); ylabel('|H(e^{j\omega})| (dB)');
title('Amplituderespons (dB skala, 0 til 2\pi) (Blackman Window)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
%ylim([-40 5]);
xlim([0 2*pi]);
grid on;

% Plot faserespons (0 til 2*pi)
figure(12);
subplot(3, 1, 3);
plot(omega * 2 * pi / fs, angle(H_blackman), 'LineWidth', 1.5);
xlabel('\omega (rad/sample)'); ylabel('Fase (radianer)');
title('Faserespons (0 til 2\pi) (Blackman Window)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
xlim([0 2*pi]);
grid on;


% Design FIR med specificeret filter og type (kaiser)
h_kaiser = fir1(N - 1, f_norm(1,3), 'LOW', kaiser(N,2)); % betaværdien er en skala for tradeoff mellem main og side lobe

% Frekvensrespons af filteret
[H_kaiser, omega] = freqz(h_kaiser, 1, 1024, 'whole', fs);

% Beregn frekvensrespons i dB
H_dB_kaiser = 20 * log10(abs(H_kaiser));

% Plot impulsresponsen
figure(13);
subplot(3, 1, 1);
stem(0:N-1, h_kaiser, 'filled');
xlabel('n'); ylabel('h[n]');
title('Impulsrespons for FIR-filter (Kaiser Window)');
grid on;

% Plot amplituderespons (dB skala langs 0 til 2*pi)
figure(13);
subplot(3, 1, 2);
plot(omega * 2 * pi / fs, H_dB_kaiser, 'LineWidth', 1.5);
hold on;
xline(f_norm(1,2)*pi, 'r--', 'DisplayName', '-1dB Ønsket');
yline(-1,'r--', 'DisplayName', '-1dB Ønsket');
xline(f_norm(1,3)*pi, 'g--', 'DisplayName', '3dB Knæk');
yline(-3,'g--', 'DisplayName', '3dB Knæk');
xline(f_norm(1,4)*pi, 'b--', 'DisplayName', '-10dB Max');
yline(-10,'b--', 'DisplayName', '-10dB Max');
hold off;
xlabel('\omega (rad/sample)'); ylabel('|H(e^{j\omega})| (dB)');
title('Amplituderespons (dB skala, 0 til 2\pi) (Kaiser Window)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
%ylim([-40 5]);
xlim([0 2*pi]);
grid on;

% Plot faserespons (0 til 2*pi)
figure(13);
subplot(3, 1, 3);
plot(omega * 2 * pi / fs, angle(H_kaiser), 'LineWidth', 1.5);
xlabel('\omega (rad/sample)'); ylabel('Fase (radianer)');
title('Faserespons (0 til 2\pi) (Kaiser Window)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
xlim([0 2*pi]);
grid on;


%Plot af de tre vigtige punkter for alle filtre:
%Plot amplituderesponsen (frekvenssampling)
figure(14);
subplot(3, 2, 1);
plot(omega * 2 * pi / fs, H_dB, 'LineWidth', 1.5);
hold on;
xline(f_norm(1,2)*pi, 'r--', 'DisplayName', '-1dB Ønsket');
yline(-1,'r--', 'DisplayName', '-1dB Ønsket');
xline(f_norm(1,3)*pi, 'g--', 'DisplayName', '3dB Knæk');
yline(-3,'g--', 'DisplayName', '3dB Knæk');
xline(f_norm(1,4)*pi, 'b--', 'DisplayName', '-10dB Max');
yline(-10,'b--', 'DisplayName', '-10dB Max');
hold off;
xlabel('\omega (rad/sample)'); ylabel('|H(e^{j\omega})| (dB)');
title('Amplituderespons (dB skala, 0 til 0.5\pi) (Frekvenssampling)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
ylim([-40 5]);
xlim([0 0.5*pi]);
grid on;

% Plot amplituderespons (dB skala langs 0 til 2*pi)
figure(14);
subplot(3, 2, 2);
plot(omega * 2 * pi / fs, H_dB_rec, 'LineWidth', 1.5);
hold on;
xline(f_norm(1,2)*pi, 'r--', 'DisplayName', '-1dB Ønsket');
yline(-1,'r--', 'DisplayName', '-1dB Ønsket');
xline(f_norm(1,3)*pi, 'g--', 'DisplayName', '3dB Knæk');
yline(-3,'g--', 'DisplayName', '3dB Knæk');
xline(f_norm(1,4)*pi, 'b--', 'DisplayName', '-10dB Max');
yline(-10,'b--', 'DisplayName', '-10dB Max');
hold off;
xlabel('\omega (rad/sample)'); ylabel('|H(e^{j\omega})| (dB)');
title('Amplituderespons (dB skala, 0 til 0.5\pi) (Rectangular Window)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
%ylim([-40 5]);
xlim([0 0.5*pi]);
grid on;

% Plot amplituderespons (dB skala langs 0 til 2*pi)
figure(14);
subplot(3, 2, 3);
plot(omega * 2 * pi / fs, H_dB_hamming, 'LineWidth', 1.5);
hold on;
xline(f_norm(1,2)*pi, 'r--', 'DisplayName', '-1dB Ønsket');
yline(-1,'r--', 'DisplayName', '-1dB Ønsket');
xline(f_norm(1,3)*pi, 'g--', 'DisplayName', '3dB Knæk');
yline(-3,'g--', 'DisplayName', '3dB Knæk');
xline(f_norm(1,4)*pi, 'b--', 'DisplayName', '-10dB Max');
yline(-10,'b--', 'DisplayName', '-10dB Max');
hold off;
xlabel('\omega (rad/sample)'); ylabel('|H(e^{j\omega})| (dB)');
title('Amplituderespons (dB skala, 0 til 0.5\pi) (Hamming Window)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
%ylim([-40 5]);
xlim([0 0.5*pi]);
grid on;

% Plot amplituderespons (dB skala langs 0 til 2*pi)
figure(14);
subplot(3, 2, 4);
plot(omega * 2 * pi / fs, H_dB_hanning, 'LineWidth', 1.5);
hold on;
xline(f_norm(1,2)*pi, 'r--', 'DisplayName', '-1dB Ønsket');
yline(-1,'r--', 'DisplayName', '-1dB Ønsket');
xline(f_norm(1,3)*pi, 'g--', 'DisplayName', '3dB Knæk');
yline(-3,'g--', 'DisplayName', '3dB Knæk');
xline(f_norm(1,4)*pi, 'b--', 'DisplayName', '-10dB Max');
yline(-10,'b--', 'DisplayName', '-10dB Max');
hold off;
xlabel('\omega (rad/sample)'); ylabel('|H(e^{j\omega})| (dB)');
title('Amplituderespons (dB skala, 0 til 0.5\pi) (Hanning Window)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
%ylim([-40 5]);
xlim([0 0.5*pi]);
grid on;

% Plot amplituderespons (dB skala langs 0 til 2*pi)
figure(14);
subplot(3, 2, 5);
plot(omega * 2 * pi / fs, H_dB_blackman, 'LineWidth', 1.5);
hold on;
xline(f_norm(1,2)*pi, 'r--', 'DisplayName', '-1dB Ønsket');
yline(-1,'r--', 'DisplayName', '-1dB Ønsket');
xline(f_norm(1,3)*pi, 'g--', 'DisplayName', '3dB Knæk');
yline(-3,'g--', 'DisplayName', '3dB Knæk');
xline(f_norm(1,4)*pi, 'b--', 'DisplayName', '-10dB Max');
yline(-10,'b--', 'DisplayName', '-10dB Max');
hold off;
xlabel('\omega (rad/sample)'); ylabel('|H(e^{j\omega})| (dB)');
title('Amplituderespons (dB skala, 0 til 0.5\pi) (Blackman Window)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
%ylim([-40 5]);
xlim([0 0.5*pi]);
grid on;

%Plot amplituderespons (dB skala langs 0 til 2*pi)
figure(14);
subplot(3, 2, 6);
plot(omega * 2 * pi / fs, H_dB_kaiser, 'LineWidth', 1.5);
hold on;
xline(f_norm(1,2)*pi, 'r--', 'DisplayName', '-1dB Ønsket');
yline(-1,'r--', 'DisplayName', '-1dB Ønsket');
xline(f_norm(1,3)*pi, 'g--', 'DisplayName', '3dB Knæk');
yline(-3,'g--', 'DisplayName', '3dB Knæk');
xline(f_norm(1,4)*pi, 'b--', 'DisplayName', '-10dB Max');
yline(-10,'b--', 'DisplayName', '-10dB Max');
hold off;
xlabel('\omega (rad/sample)'); ylabel('|H(e^{j\omega})| (dB)');
title('Amplituderespons (dB skala, 0 til 0.5\pi) (Kaiser Window)');
xticks([0 0.25*pi 0.5*pi 0.75*pi pi 1.25*pi 1.5*pi 1.75*pi 2*pi])
xticklabels({'0','0.25\pi','0.5\pi','0.75\pi','\pi','1.25\pi','1.5\pi','1.75\pi','2\pi'})
%ylim([-40 5]);
xlim([0 0.5*pi]);
grid on;