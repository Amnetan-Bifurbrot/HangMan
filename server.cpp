// zgaduje server a client wiesza, bo inaczej sie srauo
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>

using namespace std;


void error(string msg){
    cerr << msg << endl;
    exit(1);
}


int main(int argc, char *argv[]){
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[255];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    if (argc < 2) {
        cerr << "ERROR - brak portu!" << endl;
        exit(1);
    }
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sockfd < 0) 
       error("Blad podczas otwierania gniazda");
       
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    
    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("Blad podczas bindowania");
        
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    
    newsockfd = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen);
    
    if (newsockfd < 0) 
        error("Blad akceptacji");
        
    cout << "Witaj w grze WISELEC" << endl;
    cout << "Jestes zgadyjacym!" << endl;
    cout << "Prosze podawac po jedej literze ;)" << endl;
 
    while(1){
        bzero(buffer,256);
        n = read(newsockfd,buffer,255);
        if (n < 0) 
			error("ERROR z czytaniem z gniazda");
        
        cout << "Wieszajcy: ";
        
        if(buffer[0] == 'q'){
			cout << "Uratowales ludzika! Gratualcje!" << endl;
			break;
		}
        // wypisujemy szyfrowane slowo
        for(int i = 0; i < strlen(buffer); i++){
			cout << buffer[i] << " ";
		}
        cout << endl;
        bzero(buffer,256); // czyscimy buffor
        cout << "Ja: ";
        cin >> buffer;
        
        n = write(newsockfd,buffer,strlen(buffer));
        if (n < 0) 
			error("ERROR z zapisaniem do gniazda");
			
        int i=strncmp("Pa" , buffer, 2);
        if(i == 0)
            break;
    }
    close(newsockfd);
    close(sockfd);
    return 0; 
}
