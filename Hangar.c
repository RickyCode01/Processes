// processo Hangar Aeroporto

void send_mex(struct message *pms, int source, char *text, int dest){
	//sem_wait(&mutex);
	memset(pms, '\0', sizeof(struct message));
	pms->pid = source;
	strcpy(pms->mex, text);
	printf("%d -> %d mex:%s\n", pms->pid, dest, pms->mex);;
	//sem_post(&mutex);
	if((write(fdw, pms, sizeof(struct message))) < 0)perror("errore write:");
	// kill(dest, SIGUSR1);
}

char get_random(char min, char max){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	srand(tv.tv_usec); // set the seed for random generation 
	unsigned char r = min + (rand() % (max-min));
	return r;
}

void Aereo(int id, int ptorre){
	char sid[10];
	struct message ms;
	//memset(&ms, '\0', sizeof(struct message));
	sigset_t sigset;

	sprintf(sid, "aereo %d", id);
	print_Event(sid, "avvio!", false);
	int mypid = getpid();
	printf("\tpid:%d\n", mypid);

	setSig(&sigset, SIGUSR1, SIGALRM, true);

	// waiting preparation 
	unsigned char myrand = get_random(3,8);
	print_Event(sid, "preparazione ", false);
	printf("in %d secondi\n", myrand);
	alarm((unsigned int)myrand); // set alarm to awake aereo
	int sig = 0;
	while(sig != SIGALRM)sigwait(&sigset, &sig); // wait until SIGALRM is received
	print_Event(sid, "pronto per decollo", true);
	//sigdelset(&sigset, SIGALRM); // remove sigalarm
	//printf("check: %d\n", sigismember(&sigset, SIGALRM));

	//pthread_mutex_trylock(&mutex);
	send_mex(&ms, mypid, "ready", ptorre); // send mex to torre
	//pthread_mutex_unlock(&mutex);
	
	print_Event(sid, "richiesta decollo inviata", true);

	/*while(true){
		alarm(2);
		sigwait(&sigset, &sig);
		if(sig == SIGALRM)send_mex(&ms, mypid, "ready", ptorre);
		else break;
	}*/
	sig = 0;
	sigwait(&sigset, &sig);
	//receive_mex(&ms);
	//printf("%d <- %d:%s\n", mypid, ms.pid, ms.mex);
	if(/*strcmp(ms.mex, "ok") == 0 && ms.pid == ptorre*/ sig == SIGUSR1){
		//waiting take off
		unsigned char myrand = get_random(5,15);
		print_Event(sid, "decollo ", false);
		printf("in %d secondi\n", myrand);
		//setSig(&sigset, SIGALRM, true);
		alarm((unsigned int)myrand); // set alarm to awake aereo
		//sig = 0; //reset signal 
		while(sig != SIGALRM)sigwait(&sigset, &sig);
		send_mex(&ms, mypid, "takeoff", ptorre);
		print_Event(sid, "decollato", true);
		return;
	}else{
		printf("messaggi non sincronizzati\n");
	}
}

void Hangar(){
	int pid[10];
	int *status;
	int ptorre = getpid()-1;
	int w;
	// const int proc = 10;

	if((fdw = open(myfifo, O_WRONLY)) < 0)perror("fdr error:");
	//sem_open(mysema, O_CREAT, S_IRWXU, 1);

	//while(read(fdr, ptorre, sizeof(int)) < 0);
	print_Event("hangar", "avvio!", true);
	for(int i = 0; i < childs; i++){
		pid[i]=fork();
		if(pid[i] == 0){
			Aereo(i, ptorre);
			exit(1); // chiude i figli
		}
	}

	for(w = 0; w < childs; w++){ // wait processes to finish
		waitpid(pid[w] ,&status, 0); 
	}
	
	if(WIFEXITED(status) && w==childs){
		//sem_close(&mutex);
		struct message mymex;
	 	send_mex(&mymex, getpid(), "end", ptorre);
	 	close(fdw);
	}

}

