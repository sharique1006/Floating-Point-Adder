#include <bits/stdc++.h>
#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#define MAXLIST 5000

using namespace std;

//A BLOCK of CACHE with parameters as data, tag, valid, dirty, priority, tag etc.
struct CACHE_BLOCK {
	int data;
	int tag;
	int valid_status;
	int dirty_status;
	int priority;
	int last_used;
};

int cache_size;
int block_size;
int cache_associativity;
int T;
//To Store All the memory requests made
string memory_access_requests[2000];
int num_sets;
//CACHE an array of array of BLOCK's
struct CACHE_BLOCK CACHE[MAXLIST][MAXLIST];
//MAIN MEMORY
int main_memory[MAXLIST];
int num_access = 0;
int num_reads = 0;
int read_hits = 0;
int read_miss = 0;
int num_writes = 0;
int write_hits = 0;
int write_miss = 0;

//Function to parse the memory requests.
void tokenize(string request, char *tokenized_inst[10]) {
	int len = request.length();
	char str[len+1];
	strcpy(str, request.c_str());
	char *token = strtok(str, " ");
	int cnt = 0;

	while(token != NULL) {
		if(strchr(token, ',')) {
			token[strlen(token)-1] = '\0';
		}
		tokenized_inst[cnt] = token;
		cnt++;
		token = strtok(NULL, " ");
	}
}

void initialize() {
	for(int i = 0; i < num_sets; i++) {
		for(int j = 0; j < cache_associativity; j++) {
			CACHE[i][j].dirty_status = 1;
		}
	}
}

//Function to display the cache
void display() {
	for(int i = 0; i < num_sets; i++) {
		cout <<"S"<<i<<": |";
		for(int j = 0; j < cache_associativity; j++) {
			if(CACHE[i][j].valid_status == 1) {
				cout <<" D:"<<CACHE[i][j].data<<"  T:"<<CACHE[i][j].tag<<"  V:"<<CACHE[i][j].valid_status<<"  P:"<<CACHE[i][j].priority<<"  Dir:"<<CACHE[i][j].dirty_status<<" |";
			}
			else {
				cout <<"                           |";
			}
		}
		cout <<endl;
	}
	cout <<endl;
}

void display_cache() {
	cout <<"CACHE: "<<endl;
	for(int i = 0; i < num_sets; i++) {
		for(int j = 0; j < cache_associativity; j++) {
			cout <<CACHE[i][j].data<<", "<<CACHE[i][j].tag<<", "<<CACHE[i][j].valid_status<<", "<<CACHE[i][j].dirty_status<<endl;
		}
	}
	cout <<endl;
}

//Function to display statistics of cache
void display_cache_statistics() {
	cout <<"Cache Statistics: "<<endl;
	cout <<"Number of Accesses = "<<num_access<<endl;
	cout <<"Number of Reads = "<<num_reads<<endl;
	cout <<"Number of Read Hits = "<<read_hits<<endl;
	cout <<"Number of Read Misses = "<<read_miss<<endl;
	cout <<"Number of Writes = "<<num_writes<<endl;
	cout <<"Number of Write Hits  = "<<write_hits<<endl;
	cout <<"Number of Write Misses  = "<<write_miss<<endl;
	float hit_ratio = 1.0*(read_hits + write_hits)/num_access;
	cout <<"Hit Ratio  = "<<hit_ratio<<endl;
}

//Function to convert an integer to binary
string convertToBinary(int num) {
	if(num == 0) {
		return "0";
	}
	string binaryNum = "";
	while(num > 0) {
		int x = num % 2;
		char c = x + '0';
		binaryNum = c + binaryNum;
		num = num/2;
	}
	return binaryNum;
}

//Function to get set_index & tag from the memory address
pair<int,int> parseAddress(int num) {
	string address = convertToBinary(num);
	int tag = num / num_sets;
	int set_index = num % num_sets;
	cout <<"address: "<<address<<" "<<"tag: "<<tag<<" "<<"set_index: "<<set_index<<endl;
	return make_pair(tag,set_index);
}

//Function to check if a low priority block is present in a given set of the CACHE
int LowPriorityPresent(int set_index) {
	for(int j = 0; j < cache_associativity; j++) {
		if(CACHE[set_index][j].priority == 0) {
			return 1;
		}
	}
	return 0;
}

//Function to get the Least Recently Used Low Prioirty Block from a given set
int getLRUBlockLowPriority(int set_index) {
	int maxlru;
	int max;
	for(int j = 0; j < cache_associativity; j++) {
		if(CACHE[set_index][j].priority == 0) {
			maxlru = j;
			max = CACHE[set_index][j].last_used;
			break;
		}
	}
	for(int p = maxlru+1; p < cache_associativity; p++) {
		if(CACHE[set_index][p].priority == 0 && CACHE[set_index][p].last_used > max) {
			maxlru = p;
			max = CACHE[set_index][p].last_used;
		}
	}
	return maxlru;
}

//Function to get the Least Recently Used High Priority Block from a given set
int getLRUBlockHighPriority(int set_index) {
	int maxlru = 0;
	int max = CACHE[set_index][0].last_used;
	for(int j = 1; j < cache_associativity; j++) {
		if(CACHE[set_index][j].last_used > max) {
			maxlru = j;
			max = CACHE[set_index][j].last_used;
		}
	}
	return maxlru;
}

//Function to handle MISS
void WRITE_MISS(int data, int tag, int set_index, int dirty) {
	int apply_LRU = 1;
	//If an invalid block is present write the data in that block
	for(int j = 0; j < cache_associativity; j++) {
		if(CACHE[set_index][j].valid_status == 0) {
			CACHE[set_index][j].data = data;
			CACHE[set_index][j].tag = tag;
			CACHE[set_index][j].valid_status = 1;
			CACHE[set_index][j].priority = 0;
			CACHE[set_index][j].dirty_status = dirty;
			apply_LRU = 0;
			break;
		}
	}
	//If all the blocks in that set are written then write in the LRU block
	if(apply_LRU == 1) {
		int block;
		//If a low prioirty is present in that set, write in the LRU Low priority block of that set
		if(LowPriorityPresent(set_index) == 1) {
			block = getLRUBlockLowPriority(set_index);
		}
		//If not then write in the LRU High Priority block of that set
		else {
			block = getLRUBlockHighPriority(set_index);
		} 
		//When writing in the LRU block put the old data from cache into the main memory
		int addr = CACHE[set_index][block].tag * num_sets + set_index;
		main_memory[addr] = CACHE[set_index][block].data;
		CACHE[set_index][block].data = data;
		CACHE[set_index][block].tag = tag;
		CACHE[set_index][block].priority = 0;
		CACHE[set_index][block].valid_status = 1;
		CACHE[set_index][block].last_used = 0;
		CACHE[set_index][block].dirty_status = dirty;
	}
}

//Function to write in CACHE
void WRITE_IN_CACHE(int memory_address, int data) {
	pair<int,int> parsed_address = parseAddress(memory_address);
	int tag = parsed_address.first;
	int set_index = parsed_address.second;
	int found = 0;
	//If the tag of the request matches with tag of any valid block in that set overwrite it and promote it to high prioirty
	for(int j = 0; j < cache_associativity; j++) {
		if(CACHE[set_index][j].valid_status == 1 && CACHE[set_index][j].tag == tag) {
			CACHE[set_index][j].data = data;
			CACHE[set_index][j].tag = tag;
			CACHE[set_index][j].priority = 1;
			CACHE[set_index][j].last_used = 0;
			CACHE[set_index][j].dirty_status = 1;
			found = 1;
			write_hits++;
			break;
		}
	}
	//If it's a miss then call write miss function
	if(found == 0) {
		write_miss++;
		WRITE_MISS(data, tag, set_index, 1);
	}
}

//Function to Read from cache
void READ_FROM_CACHE(int memory_address) {
	pair<int,int> parsed_address = parseAddress(memory_address);
	int tag = parsed_address.first;
	int set_index = parsed_address.second;
	int found = 0;
	//If the tag matches with any valid block of that set, it's a read hit, promote that block to high priority
	for(int j = 0; j < cache_associativity; j++) {
		if(CACHE[set_index][j].valid_status == 1 && CACHE[set_index][j].tag == tag) {
			CACHE[set_index][j].priority = 1;
			CACHE[set_index][j].last_used = 0;
			read_hits++;
			found = 1;
			break;
		}
	}
	//If it's a miss take the data from main memory and write it back in the cache
	if(found == 0) {
		read_miss++;
		int data = main_memory[memory_address];
		WRITE_MISS(data, tag, set_index, 0);
	}
}

//Function to keep a track on the least recently used block
void LRU() {
	for(int i = 0; i < num_sets; i++) {
		for(int j = 0; j < cache_associativity; j++) {
			if(CACHE[i][j].valid_status == 1) {
				CACHE[i][j].last_used += 1;
			}
		}
	}
}

//Function to demote a High Prioirty block if not accessed for T Cache access
void TCACHEACCESS() {
	for(int i = 0; i < num_sets; i++) {
		for(int j = 0; j < cache_associativity; j++) {
			if(CACHE[i][j].last_used >= T) {
				CACHE[i][j].priority = 0;
			}
		}
	}
}

//Main Function
int main(int argc, char *argv[]) {
	ifstream infile;
	infile.open(argv[1]);
	string str;

	getline(infile,str);
	cache_size = atoi(str.c_str());
	getline(infile,str);
	block_size = atoi(str.c_str());
	getline(infile,str);
	cache_associativity = atoi(str.c_str());
	getline(infile,str);
	T = atoi(str.c_str());

	num_sets = cache_size/(block_size*cache_associativity);

	cout <<"cache_size: "<<cache_size<<endl;
	cout <<"block_size: "<<block_size<<endl;
	cout <<"cache_associativity: "<<cache_associativity<<endl;
	cout <<"T: "<<T<<endl;
	cout <<"num_sets: "<<num_sets<<endl;

	//Read all the memory request from input file
	int index = 0;
	while(getline(infile, str)) {
		memory_access_requests[index] = str;
		index++;
	}

	cout <<endl;
	cout <<"MEMORY ACCESS REQUESTS:"<<endl;

	initialize();

	//Solve each memory request
	for(int i = 0; i < index; i++) {
		num_access++;
		cout <<memory_access_requests[i]<<endl;
		char *tokenized_inst[10];
		tokenize(memory_access_requests[i], tokenized_inst);
		//Handle Write
		if(strcmp(tokenized_inst[1],"W") == 0) {
			num_writes++;
			int memory_address = atoi(tokenized_inst[0]);
			int data = atoi(tokenized_inst[2]);
			WRITE_IN_CACHE(memory_address, data);
		}
		//Handle Read
		else if(strcmp(tokenized_inst[1], "R") == 0) {
			num_reads++;
			int memory_address = atoi(tokenized_inst[0]);
			READ_FROM_CACHE(memory_address);
		}
		LRU();
		TCACHEACCESS();
		display();
	}
	// for(int i = 0 ; i < MAXLIST; i++) {
	// 	if(main_memory[i] != 0) {
	// 		cout <<"main_memory["<<i<<"]: "<<main_memory[i]<<endl;
	// 	}
	// }
	// cout <<endl;
	display_cache();
	display_cache_statistics();
	return 0;
}