// processo Hangar Aeroporto

char get_random(char min, char max){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	srand(tv.tv_usec); // set the seed for random generation 
	unsigned char r = min + (rand() % (max-min));
	return r;
}

void Aereo(int id, struct message *pms, int ptorre){

	char sid[10];
	sigset_t sigset;
	//char buff[SIZE];
	sprintf(sid, "aereo %d", id);
	print_Event(sid, "avvio!", false);
	int mypid = getpid();
	printf("\tpid:%d\n", mypid);
	unsigned char myrand = get_random(3,8);
	print_Event(sid, "preparazione ", false);
	printf("in %d secondi\n", myrand);

	sigemptyset(&sigset);
	sigaddset(&sigset, SIGALRM);
	sigaddset(&sigset, SIGUSR1);
	sigprocmask(SIG_BLOCK, &sigset, NULL);

	//setSig(&sigset, SIGALRM, true);
	alarm((unsigned int)myrand); // set alarm to awake aereo
	int sig = 0;
	while(sig != SIGALRM)sigwait(&sigset, &sig); // wait until SIGALRM is received
	print_Event(sid, "pronto per decollo", true);
	//sigdelset(&sigset, SIGALRM); // remove sigalarm
	//printf("check: %d\n", sigismember(&sigset, SIGALRM));

	send_mex(pms, mypid, "ready", ptorre); // send mex to torre
	//checkSig(ptorre, 1);
	
	print_Event(sid, "richiesta decollo inviata", true);

	//setSig(&sigset, SIGUSR1, true);
	while(true){
		alarm(2);
		sigwait(&sigset, &sig);
		if(sig == SIGALRM)send_mex(pms, mypid, "resend", ptorre);
		else break;
	}
	receive_mex(pms);

	//printf("ms.pid:%d, ms.mex:%s\n", pms->pid, pms->mex);
	if(strcmp(pms->mex, "ok") == 0 && pms->pid == ptorre){
		unsigned char myrand = get_random(5,15);
		print_Event(sid, "decollo ", false);
		printf("in %d secondi\n", myrand);
		//setSig(&sigset, SIGALRM, true);
		alarm((unsigned int)myrand); // set alarm to awake aereo
		sig = 0; //reset signal 
		while(sig != SIGALRM)sigwait(&sigset, &sig);
		send_mex(pms, mypid, "ok", ptorre);
		print_Event(sid, "decollato", true);
		return;
	}
}

void Hangar(){
	int pid[10];
	int *status;
	int ptorre = getpid()-1;
	struct message ms;
	 
	//while(read(fdr, ptorre, sizeof(int)) < 0);
	print_Event("hangar", "avvio!", true);
	for(int i = 0; i < 3; i++){
		pid[i]=fork();
		if(pid[i] == 0){
			Aereo(i, &ms, ptorre);
			return;
		}
	}
	for(int i = 0; i < 3; i++){
		waitpid(pid[i] ,&status ,0); // wait all process to finish
		printf("finish %d\n", pid[i]);
	}
	
	if(WIFEXITED(status)){
		printf("here!");
	 	send_mex(&ms, getpid(), "end", ptorre);
	}

}

