
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include "sha1.h"

#define NUM_THREADS 4

bool done = false;
const unsigned int nonceMin = 0x000000;
const unsigned int nonceMax = 0xFFFFFF;
const char originalHash[41] = "759c692d2cc40908d30b80d2e5d581a1522f9597";
const char date[] = "2013-05-22 02:06:10";
const char mines[8][6] = {
	"1,1,1",
	"2,2,2",
	"2,1,1",
	"1,2,1",
	"1,1,2",
	"2,2,1",
	"1,2,2",
	"2,1,2",
};

typedef struct {
	unsigned int nonceStart;
	unsigned int nonceEnd;
} tparams;

void *trun(void *p) {
	// Some working variables
	unsigned char rawHash[20];
	char hexHash[41];
	char str[40];
	tparams *params = (tparams*)p;

	// See if the correct nonce lies within our bounds
	for(unsigned int i = params->nonceStart; i <= params->nonceEnd; i++) {
		// Check each of the possible mine positions with this nonce
		for(int j = 0; j < 8 && !done; j++) {
			sprintf(str, "(%s | %s)%0.6x", mines[j], date, i);
			sha1::calc(str, strlen(str), rawHash);
			sha1::toHexString(rawHash, hexHash);

			// If the hashes match then we've got the mine positions, we're done!
			if(strcmp(hexHash, originalHash) == 0) {
				std::cout << "Mine positions found, they are " << mines[j] << "." << std::endl;
				done = true;
				pthread_exit(0);
			}
		}
	}

	pthread_exit(0);
}

int main(int argc, char *argv[]) {
	// Some working variables
	time_t startTime, endTime;
	pthread_t threads[NUM_THREADS];
	tparams params[NUM_THREADS];
	unsigned int workSize = nonceMax / NUM_THREADS;

	time(&startTime);
	for(int t = 0; t < NUM_THREADS; t++) {
		params[t].nonceStart = t * workSize;
		params[t].nonceEnd = params[t].nonceStart + workSize;
		pthread_create(&threads[t], NULL, trun, (void*)&params[t]);
	}

	for(int t = 0; t < NUM_THREADS; t++) {
		pthread_join(threads[t], NULL);
	}
	time(&endTime);

	// If we get this far we scanned our whole range and didn't find anything
	if(!done) {
		std::cout << "Sorry, couldn't figure out the mine positions." << std::endl;
	}
	std::cout << "Time taken: " << endTime - startTime << " seconds." << std::endl;

	pthread_exit(NULL);
}

