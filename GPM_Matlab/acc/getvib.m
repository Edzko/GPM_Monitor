% function y = getvib(u,port,baud)

function y = getvib(n,port,baud)

data = 1:10;
y=1:10;

for i=1:n
    data=acc(port,baud);
    if data(1)~=-1
        break;
    end
end

tic
while toc<0.01
    pause(0)
end

y = data