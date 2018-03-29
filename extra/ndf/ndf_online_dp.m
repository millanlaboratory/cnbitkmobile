function ndf_online_dp(arg0, arg1, arg2)

    sfeatures = [];
    slabels   = [];
    ccue = 0;
    try
        ndf_online_dp_include();

        % Initialize loop structure
        ndf_loopnew;
        
        % Initialize settings
        settings = eegc3_smr_newsettings;
    
        % Connect to the CnbiTk loop
        if(cl_connect(loop.cl) == false)
            disp('[ndf_online_dp] Cannot connect to CNBI Loop, killing matlab');
            exit;
        end
        
        % Prepare NDF srtructure
        ndf.conf  = {};
        ndf.size  = 0;
        ndf.frame = ndf_frame();
        ndf.sink  = ndf_sink('/tmp/cl.pipe.ndf.14');
        
        % Configure TiD message
        idmessage_setevent(loop.mDo, 0);
        
        % Pipe opening and NDF configuration
        disp('[ndf_mi] Receiving ACK...');
        [ndf.conf, ndf.size] = ndf_ack(ndf.sink);
        
        % Buffer initialization
        buffer.eeg = ndf_ringbuffer(ndf.conf.sf, ndf.conf.eeg_channels, 1.00);
        buffer.tri = ndf_ringbuffer(ndf.conf.sf, ndf.conf.tri_channels, 1.00);
        
        % NDF ACK check
        % - The NDF id describes the acquisition module running
        % - Bind your modules to a particular configuration (if needed)
        disp(['[ndf_mi] NDF type id: ' num2str(ndf.conf.id)]);
        
        % Initialize ndf_jump structure
        % - Each NDF frame carries an index number
        % - ndf_jump*.m are methods to verify whether your script is
        %   running too slow
        disp('[ndf_mi] Receiving NDF frames...');
        loop.jump.tic = ndf_tic();
        
        % Figure
       % figure;

        while(true)
            % Read NDF frame from pipe
            loop.jump.toc = ndf_toc(loop.jump.tic);
            loop.jump.tic = ndf_tic();
            [ndf.frame, ndf.size] = ndf_read(ndf.sink, ndf.conf, ndf.frame);

            % Acquisition is down, exit
            if(ndf.size == 0)
                disp('[ndf_mi] Broken pipe');
                break;
            end

            % Buffer NDF streams to the ring-buffers
            % - If you want to keep track of the timestamps for debugging, 
            %   update buffer.tim in this way:       
            buffer.eeg = ndf_add2buffer(buffer.eeg, ndf.frame.eeg);
            buffer.tri = ndf_add2buffer(buffer.tri, ndf.frame.tri);

            % Preprocess EEG
            sample = eegc3_smr_preprocess(buffer.eeg, ...
                                settings.modules.smr.options.prep.dc, ...
                                settings.modules.smr.options.prep.car, ...  
                                settings.modules.smr.options.prep.laplacian, ...
                                settings.modules.smr.laplacian);
            
            % Features extraction
            features = eegc3_smr_features(sample, ndf.conf.sf, ...
                                        settings.modules.smr.psd.freqs, ...
                                        settings.modules.smr.psd.win, ...
                                        settings.modules.smr.psd.ovl, ...
                                        1:ndf.conf.eeg_channels);
            % Feature rearrangment
            rfeatures = features';
            nfeatures = eegc3_smr_npsd(rfeatures);
            
            vfeatures = reshape(nfeatures, [numel(nfeatures) 1]);
            
            
             % Handle async TOBI iD communication
            if(tid_isattached(loop.iD) == true)
                while(tid_getmessage(loop.iD, loop.sDi) == true)
                    id_event = idmessage_getevent(loop.mDi);
                    
                    switch(id_event)
                        case 773
                            ccue = 773;
                            printf('[ndf_online_dp] Cue 773\n');
                        case 771
                            ccue = 771;
                            printf('[ndf_online_dp] Cue 771\n');
                        case 781+hex2dec('8000');
                            ccue = 0;
                            printf('[ndf_online_dp] Cue closed\n');
                    end
%                     if(id_event == 781)
%                         printf('[ndf_mi] Resetting NDF=%d/iD=%d\n', ...
%                                 ndf.frame.index, idmessage_getbidx(loop.mDi));
%                     end
                end
            else
                tid_attach(loop.iD, '/bus');
            end
            
            % Saving features and labels
            sfeatures = cat(2, sfeatures, vfeatures);
            slabels   = cat(1, slabels, ccue);
            
            % Computing fisher score
            fs = fisher_score(sfeatures, slabels);
            
            rfs = reshape(fs, [length(settings.modules.smr.psd.freqs) ndf.conf.eeg_channels]);
            imagesc(rfs');
            set(gca, 'clim', [0 0.5]);
            colorbar;
            drawnow;
            
%             disp(num2str(size(afeature)));
%             disp(num2str(size(nfeature)));
%             disp(num2str(size(sfeatures)));
%             a = reshape(nanmean(sfeatures, 1), [23 16]);
%             disp(num2str(size(a)));
%            
%             imagesc(a);
%             drawnow;
%             
            
           
            
            
            % Check if module is running slow
            loop.jump = ndf_jump_update(loop.jump, ndf.frame.index);
            if(loop.jump.isjumping)
                disp('[ndf_mi] Error: running slow');
            end
            
        end
        
    catch exception
        ndf_printexception(exception);
        disp('[ndf_mi] Going down');
        loop.exit = true;
    end
    
    try 
        % Tear down loop structure
        ndf_loopdelete;
    catch exception
        ndf_printexception(exception);
        loop.exit = true;
    end

    disp('[ndf_mi] Killing Matlab...');
    if(loop.exit == true)
        exit;
    end
    
end


function fisher = fisher_score(features, labels) 

    nfeatures = size(features, 1);
    fisher = zeros(nfeatures, 1);
    
    classes = unique(labels(labels~=0));
    nclasses = length(classes);
    
    if(nclasses < 2)
        return
    end
    
    if (nclasses > 2)
        keyboard
        error('chk:cls', 'Number of classes is not equal 2');
    end
    
    
    for fId = 1:nfeatures
        
        m1 = nanmean(features(fId, labels == classes(1)), 2);
        m2 = nanmean(features(fId, labels == classes(2)), 2);
        
        s1 = nanstd(features(fId, labels == classes(1)), [], 2);
        s2 = nanstd(features(fId, labels == classes(2)), [], 2);

        fisher(fId) = abs(m2 - m1) ./ sqrt(s1.^2 + s2.^2);
    end
    
    

end