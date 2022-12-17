// processo Torre di controllo Areporto

struct mysem{
	int count;
	int fifo[10];
}s;

void receive_mex(struct message *pms){ // clean struct and send message
	memset(pms, '\0', sizeof(struct message));
	read(fdr, pms, sizeof(struct message));
}

void Torre(){
	print_Event("torre", "avvio!", true);
	int mypid = getpid();
	//printf("\tpid:%d\n",mypid);
	struct message ms; // message struct

	if((fdr = open(myfifo, O_RDONLY)) < 0)perror("fdr error: ");

	// sigset_t sigset; // set mask signal
	// int sig; // signal from sigwait
	int i = 0, j = 0; // init queue index 
	s.count = 2; //	init counter for railway resources

	// setSig(&sigset, SIGUSR1, 0, true); // set signals
	while(strcmp(ms.mex, "end") != 0){
		// sig = 0; //reset signal
		// sigwait(&sigset, &sig); // wait signals signal
		receive_mex(&ms);
		//printf("torre riceve da %d:%s\n", ms.pid, ms.mex);
		if(strcmp(ms.mex, "ready") == 0){
			s.count--;
			//printf("count:%d\n", s.count);
			if(s.count < 0){ // add process to queue
				s.fifo[i] = ms.pid;
				//printf("accodato:%d\n",s.fifo[i]);
				i++;
			}else{
				//send_mex(&ms, mypid, "ok", ms.pid);
				kill(ms.pid, SIGUSR1); // send sig to child to allow takeoff 
			}
		}else if(strcmp(ms.mex, "takeoff") == 0){ // check if childs in queue when takeoff
			s.count++;
			//printf("count:%d", s.count);
			if(s.fifo[j] != NULL){ // unlock child in queue
				//send_mex(&ms, mypid, "ok", s.fifo[j]);
				kill(s.fifo[j], SIGUSR1);
				//printf("sblocco %d\n", s.fifo[j]);
				j++;
			}
		}
	}

	close(fdr);
	print_Event("torre", "fine", true);
}
