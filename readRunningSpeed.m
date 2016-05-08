% initialize serial communication
s = serial('COM6');
set(s,'BaudRate', 9600);
set(s,'DataBits', 8);
set(s,'StopBits', 1);
fopen(s);
s.ReadAsyncMode = 'continuous';
readasync(s);

% variables for storing data
ind = 1;
rewards = [];
speed.val = [];
speed.ts = [];

% plots for display
figure;
h = plot([0 1],[0 1]);
hold on;
hrew = plot(0,0,'rx');
% will run until Ctrl + C pressed
while true
    t = fgetl(s);
    a = sscanf(t,'%d');
    if numel(a)==2
        % if a speed reading
        speed.val(ind) = a(2);
        speed.ts(ind) = a(1);
        
        % update plots
        set(h,'XData',speed.ts(max(ind-99,1):ind), ...
            'YData',speed.val(max(ind-99,1):ind));
        set(hrew,'XData',rewards(rewards>=max(ind-99,1)),...
            'YData',zeros(size(rewards>=max(ind-99,1))));
        xlim([speed.ts(max(ind-99,1)) speed.ts(ind)+1])
        drawnow;
        
        ind = ind + 1;
    else
        % if a reward timestamp
        rewards = [ rewards a ];
    end
end

% close serial
fclose(s);
delete(s);
