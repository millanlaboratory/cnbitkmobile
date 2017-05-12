% Copyright (C) 2009-2011  EPFL (Ecole Polytechnique Fédérale de Lausanne)
% Michele Tavella <michele.tavella@epfl.ch>
%
% This program is free software: you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation, either version 3 of the License, or
% (at your option) any later version.
%
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License
% along with this program.  If not, see <http://www.gnu.org/licenses/>.

function ndf_mi_mobile(arg0, arg1, arg2)

% For historical reasons this function accepts 3 arguments.
% Normally not needed, but the users might want to pass something
% when launching this function for debugging (and not within the loop)
if(nargin == 0) 
	arg0 = ''; 
	arg1 = ''; 
	arg2 = ''; 
end

% Include all the required toolboxes
ndf_mi_mobile_include();

% Prepare and enter main loop
try 
	% Initialize loop structure
	ndf_loopnew;

	% Connect to the CnbiTk loop
	if(cl_connect(loop.cl) == false)
		disp('[ndf_mi_mobile] Cannot connect to CNBI Loop, killing matlab');
		exit;
	end	

	% Configure loop structure
	loop = ndf_loopconfig(loop, 'mi');
	if(loop.cfg.config == 0)
		disp('[ndf_mi_mobile] Cannot retrieve loop configuration, killing matlab');
		exit;
	end
	loop = ndf_loopnames(loop);
	
	if(isempty(loop.cfg.ndf.pipe))
		disp('[ndf_mi_mobile] NDF configuration failed, killing matlab:');
		disp(['  Pipename:   "' loop.cfg.ndf.pipe '"']);
		disp(['  iC address: "' loop.cfg.ndf.ic '"']);
		exit;
	end
	
	% Dumping initial IC message
	icmessage_dumpmessage(loop.mC);

	% Prepare NDF srtructure
	ndf.conf  = {};
	ndf.size  = 0;
	ndf.frame = ndf_frame();
	ndf.sink  = ndf_sink(loop.cfg.ndf.pipe);
	
	% Pipe opening and NDF configurationf
	disp('[ndf_mi_mobile] Receiving ACK...');
	[ndf.conf, ndf.size] = ndf_ack(ndf.sink);
    
    % Read in parameters
	user.nTasks = ccfgtaskset_count(loop.cfg.taskset);
    for t = 0:user.nTasks-1
        task = ccfgtaskset_gettaskbyid(loop.cfg.taskset, t);
        user.tasklabel{t+1} = ccfgtask_getgdf(task);
    end
	cl_updatelog(loop.cl, sprintf('classifier=%s', loop.cfg.classifier.file));

	user.bci = eegc3_smr_newbci();
	user.classifier = [loop.cfg.ns.path '/' loop.cfg.classifier.file];
    try
        user.bci.analysis = load(user.classifier);
		user.bci.analysis = user.bci.analysis.analysis;
		user.bci.support = eegc3_smr_newsupport(user.bci.analysis, [], []);
    catch exception
		ndf_printexception(exception);
        disp('[ndf_mi_mobile] Killing Matlab...');
        exit;
    end

	if(user.bci.analysis.settings.eeg.chs ~= ndf.conf.eeg_channels)
		disp('[ndf_mi_mobile] Error: NDF channels differ from classifier settings');
        disp('[ndf_mi_mobile] Killing Matlab...');
        exit;
	end
	
	if(user.bci.analysis.settings.eeg.fs ~= ndf.conf.sf)
		disp('[ndf_mi_mobile] Error: NDF sample rate differs from classifier settings');
        disp('[ndf_mi_mobile] Killing Matlab...');
        exit;
	end

	% Initialize buffers for EEG, Triggers
	buffer.eeg = ndf_ringbuffer(ndf.conf.sf, ndf.conf.eeg_channels, 1.00);
	buffer.tri = ndf_ringbuffer(ndf.conf.sf, ndf.conf.tri_channels, 1.00);
	
	% NDF ACK check
	disp(['[ndf_mi_mobile] NDF type id: ' num2str(ndf.conf.id)]);
    
	% Initialize ndf_jump structure
	disp('[ndf_mi_mobile] Receiving NDF frames...');
	loop.jump.tic = ndf_tic();
    
	while(true)
		% Read NDF frame from pipe
		loop.jump.toc = ndf_toc(loop.jump.tic);
		loop.jump.tic = ndf_tic();
		[ndf.frame, ndf.size] = ndf_read(ndf.sink, ndf.conf, ndf.frame);

		% Acquisition is down, exit
		if(ndf.size == 0)
			disp('[ndf_mi_mobile] Broken pipe');
			break;
		end
		
		% Buffer NDF streams to the ring-buffers
		buffer.eeg = ndf_add2buffer(buffer.eeg, ndf.frame.eeg);
		buffer.tri = ndf_add2buffer(buffer.tri, ndf.frame.tri);
        
       
		% SMR processing and classification
        user.bci.support = ...
            eegc3_smr_mobile_classify(user.bci.analysis, buffer.eeg, user.bci.support);
       
		% Handle sync TOBI iC communication and sending raw probabilities
		if(tic_isattached(loop.iC) == true)
			for t = 1:user.nTasks
                % Map the tasklabel to the probabilities output from EEGC3
                taskloc = find(user.tasklabel{t} == user.bci.analysis.settings.task.classes_old);
                if length(taskloc) ~= 1
                    disp('[ndf_mi_mobile] Task not present in classifier')
                    disp('[ndf_mi_mobile] Killing MATLAB')
                    exit;
                end
				icmessage_setvalue(loop.mC, loop.cfg.classifier.id, ...
					num2str(user.tasklabel{t}), user.bci.support.cprobs(taskloc));
			end
			tic_setmessage(loop.iC, loop.sC, ndf.frame.index);
		else
			tic_attach(loop.iC, loop.cfg.ndf.ic);
        end
        
		% Check if module is running slow
		loop.jump = ndf_jump_update(loop.jump, ndf.frame.index);
		if(loop.jump.isjumping)
			disp('[ndf_mi_mobile] Error: running slow');
			break;
        end
        
	end
catch exception
	ndf_printexception(exception);
	disp('[ndf_mi_mobile] Going down');
	loop.exit = true;
end

try 
	% Tear down loop structure
	ndf_loopdelete;
catch exception
	ndf_printexception(exception);
	loop.exit = true;
end

disp(['[ndf_mi_mobile] Loop uptime: ' ...
	num2str(ndf_toc(loop.uptime)/1000/60) ...
	' minutes']);

disp('[ndf_mi_mobile] Killing Matlab...');
if(loop.exit == true)
	exit;
end
