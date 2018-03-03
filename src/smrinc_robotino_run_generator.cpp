#include <getopt.h>
#include <string>
#include <vector>
#include <iostream>
#include <random>
#include <algorithm>
#include <ctime>
#include <locale>
#include <fstream>

void usage(void) { 
	printf("Usage: smrinc_robotino_run_generator [OPTION]\n");
	printf("\nSession optional arguments:\n");
	printf("  -c       Number of conditions per session [default: 2]\n");
	printf("  -r       Number of repetitions per conditions [default: 2]\n");
	printf("\nRun optional arguments:\n");
	printf("  -t       Number of targets per run [default: 5]\n");
	printf("  -p       Number of repetitions per target [default: 2]\n");
	printf("\nGeneral optional arguments:\n");
	printf("  -s       Save results to './CURRENTTIME_generated_runs.txt' file [default: false]\n");
	printf("  -S       Save results to the provided file [default: './CURRENTTIME_generated_runs.txt']\n");
	printf("  -h       display this help and exit\n");
	exit(EXIT_SUCCESS);
}

std::string dump_header(void) {
	
	std::string s;
	s  = "==============================================================\n";
	s += "Run\tDate\t\tCond\tTargets\n";
	s += "==============================================================\n";
	return s;
}


int main(int argc, char** argv) {

	unsigned int nruns;
	unsigned int ncond		= 2;
	unsigned int nrepcond	= 2;
	unsigned int ntarg		= 5;
	unsigned int nreptarg	= 2;
	bool save_flag			= false;
	std::string filename    = "";
	std::string	entry;

	std::vector<unsigned int>	runs;
	std::vector<unsigned int>	targets;
	std::vector<std::string>	sresult;

	// Optional arguments
	int opt;

	while((opt = getopt(argc, argv, "c:r:t:p:S:sh")) != -1) {
		if(opt == 'c') {
			ncond = std::stoul(optarg);
		} else if(opt == 'r') {
			nrepcond = std::stoul(optarg);
		} else if(opt == 't') {
			ntarg = std::stoul(optarg);
		} else if(opt == 'p') {
			nreptarg = std::stoul(optarg);
		} else if(opt == 's') {
			save_flag = true;
		} else if(opt == 'S') {
			save_flag = true;
			filename = std::string(optarg);
		} else {
			usage();
			exit(opt == 'h' ? EXIT_SUCCESS : EXIT_FAILURE);
		}

	}


	// Initialize random
	std::random_device rd;
    std::mt19937 g(rd());
 
	// Fill the run vector
	for(auto i=0; i<ncond; i++) 
		for(auto u=0; u<nrepcond; u++) 
			runs.push_back(i+1);

	// Fill the target vector
	for(auto j=0; j<ntarg; j++) 
		for(auto y=0; y<nreptarg; y++) 
			targets.push_back(j+1);

	// Shuffle the run vector
    std::shuffle(runs.begin(), runs.end(), g);	

	// Get the current date
	std::time_t t = std::time(nullptr);	
	char date[100];
	std::strftime(date, sizeof(date), "%Y%m%d", std::localtime(&t));
	std::string sdate(date);

	for(auto itr=runs.begin(); itr!=runs.end(); ++itr) {

		entry = std::to_string(std::distance(runs.begin(), itr)+1) + "\t";
		entry += sdate + "\t";
		entry += std::to_string(*itr) + "\t";	
		
		// Shuffle the target vector for this run
		std::shuffle(targets.begin(), targets.end(), g);	
		for(auto itt=targets.begin(); itt!=targets.end(); ++itt) {
			entry += std::to_string(*itt) + " ";
		}

		sresult.push_back(entry);
	}
 


	// Open file
	if(save_flag == true) {
		if(filename.empty())
			filename = "./" + sdate + "_generated_runs.txt";	

		std::ofstream	ofs;
		ofs.open(filename, std::ofstream::out | std::ofstream::app);
		ofs<<dump_header();
		for(auto its=sresult.begin(); its!=sresult.end(); ++its)
			ofs<<(*its)<<"\n";

		ofs<<"\n\n";
		ofs.close();
	}


	std::cout<<dump_header()<<std::endl;
	for(auto its=sresult.begin(); its!=sresult.end(); ++its)
		std::cout<<(*its)<<std::endl;

}

