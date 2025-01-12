% Open-loop system G(s)
s = tf('s');
G = 10 / (s * (s + 1) * (s + 2));

% Bode plot of the open-loop system
figure(1);
bode(G);
%margin(G);
grid on;
title('Open-Loop Bode Plot');

% Step response of the open-loop system
figure(2);
step(feedback(G, 1));
title('Open-Loop Step Response');
xlim([1400 1500]);

% Phase lead compensator design
K_c = 1; % Gain to meet steady-state and dynamic demands
z = 1;    % Zero of the compensator
p = 5;   % Pole of the compensator (z < p)

D = K_c * (s + z) / (s + p);

% Compensated open-loop transfer function
G_comp = D * G;

% Bode plot of the compensated system
figure(3);
bode(G_comp); % Includes GM and PM analysis
grid on;
title('Compensated Open-Loop Bode Plot');

% Closed-loop transfer function
T = feedback(G_comp, 1);

% Step response
figure(4);
step(T);
grid on;
title('Closed-Loop Step Response');

% Phase lead compensator design
K_c = 1; % Gain to meet steady-state and dynamic demands
z = 1;    % Zero of the compensator
p = 9;   % Pole of the compensator (z < p)

D = K_c * (s + z) / (s + p);

% Compensated open-loop transfer function
G_comp = D * G;

% Bode plot of the compensated system
figure(5);
bode(G_comp); % Includes GM and PM analysis
grid on;
title('Compensated Open-Loop Bode Plot');

% Closed-loop transfer function
T = feedback(G_comp, 1);

% Step response
figure(6);
step(T);
grid on;
title('Closed-Loop Step Response');

% Addition of lag-compensator
z = 18/100000;    % Zero of the compensator
p = 1/100000;   % Pole of the compensator (z > p)

D_Lag = (s + z) / (s + p);

% Addition of Lag compensator to system
GLeadLagged = G_comp*D_Lag;

% Bode plot of the compensated system
figure(7);
bode(GLeadLagged); % Includes GM and PM analysis
grid on;
title('Lead and Lag Compensated Open-Loop Bode Plot');

% Closed-loop transfer function
TLag = feedback(GLeadLagged, 1);

% Step response
figure(8);
step(TLag);
grid on;
title('Closed-Loop Step Response with Lead and Lag');

% Pole-zero map for stability analysis
figure(9);
pzmap(TLag);
grid on;
title('Pole-Zero Map of the Closed-Loop System');

%Root locus of the system
figure(10);
rlocus(T);
grid on;
title('Root Locus of the System');