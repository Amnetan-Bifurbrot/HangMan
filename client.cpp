#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

using namespace std;


void error(string msg){
    cerr << msg << endl;
    exit(0);
}


int main(int argc, char *argv[]){
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[255]; 			//sluzy do komunikacji
    char litera[1];				//sluzy do wysylania liter
    
    //*********************LACZENIE SIE*******************************
    
    if (argc < 3) {
		cerr << "Niepoprawna ilosc argumentow" << endl;
		cerr << "Skladnia: NazwaProgramu IP port" << endl;
		exit(0);
    }
    
    portno = atoi(argv[2]);			//port
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sockfd < 0) 
        error("Blad podczas otwierania gniazda");
        
    server = gethostbyname(argv[1]);
    
    if (server == NULL) {
		cerr << "ERROR - brak takiego hosta" << endl;
        exit(0);
    }
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_port = htons(portno);
    
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("Blad polaczenia \n");
    
    
    //*********************LACZENIE SIE******************************
    
    cout << "*********************" << endl;        
    cout << "Witaj w grze WISIELEC" << endl;
    cout << "Jestes zgadujacym!" << endl;
    cout << "*********************" << endl;    

	bzero(buffer,255);								
    n = write(sockfd,"a",1);											//0. probne wyslanie czegokolwiek
    if (n < 0) 
             error("ERROR z pisaniem do gniazda\n");
    bzero(buffer,255);													//czyszczenie bufora
    
    cout << "Oczekiwanie na wybranie slowa przez wieszajacego..." << endl;


    n = read(sockfd,buffer,255);										//1. odebranie slowa
								
    if (n < 0) 
        error("ERROR z czytaniem z gniazda\n");
	
	cout << "Slowo: " ;
	for(int i = 0; i < strlen(buffer); i++){
			cout << buffer[i] << " ";
		}
    cout << endl;
       
    while(1){                  
        cout << "Podaj litere: ";
        
        cin >> litera;													//wpisanie litery
		
		bzero(buffer,255);
        n = write(sockfd,litera,strlen(litera));						//2. wyslanie litery
        
        if (n < 0) 
             error("ERROR z pisaniem do gniazda\n");
             
        bzero(buffer,255);  
        n = read(sockfd,buffer,255);									//3. odebranie informacji zwrotnej z servera
        
        if (n < 0) 
			error("ERROR z czytaniem z gniazda");
		
		//cout << buffer; // TO NIE WYPISZE CALEGO BUFORA LADNIE, trzeba dac " "
		
		for(int i = 0; i < strlen(buffer); i++){
			cout << buffer[i] << " ";
		}
		cout << endl;
			
		/*if(buffer[0] == NULL){
			cout << "kuniec";
			break;
        }*/
    }
    close(sockfd);
    return 0;
}
