// header file for standard type definition and shared functions

const int childs = 10;  
const int tracks = 3; 
char *myfifo = "/tmp/myfifo"; // named pipe path
int fdw, fdr; // files descriptor for read and write pipe

const char *colours[] = {"\e[31m", "\e[32m", "\e[33m", "\e[34m", "\e[35m","\e[36m", "\e[37m"};

const int size = sizeof(colours)/sizeof(colours[0]); // number of elements of array
 
struct message{
	int pid;
	int child_n;
	char mex[10];
};

// extern void send_mex(struct message *ms, int source, char *text, int dest);
// extern void receive_mex(struct message *ms);
extern void print_Event(char* source, char* description, bool newline); 
extern void setSig(sigset_t *sigset, int signum1, int signum2, bool block);
