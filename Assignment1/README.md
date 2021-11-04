### CSC360 Assignment1 
Student Name: Haidan Liu 
Student Id: V00946959

This assignment implements a simlple unix shell vsh using c language. The vsh shell has following features. 

#### Features


- Continue to prompt the user for commands until the user enters "exit" to exit the shell. The maximum number of character entered is 80 characters. The command can only execute with up to 9 arguments. Everything else will be ignored.
- The. vshrc file contains up to 10 directories for searching for the location of the command. If the command cannot be found, then an error message will be printed out. The vshrc file must be located in the same folder for successful execution.
- If there is a "::file name" in the command, the output will be put into a file. In addition, if there is a "file name:", the file will be regarded as an input file. They can be in the same command. If the input file does not exist, then there will be an error message printed out. If the output file does not exist, then the file will be created.
- If there is a "##" at the end of a command, then the time used to execute the command will be printed out. If there is no output file, the time will display immediately; Otherwise, the time will be put in the output file even in the failed execution situation. 















