Progetto esame laboratorio sistemi operativi 
Anno 2021/2022
Docente Fabio Rossi 
Studente Manoni Riccardo
Matricola 331916

Per questo progetto ho scelto di utilizzare due principali strumenti visti duranti il corso per la comunicazione tra i vari processi: i segnali e le named pipe.
L'idea alla base è quella che gli aerei generati da hangar, possano comunicare con la torre per mezzo della named pipe utilizzando un file descriptor aperto in sola scrittura. La torre viceversa apre la named pipe con un file in sola lettura sfruttando l'attesa passiva dei messaggi nella pipe e comunica con gli aerei attraverso i segnali.
Questa scelta è dovuta a diversi fattori: il primo è che se comunicassi solo attraverso i segnali avrei dei problemi nella loro gestione in quanto, potrebbero essere generati piu segnali contemporaneamente dai vari figli e alcuni di essi andrebbero persi. 
La torre poi avrà difficoltà a comunicare con i figli solo utilizzando la pipe poichè ogni figlio è un diverso processo, mentre la coda dei messaggi è unica quindi per evitare complicazioni dovute alla caratterizzione dei messaggi ho scelto una strategia combinata.
La torre utilizzando la pipe per la ricezione dei messaggi sfrutta la logica FIFO che la pipe stessa implementa. Questo approccio oltre ad essere funzionale è anche completo poichè utilizza più strumenti visti durante il corso.

Struttura:
 
l'header(Aeroporto.h) contiene le costanti utili ai vari moduli del programma, come il numero dei figli generati e le piste disponibili, cosi da poter cambiare un solo parametro qualora si voglia testare il programma diversamente da quanto inizialmente impostato senza dover cambiare ogni riferimento. 
Inoltre esso contiene la dichiarazione della struttura di un messaggio che poi verrà utilizzata dagli aerei e dall'hangar per comunicare con la torre e dalla torre per ricevere i vari messaggi.
Le due funzioni presenti all'interno dell'header, Print_event e Sigset sono utilizzate da più processi rispettivamente per stampare con in formato adeguato i messaggi e per impostare la mascheratura dei segnali da bloccare.

Il programma padre(Aeroporto.c) contiene il codice iniziale per l'avvio dei processi Torre ed Hangar originati tramite le fork, le dichiarazioni delle funzioni globali dichiarate nell'header e gli header delle varie librerie C.
Inoltre nel main viene creata la named pipe e settata la struttura sigaction per determinare l'azione da compiere nel caso in cui i segnali SIGALRM e SIGUSR1 siano generati e in modo tale da condividere questo settaggio con i figli.

Torre.c è il modulo che viene eseguito dal primo figlio generato da Aeroporto.c e si occupa di gestire le richieste provenienti dagli aerei. Contiene due funzioni utili al suo funzionamento: receive_mex che permette di leggere un messaggio dalla pipe e get_runway per gestire l'array delle piste.
Per selezionare una pista libera è stato creato un array di interi contente i pid dei processi a cui viene assegnata una pista(prende spunto dalla compare_and_swap): quando un aereo richiede il decollo si cerca nell'array una pista libera(valore 0) e se questa è presente viene assegnata al all'aereo altrimenti il messaggio viene inserito in coda fifo in attesa che una pista si liberi. Nel caso invece che un aereo sia decollato si cerca un aereo pronto al decollo nella coda e se non presente si libera la pista per future richieste.

Hangar.c è il modulo che viene eseguito dal secondo figlio di Aeroporto.c e si occupa di creare e gestire il comportamento dei vari aerei(figli del processo hangar). 
Contiene le definizioni di due funzioni utili al suo funzionamento: send_message per scrivere un messaggio nella pipe e get_random per generare numeri randomici in un determinato range.
Questo modulo contiene il codice eseguito da ogni aereo dopo essere stato generato: inizialmente viene schedulato un segnale di alarm precedentemente bloccato tramite la funzione setSig e si rimane in attesa passiva con la sigwait fino alla sua ricezione.
A segnale ricevuto, viene inviato il messaggio di pronto al decollo alla torre.
Viene quindi attesa la ricezione del segnale SIGUSR1 dalla torre che concede il decollo e alla sua ricezione viene nuovamente schedulato un segnale di ALARM per l'attesa passiva del decollo.
Infine quando il segnale arriva l'aereo decolla e invia a torre il messaggio di decollo riuscito per poi terminare.
Il processo padre resta in attesa finche tutti i figli sono terminati a segnalare che tutti gli aerei sono decollati e prima di terminare invia a torre il messaggio di fine.








