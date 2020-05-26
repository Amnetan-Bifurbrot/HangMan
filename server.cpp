#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sstream>

using namespace std;


void error(string msg){
    cerr << msg << endl;
    exit(1);
}


int main(int argc, char *argv[]){
    unsigned int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[255];		//sluzy do komunikacji
    char buffer0[255];		//slowo do zgadniecia
    //char used[255];			//przechowuje wykorzystane literki
    bool guessed[255];		//sluzy do zaslaniania literek
    
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    if (argc < 2) {
        cerr << "Nie podano numeru portu!" << endl;
        exit(1);
    }
    
     
    //********************LACZENIE SIE******************************
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sockfd < 0) 
       error("Blad podczas otwierania gniazda");
       
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    
    n = bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr));
    
    if (n < 0) 
        error("Blad podczas bindowania");
        
    cout << "Oczekiwanie na dolaczenie zgadujacego..." << endl;
        
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    
    if (newsockfd < 0) 
        error("Blad akceptacji");
        
    //*********************LACZENIE SIE**************************
        
    cout << "*********************" << endl;    
    cout << "Witaj w grze WISIELEC" << endl;
    cout << "Jestes wieszajacym!" << endl;
    cout << "*********************" << endl;  
    
    cout << "Podaj slowo do zgadywania:" << endl;
    
    bzero(buffer,255); 													//czyszczenie bufora
    cin >> buffer0; 													//podanie slowa

    for(int i = 0; i < 255; i++){
		guessed[i] = false;												//ustawienie guessed na same false-y
	}
	        
	char slowo[strlen(buffer0)];
    for(unsigned int i = 0; i < strlen(buffer0); i++){
		slowo[i] = '_';
	}
	
	n = read(newsockfd,buffer,255);										//0. probne odczytanie czegokolwiek
	cout << "probnie odczytano " << buffer << endl;
	if (n < 0) 
        error("ERROR z czytaniem z gniazda\n");
	bzero(buffer,255);
	cout << "przed 1" << endl;
	n = write(newsockfd,slowo,strlen(buffer0));							//1. wyslanie __________
	cout << "p0 1" << endl;
	if (n < 0) 
             error("ERROR z pisaniem do gniazda\n");

	//bzero(buffer,255);

    while(1){															//zaczynamy zgadywanie
		cout << "przed 2" << endl;
        bzero(buffer,255);
        n = read(newsockfd,buffer,255);									//2. odebranie literki i zapisanie jej w buforze 
        if (n < 0) 
			error("ERROR z czytaniem z gniazda");
			
		cout << "Zgadujacy wybral literke " << buffer << endl;
        
        unsigned int counter = 0;										//counter do liczenia ilosci pasujacych literek
        for(unsigned int i = 0; i < strlen(buffer0); i++){				//sprawdzenie czy literka pasuje do slowa
			if(buffer0[i] == buffer[0]){
				guessed[i] = true;										//jesli literka na danym miejscu w slowie sie zgadza, to bedzie ona wyswietlana
				cout << "Zgadza sie" << i << endl;
				counter++;
			}
		}
		
		bzero(buffer,255);
        
		if(counter == strlen(buffer0)){
			cout << "Koniec gry - zgadujacemu udalo sie zgadnac slowo!" << endl;

			n = write(newsockfd,NULL,1);									//3. wyslanie NULLa zeby zastopowac wszystko (wygrana/przegrana zgadujacego)
			break;														//na razie jest tylko wygrana, ale to sie dorobi
		}
		
		for(unsigned int i = 0; i < strlen(buffer0); i++){				//formowanie slowa do wyslania
			if(guessed[0])
				slowo[i] = buffer0[i];
			else
				slowo[i] = '_';
		}
		
		cout << slowo << endl;
		n = write(newsockfd,slowo,strlen(slowo));							//3. wyslanie slowa z powrotem
		
    }
    close(newsockfd);
    close(sockfd);
    return 0; 
}
