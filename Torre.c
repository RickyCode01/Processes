// processo Torre di controllo Areporto

#include <stdio.h>
void receive_mex(struct message *pms){ // clean struct and send message
	memset(pms, '\0', sizeof(struct message));
	read(fdr, pms, sizeof(struct message));
}

int get_track(int *arr , int old, int new){
	int i;
	bool change = false;
	//printf("%d -> %d\n", old, new);
	for(i = 0; i < tracks; i++){
		if(*(arr + i) == old){
			*(arr + i) = new;
			//for(int j = 0; j < tracks; j++){ printf("track %d -> %d\n",j , *(arr + j));}
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
	struct message fifo[childs];
	memset(&track, 0, sizeof(track)); // set all to false
	fifo[0].pid = 0; // pid for NULL struct	
	//for(int e = 0; e < tracks; e++){ printf("track %d -> %d\n",e , track[e]);}

	if((fdr = open(myfifo, O_RDONLY)) < 0)perror("fdr error: ");

	int i = 0, j = 0; // init queue index 
	int pista;
	bool free = false;

	// setSig(&sigset, SIGUSR1, 0, true); // set signals
	while(strcmp(ms.mex, "end") != 0){
		receive_mex(&ms);
		//printf("torre riceve da %d:%s\n", ms.pid, ms.mex);
		if(strcmp(ms.mex, "ready") == 0){
			pista = get_track(&track, 0, ms.pid);
			if(pista < 0){ 
				fifo[i] = ms; // add process to queue
				//printf("accodato:%d\n",fifo[i].pid);
				i++;
				fifo[i].pid = 0; // set end of queue 
			}else{
				//send_mex(&ms, mypid, "ok", ms.pid);
				print_Event("torre", "decollo autorizzato per", false);
				printf(" aereo %d su pista %d\n", ms.child_n, pista);
				kill(ms.pid, SIGUSR1); // send sig to child to allow takeoff 
			}
		}else if(strcmp(ms.mex, "takeoff") == 0){ // check if childs in queue when takeoff
			if(fifo[j].pid != 0){ // if child in queue 
				if(free){pista = get_track(&track, 0, fifo[j].pid); free = false;} // if a track was free
				else pista = get_track(&track, ms.pid, fifo[j].pid); // swap track between old and new child 
				//for(int e = 0; e < tracks; e++){ printf("track %d -> %d\n",e , track[e]);}
				print_Event("torre", "decollo autorizzato per", false);
				printf(" aereo %d su pista %d\n", fifo[j].child_n, pista);
				kill(fifo[j].pid, SIGUSR1);
				//printf("sblocco %d\n", fifo[j]);
				j++;
			}else{
				free = true;
				get_track(&track, ms.pid, 0); //free track
			}
		}
	}

	close(fdr);
	print_Event("torre", "fine", true);
}
