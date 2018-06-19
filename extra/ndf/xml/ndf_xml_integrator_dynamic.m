function integrator = ndf_mobile_xml_integrator_dynamic(config, integrator)
    
    DEFAULT_PHI = 0.9;
    DEFAULT_CHI = 1.0;
    DEFAULT_INC = 0.6;
    DEFAULT_NRP = 0.8;
    
    xml_path_phi = 'integrator/dynamic/phi';
    xml_path_chi = 'integrator/dynamic/chi';
    xml_path_inc = 'integrator/dynamic/inc';
    xml_path_nrp = 'integrator/dynamic/nrp';
    
    % Getting phi parameter
    ccfg_root(config);
    ccfg_setbranch(config);
    integrator.param.phi = ccfg_quickfloat(config, xml_path_phi);
    
    % Getting chi parameter
    ccfg_root(config);
    ccfg_setbranch(config);
    integrator.param.chi = ccfg_quickfloat(config, xml_path_chi);
    
    % Getting inc parameter
    ccfg_root(config);
    ccfg_setbranch(config);
    integrator.param.inc = ccfg_quickfloat(config, xml_path_inc);
    
    % Getting no-return parameter
    ccfg_root(config);
    ccfg_setbranch(config);
    integrator.param.nrp = ccfg_quickfloat(config, xml_path_nrp);
            
    % Checking parameters
    if(isnan(integrator.param.phi))
        warning('chk:param', '[ndf_mobile_xml] - phi parameter not defined in xml. Using default value');
        integrator.param.chi = DEFAULT_PHI;
    end
    
    if(isnan(integrator.param.chi))
        warning('chk:param', '[ndf_mobile_xml] - chi parameter not defined in xml. Using default value');
        integrator.param.chi = DEFAULT_CHI;
    end
    
    if(isnan(integrator.param.inc))
        warning('chk:param', '[ndf_mobile_xml] - inc parameter not defined in xml. Using default value');
        integrator.param.inc = DEFAULT_INC;
    end
    
    if(isnan(integrator.param.nrp))
        warning('chk:param', '[ndf_mobile_xml] - nrp parameter not defined in xml. Using default value');
        integrator.param.nrpt = DEFAULT_NRP;
    end
            
    % Dumping integrator parameters
    disp( '[ndf_mobile_xml] + integrator parameters for dynamic:')
    disp(['             |- phi: ' num2str(integrator.param.phi)]);
    disp(['             |- chi: ' num2str(integrator.param.chi)]);
    disp(['             |- inc: ' num2str(integrator.param.inc)]);
    disp(['             |- nrp: ' num2str(integrator.param.nrp)]);


end
