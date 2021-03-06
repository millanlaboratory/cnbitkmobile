function integrator = smrinc_integrator_xmlparam_dynamic_polyfit(config, integrator)
    
    DEFAULT_CHI    = 1.0;
    DEFAULT_BIAS   = 0.5;
    DEFAULT_INCLIM = 0.6;
    DEFAULT_NRPT   = 0.8;
    DEFAULT_DEGREE = 8;
    
    xml_path_phi    = 'integrator/dynamic/phi';
    xml_path_chi    = 'integrator/dynamic/chi';
    xml_path_bias   = 'integrator/dynamic/bias';
    xml_path_inclim = 'integrator/dynamic/inc';
    xml_path_nrpt   = 'integrator/dynamic/noreturn';
    xml_path_degree = 'integrator/dynamic/degree';
    
    % Getting phi parameter
    ccfg_root(config);
    ccfg_setbranch(config);
    integrator.param.phi = ccfg_quickfloat(config, xml_path_phi);
    
    % Getting chi parameter
    ccfg_root(config);
    ccfg_setbranch(config);
    integrator.param.chi = ccfg_quickfloat(config, xml_path_chi);
    
    % Getting bias parameter
    ccfg_root(config);
    ccfg_setbranch(config);
    integrator.param.bias = ccfg_quickfloat(config, xml_path_bias);
    
    % Getting inc parameter
    ccfg_root(config);
    ccfg_setbranch(config);
    integrator.param.inclim = ccfg_quickfloat(config, xml_path_inclim);
    
    % Getting no-return parameter
    ccfg_root(config);
    ccfg_setbranch(config);
    integrator.param.nrpt = ccfg_quickfloat(config, xml_path_nrpt);
    
    % Getting degree parameter
    ccfg_root(config);
    ccfg_setbranch(config);
    integrator.param.degree = ccfg_quickint(config, xml_path_degree);
            
    % Checking parameters
    if(isnan(integrator.param.phi))
        error('chk:param', 'phi parameter for dynamic integrator not defined in xml');
    end
    
    if(isnan(integrator.param.chi))
        integrator.param.chi = DEFAULT_CHI;
    end
    
    if(isnan(integrator.param.bias))
        integrator.param.bias = DEFAULT_BIAS;
    end
    
    if(isnan(integrator.param.inclim))
        integrator.param.inclim = DEFAULT_INCLIM;
    end
    
    if(isnan(integrator.param.nrpt))
        integrator.param.nrpt = DEFAULT_NRPT;
    end
    
    if(isnan(integrator.param.degree))
        integrator.param.degree = DEFAULT_DEGREE;
    end
            
    % Dumping integrator parameters
    disp( '[ndf_smrinc] + integrator parameters for dynamic:')
    disp(['             |- phi:       ' num2str(integrator.param.phi)]);
    disp(['             |- chi:       ' num2str(integrator.param.chi)]);
    disp(['             |- bias:      ' num2str(integrator.param.bias)]);
    disp(['             |- inc:       ' num2str(integrator.param.inclim)]);
    disp(['             |- no-return: ' num2str(integrator.param.nrpt)]);
    disp(['             |- degree:    ' num2str(integrator.param.degree)]);

end