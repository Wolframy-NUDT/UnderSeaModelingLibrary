%%%
% program to test the obtained values of the known
% and numberically calculated values for source images
% producing a complex pressure field in a wedge like
% ocean. The program is setup in 2D, where y = 0, phi = 0.
%
% down is positive z and down from the horizontal is positive theta
%
%			surface								apex
%*****************************************************************************************
%*	+z				+theta						*
%*	|				   |					*
%*	v				   v				*
%*								*
%*							*
%(S)	------->	(T)			*
%*					*
%*				*	bottom
%*			*
%*		*
%*	*
%*

function penetrable_wedge_2d

close all

		% environmental parameters
	wedge_depth = 200;				% deepest point of the wedge
	wedge_range = 4000;				% distance from the wedge apex to the wedge end
	wedge_theta = atan(wedge_depth/wedge_range);	% angle of the apex to the horizontal
	water_speed = 1500;				% isovelocity of the water speed	
	density = 1.5;					% density ratio of the sedi/water
	speed = 1700/water_speed;			% sedi_speed/water_speed
	atten = 0.5;					% attenuation factor dB/wavelength
	
		% source parameters
	source_freq = 25;
	source_x = 4000;
	source_z = 100;			% depth of the source (from the surface)
	source_theta = atan(source_z/source_x);
	
		% target parameters
	range = 1:1:source_x;
	target_x = source_x - range;
	target_z = 30;
	target_theta = atan(target_z ./ target_x);

		% wave parameters
	wave_number = 2 * pi * source_freq / water_speed;
	wave_theta = (0:(1/1024):1)*pi/2;
	dtheta = wave_theta(2) - wave_theta(1);

		% interface interactions
	b = [5,10,15,20,25,30,35,40,45,50];			% number of interactions with the bottom
	tolerance = 1.0e-10;

	pressure = zeros( length(b), length(target_x) );

for t=1:length(b)	
	pressure(t,:) = pressure_eqn( source_x, source_z, source_theta, ...
			target_x(:), target_z, target_theta, ...
			wave_number, wedge_theta, ...
			b(t), density, speed, atten, tolerance, dtheta);
end
clear t

	save penetrable_wedge_2d

		%plot results
	plot( range, 20*log10(abs(pressure(1,:))), range, -20*log10(range), range, 20*log10(abs(pressure(2,:))), range, 20*log10(abs(pressure(3,:))), range, 20*log10(abs(pressure(4,:))), range, 20*log10(abs(pressure(5,:))), range, 20*log10(abs(pressure(6,:))), range, 20*log10(abs(pressure(7,:))), range, 20*log10(abs(pressure(8,:))), range, 20*log10(abs(pressure(9,:))), range, 20*log10(abs(pressure(10,:))) );
	grid ;
	xlabel('Range (m)');
	ylabel('Transmission Loss (dB)');

end
