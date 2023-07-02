
/*****************************************************************************

TITLE: Emulator C source code for Custom Assembly language  																															
AUTHOR:   		SAI TULASI KANISHKA PONNADA 
ROLL NUMBER:	2101CS57

:::Declaration of Authorship:::
This EMU file is part of the assignment of CS209/CS210 at the 
department of Computer Science and Engineering, IIT Patna, 2023. 

--Sem4, Computer Architecture Lab Project, Professor Jimson
*****************************************************************************/

#include<bits/stdc++.h>
using namespace std;




/* --------------- PARAMETERS OF THE DEVICE BEING EMULATED --------------- */

int stack_pointer, program_counter, regis_A, regis_B;




// Memory with 2^24 addresses

int harddisk[1<<24]; 
int harddisk2[1<<24]; 
//here the memoryfile is termed as harddisk as it is the physical equivalent for this emulated device


string instruction_names[] = {"ldc", "adc", "ldl", "stl", "ldnl", "stnl", "add", "sub", "shl", "shr", "adj", "a2sp", "sp2a", "call", "return", "brz", "brlz", "br", "HALT"};


// Custom Assembly functions and their definitions from the instruction set defined in the question paper



//Store local -------------------------------------------------------------
void stl(int offset_to_be_added) {
	harddisk2[stack_pointer + offset_to_be_added] = regis_A;
	regis_A = regis_B;
}


//Load non-local --------------------------------------------------------------
void ldnl(int offset_to_be_added) {
	regis_A = harddisk2[regis_A + offset_to_be_added];
}

 
//Store non-local -------------------------------------------------------------
void stnl(int offset_to_be_added) {
	harddisk2[regis_A + offset_to_be_added] = regis_B;
}

//Load accumulator with the value specified ---------------------------------------
void ldc(int input_to_funct) {
	regis_B = regis_A;	
	regis_A = input_to_funct;
}

//Add the value specified to the accumulator -------------------------------------
void adc(int input_to_funct) {
	regis_A += input_to_funct;
}


//Load local -------------------------------------------------------------

void ldl(int offset_to_be_added) {
	regis_B = regis_A;
	regis_A = harddisk2[stack_pointer + offset_to_be_added];
}


//Addition -------------------------------------------------------------
void add() {
	regis_A += regis_B;
}

//Subtraction -------------------------------------------------------------
void sub() {
	regis_A = regis_B - regis_A;
}


//Shift left -------------------------------------------------------------
void shl() {
	regis_A = regis_B << regis_A;
}

//Shift right -------------------------------------------------------------
void shr() {
	regis_A= regis_B >> regis_A;
}

// Call procedure -------------------------------------------------------------
void call(int offset_to_be_added) {
	regis_B = regis_A;
	regis_A = program_counter;
	program_counter += offset_to_be_added;
}


// Return from procedure ---------------------------------------------------
void RETURN() {																
	program_counter = regis_A;
	regis_A = regis_B;
}

//If accumulator is zero, branch to specified offset------------------------
void brz(int offset_to_be_added) {
	if (regis_A == 0) program_counter += offset_to_be_added;
}


//Adjust Stack pointer -------------------------------------------------------------
void adj(int input_to_funct) {
	stack_pointer += input_to_funct;
}

//Transfer regis A to Stack Pointer; -----------------------------------------------
void a2sp() {
	stack_pointer = regis_A;
	regis_A = regis_B;
}


//Transfer Stack pointer to regis_A ------------------------------------------------
void sp2a() {
	regis_B = regis_A;
	regis_A = stack_pointer;
}


//If accumulator is less than zero, branch to specified offset ---------------------------------------
void brlz(int offset_to_be_added) {
	if (regis_A < 0) program_counter += offset_to_be_added;
}

// Branch to specified offset -------------------------------------------------------------
void br(int offset_to_be_added) {
	program_counter += offset_to_be_added;
} 




//--------------- SUPPORT FUNCTIONS--------------- 

string opcode_to_operand_type(int x) {
	if (x ==0 || x==1 || x==10) return "VALUE";
	else if (x == 2 || x == 3 || x == 4 || x==5 || x==13 || x==15 || x==16 || x==17) return "OFFSET";
	return "";
}


void show_isa_function(){

	cout<<"OPCODE\tMNEMONIC\tOPERAND\n";
	for (int i = 0; i <= 18; i++) {
		// fprintf(stderr, "%d\t%s\t%s\n", i, mnemonic[i], getType(i));
		cout<<i<<"\t"<<instruction_names[i]<<"\t\t"<<opcode_to_operand_type(i)<<"\n";
	}
	// fprintf(stderr, "\tSET\tvalue\n");
	return;

}



// Convert 32 bit binary string to number
int bin_string_to_decimal(string bin) {
	int number = 0;
	for (int i = 0; i < 32; i++) {
		number *= 2;
		if (bin[i] == '1') number += 1; 
	}
	return number;
}


void liveEmulation(int opcode, int operand) {
	fprintf(stderr, "PC=%08x, SP=%08x, A=%08x, B=%08x %s\t%08x\n", program_counter, stack_pointer, regis_A, regis_B, instruction_names[opcode].c_str(), operand);
}



int extract_opcode_from_harddisk(int x)
{
	//extract the last 8 bits from the instruction baciscally
		return ((1 << 8) - 1) & x;
}


int extract_operand_from_harddisk(int x)
{
	//extarct the operand from instruction : the first 24 bits; enough to right shift by 8 bits
	return(x>>8);
}

int executeInstruction(int opcode, int operand) {
	if (opcode == 0) ldc(operand);
	if (opcode == 1) adc(operand);
	if (opcode == 2) ldl(operand);
	if (opcode == 3) stl(operand);
	if (opcode == 4) ldnl(operand);
	if (opcode == 5) stnl(operand);
	if (opcode == 6) add();
	if (opcode == 7) sub();
	if (opcode == 8) shl();
	if (opcode == 9) shr();
	if (opcode == 10) adj(operand);
	if (opcode == 11) a2sp();
	if (opcode == 12) sp2a();
	if (opcode == 13) call(operand);
	if (opcode == 14) RETURN();
	if (opcode == 15) brz(operand);
	if (opcode == 16) brlz(operand);
	if (opcode == 17) br(operand);
	else return 0;
	return 1;
}

void emulatefunc(bool live)
	{
		stack_pointer = (1 << 23) -1;
		int operand = 0, opcode = 0;
		int instruction_num = 0;
		program_counter = 0;

		while ((opcode = extract_opcode_from_harddisk(harddisk2[program_counter])) <= 18) {
			
			operand = extract_operand_from_harddisk(harddisk2[program_counter]);

			if(live) liveEmulation(opcode,operand);

			// if (trace) printTrace(opcode, operand);
			if (opcode == 18) break;
			// Invalid Opcode
			if (opcode > 18) break;

			program_counter += 1;
			executeInstruction(opcode, operand);
			instruction_num += 1;
		}
		if(live) fprintf(stderr, "\n%d instructions executed.\n", instruction_num);
	}

void reset_hdd2()
{
	int x = 1<<24;
	for(int i=0;i<x;i++)
	{
		harddisk2[i] = harddisk[i];
	}
}

int main()
{


	FILE* dumpfile;

	int total_no_instructions;
    fstream objstream;

	// FILE* objfile;
//start the emulator design in the terminal and show the opertaing options


//---------------------------------------------------------------
cout<<"    ._________________.          --OPTIONS--\n";
   cout<<"    |.---------------.|\n";
   cout<<"    ||               ||    1) dump_before\n";
   cout<<"    ||    -._ .-.    ||    2) dump_after\n";
   cout<<"    ||   -._| | |    ||    3) show_isa\n";
   cout<<"    ||   -._| | |    ||    4) emulate\n";
  cout<< "    ||   -._|.-.|    ||    5) export_dump\n";
   cout<<"    ||_______________||    6) help\n";
   cout<<"    /.-.-.-.-.-.-.-.-.\\    7) exit\n";
  cout<<"   /.-.-.-.-.-.-.-.-.-.\\\n";
 cout<<"  /.-.-.-.-.-.-.-.-.-.-.\\\n";
cout<<" /______/__________\\___o_\\\n";
cout<<" \\_______________________/\n";

cout<<"\n";


// -------------------------------------------------------------


printf("Enter the Object_code_name in current_folder to emulate:\n");
	//take the input of the file name as a string
	string filename;
	cout<<"-->";
	cin>>filename;

    objstream.open(filename, ios::in); 

    if (objstream.is_open()) { 
    	int instr_number = 0;
        string bin32_string;
        // Read data from the file object and put it into a string.
        while (getline(objstream, bin32_string)) { 
            // Print the data of the string.
            // cout << sa << "\n"; 
            harddisk[instr_number] = bin_string_to_decimal(bin32_string);
            instr_number++;

        }

        total_no_instructions = instr_number;
        
        // Close the file object.
        objstream.close(); 
    }
    else 
    	{
    		cout<<"-------> No such file\n";
    		exit(0);
    	}

    int maxi = 1<<24;
    for(int i=0;i<maxi;i++)
    {
    	harddisk2[i] = harddisk[i];
    }

// -------------------------------------------------------------

string file_name_without_o = filename.substr(0,filename.find('.'));

string export_dump_name = file_name_without_o+"_MEMORY_DUMP.txt";



// cout<<dumpfile;


// objfile = fopen(filename, "rb");


cout<<"\nENTER OPTION:\n";
string control = "test";


while(control!="exit")
{
	cout<<"\n--> ";
	cin>>control;

	if(control=="help")
	{	
		cout<<"\n::COMMANDS::\n";
		cout<<"1) dump_before : show memory dump before emulation\n";
		cout<<"2) dump_after  : show memory dump after emulation\n";
		cout<<"3) show_isa    : show the instuction set in terminal\n";
		cout<<"4) emulate     : perform emulation in the terminal\n";
		cout<<"5) export_dump : store in a dump file\n";
		cout<<"6) help        : show command meanings\n";
		cout<<"7) exit        : exit the emulator\n";
	}
	if(control=="show_isa")
	{
		show_isa_function();
	}
	if(control =="dump_before")
	{	
		cout<<"MEMORY DUMP BEFORE EXECUTION (HEX)\n";
		for(int i=0;i<total_no_instructions;i++)
		{
			fprintf(stderr,"%08x ",harddisk[i]);
			if(i%4==3) cout<<"\n";

		}

	}
	if(control == "emulate")
	{
		// for(int i=0;i<total_no_instructions;i++)
		// {
		// 	//read the instruciton from the hardrive (instruction memory and data memory) and then
		// 	//emulate them on the terminal

		emulatefunc(1);
		reset_hdd2();
	}


	if(control =="dump_after")
	{
		emulatefunc(0);
		cout<<"MEMORY DUMP AFTER EXECUTION (HEX)\n";
		for(int i=0;i<total_no_instructions;i++)
		{
			fprintf(stderr,"%08x ",harddisk2[i]);
			if(i%4==3) cout<<"\n";
		}
		reset_hdd2();
	}
	if(control=="export_dump")
	{
			dumpfile = fopen(export_dump_name.c_str(), "w");

			emulatefunc(0);
			for(int i=0;i<total_no_instructions;i++)
			{
						fprintf(dumpfile,"%08x  ",harddisk2[i]);
						if(i%4==3) fprintf(dumpfile, "\n");
			}
			reset_hdd2();
			fclose(dumpfile);



	}

}




































}