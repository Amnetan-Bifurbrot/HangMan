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
    int sockfd, portno, n;		//gniazdo clienta, numer portu, zmienna pomocnicza
    struct sockaddr_in serv_addr;//adres clienta
    struct hostent *server;		//host (chyba)

    char buffer[255]; 			//sluzy do komunikacji
    char litera[1];				//sluzy do wysylania liter
    




    
    if (argc < 3) {
		cerr << "Niepoprawna ilosc argumentow" << endl;
		cerr << "Skladnia: NazwaProgramu IP port" << endl;
		exit(0);
    }
    
    portno = atoi(argv[2]);			//port
    sockfd = socket(AF_INET, SOCK_STREAM, 0);							//otwieranie gniazda
    
    if (sockfd < 0) 
        error("Blad podczas otwierania gniazda");
        
    server = gethostbyname(argv[1]);									//odnajdywanie hosta
    
    if (server == NULL) {
		cerr << "Blad podczas wyszukiwania hosta" << endl;
        exit(0);
    }
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_port = htons(portno);
    
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) //laczenie sie z serverem
        error("Blad polaczenia \n");
       





    
    cout << "*************************" << endl;        
    cout << "* Witaj w grze WISIELEC *" << endl;
    cout << "*  Jestes zgadujacym!   *" << endl;
    cout << "*************************" << endl;    

	bzero(buffer,255);								
    n = write(sockfd,"a",1);											//0. probne wyslanie czegokolwiek
    if (n < 0) 
             error("Blad pisania do gniazda\n");
   
    cout << "Oczekiwanie na wybranie slowa przez wieszajacego..." << endl;

	bzero(buffer,255);
    n = read(sockfd,buffer,255);										//1. odebranie slowa							
    if (n < 0) 
        error("Blad czytania z gniazda\n");
	
											
	int k = 0;															//zmienna zwiazana z wypisywaniem (bo czasem cos sie psulo jak server wysylal kod 4)
    while(1){          
		
		cout << "Slowo: ";
      
		for(unsigned int i = k; i < strlen(buffer) - 1; i++){
			cout << buffer[i] << " ";
		}

		cout << endl << "Ilosc mozliwych bledow do wykorzystania: " << buffer[strlen(buffer) - 1] << endl;	
		        
        cout << "Podaj litere: ";
        
        cin >> litera;													//wpisanie litery
		
		bzero(buffer,255);
        n = write(sockfd,litera,strlen(litera));						//2. wyslanie litery
        if (n < 0) 
             error("Blad pisania do gniazda\n");
             
        bzero(buffer,255);  
        n = read(sockfd,buffer,255);									//3. odebranie informacji zwrotnej z servera
        if (n < 0) 
			error("Blad czytania z gniazda");
		k = 0;
		if(buffer[0] == '2'){											//wygrana
			cout << "Udalo Ci sie odgadnac slowo!" << endl << "Slowo: ";
			for(unsigned int i = 1; i < strlen(buffer) - 1; i++){
			cout << buffer[i] << " ";
			}
			break;
        }else if(buffer[0] == '1'){										//niepoprawny znak
			cout << "Nie wyslales litery!" << endl;
			continue;
		}else if(buffer[0] == '3'){										//koniec szans na blad
			cout << "Skonczyly Ci sie szanse!" << endl;
			break;
		}else if(buffer[0] == '4'){
			cout << "Tej litery nie ma w zgadywanym slowie!" << endl;	//utrata jednej szansy
			k = 1;
		}
    }
    close(sockfd);
    return 0;
}
