// header file for standard type definition and shared functions

const int childs =  10;  // costante figli e numero colori
char *myfifo = "/tmp/myfifo"; // named pipe path
int fdw, fdr; // files descriptor fro read adn write pipe

const char *colours[] = {"\033[31m", "\033[32m", "\033[33m", "\033[34m", "\033[35m","\033[36m", "\033[37m"};

const int size = sizeof(colours)/sizeof(colours[0]);

struct message{
	int pid;
	char mex[10];
};

//extern void send_mex(struct message *ms, int source, char *text, int dest);
extern void receive_mex(struct message *ms);
extern void print_Event(char* source, char* description, bool newline);
extern void setSig(sigset_t *sigset, int signum1, int signum2, bool block);
