%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%55
%% tests the for loop structure

a = zeros(5,1);

b = [1:5];
c = [1:5];
t = 1;

while ( t <= length(a) )
	a(t,1) = b(t)+c(t);
	t += 1;
end

a
