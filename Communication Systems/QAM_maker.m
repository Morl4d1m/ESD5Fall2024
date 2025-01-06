M = 16;      % Modulation order
k = log2(M); % Number of bits per symbol
n = 30000;   % Number of symbols per frame
sps = 1;     % Number of samples per symbol (oversampling factor)
rng default  % Use default random number generator

stem(dataIn(1:40),'filled');
title('Random Bits');
xlabel('Bit Index');
ylabel('Binary Value');

dataSymbolsIn = bit2int(dataIn,k);

figure;                    % Create new figure window.
stem(dataSymbolsIn(1:10));
title('Random Symbols');
xlabel('Symbol Index');
ylabel('Integer Value');

dataMod = qammod(dataSymbolsIn,M,'bin'); % Binary-encoded
dataModG = qammod(dataSymbolsIn,M);      % Gray-encoded

EbNo = 10;
snr = convertSNR(EbNo,'ebno', ...
    samplespersymbol=sps, ...
    bitspersymbol=k);

receivedSignal = awgn(dataMod,snr,'measured');
receivedSignalG = awgn(dataModG,snr,'measured');

dataIn = randi([0 1],n*k,1); % Generate vector of binary data
sPlotFig = scatterplot(receivedSignal,1,0,'g.');
hold on
scatterplot(dataMod,1,0,'k*',sPlotFig)
