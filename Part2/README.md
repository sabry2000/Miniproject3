# Part 2: File Systems

Ahmed Sabry - 99025207  
Tim Offenwanger - 22951157  
Moataz Abdelraouf - 3880223  

Ahmed Sabry and Moataz Abdelraouf were responsible for part 2. There is a Makefile which we tested on Virtual box which should compile/run part 2.

We split the methods that were assigned to us and any helper mthods that were created were written by the person responsible for implementing the specific method.  

We designed it such that the first KB of data is placed into its desired data structures, which is an array of bytes, and an array of index nodes, 16 index nodes.  

Unfortunately, executing the makefile has resulted in this output:  
![Screenshot from 2020-04-08 19-35-46](https://user-images.githubusercontent.com/52863189/78852182-859c6880-79d0-11ea-949f-27d1727c9929.png)

To compile and run the code, just type "make" in the terminal. Make sue you have input.txt, lab3.code.cpp, create_fs.c, and Makefile in your directory.  

We ran into issues running the make file, so if it does not work, copy and past every command in the make file individually an then run that command. The compile commands should be run first, and then the run commands.
