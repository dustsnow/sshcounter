sshcounter
==========

SSHCounter is a daemon transfering number of ssh connection over bluetooth device to Arduino or anyother micontroller.
It initially designed for Georgia Tech Golem Krang. 

The functionality can be expanded to other functionality need bluetooth support.

July 23, 2013
The daemon can connect to a hot-plugging bluetooth device which is hard-coded in sshc.c file. It will support config file in the 
future.
It designed for transfering one byte of data right now since the intention of this program is just displaying the number of 
ssh connection on a single seven segments display. It can be easily expanded to be able to transfer more data.
