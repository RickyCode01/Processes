// processo Hangar Aeroporto

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
	memset(&ms, '\0', sizeof(struct message));
	sigset_t sigset;

	sprintf(sid, "aereo %d", id);
	print_Event(sid, "avvio!", false);
	int mypid = getpid();
	printf("\tpid:%d\n", mypid);

	// set signals for waiting and sync
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGALRM);
	sigaddset(&sigset, SIGUSR1);
	sigprocmask(SIG_BLOCK, &sigset, NULL);

	// waiting preparation 
	unsigned char myrand = get_random(3,8);
	print_Event(sid, "preparazione ", false);
	printf("in %d secondi\n", myrand);
	//setSig(&sigset, SIGALRM, true);
	alarm((unsigned int)myrand); // set alarm to awake aereo
	int sig = 0;
	while(sig != SIGALRM)sigwait(&sigset, &sig); // wait until SIGALRM is received
	print_Event(sid, "pronto per decollo", true);
	//sigdelset(&sigset, SIGALRM); // remove sigalarm
	//printf("check: %d\n", sigismember(&sigset, SIGALRM));

	//pthread_mutex_trylock(&mutex);
	send_mex(&ms, mypid, "ready", ptorre); // send mex to torre
	//pthread_mutex_unlock(&mutex);
	//checkSig(ptorre, 1);
	
	print_Event(sid, "richiesta decollo inviata", true);

	//setSig(&sigset, SIGUSR1, true);
	/*while(true){
		alarm(2);
		sigwait(&sigset, &sig);
		if(sig == SIGALRM)send_mex(&ms, mypid, "ready", ptorre);
		else break;
	}*/

	sigwait(&sigset, &sig);
	receive_mex(&ms);
	printf("%d here! ", mypid);

	printf("ms.pid:%d, ms.mex:%s\n", ms.pid, ms.mex);
	if(strcmp(ms.mex, "ok") == 0 && ms.pid == ptorre){

		//waiting take off
		unsigned char myrand = get_random(5,15);
		print_Event(sid, "decollo ", false);
		printf("in %d secondi\n", myrand);
		//setSig(&sigset, SIGALRM, true);
		alarm((unsigned int)myrand); // set alarm to awake aereo
		sig = 0; //reset signal 
		while(sig != SIGALRM)sigwait(&sigset, &sig);

		send_mex(&ms, mypid, "ok", ptorre);
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
	const int proc = 4;
	struct message ms;
	memset(&ms, '\0', sizeof(struct message));
	 
	//while(read(fdr, ptorre, sizeof(int)) < 0);
	print_Event("hangar", "avvio!", true);
	for(int i = 0; i < proc; i++){
		pid[i]=fork();
		if(pid[i] == 0){
			Aereo(i, ptorre);
			exit(1); // chiude i figli
		}
	}

	for(w = 0; w < proc; w++){ // wait processes to finish
		waitpid(pid[w] ,&status, 0); 
	}
	
	if(WIFEXITED(status) && w==proc){
		//printf("here!");
	 	send_mex(&ms, getpid(), "end", ptorre);
	}

}

