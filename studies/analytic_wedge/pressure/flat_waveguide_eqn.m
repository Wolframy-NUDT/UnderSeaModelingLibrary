%%%%%%
% exact equation for the pressure at a given location in the wedge slice
% cartesian coordinate system is used in this calculation

function P = flat_waveguide_eqn( source_x, source_z, ...
			target_x, target_z, ...
			wave_number, wave_theta, water_depth, ...
			b, density, speed, atten)

		% direct path contribution
	r = cart_distance(source_x, source_z, target_x, target_z);
	P = ( exp(1i .* wave_number .* r ) ) / r;

		% first image, surface only reflection, contribution
	r = cart_distance(source_x, -source_z, target_x, target_z);
	P += ( (-1) .* exp(1i .* wave_number .* r ) ) / r;

		%%%% contribution for all other images (s,b)%%%%
	for t = 1:b

		% image below (n-1,n)
	r = cart_distance(source_x, 2*t*water_depth - source_z, target_x, target_z);
	P += pressure_image( r, cos((2*t*water_depth - source_z)/r), wave_number, wave_theta, ...
			t-1, t, density, speed, atten);

		% image below (n,n)
	r = cart_distance(source_x, 2*t*water_depth + source_z, target_x, target_z);
	P += pressure_image( r, cos((2*t*water_depth + source_z)/r), wave_number, wave_theta, ...
			t, t, density, speed, atten);

		% image above (n,n)
	r = cart_distance(source_x, (2*t+1)*water_depth - source_z, target_x, target_z);
	P += pressure_image( r, cos(((2*t+1)*water_depth - source_z)/r), wave_number, wave_theta, ...
			t, t, density, speed, atten);

		% image above (n+1,n)
	r = cart_distance(source_x, (2*t+1)*water_depth + source_z, target_x, target_z);
	P += pressure_image( r, cos(((2*t+1)*water_depth + source_z)/r), wave_number, wave_theta, ...
			t+1, t, density, speed, atten);

	end

end
