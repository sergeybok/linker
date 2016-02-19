#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>

using namespace std;


class ErrorWarning {
	int ruleBroken;
	int module;
	int address;
	int max;
	string sym;
public:
	void setMod(int m) {
		module = m;
	}
	void setAddr(int a) {
		address = a;
	}
	void setMax(int m) {
		max = m;
	}
	void setSym(string s) {
		sym = s;
	}
	string getName() {
		return sym;
	}
	void setRule(int r) {
		ruleBroken = r;
	}
	int getRule() {
		return ruleBroken;
	}
	string msgString() {
		if (ruleBroken == 8) {
			return  "Error: Absolute address exceeds machine size; zero used";
		}
		if (ruleBroken == 9) {
			return "Error: Relative address exceeds module size; zero used";
		}
		if (ruleBroken == 6) {
			return "Error: External address exceeds length of uselist; treated as immediate";
		}
		if (ruleBroken==3) {
			return ("Error: " + sym + " is not defined; zero used");
		}
		if (ruleBroken==2) {
			return "Error: This variable is multiple times defined; first value used";
		}
		if (ruleBroken==10) {
			return "Error: Illegal immediate value; treated as 9999";
		}
		if (ruleBroken==11) {
			return "Error: Illegal opcode; treated as 9999";
		}
		if (ruleBroken==5) {
			return ("Warning: Module "+to_string(module+1)+": "+sym+" to big "+to_string(address)+" (max="+to_string(max)+") assume zero relative");
		}
		if (ruleBroken==7) {
			return ("Warning: Module "+to_string(module+1)+": "+sym+" appeared in the uselist but was not actually used");
		}
		if (ruleBroken==4) {
			return ("Warning: Module "+to_string(module+1)+": "+sym+" was defined but never used");
		}
		return "NONE";
	}
	
};

class Symbol {
	string name;
	int val, num_module,abs_addr;
	bool used;
public:
	void setAbsAddr(int a) {
		abs_addr = a;
	}
	int getAbsAddr() {
		return abs_addr;
	}
	void setName(string n) {
		name = n;
	}
	void setVal(int i) {
		val = i;
	}
	void setNumMod(int i) {
		num_module = i;
	}
	void setUsed(bool b) {
		used = b;
	}
	string getName() {
		return name;
	}
	int getVal() {
		return val;
	}
	int getNumMod() {
		return num_module;
	}
	bool getUsed() {
		return used;
	}
	void print() {
		cout << "Symbol: " << name << endl;
		cout << "location: " << val << endl;
		cout << "module: " << num_module << endl;
		cout << "Used: " << used << endl;
	}
};


class Instruction {
	char classification;
	int opcode;
	int address;
	int memory;
	vector<ErrorWarning> errList;
public:
	int getMemory() {
		return memory;
	}
	void setMemory(int i) {
		memory = i;
	}
	void addErr(ErrorWarning e) {
		errList.push_back(e);
	}
	vector<ErrorWarning> getErrList() {
		return errList;
	}
	void setClass(char c) {
		classification = c;
	}
	void setOp(int i) {
		opcode = i;
	}
	void setAddr(int i) {
		address = i;
	}
	char getClass() {
		return classification;
	}
	int getOp() {
		return opcode;
	}
	int getAddr() {
		return address;
	}
	void print() {
		
		int ad = opcode*1000 + address;
		cout << classification << " " << ad << endl;
	}
	void print_address() {
		int ad = opcode*1000 + address;
		cout << "address: " << ad << endl;
	}
};


class Module {
	int start;
	int end;
	int len;
	//list of defs
	vector<Symbol> defList;
	//list of uses
	vector<string> useList;
	//list of instr
	vector<Instruction> instructionList;
	//list of errors/warnings
	std::vector<ErrorWarning> errWarnList;
public:
	void addErr(ErrorWarning ew) {
		errWarnList.push_back(ew);
	}
	vector<ErrorWarning> getErrList() {
		return errWarnList;
	}
	int getStart() {
		return start;
	}
	int getEnd() {
		return end;
	}
	int getLength() {
		return end - start;
	}
	void setStart(int s) {
		start = s;
	}
	void setEnd(int e) {
		end = e;
	}
	void setLen(int l) {
		len = l;
	}
	void addLen(int i) {
		len = len + i;
	}
	void setDefList(std::vector<Symbol> d) {
		defList = d;
	}
	std::vector<Symbol> getDefList() {
		return defList;
	}
	int getDefListSize() {
		return defList.size();
	}
	void setUseList(std::vector<string> u) {
		useList = u;
	}
	std::vector<string> getUseList() {
		return useList;
	}
	void setInstructionList(std::vector<Instruction> i) {
		instructionList = i;
	}
	std::vector<Instruction> getInstructionList() {
		return instructionList;
	}
	int getInstructionSize() {
		return instructionList.size();
	}
	void print() {
		cout << "Start: " << getStart() << endl;
		cout << "end: " << getEnd() << endl;
		cout << "length: " << getLength() << endl;
	}
	void print_defs(){
		for (int i = 0; i< defList.size(); i++) {
			defList[i].print();
		}
	}
	void print_uses(){
		for (int i = 0; i< useList.size(); i++) {
			cout <<(useList[i]) << endl;
		}
	}
	void print_in(){
		for (int i = 0; i< instructionList.size(); i++) {
			instructionList[i].print();
		}
	}
};




// ~~~~~~~~~~~~~~~~~~~~~~Constants~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const int MAX_MEMORY = 512;
const int MAX_WORD = 16;
static ifstream file;
static string filename;
static int line_num;
static vector<Symbol> symbolsDefined;
static vector<Module> globalModules;
static std::vector<ErrorWarning> WARNINGS;
static std::vector<ErrorWarning> MULTIPLE;




// ~~~~~~~~~~~~~~~~~~~~~~~~Start of Code~~~~~~~~~~~~~~~~~~~~~~~


void updateLine() {
	if (file.peek() == '\n'){
		line_num++;
	}
}

int eraseFromVector(string s, vector<Symbol> &v) {
	for (int i = 0; i< v.size(); i++) {
		if (s == v[i].getName())
		{
			v.erase(v.begin() +i);
			return i;
		}
	}
	return -1;
} 

int getOffset(string s) {
	int i =0;
	string line,word;
	file.open(filename);
	if (s=="NULL")
	{
		string prevword;
		string prevline;
		int offset;

		while(getline(file,line)) {
			
			if ((line.length() > 0) && (line.at(0) != ' ') && (line.at(0)!= '\n') && (line.at(0)!='\0' )) {
				//cout << line.length();
				prevline = line;
				i++;
			} else break;
		}
		i--;
		line_num = i;
		return prevline.length();
		
	}
	while (getline(file,line)) {
		if (i == line_num)
		{
			int offset = line.find(s);
			if (offset != -1)
				return offset;
			else {
				getline(file, line);
				line_num++;
				return line.find(s);
			}
		}
		i++;
	}
	file.close();

	return -1;
}

void _parseError(int errcode, string s) {
	file.close();
	int offset = getOffset(s);
	static string errstr[] = {
		"NUM_EXPECTED",		// 0 number expected
		"SYM_EXPECTED",		// 1 symbol expected
		"ADDR_EXPECTED", 	// 2 addressing expected
		"SYM_TOLONG", 		// 3 symbol name is too long
		"TO_MANY_DEF_IN_MODULE",	// 4 >16
		"TO_MANY_USE_IN_MODULE",	// 5 >16
		"TO_MANY_INSTR"		// 6 total num_instr exceeds 512
	};
	cout <<"Parse Error line "<<(line_num+1)<<" offset "<<(offset+1) << ": "<<errstr[errcode]<<endl;
	//The plus 1's accomodate starting counting from 1, instead of 0
}


bool isNumber(string s) {	
	bool b = true;
	for (int i = 0; i < s.length(); i++) {
		if (!isdigit(s.at(i))) {
			return false;
		}
	}
	return b;
}

int inVector(string s, std::vector<string> v) {
	int index = -1;
	for (int i=0; i<v.size(); i++) {
		if(s == v[i]) {
			return i;
		}
	}
	return index;
}
int inVector(string s, std::vector<Symbol> v) {
	for (int i =0; i < v.size(); i++) {
		if (v[i].getName() == s)
		{
			return i;
		}
	}
	return -1;
}

int findExternal(string s) {
	int ad;
	for (int i = 0; i < symbolsDefined.size(); i++) {
		if (symbolsDefined[i].getName() == s){ 
			return symbolsDefined[i].getAbsAddr();
		}
	}
	return -1;
}

vector<Symbol> readDefList(int mod, int mod_address, int num, ifstream& file) {
	updateLine();
	int i = 0;
	string word;
	string prevword;
	std::vector<Symbol> v;
	int val;
	string sym;
	while ((i < 2*num)) {
		if (!(file>>word)) {
			if (i%2 == 0) {
				_parseError(1,"NULL");
				exit(1);
			}
			else {
				_parseError(0,"NULL");
				exit(1);
			}
		}
		if (i%2 == 0) {
			if (!isalpha(word.at(0))) {
				_parseError(1,word);
				exit(1);
			}
			if (word.length() > MAX_WORD) {
				_parseError(3,word);
				exit(1);
			}
			sym = word;
			if (findExternal(sym) != -1) {
				// Defined multiple times warning
				ErrorWarning e;
				e.setRule(2);
				e.setSym(sym);
				MULTIPLE.push_back(e);
				sym = "ERROR";
			}
		}
		else {
			if (!isNumber(word)) {
				_parseError(0,word);
				exit(1);
			}
			if (sym != "ERROR") {
				Symbol s;
				val = std::stoi(word);
				s.setName(sym);
				s.setVal(val);
				s.setAbsAddr(val + mod_address);
				s.setNumMod(mod);
				v.push_back(s);
				symbolsDefined.push_back(s);
			}
		}
		i++;
		prevword = word;
		updateLine();
	}
	return v;
}

vector<Symbol> readDefList2(int mod, int num, ifstream& file, vector<string> symDef) {
	updateLine();
	int i = 0;
	string word;
	std::vector<Symbol> v;
	int val;
	string sym;
	while ((i < 2*num) && (file>>word)) {
		if (i%2 == 0) {
			sym = word;
			if (inVector(sym,symDef) != -1) {
				// Defined multiple times warning `````````````````````````
				
				sym = "ERROR";
			}
		}
		else {
			if (sym != "ERROR") {
				Symbol s;
				val = std::stoi(word);
				s.setName(sym);
				s.setVal(val);
				s.setNumMod(mod);
				v.push_back(s);
			}
		}
		i++;
		updateLine();
	}
	return v;
}

vector<string> readUseList(int num, ifstream& file) {
	updateLine();
	int i = 0;
	string word;
	vector<string> v;
	while ((i < num)) {
		if (!(file>>word)) {
			_parseError(1,"NULL");
			exit(1);
		}
		if (!isalpha(word.at(0))) {
			_parseError(1,word);
			exit(1);
		}
		// IF not in symbols used from p1, warning

		v.push_back(word);
		i++;
		updateLine();
	}
	return v;
}

vector<string> readUseList2(int num, ifstream& file, vector<string> &symUsed) {
	updateLine();
	int i = 0;
	string word;
	vector<string> v;
	while ((i < num) && (file>>word)) {
		
		// IF not in symbols used from p1, warning ``````
		if (inVector(word,symbolsDefined) == -1) {
			//Throw Error
			
			//cout << "throws error" << endl;
		}

		v.push_back(word);
		symUsed.push_back(word);
		i++;
		updateLine();
	}
	return v;
}

vector<Instruction> readInstructions(int num, ifstream& file) {
	std::vector<Instruction> v;
	string word;
	string prevword;
	int i = 0;
	char classif;
	int opcode;
	int address;
	updateLine();
	if (file.peek() == EOF && num>1) {
		_parseError(2,"NULL");
		exit(1);
	}
	while ((i < num*2)) {
		if (!(file>>word)) {
			if (i%2 == 0) {
				_parseError(2,"NULL");
				exit(1);
			}
			else {
				_parseError(0,"NULL");
				exit(1);
			}
		}
		if (i%2 == 0) {
			
			if (word.length() > 1){
				// same problem as below
				_parseError(2,word);
				exit(1);
			}
			char c = word.at(0);
			if (!(c=='I' || c=='R' || c== 'A' || c=='E')) {
				// not instruction
				_parseError(2,word);
				exit(1);
			}
			classif = c;
		}
		else {
			if (!isNumber(word)) {
				// need to make sure i call right ones
				_parseError(0,word);
				exit(1);
			}
			
			if (word.length() < 4) {
				opcode = 0;
				address = 0;
			} else {
				opcode = (int)(word.at(0)) - 48;
				
				//address = std::stoi(word.substr(1));
			}
			Instruction in;
			in.setClass(classif);
        	
			in.setOp(opcode);
			in.setAddr(address);
			v.push_back(in);
		}
		updateLine();
		prevword = word;
		i++;
	}
	return v;
}

vector<Instruction> readInstructions2(int num, ifstream& file,int mod_i, int mod_address,vector<string> useList, Module &mod) {
	std::vector<Instruction> v;
	vector<string> actuallyUsed;
	string word;
	int i = 0;
	char classif;
	int opcode;
	int address;
	updateLine();
	while ((i < num*2) && (file>>word)) {
		if (i%2 == 0) {
			char c = word.at(0);
			classif = c;
		}
		else {

			//Need to add checks here for stuff
			Instruction in;
			in.setClass(classif);

			
			int number = std::stoi(word);
			if (word.length() < 4) {
				opcode = 0;
				address = number;
			}
        	else {
        		opcode = (int)(word.at(0)) - 48;
				address = std::stoi(word.substr(1));
			}
			in.setOp(opcode);
			in.setAddr(address);
		
			if (classif == 'I')
			{
				
				if (number > 9999)
				{
					//
					ErrorWarning e;
					e.setRule(10);
					in.addErr(e);
					number = 9999;
				}
				in.setMemory(number);
			}
			else if (number> 9999) {
				ErrorWarning e;
				e.setRule(11);
				in.addErr(e);
				in.setMemory(9999);
			}
			else if(classif == 'A') {
				// check stuff
				
				if (address > MAX_MEMORY) {
					//error
					ErrorWarning e;
					e.setRule(8);
					in.addErr(e);
					address = 0;
				}
				int m = opcode*1000 + address;
				in.setMemory(m);
			}
			else if (classif == 'R') {
				//some check as well
				if (address >= num) {
					ErrorWarning e;
					e.setRule(9);
					in.addErr(e);
					address = 0;
				}
				address = address+mod_address;

				int m = opcode*1000 + address;
				in.setMemory(m);
			}
			else if (classif == 'E') {
				//some checks
				if (address >= useList.size()) {
					ErrorWarning e;
					e.setRule(6);
					in.addErr(e);
					in.setMemory(opcode*1000 + address);
				}
				else {
					int ext = findExternal(useList[address]);
					if (ext == -1) { // Not defined
						ErrorWarning e;
						e.setSym(useList[address]);
						e.setMod(mod_i);
						e.setRule(3);
						in.addErr(e);
						ext = 0;
					}
					int m = opcode*1000 + ext;
					in.setMemory(m);
					if (inVector(useList[address],actuallyUsed) == -1)
						actuallyUsed.push_back(useList[address]);
				}

			}
			v.push_back(in);
		}
		updateLine();
		i++;
	}
	if (useList.size() > actuallyUsed.size()) {
		for (int i = 0; i< useList.size(); i++) {
			if (inVector(useList[i],actuallyUsed) == -1) {
				ErrorWarning w;
				w.setRule(7);
				w.setSym(useList[i]);
				w.setMod(mod_i);
				mod.addErr(w);
			}
		}
	}
	return v;
}



int read_file_1() {
	file.open(filename);
	string word;
	
	
	int mod_i = 0;
	int mod_address = 0;
	line_num = 0;
	updateLine();
	
	while (file>>word) {
		Module mod;
		mod.setStart(mod_address);
		
		//DEFS
		if (!isNumber(word)) {
			_parseError(0,word);
			exit(1);
		}
		int numDefs = std::stoi(word);
		if (numDefs > 16) {
			_parseError(4,word);
			exit(1);
		}
		mod.setDefList(readDefList(mod_i,mod_address,numDefs,file));
		

		
		//USES
		if ((file>>word));
		else {
			_parseError(0,"NULL");
			exit(1);
		}
		if (!isNumber(word)) {
			_parseError(0,word);
			exit(1);
		}
		int numUse = std::stoi(word);
		if (numUse > 16) {
			_parseError(5,word);
			exit(1);
		}
		mod.setUseList(readUseList(numUse,file));

		
		//Instructions
		if ((file>>word)); 
		else {
			_parseError(0,"NULL");
			exit(1);
		}
		if (!isNumber(word)) {
			_parseError(0,word);
			exit(1);
		}
		int numInstr = std::stoi(word);

		if (numInstr + mod_address > MAX_MEMORY) {
			_parseError(6,word);
			exit(1);
		}
		mod.setInstructionList(readInstructions(numInstr,file));

		
		mod_address = mod_address + mod.getInstructionSize(); //--------------------------------------------
		mod.setEnd(mod_address);
		for (int i = 0; i< mod.getDefListSize(); i++) {
			if (mod.getDefList()[i].getVal() > (mod.getLength()-1) ) {
				ErrorWarning w;
				w.setRule(5);
				w.setSym(mod.getDefList()[i].getName());
				w.setMod(mod_i);
				w.setAddr(mod.getDefList()[i].getVal());
				w.setMax(mod.getLength()-1);
				WARNINGS.push_back(w);
				mod.getDefList()[i].setVal(0);
				mod.getDefList()[i].setAbsAddr(mod.getStart());
				int n = eraseFromVector(mod.getDefList()[i].getName(), symbolsDefined);
				Symbol new_sym;
				new_sym.setVal(0);
				new_sym.setAbsAddr(mod.getStart());
				new_sym.setName(mod.getDefList()[i].getName());
				symbolsDefined.insert(symbolsDefined.begin()+n, new_sym);
			}
		}



		globalModules.push_back(mod);
		mod_i++;
	}


	

	file.close();

	return 0;
}


int read_file_2() {
	file.open(filename);
	string word;


	vector<Module> modules;
	vector<string> symDefined2;
	vector<string> symUsed;

	
	int mod_i = 0;
	int mod_address = 0;
	line_num = 0;
	updateLine();

	while (file>>word) {
		Module mod;
		mod.setStart(mod_address);
	
		//DEFS
		int numDefs = std::stoi(word);
		mod.setDefList(readDefList2(mod_i,numDefs,file,symDefined2));

		//USES
		if ((file>>word));
		else {
			//shouldn't happen
			exit(1);
		}
		int numUse = std::stoi(word);
		std::vector<string> useList = readUseList2(numUse,file,symUsed);
		mod.setUseList(useList);

		//Instructions
		if ((file>>word)); 
		else {
			// Shouldn't happen
			exit(1);
		}
		int numInstr = std::stoi(word);
		mod.setInstructionList(readInstructions2(numInstr,file,mod_i, mod_address,useList,mod));

		mod_i++;
		mod_address = mod_address + mod.getInstructionSize();
		mod.setEnd(mod_address);

		modules.push_back(mod);
	}

	//cout << endl;
	for (int i =0; i< WARNINGS.size(); i++) {
		cout << WARNINGS[i].msgString() << endl;
	}
	cout << "Symbol Table" << endl;
	for (int i = 0; i < symbolsDefined.size(); i++) {
		cout << symbolsDefined[i].getName() << "=" << symbolsDefined[i].getAbsAddr();
		if(MULTIPLE.size() > 0) {
			for (int j=0;j< MULTIPLE.size(); j++) {
				if(MULTIPLE[j].getName() == symbolsDefined[i].getName()) {
					cout << " "<< MULTIPLE[j].msgString();
					MULTIPLE.erase(MULTIPLE.begin()+j);
				}
			}
		}
		cout << endl;
	}
	cout << endl;
	cout << "Memory Map" << endl;
	int k = 0;
	cout.fill('0');
	for (int i = 0; i < modules.size(); i++) {
		for (int j = 0; j < modules[i].getInstructionSize(); j++) {
			cout <<setw(3)<<right<< k << ": "<< setw(4)<<right << modules[i].getInstructionList()[j].getMemory();
			if (modules[i].getInstructionList()[j].getErrList().size() > 0) {
				cout << " "<< modules[i].getInstructionList()[j].getErrList()[0].msgString();
			}
			cout << endl;
			k++;
		}
		for (int j = 0; j < modules[i].getErrList().size(); j++) {
			cout << modules[i].getErrList()[j].msgString() << endl;
		}
	}
	cout << endl;
	for (int i=0; i< symbolsDefined.size(); i++) {
		if (inVector(symbolsDefined[i].getName(),symUsed) == -1) {
			ErrorWarning w;
			w.setRule(4);
			w.setMod(symbolsDefined[i].getNumMod());
			w.setSym(symbolsDefined[i].getName());
			cout << w.msgString() << endl;
		}
	}
	symbolsDefined.clear();
	WARNINGS.clear();
	MULTIPLE.clear();
	file.close();
	return 0;
}


int main(int argc, char* argv[]) {
	
	filename = argv[1];
	int some = read_file_1();
	int other = read_file_2();
	

	return 0;
}















