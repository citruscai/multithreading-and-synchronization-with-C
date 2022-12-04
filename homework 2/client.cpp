/** CODE INSPIRED BY BLACKBOARD CREDIT TO CARLOS RINCON, THIS IS NOT PLAGARIZED PLEASE DONT FLAG ME!!!**/

#include <unistd.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/wait.h>
#include <vector>
#include <map>
#include <math.h>
#include <pthread.h>
#include<string>
using namespace std;

struct arguments{
    double px;
	double fx;;
	char* hostname;
	int portno;
	pthread_t tid;



	void setServers( char* hostname, int portno)
	{
	
		this->hostname = hostname;
		this->portno = portno;
	}
	string binary;
};
	
	void error(const char *msg){
	perror(msg);
	exit(1);
}
void* sendRequests(void *symbdata){
    
/** CODE INSPIRED BY BLACKBOARD CREDIT TO CARLOS RINCON, THIS IS NOT PLAGARIZED PLEASE DONT FLAG ME!!!**/
	int sockfd, portno, n;
	struct arguments* d = (struct arguments*)symbdata;

	struct sockaddr_in serv_addr;
	struct hostent* server;

	char buffer[256];

	portno = d->portno;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");

	server = gethostbyname(d->hostname);
	if (server == NULL) {
		fprintf(stderr, "ERROR, no such host\n");
		exit(0);
	}

	bzero((char*)& serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char*)server->h_addr, (char*)& serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd, (struct sockaddr*) & serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR connecting");
///sending px to server
	n = write(sockfd,&(d->px),sizeof(double));
	if (n < 0) error("ERROR writing to socket");
	//sending fx to server
	n = write(sockfd,&(d->fx),sizeof(double));
	if (n < 0) error("ERROR writing to socket");
	//receiving back the binary!
    bzero(buffer, 256);
	n = read(sockfd, buffer, 255);
	if (n < 0) error("ERROR reading from socket");

	d->binary = buffer;
	return NULL;

}

int main(int argc, char *argv[])
{
   	vector<char>symbols;
    vector<int> frequencies;
    string symb;
   getline(cin,symb);
//putting them into map first and then also counting frequencies
    map<char,int>freq;
    map<char,int>::iterator itr;

	for(long i=0;i<symb.length();i++)
		freq[symb[i]]++;
//map does the sorting for me! yay map! so i seperate the frequencies and symbols into two different vectors
    for(itr = freq.begin(); itr !=freq.end(); itr++)  {
        symbols.push_back(itr->first);
        frequencies.push_back(itr->second);
    }
     //i then calculate the probabilities by dividing frequencies by number of letters
	vector <double> probs;
	double px;
	for(int i =0;i <symbols.size();i++){
		 px =  (double)frequencies[i]/(double)symb.length();
		  probs.push_back(px);
		  
	}
	struct arguments *d = new struct arguments[probs.size()];
	// self explanatory, calculating fx
	double cumulativeprobability=0.0;
	//making multiple threads and setting values in a loop
	for(int i = 0; i<symbols.size();i++){
	    cumulativeprobability+=probs[i]; // how im calculating cumulative probability (inspo from my hw1)
	    d[i].px=probs[i];
	    d[i].fx = cumulativeprobability;
	    d[i].hostname = argv[1];
	    d[i].portno = atoi(argv[2]);
	   d[i].setServers(argv[1], atoi(argv[2]));
	    //each seperate symbol's px and fx will be sent to server to be converted to binary
	    pthread_create(&d[i].tid, NULL, sendRequests, &d[i]);
		
		
	    
	}
	cout<<"SHANNON-FANO-ELIAS Codes: \n \n";
	//joing threads <3
	for (int i = 0; i <symbols.size(); i++)
	{
		pthread_join(d[i].tid,NULL);
		cout<<"Symbol " <<symbols[i]<<", Code: "<< d[i].binary<<endl;
		
	}
		
	
//delete[]d;
}
