function integrator = smrinc_integrator_xmlparam_vema(config, integrator)
    
    xml_path_rho   = 'online/mi/integrator/vema/rho';
    xml_path_gamma = 'online/mi/integrator/vema/gamma';
    
    % Getting alpha parameter
    ccfg_root(config);
    ccfg_setbranch(config);
    integrator.param.rho = ccfg_quickfloat(config, xml_path_rho);
    
    % Getting rejection parameter
    ccfg_root(config);
    ccfg_setbranch(config);
    integrator.param.gamma = ccfg_quickfloat(config, xml_path_gamma);
            
    % Checking parameters
    if(isnan(integrator.param.rho))
        error('chk:param', 'rho parameter for vema integrator not defined in xml');
    end
    
    if(isnan(integrator.param.gamma))
        error('chk:param', 'gamma parameter for vema integrator not defined in xml');
    end
            
    % Dumping integrator parameters
    disp( '[ndf_smrinc] + integrator parameters for vema:')
    disp(['             |- rho:   ' num2str(integrator.param.rho)]);
    disp(['             |- gamma: ' num2str(integrator.param.gamma)]);
end