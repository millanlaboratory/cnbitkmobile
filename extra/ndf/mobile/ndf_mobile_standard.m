function ndf_mobile_standard

% Include all the required toolboxes
ndf_mobile_include();

% Prepare and enter main loop
try 
	% Initialize loop structure
    loop = [];
	ndf_loopnew;
    
	% Connect to the CnbiTk loop
	if(cl_connect(loop.cl) == false)
		disp('[ndf_mobile] Cannot connect to CNBI Loop, killing matlab');
		exit;
	end	

	% Check the names
	% - if in format /name, then query the nameserver for the IPs/filenames
	% - otherwise, keep them as they are
	% - if pn, aD or aC are empty after calling ndf_checknames, their value
	%   will be set according to what is stored in the XML configuration
	% - also, if the nameserver query fails, pn, aD and aC will be empty and
	%   their values will be set according to the XML configuration
	loop = ndf_loopconfig(loop, 'mi');
	if(loop.cfg.config == 0)
		disp('[ndf_mobile] Cannot retrieve loop configuration, killing matlab');
		exit;
	end
	loop = ndf_loopnames(loop);
    
    % Configuration iC for mobile
    ccfg_root(loop.cfg.config);
    ccfg_setbranch(loop.cfg.config);

	try 
		loop.cfg.ndf.mobile.ic = ccfg_quickstring(loop.cfg.config, 'device/ndf/ic');
	catch exception
		disp('[ndf_mobile] device/ndf/ic field does not exist. Using default value: /ctrl2')
		loop.cfg.ndf.mobile.ic = '/ctrl2';
	end


    ccfg_root(loop.cfg.config);
    ccfg_setbranch(loop.cfg.config);
	
	try
		loop.cfg.ndf.mobile.label = ccfg_quickgdf(loop.cfg.config, 'device/ndf/label');
	catch exception
		disp('[ndf_mobile] device/ndf/label field does not exist. Using default value: 0x0580 (1408)')
		loop.cfg.ndf.mobile.label = 1408;
	end
    
    loop.mobile.iC = tic_newsetonly();
    loop.mobile.mC = icmessage_new();
    loop.mobile.sC = icserializerrapid_new(loop.mobile.mC);
    icmessage_addclassifier(loop.mobile.mC, loop.cfg.classifier.id, loop.cfg.classifier.id, 0, 0);
    icmessage_addclass(loop.mobile.mC, loop.cfg.classifier.id, num2str(loop.cfg.ndf.mobile.label), 0.5);
     
	
	if(isempty(loop.cfg.ndf.pipe))
		disp('[ndf_mobile] NDF configuration failed, killing matlab:');
		disp(['  Pipename:   "' loop.cfg.ndf.pipe '"']);
		disp(['  iC address: "' loop.cfg.ndf.ic '"']);
		disp(['  iD address: "' loop.cfg.ndf.id '"']);
		exit;
	end

	% Prepare NDF srtructure
	ndf.conf  = {};
	ndf.size  = 0;
	ndf.frame = ndf_frame();
	ndf.sink  = ndf_sink(loop.cfg.ndf.pipe);

	% Configure TiD message
	idmessage_setevent(loop.mDo, 0);
	% Dump TiC/TiD messages
	icmessage_dumpmessage(loop.mC);
    icmessage_dumpmessage(loop.mobile.mC);
	idmessage_dumpmessage(loop.mDo);
	
	% Pipe opening and NDF configurationf
	% - Here the pipe is opened
	% - ... and the NDF ACK frame is received
	disp('[ndf_mobile] Receiving ACK...');
	[ndf.conf, ndf.size] = ndf_ack(ndf.sink);
    
    % Read in parameters
    user.nTasks = ccfgtaskset_count(loop.cfg.taskset);
    for t = 0:user.nTasks-1
        task = ccfgtaskset_gettaskbyid(loop.cfg.taskset, t);
        user.tasklabel{t+1} = ccfgtask_getgdf(task);
        user.thresholds(t+1) = ccfgtask_getconfig_float(task, 'threshold');
    end
    
    % Updating log file with classifier
	cl_updatelog(loop.cl, sprintf('classifier=%s', loop.cfg.classifier.file));

    % Read integrator parameters
    integrator.nprobs    = 0.5;
    integrator.type = cl_retrieveconfig(loop.cl, 'integrator', 'type');
    if isempty(integrator.type)
        disp('[ndf_mobile] Cannot retrieve the integrator type from nameserver')
        disp('[ndf_mobile] Killing MATLAB')
        exit;
    end
    integrator.filepath = ['/tmp/cnbitk-' getenv('USER') '/' datestr(now,'yyyymmdd') '.dynamic_parameters.txt'];
    
    % Updating log file with integrator type and default parameters (from XML)
    cl_updatelog(loop.cl, sprintf('integrator=%s', integrator.type));
    try 
        switch (integrator.type)
            case 'ema'
                integrator = ndf_xml_integrator_ema(loop.cfg.config, integrator);
                cl_updatelog(loop.cl, sprintf('rejection=%f', integrator.param.rejection));
                cl_updatelog(loop.cl, sprintf('alpha=%f',     integrator.param.alpha));
            case 'dynamic'
                integrator = ndf_xml_integrator_dynamic(loop.cfg.config, integrator);
                cl_updatelog(loop.cl, sprintf('phi=%f',    integrator.param.phi));
                cl_updatelog(loop.cl, sprintf('chi=%f',    integrator.param.chi));
                cl_updatelog(loop.cl, sprintf('inc=%f',    integrator.param.inc));
                cl_updatelog(loop.cl, sprintf('nrp=%f',   integrator.param.nrp));
        end  
    catch exception
        ndf_printexception(exception);
        disp('[ndf_mobile] Killing Matlab...');
        exit;
    end
    
    % Retrieve control type and update log file
    control.continuous = false;
    control.discrete   = false;
    control.type = cl_retrieveconfig(loop.cl, 'control', 'type');
    if isempty(control.type)
        disp('[ndf_mobile] Cannot retrieve the control type from nameserver')
        disp('[ndf_mobile] Killing MATLAB')
        exit;
    end
    switch(control.type)
        case 'continuous'
            control.continuous = true;
        case 'discrete'
            control.discrete = true;
        otherwise
            disp(['[ndf_mobile] - Unknown control type: ' control.type '. Exit']);
            exit;
    end
    disp(['[ndf_mobile] - Control type set as ' control.type])
    cl_updatelog(loop.cl, sprintf('control=%s', control.type));
    
    % Updating log file with thresholds
    for t = 0:user.nTasks-1
        cl_updatelog(loop.cl, ['th_' num2str(user.tasklabel{t+1}) '=' num2str(user.thresholds(t+1))]);
    end
	

	% -------------------------------------------------------------- %
	% User EEG data configuration                                    %
	% -------------------------------------------------------------- %
	user.bci = eegc3_smr_newbci();
	user.classifier = [loop.cfg.ns.path '/' loop.cfg.classifier.file];
    try
        user.bci.analysis = load(user.classifier);
		user.bci.analysis = user.bci.analysis.analysis;
		user.bci.support = eegc3_smr_newsupport(user.bci.analysis, 0.55, 0.97); % Added useless to rejection and alpha                                       
        user.bci.support.rawprobs = 0.5;                                        % Added field
    catch exception
		ndf_printexception(exception);
        disp('[ndf_mobile] Killing Matlab...');
        exit;
    end

	if(user.bci.analysis.settings.eeg.chs ~= ndf.conf.eeg_channels)
		disp('[ndf_mi] Error: NDF channels differ from classifier settings');
        disp('[ndf_mi] Killing Matlab...');
        exit;
	end
	
	if(user.bci.analysis.settings.eeg.fs ~= ndf.conf.sf)
		disp('[ndf_mobile] Error: NDF sample rate differs from classifier settings');
        disp('[ndf_mobile] Killing Matlab...');
        exit;
	end


	buffer.eeg = ndf_ringbuffer(ndf.conf.sf, ndf.conf.eeg_channels, 1.00);
	buffer.tri = ndf_ringbuffer(ndf.conf.sf, ndf.conf.tri_channels, 1.00);
	buffer.tim = ndf_ringbuffer(ndf.conf.sf/ndf.conf.samples, ndf.conf.tim_channels, 5.00);

	% NDF ACK check
	% - The NDF id describes the acquisition module running
	% - Bind your modules to a particular configuration (if needed)
	disp(['[ndf_mobile] NDF type id: ' num2str(ndf.conf.id)]);
    
	% Initialize ndf_jump structure
	% - Each NDF frame carries an index number
	% - ndf_jump*.m are methods to verify whether your script is
	%   running too slow
	disp('[ndf_mobile] Receiving NDF frames...');
	loop.jump.tic = ndf_tic();
    
    
    integrator.dt = ndf.conf.samples/ndf.conf.sf;
    
	while(true)
		% Read NDF frame from pipe
		loop.jump.toc = ndf_toc(loop.jump.tic);
		loop.jump.tic = ndf_tic();
		[ndf.frame, ndf.size] = ndf_read(ndf.sink, ndf.conf, ndf.frame);

		% Acquisition is down, exit
		if(ndf.size == 0)
			disp('[ndf_mobile] Broken pipe');
			break;
		end
		
		% Buffer NDF streams to the ring-buffers
		buffer.eeg = ndf_add2buffer(buffer.eeg, ndf.frame.eeg);
		buffer.tri = ndf_add2buffer(buffer.tri, ndf.frame.tri);
        
        % Classify current eeg frame
        user.bci.support = eegc3_smr_classify(user.bci.analysis, buffer.eeg, user.bci.support);
        
        % Integrate raw probabilities
        integrator.cprobs = user.bci.support.rawprobs(1);
        
        switch(integrator.type)
            case 'ema'
                useralpha = ndf_read_param(integrator.filepath, 'integrator', 'ema', 'alpha');
                if(isempty(useralpha) == false)
                    if(str2double(useralpha) ~= double(integrator.param.alpha))
                        integrator.param.alpha = str2double(useralpha);
                        disp(['[ndf_mobile] - DEBUG: Changed alpha parameter for ema to: ' useralpha]);
                    end
                end
                integrator.nprobs = ndf_integrator_ema(integrator.cprobs, integrator.nprobs, integrator.param.alpha, integrator.param.rejection);
            case 'dynamic'
                userphi = ndf_read_param(integrator.filepath, 'integrator', 'dynamic', 'phi');
                if(isempty(userphi) == false)
                    if(str2double(userphi) ~= double(integrator.param.phi))
                        integrator.param.phi = str2double(userphi);
                        disp(['[ndf_mobile] - DEBUG: Changed phi parameter for dynamic to: ' userphi]);
                    end
                end
                userchi = ndf_read_param(integrator.filepath, 'integrator', 'dynamic', 'chi');
                if(isempty(userchi) == false)
                    if(str2double(userchi) ~= double(integrator.param.chi))
                        integrator.param.chi = str2double(userchi);
                        disp(['[ndf_mobile] - DEBUG: Changed chi parameter for dynamic to: ' userchi]);
                    end
                end
                userinc = ndf_read_param(integrator.filepath, 'integrator', 'dynamic', 'inc');
                if(isempty(userinc) == false)
                    if(str2double(userinc) ~= double(integrator.param.inc))
                        integrator.param.inc = str2double(userinc);
                        disp(['[ndf_mobile] - DEBUG: Changed inc parameter for dynamic to: ' userinc]);
                    end
                end
                usernrp = ndf_read_param(integrator.filepath, 'integrator', 'dynamic', 'nrp');
                if(isempty(usernrp) == false)
                    if(str2double(usernrp) ~= double(integrator.param.nrp))
                        integrator.param.nrp = str2double(usernrp);
                        disp(['[ndf_mobile] - DEBUG: Changed nrp parameter for dynamic to: ' usernrp]);
                    end
                end
                integrator.nprobs = ndf_integrator_dynamic(integrator.cprobs, integrator.nprobs, ...
                                                              integrator.param.phi, integrator.param.chi, ...
                                                              integrator.param.inc, integrator.param.nrp, ...
                                                              integrator.dt);
        end
        
		% Handle async TOBI iD communication
        if(tid_isattached(loop.iD) == true)
            while(tid_getmessage(loop.iD, loop.sDi) == true)
                id_event = idmessage_getevent(loop.mDi);
                
                if(id_event == 781)
                    integrator.nprobs = 0.5;
					printf('[ndf_mobile] Resetting NDF=%d/iD=%d\n', ndf.frame.index, idmessage_getbidx(loop.mDi));
                end
            end
        else
			tid_attach(loop.iD, loop.cfg.ndf.id);
        end
		
		% Handle sync TOBI iC communication
        if(tic_isattached(loop.iC) == true)
			for t = 1:user.nTasks
                % Map the tasklabel to the probabilities output from EEGC3
                taskloc = find(user.tasklabel{t} == user.bci.analysis.settings.task.classes_old);
                if length(taskloc) ~= 1
                    disp('[ndf_mobile] Task not present in classifier')
                    disp('[ndf_mobile] Killing MATLAB')
                    exit;
                end
                integrator.sprob = [integrator.nprobs 1-integrator.nprobs];
				icmessage_setvalue(loop.mC, loop.cfg.classifier.id, num2str(user.tasklabel{t}), integrator.sprob(taskloc));
			end
			tic_setmessage(loop.iC, loop.sC, ndf.frame.index);
		else
			tic_attach(loop.iC, loop.cfg.ndf.ic);
        end
        
        % Handle sync TOBI iC communication to mobile
        if(control.continuous == true)
            if(tic_isattached(loop.mobile.iC) == true)
                icmessage_setvalue(loop.mobile.mC, loop.cfg.classifier.id, num2str(loop.cfg.ndf.mobile.label), integrator.nprobs);
                tic_setmessage(loop.mobile.iC, loop.mobile.sC, ndf.frame.index);
            else
                tic_attach(loop.mobile.iC, loop.cfg.ndf.mobile.ic);
            end
        end
        
		% -------------------------------------------------------------- %
		% /User main loop                                                %
		% -------------------------------------------------------------- %

		% Check if module is running slow
		loop.jump = ndf_jump_update(loop.jump, ndf.frame.index);
        if(loop.jump.isjumping)
			disp(['[ndf_mobile] Error: running slow ' num2str(ndf_toc(loop.uptime)/1000/60)]);
        end
        
	end
catch exception
	ndf_printexception(exception);
	disp('[ndf_mobile] Going down');
	loop.exit = true;
end

try 
	% Tear down loop structure
	ndf_loopdelete;
catch exception
	ndf_printexception(exception);
	loop.exit = true;
end

disp(['[ndf_mobile] Loop uptime: ' ...
	num2str(ndf_toc(loop.uptime)/1000/60) ...
	' minutes']);

disp('[ndf_mobile] Killing Matlab...');
if(loop.exit == true)
	exit;
end
