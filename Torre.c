// processo Torre di controllo Areporto

void receive_mex(struct message *pms){ // clean struct and send message
	memset(pms, '\0', sizeof(struct message));
	read(fdr, pms, sizeof(struct message));
}

int release_track(int *arr , int pid, bool free){
	int i;
	bool change = false;
	for(i = 0; i < tracks; i++){
		if(free && *(arr + i) == pid){
			//printf("sblocco %d\n", pid);
			*(arr + i) = 0;
			//for(int j = 0; j < tracks; j++){ printf("track %d -> %d\n",j , *(arr + j));}
			change = true;
			break;
		}else if(*(arr + i) == 0){
			*(arr + i) = pid;
			// for(int j = 0; j < tracks; j++){ printf("track %d -> %d\n",j , *(arr + j));}
			change = true;
			break;
		}
	}
	if(change == true)return i;
	else return -1;
}

void Torre(){
	print_Event("torre", "avvio!", true);
	int mypid = getpid();
	//printf("\tpid:%d\n",mypid);
	struct message ms; // message struct
	int track[tracks];
	int fifo[10];
	memset(&track, 0, sizeof(track)); // set all to false
	//for(int e = 0; e < tracks; e++){ printf("track %d -> %d\n",e , track[e]);}

	if((fdr = open(myfifo, O_RDONLY)) < 0)perror("fdr error: ");

	int i = 0, j = 0; // init queue index 
	int pista;

	// setSig(&sigset, SIGUSR1, 0, true); // set signals
	while(strcmp(ms.mex, "end") != 0){
		receive_mex(&ms);
		//printf("torre riceve da %d:%s\n", ms.pid, ms.mex);
		if(strcmp(ms.mex, "ready") == 0){
			pista = release_track(&track, ms.pid, false);
			if(pista < 0){ // add process to queue
				fifo[i] = ms.pid;
				//printf("accodato:%d\n",fifo[i]);
				i++;
			}else{
				//send_mex(&ms, mypid, "ok", ms.pid);
				print_Event("torre", "decollo autorizato per", false);
				printf(" %d su pista %d\n", ms.pid, pista);
				kill(ms.pid, SIGUSR1); // send sig to child to allow takeoff 
			}
		}else if(strcmp(ms.mex, "takeoff") == 0){ // check if childs in queue when takeoff
			release_track(&track, ms.pid, true);
			if(fifo[j] != NULL){ // unlock child in queue
				pista = release_track(&track, fifo[j], false);
				//for(int e = 0; e < tracks; e++){ printf("track %d -> %d\n",e , track[e]);}
				print_Event("torre", "decollo autorizato per", false);
				printf(" %d su pista %d\n", fifo[j], pista);
				kill(fifo[j], SIGUSR1);
				//printf("sblocco %d\n", fifo[j]);
				j++;
			}
		}
	}

	close(fdr);
	print_Event("torre", "fine", true);
}
