#include <pthread.h>

#include <stdio.h>

#include <unistd.h>

#include <stdlib.h>

#include <vector>

#include <string>

#include <cmath>

#include <semaphore.h>

#include <iostream>

#include <map>

using namespace std;

struct thread_args {
  double prob;
  double fx = 0.0;
  string symbol;
  pthread_mutex_t * bsem;
  pthread_cond_t * waitTurn;
  int * parentindex = 0;
  int index = 0;
};

void * shaneliascode(void * args) {
  thread_args * m = (struct thread_args * ) args;
  int localID = m -> index;
  double px = m -> prob;
  double fx = m -> fx;
  string symbol = m -> symbol;

  pthread_mutex_unlock(m -> bsem); //unlock after local variables have been set

  //SHANNON ELIAS CALCULATION   
  fx += px; //cumulative prob
  m -> fx = fx;
  double barfx = (fx - px) + px / 2;
  int blength = ceil(log2(1 / px)) + 1;

  //an empty string of 0's to start with
  string binary = "000000000000";
  //converting the barfx to binary
  double j = 0.5;
  for (int i = 0; i < blength; i++) {
    if (j > barfx) binary[i] = '0';
    else {
      binary[i] = '1';
      barfx -= j;
    }
    j /= 2;

  }
  //using substring to reduce the binary string to the targeted length
  string newbinary = binary.substr(0, blength);

  pthread_mutex_lock(m -> bsem);
  while (( * m -> parentindex) != localID) {
    pthread_cond_wait(m -> waitTurn, m -> bsem);
  }

  pthread_mutex_unlock(m -> bsem);

  cout << "Symbol " << symbol << ", Code: " << newbinary << endl;

  pthread_mutex_lock(m -> bsem);
  ( * m -> parentindex) = ( * m -> parentindex) + 1;
  pthread_cond_broadcast(m -> waitTurn);
  pthread_mutex_unlock(m -> bsem);

  return NULL;
}

int main() {

  vector < char > symbols;
  vector < int > frequencies;
  string symb;
  getline(cin, symb);
  //putting them into map first and then also counting frequencies
  map < char, int > freq;
  map < char, int > ::iterator itr;

  for (long i = 0; i < symb.length(); i++)
    freq[symb[i]]++;
  //map does the sorting for me! yay map! so i seperate the frequencies and symbols into two different vectors
  for (itr = freq.begin(); itr != freq.end(); itr++) {
    symbols.push_back(itr -> first);
    frequencies.push_back(itr -> second);
  }
  //i then calculate the probabilities by dividing frequencies by number of letters
  vector < double > probs;
  double px;
  for (int i = 0; i < symbols.size(); i++) {
    px = (double) frequencies[i] / (double) symb.length();
    probs.push_back(px);

  }
  static pthread_mutex_t nthreads;
  static pthread_cond_t turn;
  static thread_args child;

  child.bsem = & nthreads;
  child.waitTurn = & turn;
  int parId = 0;
  child.parentindex = & parId;
  pthread_t * n_thread = new pthread_t[symbols.size()];

  std::cout << "SHANNON-FANO-ELIAS Codes:  " << "\n\n";

  for (int i = 0; i < symbols.size(); i++) {

    pthread_mutex_lock( & nthreads);
    child.index = i;
    child.symbol = symbols[i];
    child.prob = probs[i];

    pthread_create( & n_thread[i], NULL, shaneliascode, & child);
  }
  for (int i = 0; i < symbols.size(); i++) {
    pthread_join(n_thread[i], NULL);
  }

  return 0;
}
