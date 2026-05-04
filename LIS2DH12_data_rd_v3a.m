pkg load instrument-control;

% --- Settings ---
serial_port = "/dev/ttyACM0"; % Update to your specific port (e.g., COM3 on Windows)
baud_rate = 115200;           % Match your Arduino Serial.begin()
max_samples = 10;           % Total samples to collect
output_file = "accel_data.csv";

% --- Initialize Connection ---
% Using the recommended serialport object
s = serialport(serial_port, baud_rate);
pause(2); % Brief wait for ESP32C3 to reset after connection

% --- Storage & Plotting Setup ---
captured_data = zeros(max_samples, 3);
figure(1);
hold on;
hX = plot(0, 0, 'r'); hY = plot(0, 0, 'g'); hZ = plot(0, 0, 'b');
legend('X', 'Y', 'Z');
title('Real-time LIS3DH Accelerometer Data');
grid on;

% fprintf('Recording started. Press Ctrl+C in the command window to stop early.\n');

% --- Main Loop ---
for i = 1:max_samples
    % Read one line: format is "x, y, z\n" from your Arduino code
    line = readline(s);
    
    if ~isempty(line)
        % str2num effectively handles comma-separated strings
        val = str2num(line) 
##        
##        if length(val) == 3
##            captured_data(i, :) = val;
##            
##            % Real-time plot update (every 10 samples to maintain performance)
##            if mod(i, 10) == 0
##                set(hX, 'XData', 1:i, 'YData', captured_data(1:i, 1));
##                set(hY, 'XData', 1:i, 'YData', captured_data(1:i, 2));
##                set(hZ, 'XData', 1:i, 'YData', captured_data(1:i, 3));
##                drawnow; % Force the plot to refresh
##            end
##        end
    end
end

% --- Finalize & Save ---
clear s; % Properly closes the serial port
%fprintf('Recording finished. Saving to %s...\n', output_file);

% csvwrite is a simple way to save numeric matrices to a file
%csvwrite(output_file, captured_data); 
%fprintf('Done!\n');
