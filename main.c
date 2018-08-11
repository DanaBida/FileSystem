#include <dirent.h>
#include <stdlib.h>

#include "lab4_util.h"


extern int system_call();

#define SYS_OPEN 2
#define SYS_GETDENTS 78
#define SYS_WRITE 1
#define STDOUT 1


struct linux_dirent {
    long           d_ino;
    long          d_off;
    unsigned short d_reclen;
    char           d_name[];
};

#define BUF_SIZE 1024

void  func  (int fd, char curr[], int index_curr, char buf[], char to_find [], char to_execute[], char to_execute_path[], char command[]);

int main(int argc, char *argv[])
{
    int fd;
    char buf[BUF_SIZE];
    char d_curr[2];
    int index_curr;

    fd = system_call(SYS_OPEN, ".", 0,0777);
    system_call(SYS_WRITE,STDOUT, ".",1);
    system_call(SYS_WRITE,STDOUT, "\n",1);
    d_curr[0]='.';
    d_curr[1] = '\0';            
    index_curr = 1;

    /*indicates if we are in formate -n <name> : only print the names of the files (with their relative paths) with this specific name. */ 
    int is_n;
    is_n = 0;
    /*indicates if we are in formate -e <name> <command> : execute commands on files with a specific name*/ 
    int is_s;
    is_s = 0;
    if(argc==3)
    	is_n = 1;
    if(argc==4)
    	is_s = 1;

    char to_execute_path[1024];

    if(is_n)
    	func  (fd, d_curr, index_curr, buf, argv[2], 0, 0, 0);
    if(is_s)
		func  (fd, d_curr, index_curr, buf, 0, argv[2], to_execute_path, argv[3]);
	else
		func  (fd, d_curr, index_curr, buf, 0, 0, 0, 0);

 	return 0;
}

void  func  (int fd, char d_curr[], int index_curr, char buf[], char to_find [], char to_execute[], char  to_execute_path[], char command[]) {
         
    struct linux_dirent *d;
	int bpos;
	char d_type;
	int nread;

    for ( ; ; ){

        nread = system_call(SYS_GETDENTS, fd, buf, BUF_SIZE);	

        if (nread == 0 || nread == -1)
            return;
      
        for (bpos = 0; bpos < nread;) {
            
            d = (struct linux_dirent *) (buf + bpos);
           
            d_type = *(buf + bpos + d->d_reclen - 1);
            
	        if(d_type==4) /*directory -> open and print*/
	        {
	        	int flag; /*indicates if the name of the folder is: ".." or "." */
	        	flag =0;
	        	if((simple_strlen(d->d_name)==2 && d->d_name[0] == '.' && d->d_name[1] == '.')||(simple_strlen(d->d_name)==1 && d->d_name[0] == '.'))
	        		flag =1;
	        	if (flag ==0){
	        	    /*cd new dir*/
	        	    int i;
	        	    char new_d_curr[simple_strlen(d_curr)+simple_strlen(d->d_name)+2];
	        	    for(i =0; i<simple_strlen(d_curr);i++)/*copy d_curr to new_d_curr*/
		          		new_d_curr[i]=d_curr[i];
	        		new_d_curr[index_curr] = '/';
	        		index_curr++;
		            for(i =0; i<simple_strlen(d->d_name);i++)
		          		new_d_curr[i+index_curr]=d->d_name[i];
		        	index_curr+=simple_strlen(d->d_name);
		            new_d_curr[index_curr] = '\0';            
		            index_curr++;
		        	fd = system_call(SYS_OPEN, new_d_curr, 0,0777);
	        	    /*print new path*/
		     		if(to_find==0 && to_execute==0){ /*regular formate*/
		     			system_call(SYS_WRITE,STDOUT, new_d_curr,simple_strlen(new_d_curr));
		            	system_call(SYS_WRITE,STDOUT, "\n",1);
		     		}
		            /*recursively open the new dir*/ 
		            func  (fd, new_d_curr, index_curr, buf, to_find, to_execute, to_execute_path, command);
	        	}             
	        }                
    
	        else /*file-> print only*/
	        {
	        	if(to_find==0&&to_execute==0) { /*regular formate*/

	        		system_call(SYS_WRITE,STDOUT, d_curr,simple_strlen(d_curr));
		            system_call(SYS_WRITE,STDOUT, "/",1);
		            system_call(SYS_WRITE,STDOUT, d->d_name,simple_strlen(d->d_name));
		            system_call(SYS_WRITE,STDOUT, "\n",1); 
	        	}
	        	/*checks if the file to_find equal to the curr file
	        	  if so - print it*/
	        	else if(to_find!=0 && to_execute==0){ /*-n formate*/
	        		if(simple_strcmp(d->d_name,to_find)==0){

			          		system_call(SYS_WRITE,STDOUT, d_curr,simple_strlen(d_curr));
			            	system_call(SYS_WRITE,STDOUT, "/",1);
			            	system_call(SYS_WRITE,STDOUT, d->d_name,simple_strlen(d->d_name));
			            	system_call(SYS_WRITE,STDOUT, "\n",1); 
		          	}
		        }
		        else if(to_find==0 && to_execute!=0){ /*-s formate*/	        	
		        	if(simple_strcmp(d->d_name, to_execute)==0){ /*if d->d_name == to_execute , adding it and it path to to_execute_path*/
				     
				        	int i;
							int to_execute_path_index;
							to_execute_path_index = simple_strlen(d_curr);
				          	for(i =0; i<simple_strlen(d_curr);i++)
				          		to_execute_path[i]=d_curr[i];
				          	to_execute_path [to_execute_path_index] = '/';
				          	to_execute_path_index++;
				          	for(i =0; i<simple_strlen(d->d_name);i++)
				          		to_execute_path[i+to_execute_path_index]=d->d_name[i];
				          	to_execute_path_index += simple_strlen(d->d_name);
				          	to_execute_path[to_execute_path_index] = '\0';
							/*adding the command to the path*/
							char s_order[simple_strlen(to_execute_path)+simple_strlen(command)+2];
							int index_sOrder;
							index_sOrder = simple_strlen(command);
							for(i=0;i<simple_strlen(command);i++)
								s_order[i] = (command)[i];
							s_order[index_sOrder] = ' ';
							index_sOrder++;
							for(i= 0; i < simple_strlen(to_execute_path) ;i++)
								s_order[i+index_sOrder] = to_execute_path[i];
							index_sOrder += simple_strlen(to_execute_path);
							s_order[index_sOrder] = '\0';		
							simple_system(s_order);/*execute the command*/
							
			       }
	        	}

	        }
            
            bpos += d->d_reclen;
    	}

	}

}
