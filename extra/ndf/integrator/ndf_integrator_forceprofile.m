function y = smrinc_integrator_forceprofile(x, inc, nrp)
% SMRINC_INTEGRATOR_FORCEPROFILE Compute force given the input and the
% parameters of the profile
    
    y = zeros(size(x));
    for idx = 1:length(x)
        cx = x(idx);
        if(cx < 1-nrp)
            y(idx) = -sin(pi/(1-nrp).*cx);
        elseif(cx > nrp)
            y(idx) = sin(pi/(1-nrp).*(cx-nrp));
        else
            y(idx) = -inc*sin(pi/(nrp-0.5).*(cx-0.5));
        end
    end
end