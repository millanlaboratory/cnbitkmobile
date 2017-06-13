function integrator = smrinc_integrator_xmlparam_ema(config, integrator)
    
    xml_path_alpha     = 'online/mi/integrator/ema/alpha';
    xml_path_rejection = 'online/mi/integrator/ema/rejection';
    
    % Getting alpha parameter
    ccfg_root(config);
    ccfg_setbranch(config);
    integrator.param.alpha = ccfg_quickfloat(config, xml_path_alpha);
    
    % Getting rejection parameter
    ccfg_root(config);
    ccfg_setbranch(config);
    integrator.param.rejection = ccfg_quickfloat(config, xml_path_rejection);
            
    % Checking parameters
    if(isnan(integrator.param.alpha))
        error('chk:param', 'alpha parameter for ema integrator not defined in xml');
    end
    
    if(isnan(integrator.param.rejection))
        error('chk:param', 'rejection parameter for ema integrator not defined in xml');
    end
            
    % Dumping integrator parameters
    disp( '[ndf_smrinc] + integrator parameters for ema:')
    disp(['             |- alpha:     ' num2str(integrator.param.alpha)]);
    disp(['             |- rejection: ' num2str(integrator.param.rejection)]);
end