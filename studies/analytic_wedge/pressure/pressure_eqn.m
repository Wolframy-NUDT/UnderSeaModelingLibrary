%%%%%%
% exact equation for the pressure at a given location in the wedge slice
% cartesian coordinate system is used in this calculation

function P = pressure_eqn( source_x, source_z, source_theta, ...
			target_x, target_z, target_theta, ...
			wave_number, wedge_theta, ...
			b, density, speed, atten, tolerance, dtheta)

		% direct path contribution
	r = cart_distance(source_x, source_z, target_x, target_z);
	P = (( exp(1i .* wave_number .* r ) ) ./ r);
%	P = (-2 .* 1i ./ r) .* sin(wave_number .* source_z .* sin(atan(source_z/source_x))) .* exp(1i .* wave_number .* r);

		% first image, surface only reflection, contribution
	r = cart_distance(source_x, -source_z, target_x, target_z);
	P += (( (-1) .* exp(1i .* wave_number .* r ) ) ./ r);
%	P += P = (2 .* 1i ./ r) .* sin(wave_number .* source_z .* sin(atan(source_z/-source_x))) .* exp(1i .* wave_number .* r);

		%%%% contribution for all other images %%%%
	for t = 1:b

		% image below (n-1,n)
	r = cart_distance(source_x .* cos( 2*t*wedge_theta - source_theta ), source_x .* sin( 2*t*wedge_theta - source_theta ), target_x, target_z);
	new = pressure_image( r, wave_number, 0, wedge_theta, ...
			t-1, t, density, speed, atten);

		% image below (n,n)
	r = cart_distance(source_x .* cos( 2*t*wedge_theta + source_theta ), source_x .* sin( 2*t*wedge_theta + source_theta ), target_x, target_z);
	new += pressure_image( r, wave_number, 0, wedge_theta, ...
			t, t, density, speed, atten);

		% image above (n,n)
	r = cart_distance(source_x .* cos( (2*t+1)*wedge_theta - source_theta ), source_x .* sin( (2*t+1)*wedge_theta - source_theta ), target_x, target_z);
	new += pressure_image( r, wave_number, 0, wedge_theta, ...
			t, t, density, speed, atten);

		% image above (n+1,n)
	r = cart_distance(source_x .* cos( (2*t+1)*wedge_theta + source_theta ), source_x .* sin( (2*t+1)*wedge_theta + source_theta ), target_x, target_z);
	new += pressure_image( r, wave_number, 0, wedge_theta, ...
			t+1, t, density, speed, atten);

	P += new;

	if ( max(abs(new./P)) < tolerance)
		break;
	end

	end

end
