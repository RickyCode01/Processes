// header file for standard type definition and shared functions

const int childs =  10, colori = 7; // costante figli e numero colori
char *myfifo = "/tmp/myfifo"; // named pipe path
//char *mysema = "/mutex"; // named semaphore name
int fdw, fdr; // files descriptor fro read adn write pipe

//sem_t mutex; // binary semaphore for mutual exclusion

const char *colours[colori] = {"\033[31m", "\033[32m", "\033[33m", "\033[34m", "\033[35m",
"\033[36m", "\033[37m"};

struct message{
	int pid;
	char mex[10];
};

//extern void send_mex(struct message *ms, int source, char *text, int dest);
extern void receive_mex(struct message *ms);
extern void print_Event(char* source, char* description, bool newline);
extern void setSig(sigset_t *sigset, int signum1, int signum2, bool block);
