function y = smrinc_integrator_vema(x, yp, rho, gamma, dt)

    F = 1 - rho.*cos((2.*yp - 1).*pi.*(1 + abs(yp - 0.5)));
    y = yp + dt.*gamma.*F.*(x-yp);
end