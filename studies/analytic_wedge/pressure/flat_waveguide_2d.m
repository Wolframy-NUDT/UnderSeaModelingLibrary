%%%
% program to test the obtained values of the known
% and numberically calculated values for source images
% producing a complex pressure field in a flat bottom
% ocean. The program is setup in 2D, where y = 0, phi = 0.
%
% down is positive z
%
%			surface								
%***************************************************************************************** z=0
%*	+z										
%*	|				   					
%*	v				   				
%*								
%*							
%(S)				------->				(T)	
%*
%*		
%*________________________________________________________________________________________
%*					bottom


function flat_waveguide_2d

close all

		% environmental parameters
	water_depth = 100;				% deepest point of the wedge
	water_range = 4000;				% distance from the wedge apex to the wedge end
	water_speed = 1500;				% isovelocity of the water speed	
	density = 1.5;					% density ratio of the sedi/water
	speed = 1500/1700;				% n or water_speed/sedi_speed
	atten = 0.0;					% attenuation factor dB/wavelength
	
		% source parameters
	source_freq = 20;
	source_x = 3000;
	source_z = 36;			% depth of the source (from the surface)
		
		% target parameters
	range = 1:1:source_x;
	target_x = source_x - range;
	target_z = 36;

		% wave parameters
	wave_number = 2 * pi * source_freq / water_speed;
	wave_theta = (0:(1/1024):1)*pi/2;
	dtheta = wave_theta(2) - wave_theta(1);

		% interface interactions
	b = 5;			% number of interactions with the bottom
	
	spectrum = zeros( length(target_x), length(wave_theta) );
	pressure = zeros( length(target_x), 1 );
	for t = 1:length(target_x)
		spectrum(t,:) = flat_waveguide_eqn( source_x, source_z, ...
			target_x(t), target_z, ...
			wave_number, wave_theta, water_depth, ...
			b, density, speed, atten);
		pressure(t) = sum( spectrum(t,:) ) .* dtheta;
	end
	clear t

	save flat_waveguide_2d

		%plot results
	plot( range, 20*log10(abs(pressure))-4);
	grid ;
	xlabel('Range (lambda)');
	ylabel('Transmission Loss (dB)');

end
