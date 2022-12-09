// processo Torre di controllo Areporto

struct mysem{
	int count;
	int fifo[10];
}s;

void Torre(){
	print_Event("torre", "avvio!", false);
	int mypid = getpid();
	printf("\tpid:%d\n",mypid);
	//write(fdw, getpid(), sizeof(getpid()));
	struct message ms; // message struct
	sigset_t sigset; // struct for set signals
	memset(&ms, '\0', sizeof(struct message)); //reset message struct

	int i = 0, j = 0; // init queue index 
	s.count = 2; //	init counter for railway resources
	setSig(&sigset, SIGUSR1, true); // set SIGUSR1 signal
	while(strcmp(ms.mex, "end") != 0){
		sigwait(&sigset); // wait SIGUSR1 signal 
		receive_mex(&ms); 
		//printf("torre read from %d:%s\n", ms.pid, ms.mex);
		/* se piu di un signal viene inviato contemporaneamente uno dei 
		due si perde -> risolvere */
		if(strcmp(ms.mex, "ready") == 0){
			s.count--;
			//printf("count:%d\n", s.count);
			if(s.count < 0){ // add process to queue
				s.fifo[i] = ms.pid;
				i++;
			}else{
				send_mex(&ms, mypid, "ok", ms.pid);
			}
		}else if(strcmp(ms.mex, "ok") == 0){ //sblocca aereo in coda fifo
			s.count++;
			//printf("count:%d", s.count);
			if(s.fifo[j] != NULL){ //sblocco aereo se in coda
				send_mex(&ms, mypid, "ok", s.fifo[j]);
				printf("sblocco %d\n", s.fifo[j]);
				j++;
			}
		}else if (strcmp(ms.mex, "resend") == 0){
			printf("%d resend\n", ms.pid);
			send_mex(&ms, mypid, "ok", ms.pid);
		}
	}
	print_Event("torre", "fine", true);
}
