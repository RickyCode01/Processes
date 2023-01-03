// processo Hangar Aeroporto

void send_mex(struct message *pms, int pid, int num, char *text, int dest){
	/* fun to send struct message to pipe */
	memset(pms, '\0', sizeof(struct message)); // clean struct
	// write fileds
	pms->pid = pid;
	pms->child_n = num;
	strcpy(pms->mex, text);
	//printf("%d -> %d mex:%s\n", pms->pid, dest, pms->mex);
	// send message
	if((write(fdw, pms, sizeof(struct message))) < 0)perror("errore write:");
}

char get_random(char min, char max){
	/* fun to generate random value in range */
	struct timeval tv;
	gettimeofday(&tv, NULL);
	srand(tv.tv_usec); // set the seed for random generation 
	unsigned char r = min + (rand() % (max-min));
	return r;
}

void Aereo(char *id, int num, int ptorre){
	/* fun executed by childs */
	struct message ms;
	sigset_t sigset;

	print_Event(id, "avvio!", true);
	int mypid = getpid();
	//printf("\tpid:%d\n", mypid);
 
	// set blocked signal of child
	setSig(&sigset, SIGUSR1, SIGALRM, true);

	// waiting preparation 
	unsigned char myrand = get_random(3,8);
	print_Event(id, "preparazione ", false);
	printf("in %d secondi\n", myrand);
	alarm((unsigned int)myrand); // set alarm to awake aereo
	int sig = 0;
	while(sig != SIGALRM)sigwait(&sigset, &sig); // wait until SIGALRM is received
	print_Event(id, "pronto per decollo", true);
	//sigdelset(&sigset, SIGALRM); // remove sigalarm
	//printf("sig_check: %d\n", sigismember(&sigset, SIGALRM));

	send_mex(&ms, mypid, num, "ready", ptorre); // send ready to torre
	
	print_Event(id, "richiedo decollo a torre", true);

	sig = 0;
	sigwait(&sigset, &sig); // wait for signaling from torre
	if(sig == SIGUSR1){ // check signal
		// preparation to takeoff
		unsigned char myrand = get_random(5,15);
		print_Event(id, "decollo ", false);
		printf("in %d secondi\n", myrand);
		alarm((unsigned int)myrand); // set alarm to awake aereo
		while(sig != SIGALRM)sigwait(&sigset, &sig); // wait to sigalarm
		send_mex(&ms, mypid, num, "takeoff", ptorre); // send takeoff to torre
		print_Event(id, "decollato", true);
		return;
	}else{
		printf("segnale ricevuto: %d", sig); // signal debugging
	}
}

void Hangar(){
	int pid[childs];
	int status;
	int count;
	char child_str[10];

	if((fdw = open(myfifo, O_WRONLY)) < 0)perror("fdr error:"); // open fd pipe for writing

	print_Event("hangar", "avvio!", true);
	for(int i = 0; i < childs; i++){ // generate childs -> fork
		sprintf(child_str, "aereo %d", i);
		char child_event[25] = "creazione ";
		strcat(child_event, child_str);
		print_Event("hangar", child_event, true);
		pid[i]=fork();
		if(pid[i] == 0){
			Aereo(child_str, i, ptower);
			exit(1); // close childs when they finished 
		}
		sleep(2); // wait 2 sec before next childs
	}

	for(count = 0; count < childs; count++){ // wait childs to finish
		waitpid(pid[count] ,&status, 0); 
	}
	
	if(WIFEXITED(status) && count==childs){
		print_Event("hangar", "tutti gli aerei sono decollati", true);
		struct message mymex;
	 	send_mex(&mymex, getpid(), -1, "end", ptower); // airplanes terminated
	 	close(fdw);
	}

}

