function [y, xchg] = smrinc_integrator_vema(x, xchg, yp, rho, gamma, dt)

% %     F = 1 - rho.*cos((2.*yp - 1).*pi.*(1 + abs(yp - 0.5)));
%     y = yp + dt.*gamma.*F.*(x-yp);
    
    
    vel = 1 - rho*cos(2*pi*(yp - 0.5)*(1 + abs(yp-0.5)));
    
    xchg = 0.2*(x-yp) + 0.8*xchg;
    bck = 1 + (1 + sign(xchg*(0.5-yp)));
    
    y = yp + dt*gamma*vel*abs(xchg)*(x-yp)*bck;
end