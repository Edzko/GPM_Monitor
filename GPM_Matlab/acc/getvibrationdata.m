acc('IP:192.168.1.1');
clear vdata
k = 1;
for i=1:1000
    newdata = acc(1);
    if newdata(1) ~= -1
        vdata(k,:) = newdata;
        %if newdata(5)==0
        %    newdata
        %end
        k=k+1;
    end
    if k>100
        break;
    end
end
acc

plot(vdata(:,1)-vdata(1,1),vdata(:,3:8),'.-')
legend({'Accel X','Accel Y','Accel Z','Gyro X','Gyro Y','Gyro Z'})
grid
xlabel('Time (s)')
