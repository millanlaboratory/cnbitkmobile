function ndf_mobile_include()

try
	if(isempty(getenv('CNBITKMAT_ROOT')))
		disp('[ndf_mobile_include] $CNBITKMAT_ROOT not found, using default');
		setenv('CNBITKMAT_ROOT', '/usr/share/cnbiloop/cnbitkmat/');
	end
	if(isempty(getenv('EEGC3_ROOT')))
		disp('[ndf_mobile_include] $EEGC3_ROOT not found, using default');
		setenv('EEGC3_ROOT', '/opt/eegc3');
	end

	mtpath_root = [getenv('CNBITKMAT_ROOT') '/mtpath'];
	if(exist(mtpath_root, 'dir'))
		addpath(mtpath_root);
	end

	if(isempty(which('mtpath_include')))
		disp('[ndf_mobile_include] mtpath not installed, killing Matlab');
		exit;
	end
	mtpath_include('$CNBITKMAT_ROOT/');
	mtpath_include('$EEGC3_ROOT/');
	mtpath_include('$EEGC3_ROOT/modules/smr');
catch exception
	disp(['[ndf_mobile_include] Exception: ' exception.message ]);
	disp(exception);
	disp(exception.stack);
	disp('[ndf_mobile_include] Killing Matlab...');
	exit;
end
