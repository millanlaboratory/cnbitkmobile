function y = ndf_integrator_ema(x, yp, alpha, rejection)

    if(x >= 1 - rejection && x <= rejection)
        x = yp;
    end
    
    y = alpha*yp + (1-alpha)*x;
    
end
