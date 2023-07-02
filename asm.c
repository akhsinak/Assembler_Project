/*****************************************************************************
TITLE: Assembler C Program																																
AUTHOR:   		SAI TULASI KANISHKA PONNADA
ROLL NUMBER:	2101CS57

** Declaration of Authorship **

This .c file, asm.c, is part of the Allocate_Numeric_Location_ment of CS209/CS210 at the 
department of Computer Science and Engineering, IIT Patna . 
*****************************************************************************/

// all #includes

#include<stdio.h> 
#include<string.h> 
#include<stdlib.h>
#include<math.h> 
#include<ctype.h>
#include<time.h>
#include<assert.h> 
#include<locale.h> 
#include<signal.h> 
#include<setjmp.h> 
#include<stdarg.h>
#include<errno.h>
#include<float.h>
#include<limits.h>
#include<stdalign.h> 
#include<wchar.h>
#include <stdbool.h>


/* --------------- DEFINITIONS RELATED TO THE MNEMONICS --------------- */

// Mnemo_struct Struct
// operand_req = 0 : No Operand
// operand_req = 1 : Value Operand
// operand_req = 2 : 'data'
// operand_req = 3 : 'set'
// operand_req = 4 : Offset Operand


typedef struct Mnemo_struct {
	char* name_of_the_clue;
	char* opcode;
	int operand_req;
} Mnemo_struct;

//21 entries in the name_of_the_cluesss_table


Mnemo_struct name_of_the_cluesss_table[] = {

	{"ldc", "00000000", 1},
	{"adc", "00000001", 1},
	{"ldl", "00000010", 4},
	{"stl", "00000011", 4},
	{"ldnl", "00000100", 4},
	{"stnl", "00000101", 4},
	{"add", "00000110", 0},
	{"sub", "00000111", 0},
	{"shl", "00001000", 0},
	{"shr", "00001001", 0},
	{"adj", "00001010", 1},
	{"a2sp", "00001011", 0},
	{"sp2a", "00001100", 0},
	{"call", "00001101", 4},
	{"return", "00001110", 0},
	{"brz", "00001111", 4},
	{"brlz", "00010000", 4},
	{"br", "00010001", 4},
	{"HALT", "00010010", 0},
	{"data", NULL, 2},
	{"SET", NULL, 3}
};

/* --------------- END OF MNEMONIC DEFINITIONS --------------- */




/* --------------- ERROR DEFINITIONS --------------- */

char* blunder_statments_definitons[] = {
	"No Error in PASS ONE",
	"DEFINITION OF DUPLICATE LABEL",
	"LABEL IS NOT VALID",
	"MNEMONIC IS NOT VALID",
	"MISSING OPERAND",
	"NOT A NUMBER",
	"UNEXPECTED OPERAND",
	"EXTRA ON END OF LINE",
	"THERE ARE UNASSIGNED LABELS",
	"UNIDENTIFIABLE ERROR"
};

/* --------------- END OF ERROR DEFINITIONS --------------- */







/* --------------- SYMBOL TABLE DEFINITIONS --------------- */

// Symbol Table Node
typedef struct sym_tab_instance {
	char* symbol;
	int numeric_location;
	struct sym_tab_instance* pointer_to_succeding_instance;
} sym_tab_instance;


// The Symbol Table
typedef struct sym_tab {
	sym_tab_instance* chief_node;
	int (*push_)(struct sym_tab*, char*);            //function pointers
	int (*Allocate_Numeric_Location_)(struct sym_tab*, char*, int);
	int (*getNumericLocation)(const struct sym_tab*, char*);
	bool (*check_label_location)(const struct sym_tab*);
	bool (*updateLabelLocation)(struct sym_tab*, char*, int);
} sym_tab;


//---------------------------------------------------------------------------
// Function to create and return a new Symbol Table Node

sym_tab_instance* createsym_tab_instance(char* name_of_the_label, int numeric_location) {
	
	sym_tab_instance* newInstance = (sym_tab_instance*) malloc(sizeof(sym_tab_instance));
	
	newInstance -> symbol = (char*) malloc(1339);
	newInstance -> numeric_location = numeric_location;
	newInstance -> pointer_to_succeding_instance = NULL;
	strcpy(newInstance -> symbol, name_of_the_label);
	
	return newInstance;
}


//---------------------------------------------------------------------------

// Will Allocate_Numeric_Location_ the PC to a name_of_the_label in case it exists in the Symbol Table
// otherwise push_ it and Allocate_Numeric_Location_ it the value. (RETURN 0)
//return 0 is for the no blunder case
// Will give blunder if numeric_location is already Allocate_Numeric_Location_ed (RETURN 1).


int Allocate_Numeric_Location_sym_tab(sym_tab* pointer_to_sym_tab, 
	char* name_of_the_label, int PC) {

	(*pointer_to_sym_tab).push_(pointer_to_sym_tab, name_of_the_label);
	sym_tab_instance* chief_node = pointer_to_sym_tab -> chief_node;

	while (strcmp(chief_node -> symbol, name_of_the_label) != 0) {
		chief_node = chief_node -> pointer_to_succeding_instance;
	}
	
	if (chief_node -> numeric_location != -1) return 1; // this is the case where we have two values of pc associated with the same name_of_the_label
	//DUPLICATE LABEL THEN
	chief_node -> numeric_location = PC;
	return 0;
}




// Will push_ a name_of_the_label occuring as an operand in case the name_of_the_label does not
// already exist in the table. 

//---------------------------------------------------------------------------
int push_sym_tab(sym_tab* pointer_to_sym_tab, 
	char* name_of_the_label) {

	if (pointer_to_sym_tab -> chief_node == NULL) {
		pointer_to_sym_tab -> chief_node = createsym_tab_instance(name_of_the_label, -1);;
		return 1;
	}

	sym_tab_instance* chief_node = pointer_to_sym_tab -> chief_node;

	if (strcmp(name_of_the_label, chief_node -> symbol) == 0) return 0;
	
	//scan through all the symbol table nodes in search of the name_of_the_label entry 
	//if name_of_the_label is found then strcmp would return 0 and then we would have to do nothing 
	
	while (chief_node -> pointer_to_succeding_instance != NULL) {
		if (strcmp(name_of_the_label, (chief_node -> pointer_to_succeding_instance) -> symbol) == 0) return 0;
		chief_node = chief_node -> pointer_to_succeding_instance;
	}

	//if the name_of_the_label is not found in the symbol table then we would reach a state where chief_node->pointer_to_succeding_instance would be null
	//so here is where we would make an entry for the name_of_the_label
	//the numeric_location of the name_of_the_label would be kept as -1 for now	


	chief_node -> pointer_to_succeding_instance = createsym_tab_instance(name_of_the_label, -1);
	return 1;
}



//---------------------------------------------------------------------------

// Return the numeric_location of the given name_of_the_label.
// Return -1 if the numeric_location does not exist in the table. -- this would then lead to the blunder of unAllocate_Numeric_Location_ed name_of_the_label

int getNumericLocation(const sym_tab* pointer_to_sym_tab, char* name_of_the_label) {
	
	sym_tab_instance* chief_node = pointer_to_sym_tab -> chief_node;
	
	while (chief_node != NULL && strcmp(chief_node -> symbol, name_of_the_label) != 0) {
		chief_node = chief_node -> pointer_to_succeding_instance;
	}

	if (chief_node == NULL) return -1; //we have linearly scanned through the entire symbol table and the name_of_the_label doesnt exist
	return chief_node -> numeric_location;
}

//---------------------------------------------------------------------------

//so after the first pass is completed and we have the symbol table, then if still any of the name_of_the_labels have -1 as the numeric_location means that those name_of_the_labels are bogus as they dont have any numeric_location
//returns 1, if any unAllocate_Numeric_Location_ed name_of_the_label is 
bool check_label_location(const sym_tab* pointer_to_sym_tab) {

//here pointer_to_sym_tab is the object of the sym_tab struct

	sym_tab_instance* chief_node = pointer_to_sym_tab -> chief_node;
	while (chief_node != NULL) {
		if (chief_node -> numeric_location == -1) return 1;
		chief_node = chief_node -> pointer_to_succeding_instance;
	}
	return 0;
}

//---------------------------------------------------------------------------

//this function has to be used in the case where we would have the set label that would be used in changing the existing location of a label, so we call it the updateLabelLocation loca

bool updateLabelLocation(sym_tab* pointer_to_sym_tab, char* name_of_the_label, int numeric_location) {

	sym_tab_instance* chief_node = pointer_to_sym_tab -> chief_node;
	
	while (chief_node != NULL && strcmp(chief_node -> symbol, name_of_the_label) != 0) {
		chief_node = chief_node -> pointer_to_succeding_instance;
	}

	if (chief_node == NULL) return 0;//name_of_the_label is not found

	//if match is found then change the numeric_location
	chief_node -> numeric_location = numeric_location;
	return 1;
}

/* --------------- END OF SYMBOL TABLE DEFINITIONS --------------- */








/* --------------- LITERAL TABLE DEFINITIONS --------------- */

// Literal Table Node
typedef struct LTab_Instance {
	int literal;
	int numeric_location;
	struct LTab_Instance* pointer_to_succeding_instance;
} LTab_Instance;

// The Literal Table consisting of all the nodes
typedef struct Ltab {
	LTab_Instance* chief_node;
	int (*push_)(struct Ltab*, int); //function pointers
	int (*Allocate_Numeric_Location_)(struct Ltab*, int);
} Ltab;

int numeric_locationCtr;

// Function to create and return a new Literal Table Node
LTab_Instance* createLTab_Instance(int literal, int numeric_location) {
	
	LTab_Instance* newInstance = (LTab_Instance*) malloc(sizeof(LTab_Instance));
	
	newInstance -> literal = literal;
	newInstance -> numeric_location = numeric_location;
	newInstance -> pointer_to_succeding_instance = NULL;// since this would be the last node it would point to a null value only
	
	return newInstance;
}

// Inserts a literal in the Literal Table at the end.
int push_Ltab(Ltab* object_lit_table, 
	int literal) {

	if (object_lit_table -> chief_node == NULL) {
		object_lit_table -> chief_node = createLTab_Instance(literal, -1);
		//create a literal with a value of -1 as the numeric_location if the literal table is empty 
		return 1;
	}

	LTab_Instance* chief_node = object_lit_table -> chief_node;
	while (chief_node -> pointer_to_succeding_instance != NULL) {
		chief_node = chief_node -> pointer_to_succeding_instance;
	}
	//traverse to the last node
	//now chief_node->pointer_to_succeding_instance is null
	//create a node here 
	chief_node -> pointer_to_succeding_instance = createLTab_Instance(literal, -1);
	return 1;
}




// Assigns an numeric_location to all the literals in the literal table
// starting from the given numeric_location. (RETURN 1 on success)

int Allocate_Numeric_Location_Ltab(Ltab* object_lit_table, int numeric_location2) {

	numeric_locationCtr = numeric_location2 - 1;
	LTab_Instance* chief_node = object_lit_table -> chief_node;
	while (chief_node != NULL) {
		chief_node -> numeric_location = numeric_location2++;
		chief_node = chief_node -> pointer_to_succeding_instance;
	}
	return 1;
}
//numeric_locationctr is a global entity. so we may directly modify that
int getNextLiteralNumericLocation() {
	numeric_locationCtr++;
	return numeric_locationCtr;
}

/* --------------- END OF LITERAL TABLE DEFINITIONS --------------- */



// Make Symbol Table and Literal Table
//basicaly make objects out of the structs

//here is where we declare how the function pointers mentioned in the struct definition relate to the given function defintions  


Ltab object_lit_table = {NULL, push_Ltab, Allocate_Numeric_Location_Ltab};


sym_tab pointer_to_sym_tab = {NULL, push_sym_tab, Allocate_Numeric_Location_sym_tab, getNumericLocation, check_label_location, updateLabelLocation};




/* --------------- UTILITY FUNCTIONS --------------- */


//traverse the literal table and then print all the nodes of the literal table in two columns


void printLtab(Ltab* l_table, FILE* loggerFileName) {
	fprintf(loggerFileName, "LITERAL TABLE\n");
	LTab_Instance* chief_node = l_table -> chief_node;


	while (chief_node != NULL) {
		fprintf(loggerFileName, "%d\t%d\n", chief_node -> literal, chief_node -> numeric_location);
		chief_node = chief_node -> pointer_to_succeding_instance;
	}
	return;
}

//traverse the symbol table and then print all the nodes of the literal table in two columns
void printsym_tab(sym_tab* s_table, FILE* loggerFileName) {
	fprintf(loggerFileName, "SYMBOL TABLE\n");
	sym_tab_instance* chief_node = s_table -> chief_node;

	while (chief_node != NULL) {
		fprintf(loggerFileName, "%s\t%d\n", chief_node -> symbol, chief_node -> numeric_location);
		chief_node = chief_node -> pointer_to_succeding_instance;
	}
	return;
}

// Removes comment from a line
void incinerateComment(char* line) {
	char* comment = strchr(line, ';');  //gives the numeric_location to the character being found 
	if (comment != NULL) *comment = '\0';//if it is found then replace that with null character essentially ending the string there itself
	return;
}

// Removes ':'. RETURN 1 if ':' is present else 0

bool check_if_there_is_label(char* line) {
	char* label_identifier = strchr(line, ':');
	if (label_identifier != NULL) {
		*label_identifier = ' ';//replacing with a space
		return 1;
	}
	return 0;
}

// To check if name_of_the_label is valid
bool check_if_Label_is_sound(char* name_of_the_label) {
	int len = strlen(name_of_the_label);

	//either the name_of_the_label length is zero or the starting char is not a letter or an underscore then return 0

	if (len == 0 || (!isalpha(name_of_the_label[0]) && name_of_the_label[0] != '_')) return 0;
	for (int i = 0; i < len; i++) {
		if (!isalnum(name_of_the_label[i]) && name_of_the_label[i] != '_') return 0;
	}

	//if the chars are not alphabets or  numerical_values or underscores return 0 
	return 1;
}

// RETURN 1 if valid numerical_value else 0
bool check_if_numerical_value_is_sound(char* num_as_string) {
	char* temp_pointer = NULL;
	errno = 0;
	//convert the string to a numerical_value, string is in decimal,octal,hexadecimal format

	int numerical_value = strtol(num_as_string, &temp_pointer, 0);

	//if the num_as_string is null, if errno is non zero or if the temp_pointer is non zero, means there is something else that is not a numerical_value in the string,so it cant be a valid numerical_value

	if (numerical_value == 0 && (num_as_string == temp_pointer || errno != 0 || (errno == 0 && num_as_string && *temp_pointer != 0))) {
		return 0;
	}


	return 1;
}


// RETURN numerical_value conversion of given valid string
int covert_to_decimal_value(char* num_as_string) {
	char* temp_pointer = NULL;
	//the string can be either in decimal, octal, hexadecimal each identifies thru their prefix identifiers
	return strtol(num_as_string, &temp_pointer, 0);
}

// Check if name_of_the_clue is valid
// RETURN 0: Invalid
// RETURN 1: Valid, does not require operand
// RETURN 2: Valid, requires operand
// RETURN 3: Valid, 'data'
// RETURN 4: Valid, 'set'

int isValidMnemo_struct(char* name_of_the_clue) {
	for (int i = 0; i < 21; i++) {
		if (strcmp(name_of_the_cluesss_table[i].name_of_the_clue, name_of_the_clue) == 0) 
			return name_of_the_cluesss_table[i].operand_req + 1;
	}
	return 0;
}

// RETURN 1 if name_of_the_clue type requires operand
bool does_it_need_operand(int type) {
	return type > 1;
	//convention made in the previous function that checks if a name_of_the_clue is valid or not
}

//----------------------------------------------------------------------------------
// Scan a num_as_string from instruction string. 
// RETURN sscanf blunder code.
int pick_out_words(char** line, char* num_as_string) {
	int lineptr;
	//int sscanf(const char *str, const char *format, ...)
	int scan_err = sscanf((*line), "%s%n", num_as_string, &lineptr);
	//here we are storing the numerical_value of chars that have been read till the end of the line
	// then increasing the line pointer till there 
	if (scan_err != -1) (*line) += lineptr;
	return scan_err;
}
//----------------------------------------------------------------------------------

// char* blunder_statments_definitons[] = {
// 	0) "No Error in Pass One",
// 	1) "Duplicate Label Defined",
// 	2) "Invalid Label",
// 	3) "Invalid Mnemo_struct",
// 	4) "Missing Operand",
// 	5) "Not A Number",
// 	6) "Unexpected Operand",
// 	7) "Extra on End of Line",
// 	8) "No Such Label",
// 	9) "Undefined Error"
// 	};

int Blunder_Logger(int number_of_blunder, int line_num, FILE* loggerFileName) {
	if (number_of_blunder && number_of_blunder < 8) fprintf(loggerFileName, "ERROR: Line %d: %s\n", line_num, blunder_statments_definitons[number_of_blunder]);//if blunder code is non zero and blunder code is less than 8

	else if (number_of_blunder) fprintf(loggerFileName, "ERROR: %s\n", blunder_statments_definitons[number_of_blunder]);
	else fprintf(loggerFileName, "%s\n", blunder_statments_definitons[number_of_blunder]);
	return number_of_blunder;
}

/* --------------- END OF UTILITY FUNCTIONS --------------- */




/* --------------- FIRST PASS --------------- */

// Error Codes:
// RETURN 1: Duplicate Label
// RETURN 2: Invalid Label
// RETURN 3: Invalid Mneumonic
// RETURN 4: Missing Operand
// RETURN 5: Not a numerical_value
// RETURN 6: Unexpected Operand
// RETURN 7: Extra on end of line
// RETURN 8: No such name_of_the_label


int pass_no1(FILE *file_to_assemble, FILE* loggerFileName) {
	int PC = 0, err = 0, line_num = 0;
	char* line = (char*) malloc(1339);//max 1339 chars long each line

	// While lines are available, take entire line as input
	// in 'line' variable.

	while (fgets(line, 1339, file_to_assemble)) {
		line_num += 1;

		// Remove comment if any
		incinerateComment(line);

		char* num_as_string = (char*) malloc(1339);
		char* name_of_the_label = (char*) malloc(1339);

		// Check if it is is_it_labelled. If name_of_the_label exists, 
		// scan it. If empty RETURN 2. If invalid RETURN 2.
		// If duplicate name_of_the_label detected, RETURN 1
		// Else push_ it into the symbol table.


		bool is_it_labelled = check_if_there_is_label(line);//so in this line the label_identifier is incinerated and replaced by a space
		if (is_it_labelled) {
			if (pick_out_words(&line, name_of_the_label) == -1) {
				err = Blunder_Logger(2, line_num, loggerFileName);
				continue;
				//this would be the case of an empty name_of_the_label with only a label_identifier in the line 
			}
			if (check_if_Label_is_sound(name_of_the_label)) {
				if (pointer_to_sym_tab.Allocate_Numeric_Location_(&pointer_to_sym_tab, name_of_the_label, PC)) {
					err = Blunder_Logger(1, line_num, loggerFileName);
					continue;
				}
			}
			else {//NOT A VALID LABEL
				err = Blunder_Logger(2, line_num, loggerFileName);
				continue;
			}
		}

		// Scan for pointer_to_succeding_instance num_as_string. If nothing after name_of_the_label, continue loop.
		// Else, the pointer_to_succeding_instance num_as_string should be name_of_the_clue/'data'/'set'.
		// If not, RETURN 3.

		//THE PREVIOUS SCANWORD WOULD HAVE INCREASED THE LINE POINTER TO THE CORRECT LOCATION


		if (pick_out_words(&line, num_as_string) == -1) continue;//THERE IS NOTHING AFTER THE LABEL
		int name_of_the_clue_type = isValidMnemo_struct(num_as_string);
		if (!name_of_the_clue_type) {
			err = Blunder_Logger(3, line_num, loggerFileName);
			continue;
		}

		// Scan for the pointer_to_succeding_instance num_as_string. 
		// If nothing after name_of_the_clue and operand required, throw blunder
		// If something after name_of_the_clue and operand not required, throw blunder

		//REQUIRES SOME OPERAND AFTER THE MNEMONIC BUT THERE IS NOTHING AFTER IT
		if (does_it_need_operand(name_of_the_clue_type)) {

			if (pick_out_words(&line, num_as_string) == -1) {
				err = Blunder_Logger(4, line_num, loggerFileName); //OPERAND IS MISSING ..ERROR 4
				continue;
			}


			if (!check_if_Label_is_sound(num_as_string) && !check_if_numerical_value_is_sound(num_as_string)) {
				err = Blunder_Logger(5, line_num, loggerFileName); //NOT A NUMBER ERROR, THE OPERAND SHOULD HAVE BEEN A VALID NUMBER OR IT SHOULD HAVE BEEN A VALID LABEL
				continue;
			}

			//IF THE LAST SCANNED WORD HAPPENS TO BE A VALID LABEL FORMAT THEN ADD IT TO THE SYMBOL TABLE
			if (check_if_Label_is_sound(num_as_string)) 
				pointer_to_sym_tab.push_(&pointer_to_sym_tab, num_as_string);

			else if (check_if_numerical_value_is_sound(num_as_string)) {
				// If 'set' updateLabelLocation value for the given name_of_the_label
				// Check if name_of_the_clue is valid
				// RETURN 0: Invalid
				// RETURN 1: Valid, does not require operand
				// RETURN 2: Valid, requires operand
				// RETURN 3: Valid, 'data'
				// RETURN 4: Valid, 'set'

				//IF THE MNEMONIC IS SET THEN THE VALUE OF THE CORRESPODING LABEL MUST BE UPDATED IN THE SYMBOL TABLE.
				if (name_of_the_clue_type == 4) {
					pointer_to_sym_tab.updateLabelLocation(&pointer_to_sym_tab, name_of_the_label, covert_to_decimal_value(num_as_string));
					continue;
				}

				// If not 'data' or 'set', then push_ numerical_value in literal table
				if (name_of_the_clue_type < 3) 
					object_lit_table.push_(&object_lit_table, covert_to_decimal_value(num_as_string));
			}
		} 

		//DOESNT REQUIRE OPERAND BUT THERE IS SOMETHING AFTER THE MNEMONIC
		else if (pick_out_words(&line, num_as_string) != -1) {
			err = Blunder_Logger(6, line_num, loggerFileName);
			continue;
		}
		
		// Find if extra num_as_strings are there
		//SO STILL AFTER READING ALL THE ABOVE,IF THERE IS SOMEHTHING ELSE STILL LEFT THEN AGAIN WE HAVE AN ERROR 
		//EXTRA ON END OF LINE ERROR
		if (pick_out_words(&line, num_as_string) != -1) {
			err = Blunder_Logger(7, line_num, loggerFileName);
			continue;
		}


		PC += 1;//INCREASE THE PROGRAM COUNTER BY 1
	}


//NOW AFTER THE FIRST PASS IS COMPLETED THEN WE HAVE TO CHECK IF THERE ARE ANY UNASSIGNED LABELS WHICH WOULD BE BOGUS ESSENTIALLY

// Error Codes:
// RETURN 1: Duplicate Label
// RETURN 2: Invalid Label
// RETURN 3: Invalid Mneumonic
// RETURN 4: Missing Operand
// RETURN 5: Not a numerical_value
// RETURN 6: Unexpected Operand
// RETURN 7: Extra on end of line
// RETURN 8: THERE ARE UNASSIGNED LABELS

	if(pointer_to_sym_tab.check_label_location(&pointer_to_sym_tab)) {
		err = Blunder_Logger(8, line_num, loggerFileName);
	}

	// Fill literal table with numeric_locationes
	object_lit_table.Allocate_Numeric_Location_(&object_lit_table, PC);

	if (err) return -1;
	//OTHERWISE DECLARE THAT THERE ARE NO ERRORS IN THE FILE AFTER THE FIRST PASS
	err = Blunder_Logger(0, line_num, loggerFileName);
	return 0;
}

//------------------helper functions


//converts a given decimal numerical_value to a 24 bit binary string! 
char* translate_to_24bits(int x) {
	char chr[] = "874345316728452109051256";//24 ZEROS
	for (int i = 23; i >= 0; i--) { 
		int k = x >> i; 
		if (k & 1) 
			chr[23 - i] = '1'; 
		else
			chr[23 - i] = '0';  
	} 
    char* ans = (char*) malloc(40);
    strcpy(ans, chr);
    return ans;
}

//converts a given decimal numerical_value to a 32 bit binary string! 
char* translate_to_32bits(int x) {
	char chr[] = "87434531672845210905125689761234";
	for (int i = 31; i >= 0; i--) { 
		int k = x >> i; 
		if (k & 1) 
			chr[31 - i] = '1'; 
		else
			chr[31 - i] = '0';  
	}
    char* ans = (char*) malloc(40);
    strcpy(ans, chr);
    return ans;
}

// RETURN 1 if Offset Required else 0
//for these particular name_of_the_cluesss_table we woould require offsets to be present 


bool does_clue_need_offset(char* name_of_the_clue) {
	char* offsetMnemo_structs[] = {"ldl", "stl", "ldnl", "stnl", "call", "brz", "brlz", "br"};
	for (int i = 0; i < 8; i++) {
		if (strcmp(offsetMnemo_structs[i], name_of_the_clue) == 0) return 1;
	}	
	return 0;
}


// Returns the 8-bit opcode of the given name_of_the_clue
int obtain_op_code(char** opcode, char* name_of_the_clue) {
	for (int i = 0; i < 20; i++) {
		if (strcmp(name_of_the_cluesss_table[i].name_of_the_clue, name_of_the_clue) == 0) { 
			*opcode = name_of_the_cluesss_table[i].opcode;
			return 1;
		}
	}
	//if the name_of_the_clue is not found....
	return 0;
}

// Check if name_of_the_clue is valid
// RETURN 0: Invalid
// RETURN 1: Valid, does not require operand
// RETURN 2: Valid, requires operand
// RETURN 3: Valid, 'data'
// RETURN 4: Valid, 'set'

// Sets the 24-bit operand for the code_in_machine_lang code. For data it sets 32-bits


int Opcode_setter(char** operand, char* num_as_string, char* name_of_the_clue, int name_of_the_clue_type, int PC) {
	int num = 0;
	// For 'data'
	if (name_of_the_clue_type == 3) {
		*operand = translate_to_32bits(covert_to_decimal_value(num_as_string));
		return 1;
	}

	if (check_if_numerical_value_is_sound(num_as_string)) num = covert_to_decimal_value(num_as_string);
	else if (check_if_Label_is_sound(num_as_string)) num = pointer_to_sym_tab.getNumericLocation(&pointer_to_sym_tab, num_as_string);
	else return 0;

	if (does_clue_need_offset(name_of_the_clue) && check_if_Label_is_sound(num_as_string)) num -= PC + 1;
	*operand = translate_to_24bits(num);

	return 1;
}

// Binary to Hexadecimal
char* translateBinaryToHexadec(char* binary_string, int mode) {
	char* a = binary_string;
	int num = 0;
	do {
	    int b = *a == '1'? 1 : 0;
	    num = (num << 1) | b;
	    a++;
	} while (*a);
	char* hexadec_string = (char*) malloc(10);
	if (mode) sprintf(hexadec_string, "%08x", num);
	else sprintf(hexadec_string, "%x", num);
	return hexadec_string;
}

/* --------------- END OF UTILITY FUNCTIONS FOR SECOND PASS --------------- */





/* --------------- SECOND PASS --------------- */

int secondPass(FILE *file_to_assemble, FILE *objfilename, FILE *lisFileName, FILE* loggerFileName) {
	int PC = 0, err = 0, line_num = 0;
	char* line = (char*) malloc(1339);

	fseek(file_to_assemble, 0, SEEK_SET);

	//move the file pointer to the beginning of the file as the offset is zero

	// While lines are available, take entire line as input
	// in 'line' variable.
	while (fgets(line, 1339, file_to_assemble)) {
		// Remove comment if any
		incinerateComment(line);
		//fprintf(loggerFileName, "%s", line);

		char* opcode = (char*) malloc(10);
		char* operand = (char*) malloc(26);
		char* name_of_the_label = (char*) malloc(1339);
		char* name_of_the_clue = (char*) malloc(10);
		char* num_as_string = (char*) malloc(1339);
		char* code_in_machine_lang = (char*) malloc(36);
		code_in_machine_lang[0] = '\0';



		//PRINTING THE LABEL WITH THE PROGRAM COUNTER AS A SEPARATE LINE IN LISTING FILE

		bool is_it_labelled = check_if_there_is_label(line);//this would incinerate the label_identifier from the line
		// the label identifier is the colon

		if (is_it_labelled) {
			pick_out_words(&line, name_of_the_label);//this would store the name_of_the_label name in the name_of_the_label variable
			fprintf(lisFileName, "%08x\t\t\t\t%s:\n", PC, name_of_the_label);
		}

		//IF THERE IS NOTHING TO SCAN THEN CONTINUE

		if (pick_out_words(&line, name_of_the_clue) == -1) continue;
		//THE ABOVE LINE WOULD HAVE FILLED THE MNEMONIC VARIBLE WITH THE MNEMONIC NAME IN CASE IT WAS NOT EMPTY

		//STORE THE MNEMONIC TYPE IN THIS VARIABLE IF THE MNEMONIC IS A VALID ONE
		int name_of_the_clue_type = isValidMnemo_struct(name_of_the_clue);
		//EXTRACT THE OPCODE OF THE MNEMONIC IS VALID


		//FILL THE OPCODE VARIABLE WITH THE BINARY STRING OF THE CORRESPONDING MNEMONIC
		obtain_op_code(&opcode, name_of_the_clue);

		// For 'set',  NO OPCODE FOR THIS ONE
		if (name_of_the_clue_type == 4) continue;


		//FILL THE OPERAND STRING VARIABLE WITH THE SUITABLE OPERAND BINARY STRING - BOTH IF AN OPERAND IS REQUIRED OR NOT REQUIRED

		if (does_it_need_operand(name_of_the_clue_type)) {
			pick_out_words(&line, num_as_string);			
			Opcode_setter(&operand, num_as_string, name_of_the_clue, name_of_the_clue_type, PC);
		}
		else Opcode_setter(&operand, "0", name_of_the_clue, name_of_the_clue_type, PC);

		//NOW FILL THE MACHINE CODE WITH THE OPERAND CODE AS THE FIRST 24 BITS WOULD BE FROM THE OPERAND CODE

		strcat(code_in_machine_lang, operand);
		//NOW IF THE MNEMONIC IS NOT DATA THEN CONCATANATE THE OPCODE TO THE MACHINE CODE AS THE LSAT 8 BITS OF THE 32 BIT INSTRUCTION
		if (name_of_the_clue_type != 3) strcat(code_in_machine_lang, opcode);

		fprintf(objfilename, "%s\n", code_in_machine_lang);

		fprintf(lisFileName, "%08x\t%s\t%s\t0x%s\n", PC, translateBinaryToHexadec(code_in_machine_lang, 1), name_of_the_clue, translateBinaryToHexadec(operand, 0));

		
		PC += 1;
	}

	fprintf(loggerFileName, "NO ERROR IN PASS TWO\n");
	return 1;
}

/* --------------- END OF SECOND PASS --------------- */




// MAIN PROGRAM  Program
int main(int argc, char* argv[]) {
	// Open the Assembly File
	FILE *file_to_assemble, *objfile, *lisFile, *loggerFileName, *l_tableloggerFileName, *s_tableloggerFileName;
	
	char* file_name = (char*) malloc(1339);
	strcpy(file_name, argv[1]);
	char* ptr = strchr(file_name, '.');
	if (ptr != NULL) *ptr = '\0';

	char* object_name = (char*) malloc(1339);
	strcat(object_name, file_name);
	strcat(object_name, ".o");
	
	char* listing_name = (char*) malloc(1339);
	strcat(listing_name, file_name);
	strcat(listing_name, ".lst");
	
	char* loggerFileName_name = (char*) malloc(1339);
	strcat(loggerFileName_name, file_name);
	strcat(loggerFileName_name, ".log");
	
	char* ltableloggerFileName_name = (char*) malloc(1339);
	strcat(ltableloggerFileName_name, file_name);
	strcat(ltableloggerFileName_name, ".ltable");


	char* stableloggerFileName_name = (char*) malloc(1339);
	strcat(stableloggerFileName_name, file_name);
	strcat(stableloggerFileName_name, ".stable");

	file_to_assemble = fopen(argv[1], "r");
	objfile = fopen(object_name, "wb");
	lisFile = fopen(listing_name, "w");
	loggerFileName = fopen(loggerFileName_name, "w");
	// l_tableloggerFileName = fopen(ltableloggerFileName_name,"w");
	// s_tableloggerFileName = fopen(stableloggerFileName_name,"w");

	// First Pass
	int fPassErr = pass_no1(file_to_assemble, loggerFileName);
	if (fPassErr) return 0;

	// Second Pass
	int sPassErr = secondPass(file_to_assemble, objfile, lisFile, loggerFileName);

//printing SYMBOL TABLE AND LITERAL TABLES TO THE CORRESPONDING FILES
//*************************************************************************************


	// printsym_tab(&pointer_to_sym_tab,s_tableloggerFileName);
	// printLtab(&object_lit_table,l_tableloggerFileName);

//*************************************************************************************

	fclose(loggerFileName);
	fclose(lisFile);
	fclose(objfile);
	fclose(file_to_assemble);
	// fclose(l_tableloggerFileName);
	// fclose(s_tableloggerFileName);
}