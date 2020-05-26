#include <iostream>
#include <stdlib.h>
// zgaduje server a client wiesza, bo inaczej sie srauo
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

    char buffer[256];
    char buffer0[256]; // zawiera pierwotne slowo do zgadniecia
    
    if (argc < 3) {
		cerr << "Niepoprawna ilosc argumentow" << endl;
		cerr << "./program IP port" << endl;
		exit(0);
    }
    
    portno = atoi(argv[2]);
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
    
    cout << "Witaj w grze WISELEC" << endl;
    cout << "Jestes wieszajacym!" << endl;
   
    //cout << "Podaj prosze kategorie" << endl; na razie bez tego
    
    bzero(buffer,256); // czyszczenie buffora
    cout << "Ja: ";
    cin >> buffer0;
    
        
	char slowo[strlen(buffer0)];
    for(int i = 0; i < strlen(buffer0); i++){
		slowo[i] = '_';
		//cout << slowo[i] << " ";
		
	}
	//cout << endl;
	n = write(sockfd,slowo,strlen(buffer0));
    int counter = 0;    
    while(1){
		
        
             
        bzero(buffer,256);
        n = read(sockfd,buffer,255);
        
        if (n < 0) 
             error("ERROR z czytaniem z gniazda\n");
             
        cout << "Zgadujacy: " << buffer << endl;

        cout << "Ja: ";
        
        for(int i = 0; i < strlen(buffer0); i++){
			if(buffer0[i] == buffer[0]){			
				slowo[i] = buffer[0];
				counter++;
			}
		}
        bzero(buffer,256);  // czyszczenie buffora
        
        if(counter == strlen(buffer0)){
			cout << "Koniec gry"<< endl;
			char stop[1];
			stop[0]  = 'q';
			n = write(sockfd,stop,strlen(stop));
			break;
		}
           
        n = write(sockfd,slowo,strlen(buffer0));
        
        if (n < 0) 
             error("ERROR z pisaniem do gniazda\n");
             
        // wypisanie "Pa" wychodzi z programu - chyab obie stroy to musza zrobic
        int i = strncmp("Pa" , buffer , 2);
        if(i == 0)
            break;
    }
    close(sockfd);
    return 0;
}
