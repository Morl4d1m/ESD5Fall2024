N = 9;
fs = 18000; % Samplefrekvens i Hz
fc = 5000; % Cutoff-frekvens i Hz
wc = 2*pi*fc/fs; % Normaliseret cutoff-frekvens

% Definér ønsket frekvensrespons H_d[k]
k = 0:N-1;
omega_k = 2*pi*k/N;
H_d = double(omega_k <= wc);

% Beregn h[n] via IDFT
h = ifft(H_d, 'symmetric');

% Assume variables `h`, `N`, and `fs` are defined in the workspace
H_freq = fft(h, 512);            % Use a high number of points for smooth frequency plot
f = (0:511) * fs / 512;          % Frequency vector for plotting

% Compute the magnitude in dB and amplitude
h_freqDB = mag2db(abs(H_freq));  % dB scale
h_freq_amp = abs(H_freq);        % Amplitude scale

% Visualiser resultatet
figure(1);
stem(0:N-1, h);
xlabel('n'); ylabel('h[n]');
title('Impulssvar for FIR-filter');

    figure(2); % Open a new figure window
    plot(f, h_freqDB);
    title('Magnitude of Frequency Response in dB |H(f)|');
    xlabel('Frequency (Hz)');
    ylabel('|H(f)| in dB');
    xlim([0 fs/2]);  % Limit the x-axis to the Nyquist frequency (half the sampling rate)
    grid on;
    figure(3); % Open a new figure window
    plot(f, h_freq_amp);
    title('Magnitude of Frequency Response |H(f)| in Amplitude');
    xlabel('Frequency (Hz)');
    ylabel('|H(f)| in Amplitude');
    %xlim([0 fs/2]);  % Limit the x-axis to the Nyquist frequency (half the sampling rate)
    grid on;