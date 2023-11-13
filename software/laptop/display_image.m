width = 40;
height = 30;
channels = 1;
bytes_per_channel = 2;

clear sensor
sensor = serialport("/dev/tty.usbmodem0007700041561", 19200);


while true
    tic
        raw_pixels = readline(sensor);
        if strlength(raw_pixels) == (width*height*channels*bytes_per_channel)
            % disp(raw_pixels)
            raw_pixels = cellstr(reshape(char(raw_pixels), channels * bytes_per_channel, [])');
            flat_pixels = uint8(hex2dec(raw_pixels));
            % disp(flat_pixels)
            pixels = reshape(flat_pixels, height, width);
            % remapped_pixels = (pixels - 150) * 8;
            demosaiced = demosaic(pixels,"bggr");
            imshow(imresize(demosaiced, 50), [0 255]);
        end
    frame_time = toc;
    disp(1/toc)
end