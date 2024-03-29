// header file for standard type definition and shared functions

const int childs = 12;  
const int runways = 3;
int ptower; // pid torre 
char *myfifo = "/tmp/myfifo"; // named pipe path
int fdw, fdr; // files descriptor for read and write pipe

const char *colours[] = {"\e[31m", "\e[32m", "\e[33m", "\e[34m", "\e[35m","\e[36m", "\e[37m"};

const int size = sizeof(colours)/sizeof(colours[0]); // number of colours
 
struct message{ // declaration of struct message
	int pid;
	int child_n;
	char mex[10];
};

/* fun used to print event in time special format */
extern void print_Event(char* source, char* description, bool newline);
/* fun to mask two signals in sigset_t data type */
extern void setSig(sigset_t *sigset, int signum1, int signum2, bool block);
