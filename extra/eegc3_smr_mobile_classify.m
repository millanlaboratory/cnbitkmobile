% - Initially written by Michele
% - Then modified by Simis who casted the Perdikisian spells in this file
% - Then corrected and integrated in eegemg by Michele
% - Finally approved by Chuck Norris
% - Modified version of the smr classify giving as output the raw probabilities
%   (no rejection, no integration)
function [support, nfeature, afeature, rfeature] = eegc3_smr_mobile_classify(analysis, buffer, ...
	support)

nfeature = [];
afeature = [];
rfeature = [];
fs = analysis.settings.eeg.fs;

if(ndf_isfull(buffer))
    if(nargout == 4)
        [support.cprobs, nfeature, afeature, rfeature] = ...
			eegc3_smr_bci(analysis, buffer);
	elseif(nargout == 3)
        [support.cprobs, nfeature, afeature] = ...
			eegc3_smr_bci(analysis, buffer);
    else
        [support.cprobs, nfeature] = ...
			eegc3_smr_bci(analysis, buffer);
    end
end
