// header file for standard type definition and shared functions

char *myfifo = "/tmp/myfifo";
int fdw, fdr;

pthread_mutex_t mutex, mutex2;

struct message{
	int pid;
	char mex[10];
};

extern void send_mex(struct message *ms, int source, char *text, int dest);
extern void receive_mex(struct message *ms);
extern void print_Event(char* source, char* description, bool newline);
extern void setSig(sigset_t *sigset, int signum1, int signum2, bool block);

