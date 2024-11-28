%Opgave 2 - Beregn steprespons vha foldningssummen - sakset fra peters
%løsning
%y[n]=SUM[m=0..infinite] 0.125e^{-0.125m} x[n-m]

%Stepresponsen beregnes vha to nestede loops, hvor det ydre loop opdaterer
%samplenr mens den indre beregner produktsummen

clear;
for n=0:49, %n er samplenummeret
    sum=0;  %Sum benyttes til at holde produktsummen

    %Den indre løkke beregner selve produktsummen og baseres på 100 led
    for m=0:99,
        %bestem værdiern af impulsresponsen til tidspunktet m
        h=0.125*exp(-0.125*m);
        %bestem værdien af stepfunktionen u[n-m]
        if (n-m)>=0
            u=1;
        else 
            u=0;
        end;
        sum = sum+(h*u);
    end;
%Opdater output-sekvensen
y(n+1)=sum;
end;
%til sidst plottes outputtet:
figure(1);
x(1:50)=0:49;
plot(x,y);
title('Step Response');
xlabel('Sample n');
ylabel('Output y[n]');

% Opgave 4 - virker efter absurd meget besvær
% Anvend den styrende differens-ligning til beregning at et filtreret
% output givet et inout-signal, som er overlejret med støj
clear;
signal_noise=readtable("C:\Users\Christian Lykke\Documents\Skole\Aalborg Universitet\ESD5\Digital Signal Processing\Opgaveregning\Opgaver - Egne Løsninger\Signal_with_noise.xlsx"); % 1000 samples at stojbefængt signal %
signal_nonoise=readtable("C:\Users\Christian Lykke\Documents\Skole\Aalborg Universitet\ESD5\Digital Signal Processing\Opgaveregning\Opgaver - Egne Løsninger\Signal_without_noise.xlsx"); % 1000 samples at støjtrit signal %

signalNoise=table2array(signal_noise(1:end, 1));
signalNoNoise=table2array(signal_nonoise(1:end, 1));

y(1) = 0.125*signalNoise(1);
for n=2:1000,
    y(n)=0.8825*y(n-1)+0.125*signalNoise(n);
end;
figure(2);
subplot(3,1,1);
plot(signalNoise);
title('Det støjbefængte signal')
subplot(3,1,2);
plot(signalNoNoise);
title('Det ønskede signal')
subplot(3,1,3);
plot(y);
title('Det filtrerede signal')

%opgave 5
% Plot filterets amplituderespons
% Først genereres en frekvens-akse med 100 punkter i intervallet [0;PI]
for i=0:99,
omega(i+1) = (pi*i)/100;
end;
% Dernæst beregnes amplitude-værdierne for hvert enkelt frekvens-værdi
amplitude = zeros(100,1);
for i=1:100,
    naevner = sqrt((cos(omega(i))-0.8825)^2+(sin(omega(i)))^2);
    amplitude(i) = 0.125/naevner;
end;
figure(3);
plot(omega, amplitude);
xlabel('Frequency');
ylabel('Amplitude');
title('Amplitude Response');