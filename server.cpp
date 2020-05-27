#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>


using namespace std;


void error(string msg){
    cerr << msg << endl;
    exit(1);
}


int main(int argc, char *argv[]){
    int sockfd, newsockfd, portno;	//gniazdo servera, gniazdo clienta i numer portu
    char lvl;				//poziom trudnosci (ilosc mozliwych bledow)
    socklen_t clilen;
    char buffer[255];		//sluzy do komunikacji
    char buffer0[255];		//slowo do zgadniecia
    char used[255];			//przechowuje wykorzystane literki
    bool guessed[255];		//sluzy do zaslaniania literek
    
    struct sockaddr_in serv_addr, cli_addr;	//adresy servera i clienta
    int n;
    if (argc < 2) {
        cerr << "Nie podano numeru portu!" << endl;
        exit(1);
    }
    
    
    
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);							//otwieranie gniazda servera
    
    if (sockfd < 0) 
       error("Blad podczas otwierania gniazda");
       
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    
    n = bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr));	//bindowanie
    
    if (n < 0) 
        error("Blad podczas bindowania");
        
    cout << "Oczekiwanie na dolaczenie zgadujacego..." << endl;
        
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);	//otwieranie gniazda clienta kiedy sie polaczy
    
    if (newsockfd < 0) 
        error("Blad akceptacji");
        



    
    cout << "*************************" << endl;    
    cout << "* Witaj w grze WISIELEC *" << endl;
    cout << "*  Jestes wieszajacym!  *" << endl;
    cout << "*************************" << endl;  
    
    cout << "Podaj slowo do zgadywania:" << endl;
    
    bzero(buffer,255); 													//czyszczenie bufora
    cin >> buffer0; 													//podanie slowa
	
	unsigned int l = strlen(buffer0);									//dlugosc podanego slowa
	
    for(unsigned int i = 0; i < l; i++){
		guessed[i] = false;												//ustawienie guessed na same false-y
	}
	        
	char slowo[l + 1];													//+ 1 bo jeszcze miejsce na informacje o ilosci mozliwych bledow
    for(unsigned int i = 0; i < l; i++){
		slowo[i] = '_';													//zaszyfrowanie slowa
	}
	
	while(1){															//podanie poziomu trudnosci (ilosci bledow)
		cout << "Podaj ilosc bledow jakie moze popelnic zgadujacy (0 - 9): ";
		cin >> lvl;
		if(!isdigit(lvl)) cout << "Nie podales cyfry!" << endl;
		else break;
	}	
	
	slowo[l] = lvl;														//wstawiamy ilosc szans na blad na koniec slowa

	n = read(newsockfd,buffer,255);										//0. probne odczytanie czegokolwiek - inaczej server nie dziala
	if (n < 0) 
        error("Blad czytania z gniazda\n");
	
	bzero(buffer,255);
	n = write(newsockfd,slowo,l + 1);									//1. wyslanie zaszyfrowanego slowa i ilosci szans
	if (n < 0) 
             error("Blad pisania do gniazda\n");

	unsigned int counter = 0;
    while(1){															//zaczynamy zgadywanie
		bzero(buffer,255);
        n = read(newsockfd,buffer,255);									//2. odebranie literki 
        if (n < 0) 
			error("Blad czytania z gniazda");
			
		if(!isalpha(buffer[0])){	
			bzero(buffer,255);								
			n = write(newsockfd, "1", 1);								//3a. jesli zgadujacy nie wyslal literki, to wysylamy mu '1', zeby wiedzial ze zrobil zle
		}
		
		bool wasUsed = false;
		for(unsigned int i = 0; i < strlen(used); i++){					//sprawdzamy czy dana litera juz sie nie pojawila
			if(used[i] == buffer[0]){
				wasUsed = true;
				break;
			}
		}
		
		if(wasUsed && isalpha(buffer[0])){ 								//3f. jesli zgadujacy wyslal litere ktora juz wczesniej zostala uzyta
			bzero(buffer,255);
			n = write(newsockfd, "5", 1);
		}						
		else {
			used[strlen(used)] = buffer[0];
		}

		
		cout << "Zgadujacy wybral literke " << buffer << endl;
		unsigned int oldcounter = counter;
        counter = 0;													//counter do liczenia ilosci pasujacych literek        
        for(unsigned int i = 0; i < l; i++){							//sprawdzenie czy literka pasuje do slowa
			if(buffer0[i] == buffer[0]){
				guessed[i] = true;										//jesli literka na danym miejscu w slowie sie zgadza, to bedzie ona wyswietlana
			}
			
			if(guessed[i]) counter++;									//liczenie ilosci zgadzajacych sie literek
		}
		
		
		
		if(counter == l){
			cout << "Koniec gry - zgadujacemu udalo sie zgadnac slowo!" << endl;
			bzero(buffer,255);
			n = write(newsockfd,"2",1);									//3b. wyslanie informacji o wygranej zgadujacego
																
		}else if(counter - oldcounter == 0 && !wasUsed && isalpha(buffer[0])){
			lvl--;
			if(lvl - 48 < 0) {											//3c. wyslanie informacji o przegranej (utracie wszystkich szans na blad) (lvl - 48 bo ASCII)
				bzero(buffer,255);
				n = write(newsockfd,"3",1);
				cout << "Zgadujacy utracil wszystkie szanse i przegrywa!" << endl; 
			}				
			else {														//3d. wyslanie informacji o utracie jednej szansy na blad
				bzero(buffer,255);
				n = write(newsockfd,"4",1);							
				cout << "Zgadujacy utracil jedna szanse na blad!" << endl;
			}
		}
		if (n < 0) 
				error("Blad pisania do gniazda\n");
		
		for(unsigned int i = 0; i < l; i++){							//formowanie slowa do wyslania
			if(guessed[i])
				slowo[i] = buffer0[i];
			else
				slowo[i] = '_';
		}
		
		slowo[l] = lvl;
		//cout << slowo << endl;
		for(unsigned int i = 0; i < strlen(used); i++){
			slowo[l + 1 + i] = used[i];
		}
		bzero(buffer,255);
		cout << "buffer: " << buffer;
		n = write(newsockfd,slowo,l + 1 + strlen(used));				//3e. wyslanie slowa razem z iloscia szans i lista wykorzystanych liter z powrotem
		if (n < 0) 
			error("Blad pisania do gniazda\n");
    }
    close(newsockfd);
    close(sockfd);
    return 0; 
}
