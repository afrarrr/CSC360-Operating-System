/*
 CSC360 Assignment1
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/time.h>

#define MAX_LINE_LENGTH 80
#define MAX_PROMPT_LENGTH 10
#define MAX_NUM_ARGS 9
#define MAX_PATH_DIRS 10

/*Read the .vshrc file which contains the dictionaries to search command.
 *The .vshrc file is in the same folder.
 */

void readrcfile (char path[MAX_PATH_DIRS][MAX_LINE_LENGTH]){
	FILE *file;
	file = fopen(".vshrc", "r");
	if (file ==NULL){
		fprintf(stderr,"Cannot open .vshrc file\n");
		exit(1);
	}

	int i=0;
	while(fgets(path[i],MAX_LINE_LENGTH,file)!=NULL){

		if (i == MAX_PATH_DIRS){
			fprintf(stderr,"More than 10 path dirs\n");
			break;
		}
		if (path[i][strlen(path[i]) - 1] == '\n')  {
			path[i][strlen(path[i]) - 1] = '\0';
		}
		i++;
	}
	path[i][0] ='\0';
	fclose(file);
}

/*Try to build the absolute path of the command to and then decide whether 
 *we can execute the command or not using stat followed stackoverflow
 * https://stackoverflow.com/questions/13098620/using-stat-to-check-if-a-file-is-executable-in-c 
*/

int actualpath (char* command,char path[MAX_PATH_DIRS][MAX_LINE_LENGTH], char absolute[MAX_LINE_LENGTH]){
	struct stat sb;
	// build the absolute path 
	for (int i=0; i< MAX_PATH_DIRS;i++){
		absolute[0]='\0';
		strcat (absolute,"/");
		strcat(absolute,path[i]);
		strcat (absolute,"/");
		strcat(absolute,command);
		// if the command was found return zero
		if (stat(absolute, &sb)==0 && sb.st_mode & S_IXUSR){
			return 0;
		}

	}
	return -1;

}

/*Process simple command that there is no '::' in the command. After the child process completed, 
 *whether it succeeds or not, with the time request, the time will be printed out.
 *Appendix b and f were used in this function 
*/

void simplecommand (char* token[MAX_NUM_ARGS], int n, int dpound, char absolute [MAX_LINE_LENGTH]){
	char * args[n+1];
	char *envp[]={ 0 };
	int pid,fd;
	int status,exit_status;
    	struct timeval before, after;

	for (int i = 0; i <n; i++) {
		args[i]=token[i];
	}
	args[n]=0;

	gettimeofday(&before, NULL);
	if ((pid =fork())==0){	
		if(execve(absolute, args, envp)==-1){
			fprintf(stderr,"Error in execve \n");	
			exit(1);
		}
	}
	waitpid(pid, &status, 0);	
	gettimeofday(&after, NULL);
	if ( WIFEXITED(status) ) {
        	exit_status = WEXITSTATUS(status);
	}	
	if(dpound != 0){
		if (exit_status==0){
			fprintf(stdout, "%lu microseconds\n",(after.tv_sec - before.tv_sec) * 1000000 + after.tv_usec - before.tv_usec);
			fflush(stdout);
		}else{
			fprintf(stdout, "0 microseconds\n");
			fflush(stdout);
		}
	}
}

/* Process commands that there is only one '::' in the command. Use the exit status of child process 
 * to decide whether the child process is successful or not. The following link is about how to get the 
 * exit status of the child process.
 * https://www.geeksforgeeks.org/exit-status-child-process-linux/
 * Appendix c and appendix f were used in this function.
 */

void onefilecommand(char* token[MAX_NUM_ARGS], int n, int dpound, char absolute [MAX_LINE_LENGTH],int cindex, char* out, char * in){
        char * args[n];
        char *envp[]={ 0 };
        int pid,fd;
        int status,exit_status;
        struct timeval before, after;

	if (cindex==0){
		for (int i = 1; i < n; i++) {
                	args[i-1]=token[i];
        	}
	}else{
                for (int i = 0; i < n-1; i++) {
                        args[i]=token[i];
                }
	}
        args[n-1]=0;
	// the file is treated as output file.
        if (out!=NULL && in ==NULL ){
	        gettimeofday(&before, NULL);
		if ((pid = fork()) == 0) {
        		fd = open(out, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
        		if (fd == -1) {
            			fprintf(stderr, "Cannot open %s for writing\n",out);
           			exit(1);
        		}
        		dup2(fd, 1);
        		dup2(fd, 2);
			if(execve(absolute, args, envp)==-1){
                        	fprintf(stderr,"Error in execve \n");
                        	exit(1);
			}
		}
        	waitpid(pid, &status, 0);
        	gettimeofday(&after, NULL);
        	if ( WIFEXITED(status) ) {
                	exit_status = WEXITSTATUS(status);
        	}
        	if(dpound != 0){
			FILE *fptr;
			fptr = fopen(out,"a");
                	if (exit_status==0){
                        	fprintf(fptr, "%lu microseconds\n",(after.tv_sec - before.tv_sec) * 1000000 + after.tv_usec - before.tv_usec);
                	}else{
                        	fprintf(fptr, "0 microseconds\n");
                	}
			fclose(fptr);
        	}
	// the file is used as input		
	} else{
                gettimeofday(&before, NULL);
                if ((pid = fork()) == 0) {
                        fd = open(in, O_RDONLY);
                        if (fd == -1) {
                               fprintf(stderr, "Cannot open %s for reading\n",in);
                               exit(1);
                        }
                        dup2(fd, 0);
                        if(execve(absolute, args, envp)==-1){
                                fprintf(stderr,"Error in execve \n");
                                exit(1);
                        }
                }
                waitpid(pid, &status, 0);
                gettimeofday(&after, NULL);
                if ( WIFEXITED(status) ) {
                        exit_status = WEXITSTATUS(status);
                }
                if(dpound != 0){
			// the child process is successful when the exit_status is zero
                        if (exit_status==0){
                                fprintf(stdout, "%lu microseconds\n",(after.tv_sec - before.tv_sec) * 1000000 + after.tv_usec - before.tv_usec);
				fflush(stdout);
                        }else{
                                fprintf(stdout, "0 microseconds\n");
				fflush(stdout);
                        }
		}
	}
}

/* Process commands that have two '::' in the child process. Since there is a output file provided,
 * the time will be printed in the output file 
 * Appendix c and f was used in the function
 */

void complexcommand (char* token[MAX_NUM_ARGS], int n , int dpound,char absolute[MAX_LINE_LENGTH],char *outputf,char* inputf,int outputindex,int inputindex){
	
        char * args[n-1];
        char *envp[]={ 0 };
        int pid,fd,fd1;
        int status,exit_status;
        struct timeval before, after;
	
       	// build *args[] for execution 
        for (int i,j = 0; j <n ; j++) {
	
		if(j!=inputindex && j!=outputindex){
			args[i]=token[j];
			i++;
		}
        }

        args[n-2]=0;

        gettimeofday(&before, NULL);
        if ((pid = fork()) == 0) {
		fd = open(outputf, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
                if (fd == -1) {
                	fprintf(stderr, "Cannot open %s for writing\n",outputf);
                        exit(1);
		}
                fd1 = open(inputf, O_RDONLY);
                if (fd1 == -1) {
                        fprintf(stderr, "Cannot open %s for reading\n",inputf);
                        exit(1);
                }

		dup2(fd1,0);
		dup2(fd, 1);
               	dup2(fd, 2);
                if(execve(absolute, args, envp)==-1){
                        fprintf(stderr,"Error in execve \n");
                       	exit(1);
                }
	}
        waitpid(pid, &status, 0);
        gettimeofday(&after, NULL);

	if ( WIFEXITED(status) ) {
		exit_status = WEXITSTATUS(status);
	}
	// process time request if any 
	if(dpound != 0){
		FILE *fptr;
                fptr = fopen(outputf,"a");
                if (exit_status==0){
			fprintf(fptr, "%lu microseconds\n",(after.tv_sec - before.tv_sec) * 1000000 + after.tv_usec - before.tv_usec);
                }else{
	
                       	fprintf(fptr, "0 microseconds\n");
                }
                fclose(fptr);
        }   	
	
}

int main (int argc, char *argv[]){


	char path[MAX_PATH_DIRS][MAX_LINE_LENGTH];
	char absolute [MAX_LINE_LENGTH];
	char input [MAX_LINE_LENGTH];
	char * t;
	char* inputf;
	char* outputf;
	char * token [13];//max argument 9 + command 1 + 2 file + 1 ## 
	int outputindex;
	int inputindex;
	int cindex;
	int num_tokens;
	int dcolon;
	int dpound;
	int arg;

	readrcfile(path);
	for(;;) {
		
		fprintf(stdout,"vsh%% ");
		fflush(stdout);

		fgets(input, MAX_LINE_LENGTH, stdin);
		

		if (input[strlen(input) - 1] == '\n') {
			input[strlen(input) - 1] = '\0';
		}
		if (strcmp (input, "exit") == 0) {
			exit(0);
		}
		outputindex=0;
		inputindex=0;
		num_tokens=0;
		cindex=0;
		dpound=0;
		dcolon=0;
		inputf=NULL;
		outputf=NULL;
		arg = 0;

		// assuming as for max_num_arguments: argument doesnt count: command,::file,file::, ## 
		// since arg actually counts the token without '::' and '##', it also counts command, so
		// (MAX_NUM_ARGS+1) was used to measure.

		t = strtok(input, " ");		
		while (t != NULL && num_tokens < 13 ) {
	        	if (strstr(t,"::")==NULL && strstr(t,"##")==NULL &&  arg < (MAX_NUM_ARGS+1)){ 
			    	token[num_tokens] = t;						
			    	num_tokens++; 
			    	arg++;
		    	}
		    	if (strstr(t,"::") !=NULL || strstr(t,"##")!=NULL){
		       		token[num_tokens] = t;
			    	num_tokens++; 
		    	}
        		t = strtok(NULL, " ");
		}
		
		// the user didnt enter anything 
		if(num_tokens==0){
			fprintf(stderr,"Please enter some command\n");
			continue;
		}
		
		// loop through all the tokens to find if there is '::' or '##' 
    		for (int i = 0; i < num_tokens; i++) {
	
			if (strstr(token[i],"::")!=NULL){
				dcolon++;
				if (strlen(token[i])==2){
					break;
				}else{
					if (strstr(((token[i])+2),":")==NULL){
						outputf = (token[i])+2;
						cindex=i;
						outputindex=i;
						
					}else {								
						inputf = strtok((token[i]),"::");
						cindex=i;
						inputindex=i;
				
					}
			         
		        	}
			}
			if  (strstr(token[i],"##")!=NULL){
				dpound++;
			}
			
		}
		
		// Missing file name in the command 			
		if (dcolon==1){
			if (outputf==NULL && inputf==NULL){
				fprintf(stderr, "Missing file name\n");
                                continue;
			}
		}

		// there is no '::' and '##' in the command, which is the simplest situation
		if(dcolon==0 && dpound==0){
			if (actualpath (token[0],path,absolute) ==0){
				simplecommand (token, num_tokens, dpound,absolute);
			}else{
				fprintf(stderr, "Cannot find the location of the command\n");
				continue;
			}
		}

		//the simplest situation with time request  
                if(dcolon==0 && dpound==1){
                        if (actualpath (token[0],path,absolute) ==0){
                                simplecommand (token, num_tokens-1, dpound,absolute);
                        }else{
                                fprintf(stderr, "Cannot find the location of the command\n");
                                continue;
                        }
                }	

		// there is a filenmae in the command, corresponding to onefilecommand function
		if (dcolon==1 && dpound==0){
			// only the filename given without command
			if(num_tokens==1 ){
				fprintf(stderr, "Please give some command for the file \n");
				continue;
			}
			if (cindex==0){
				if (actualpath (token[1],path,absolute) ==0){
                                	onefilecommand (token, num_tokens, dpound,absolute,cindex,outputf,inputf);
                        	}else{
                                	fprintf(stderr, "Cannot find the location of the command\n");
                                	continue;
                        	}
			}else{
			        if (actualpath (token[0],path,absolute) ==0){
                                	onefilecommand (token, num_tokens, dpound,absolute,cindex,outputf,inputf);
                        	}else{
                                	fprintf(stderr, "Cannot find the location of the command\n");
                                	continue;
				}
                        }
		}

		// based on the one file situation, plus time request
                if (dcolon==1 && dpound==1){
			// only the filename and ## given without request
                        if(num_tokens == 2){
                                fprintf(stderr, "No commmand provided \n");
                                continue;
                        }
                        if (cindex==0){
                                if (actualpath (token[1],path,absolute) ==0){
                                        onefilecommand (token, num_tokens-1, dpound,absolute,cindex,outputf,inputf);
                                }else{
                                        fprintf(stderr, "Cannot find the location of the command\n");
                                        continue;
                                }
                        }else{
                                if (actualpath (token[0],path,absolute) ==0){
                                        onefilecommand (token, num_tokens-1, dpound,absolute,cindex,outputf,inputf);
                                }else{
                                        fprintf(stderr, "Cannot find the location of the command\n");
                                        continue;
                                }
                        }
                }
		
		// there is input and outputfile in the command and dont need to report time
                if (dcolon==2 && dpound==0){
			// only have input and output without command
                        if(num_tokens== 2){
                                fprintf(stderr, "No commmand provided \n");
                                continue;
                        }
                        for (int j=0;j<num_tokens;j++){
				if(j!=outputindex && j!=inputindex){
					if (actualpath (token[j],path,absolute) ==0){
						complexcommand (token, num_tokens, dpound,absolute,outputf,inputf,outputindex,inputindex);
					}else{
					        fprintf(stderr, "Cannot find the location of the command\n");
                                        	continue;
					}
				}
			} 
                }
		// the complex situation (two filename) and time request
                if (dcolon==2 && dpound==1){
			// missing command
                        if(num_tokens== 3){
                                fprintf(stderr, "No commmand provided \n");
                                continue;
                        }
                        for (int j=0;j<num_tokens-1;j++){
                                if(j!=outputindex && j!=inputindex){
                                        if (actualpath (token[j],path,absolute) ==0){
                                                complexcommand (token, num_tokens-1, dpound,absolute,outputf,inputf,outputindex,inputindex);
				
                                        }else{
                                                fprintf(stderr, "Cannot find the location of the command\n");
						continue;
                                        }
                                }
                        }
                }
			
	}
}
	
