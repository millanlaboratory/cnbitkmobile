function y = smrinc_integrator_dynamic(x, yp, p, phi, chi, dt)
    
    %% Free Force
    free_coeff = p;
    cx = 0:0.01:1;
    mf = max(abs(polyval(free_coeff, cx)));
    Ff = phi*polyval(free_coeff, yp)./mf;

    %% BCI force
    % Polyfit coefficients for the following inputs:
    % x1 = [-0.5 -0.25 0 0.25 0.5];
    % y1 = [-1 -0.2 0 0.2 1];
    % coeff = [-0.0000    6.4000    0.0000    0.4000    0.0000];
    bci_coeff = [-0.0000    6.4000    0.0000    0.4000    0.0000];
    Fb = (1-phi)*polyval(bci_coeff, x-0.5);
    
    % Original bci force
    y = yp + dt*chi*(Ff + Fb);

end