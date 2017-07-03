clc
clear
data_write = 1:1000;
open_file = fopen('test.dat','w')
fwrite(open_file,round(data_write),'int16');
fclose(open_file);

open_file = fopen('test.dat','r');
fseek(open_file,0,'bof');
data = fread(open_file,1000,'int16=>double');
fclose(open_file);