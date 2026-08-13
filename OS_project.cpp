#include <iostream>
#include "Virtual_Memory_manager.h"
#include <fstream>
#include <iomanip>
#include"Drawer.h"
using namespace std;



int main() {
     
	// Clear the comparison CSV and write header for a fresh run
	// Row order is always: 1=FIFO, 2=LRU, 3=OPT
	ofstream clear_csv("comparison_metrics.csv", ios::trunc);
	clear_csv << "Algorithm,TotalRequests,TLBHits,TLBMisses,HitRate,RAMReads,RAMWrites,DirtyEvictions,EAT" << endl;
	clear_csv.close();

	Virtual_Memory_manager Sherlock; 
	cout << "this is the one with FIFO\n";
	Sherlock.Walker();
	cout << "This is the one with LRU\n";
	Sherlock.Runner();
	cout << "This is the one with OPT\n";
	Sherlock.SPRINTER_OPT_TLB();

	//Drawer Sherlock1;
	//cout << "this is the one with FIFO\n";
	//Sherlock1.Walker1();
	//cout << "This is the one with LRU\n";
	//Sherlock1.Runner1();
	//cout << "This is the one with OPT\n";
	//Sherlock1.SPRINTER_OPT_TLB1();

}