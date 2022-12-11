// processo Torre di controllo Areporto

struct mysem{
	int count;
	int fifo[10];
}s;

void Torre(){
	print_Event("torre", "avvio!", false);
	int mypid = getpid();
	printf("\tpid:%d\n",mypid);
	write(fdw, getpid(), sizeof(getpid()));
	struct message ms; // message struct
	memset(&ms, '\0', sizeof(struct message)); //reset message struct
	
	sigset_t sigset; // set mask signal
	int sig; // signal from sigwait
	int i = 0, j = 0; // init queue index 
	s.count = 2; //	init counter for railway resources

	setSig(&sigset, SIGUSR1, SIGUSR2, true); // set signals
	while(strcmp(ms.mex, "end") != 0){
		sig = 0; //reset signal
		sigwait(&sigset, &sig); // wait SIGUSR1 signal
		receive_mex(&ms);
		// printf("torre riceve da %d:%s\n", ms.pid, ms.mex);
		/* se piu di un signal viene inviato contemporaneamente uno dei 
		due si perde -> risolvere */
		if(sig == SIGUSR1 && strcmp(ms.mex, "ready") == 0){
			s.count--;
			//printf("count:%d\n", s.count);
			if(s.count < 0){ // add process to queue
				s.fifo[i] = ms.pid;
				i++;
			}else{
				send_mex(&ms, mypid, "ok", ms.pid);
			}
		}else if(sig == SIGUSR2){ //sblocca aereo in coda fifo
			s.count++;
			// printf("count:%d", s.count);
			if(s.fifo[j] != NULL){ //sblocco aereo se in coda
				send_mex(&ms, mypid, "ok", s.fifo[j]);
				printf("sblocco %d\n", s.fifo[j]);
				j++;
			}
		}
	}
	print_Event("torre", "fine", true);
}
