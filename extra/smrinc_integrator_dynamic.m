function y = smrinc_integrator_dynamic(x, yp, p, phi, dt)
    
    cx = 0:0.01:1;
    mf = max(abs(polyval(p, cx)));


    Ff = phi*polyval(p, yp)./mf;

    % New bci force
    %Fb = (1-phi)*sin(pi*(x-yp)/4);
    
    % New new bci force
    x1 = [-0.5 -0.25 0 0.25 0.5];
    y1 = [-1 -0.2 0 0.2 1];
    %coeff = polyfit(x1, y1, 4);
    coeff = [-0.0000    6.4000    0.0000    0.4000    0.0000];  % to speed-up
    Fb = (1-phi)*polyval(coeff, x-0.5);
    
    % Original bci force
    % Fb = (1-phi)*polyval(p, x)./mf;
    y = yp + dt*(Ff + Fb);

end