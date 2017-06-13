function [coeff, support] = smrinc_integrator_forceprofile(inclim, nrpt, bias, degree)
% SMRINC_INTEGRATOR_FORCEPROFILE Get force profile
%   coeff = SMRINC_INTEGRATOR_FORCEPROFILE(inclim, nrpt) returns the force profile
%   for the given INC limit and no-return point. The profile is generated
%   keeping into account a linear force in between the INC limit
%   [1-inclimit inclimit] and a polynomial force in the rest of the space.
%   [1-nrpt nrpt] correspond to the no-return points (unstable equilibrium points).
%   By construction, three stable equilibrium points (attractors) are
%   provided: at 0, 0.5 and 1.
%
%   coeff = SMRINC_INTEGRATOR_FORCEPROFILE(inclim, nrpt, bias) add a bias to one
%   direction (towards 0 or towards 1). By default, the profile is balanced
%   (bias=0.5).
%
%   coeff = SMRINC_INTEGRATOR_FORCEPROFILE(inclim, nrpt, bias, degree) specify the
%   degree of the polynomial fit (by default, degree=8)
%
%   [coeff, support] = SMRINC_INTEGRATOR_FORCEPROFILE(...) returns also a
%   structure with the points around which the profile is fit, the bias and
%   the degree of the polynomial fit
%
% See also: polyfit, polyval

    if nargin == 2
        bias = 0.5;
        degree = 8;
    elseif nargin == 3
        degree = 8;
    end
    
    % Definition of fixed parameters for the support
    min_x  = 0;
    max_x  = 1; 
    step_x = 0.001;
    middle_x = (max_x - min_x)/2;
    
    % Checking input arguments
    if(inclim < min_x || inclim > max_x)
        error('chk:arg', 'inclim must be in the range of the support [0, 1]')
    end
    
    if(nrpt < min_x || nrpt > max_x)
        error('chk:arg', 'nrpt must be in the range of the support [0, 1]')
    end
    
    if(nrpt < inclim)
        error('chk:arg', 'Bad format of input argument. nrpt must be greater than inclim')
    end
    
    if(bias < min_x || bias > max_x)
        error('chk:arg', 'bias must be in the range of the support [0, 1]');
    end
    
    % Normalizing bias value
    bias_shift = [0 0];
    bias_amplitude = [-1 1];
    if(bias > 0.5)
        bias_shift(2) = bias -0.5;
        bias_amplitude(2) = bias_amplitude(2)-bias;
    elseif(bias < 0.5)
        bias_shift(1)  = bias -0.5;
        bias_amplitude(1) = bias + bias_amplitude(1);
    end
    
    % Definition of equilibrium points (intersection with x-axis equal 0)
    % Specifically: at 0, 0.5, 1 and at the no-return points
    equilibrium_x  = [min_x   max_x-nrpt+bias_shift(1)   middle_x   nrpt-bias_shift(2)   max_x];
    equilibrium_y  = zeros(1, length(equilibrium_x));
    
    % Definition of the INC region
    % Specifically: in between the INC limits provided
    inc_x = (max_x-inclim):step_x:inclim;
    inc_y = (middle_x - inc_x);

    % Definition of additional points for the polyfit function
    add_x = [(max_x-nrpt)/2  (nrpt + max_x-min_x)/2];
    add_y = bias_amplitude;
    
    % Definition of the overall support anchor points for the polynomial fit
    supp_x = [equilibrium_x inc_x add_x];
    supp_y = [equilibrium_y inc_y add_y];
    
    % Sorting the anchor points for the support
    [anchor_x, anch_id] = sort(supp_x);
    anchor_y = supp_y(anch_id);
    
    % Getting the polynomial coefficients for the given anchor points of the support
    coeff = polyfit(anchor_x, anchor_y, degree);
    
    support.anchor.x = anchor_x;
    support.anchor.y = anchor_y;
    support.bias     = bias;
    support.degree   = degree;
end