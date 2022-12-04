#include <stdio.h>

#include <pthread.h>

#include <math.h>

#include <vector>

#include <sstream>

#include<iostream>

using namespace std;

struct thread_args {
  string symbol;
  double probability;
  double fx;
  double barfx;
  pthread_t tid;
  string binary;
  int blength;

};

void * shaneliascode(void * multiargs) {

  struct thread_args * m = (struct thread_args * ) multiargs;
  string symbol = m -> symbol;
  double probability = m -> probability;
  double fx = m -> fx;
  double barfx = (fx - probability) + probability / 2;
  int blength = m -> blength;
  blength = ceil(log2(1 / probability)) + 1;

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
  m -> binary = newbinary;

}

int main() {

  string symb;
  double prob;
  //grabbing first line of symbols
  getline(cin, symb);

  cout << "SHANNON-FANO-ELIAS Codes:  " << "\n";
  //storing both lines in vectors
  vector < string > symbols;
  vector < double > probabilities;
  //storing the symbols in vector
  stringstream s(symb);
  while (s >> symb) {
    symbols.push_back(symb);
  }
  //storing the probabilties in a  vector
  while (cin >> prob) {
    probabilities.push_back(prob);
  }
  double fx = 0.0;
  //making an array of structs, each thread will have its own struct
  struct thread_args * multiargs = new thread_args[symbols.size()];

  for (int i = 0; i < symbols.size(); i++) {
    multiargs[i].probability = probabilities[i];
    multiargs[i].symbol = symbols[i];
    //cumulative sum
    fx += multiargs[i].probability;
    multiargs[i].fx = fx;
    //having the thread accept struct as an argument
    pthread_create( & multiargs[i].tid, NULL, shaneliascode, & multiargs[i]);
  }
  for (int i = 0; i < symbols.size(); i++) {
    pthread_join(multiargs[i].tid, NULL);
    cout << "\n Symbol " << multiargs[i].symbol << ", Code: " << multiargs[i].binary;
    //cout<< multiargs[i].binary;
  }

  delete[] multiargs;

}