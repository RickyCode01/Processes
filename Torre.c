// processo Torre di controllo Areporto

void receive_mex(struct message *pms){ 
	/* clean struct and take a message from pipe */
	memset(pms, '\0', sizeof(struct message));
	read(fdr, pms, sizeof(struct message));
}

int get_runway(int *arr , int old, int new){
	/* fun to handle runways */
	int i;
	bool change = false; // flag for changes
	//printf("%d -> %d\n", old, new);
	for(i = 0; i < runways; i++){
		if(*(arr + i) == old){
			*(arr + i) = new;
			//for(int j = 0; j < runways; j++){ printf("runway %d -> %d\n",j , *(arr + j));}
			change = true;
			break;
		}
	}
	if(change == true)return i;
	else return -1; // no changes -> error
}

void Torre(){
	print_Event("torre", "avvio!", true);
	int mypid = getpid();
	//printf("\tpid:%d\n",mypid);
	struct message ms; // message struct to save queue messages
	int runway[runways]; // runways array assigned to airplane
	struct message fifo[childs]; // queue of messages 
	memset(&runway, 0, sizeof(runway)); // initial reset runways
	fifo[0].pid = 0; // set value 0 for check end struct
	//for(int e = 0; e < runway; e++){ printf("runway %d -> %d\n",e , runway[e]);}

	if((fdr = open(myfifo, O_RDONLY)) < 0)perror("fdr error: "); // open fd fifo for reading

	int i = 0, j = 0; // init queue indexes
	int pista; // returned runway number from fun
	bool free; // boolean flag to check for free runway

	// setSig(&sigset, SIGUSR1, 0, true); // set signals
	while(strcmp(ms.mex, "end") != 0){ // wait for end message from hangar
		receive_mex(&ms); // passive waiting for messages in named pipe
		// printf("torre riceve da %d:%s\n", ms.pid, ms.mex);
		if(strcmp(ms.mex, "ready") == 0){ // check for ready messages from airplane
			print_Event("torre", "richiesta decollo ", false);
			printf("aereo %d ricevuta\n", ms.child_n);
			pista = get_runway(&runway, 0, ms.pid); // give runway to takeoff
			if(pista < 0){ // if runways aren't available
				fifo[i] = ms; // add process to queue
				//printf("accodato:%d\n",fifo[i].pid);
				i++;
				fifo[i].pid = 0; // set end of queue
				if(free)free = false; // if free was setted, reset it
			}else{
				print_Event("torre", "decollo autorizzato per", false);
				printf(" aereo %d su pista %d\n", ms.child_n, pista);
				kill(ms.pid, SIGUSR1); // send sig to allow takeoff
			}
		}else if(strcmp(ms.mex, "takeoff") == 0){
			if(fifo[j].pid != 0){ // check if airplane in queue when takeoff
				if(free){pista = get_runway(&runway, 0, fifo[j].pid); free = false;} // if a runway was free
				else pista = get_runway(&runway, ms.pid, fifo[j].pid); // swap runway between old and new child 
				//for(int e = 0; e < runway; e++){ printf("runway %d -> %d\n",e , runway[e]);}
				print_Event("torre", "decollo autorizzato per", false);
				printf(" aereo %d su pista %d\n", fifo[j].child_n, pista);
				kill(fifo[j].pid, SIGUSR1);
				//printf("sblocco %d\n", fifo[j]);
				j++;
			}else{
				free = true;
				get_runway(&runway, ms.pid, 0); //free runway
			}
		}
	}

	close(fdr);
	print_Event("torre", "terminazione", true);
}
