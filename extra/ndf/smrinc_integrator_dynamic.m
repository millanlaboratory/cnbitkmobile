function y = smrinc_integrator_dynamic(x, yp, phi, chi, inc, nrp, dt)
  
    
    %% Normalization factor
    cx = 0:0.01:1;
    mf = max(abs(smrinc_integrator_forceprofile(cx, inc, nrp)));
    
    %% Free Force
    Ff = phi*smrinc_integrator_forceprofile(yp, inc, nrp)./mf;
    
    %% BCI Force
    bci_coeff = [-0.0000    6.4000    0.0000    0.4000    0.0000];
    Fb = (1-phi)*polyval(bci_coeff, x-0.5);
    
    % Original bci force
    y = yp + dt*chi*(Ff + Fb);

end