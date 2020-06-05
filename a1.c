//Author: Devon Allie
//Date:	May 24th 2020


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TABLE_MAX 33
#define MAX_CHAR 80
#define INPUT_FILE_PATH 1
#define OUTPUT_FILE_PATH 2
#define SET 1
#define RESET 0
#define NO_COMMENT 1

struct record readRecord(char input[MAX_CHAR]);

struct record updateRecord(struct record XM2record);

/*
* The following is an array of structs where each element of the struct is an
* XM2 instruction, its corresponding XM3 instruction, and a keyword to
* identify what rule the conversion process should follow
*/

typedef struct instructionTable
{
	char XM2_instruction[MAX_CHAR];
	char XM3_instruction[MAX_CHAR];
	char type[MAX_CHAR];
}instructionTable;

/*
* Each item in the array is of the instructionTable datatype which contains
* an XM2_instruction, and XM3_instruction, and a type.
*/

instructionTable table[]	=  
{
{"SPL0",		"SETPRI #0", 	"noOp"},
{"SPL1",		"SETPRI #1", 	"noOp"},
{"SPL2",		"SETPRI #2", 	"noOp"},
{"SPL3",		"SETPRI #3", 	"noOp"},
{"SPL4",		"SETPRI #4", 	"noOp"},
{"SPL5",		"SETPRI #5", 	"noOp"},
{"SPL6",		"SETPRI #6", 	"noOp"},
{"SPL7",		"SETPRI #7", 	"noOp"},
{"CLC",			"CLRCC C", 		"noOp"},
{"CLN",			"CLRCC N", 		"noOp"},
{"CLZ",			"CLRCC Z", 		"noOp"},
{"CLV",			"CLRCC V", 		"noOp"},
{"SEC",			"SETCC C", 		"noOp"},
{"SEN",			"SETCC N", 		"noOp"},
{"SEZ",			"SETCC Z", 		"noOp"},
{"SEV",			"SETCC V", 		"noOp"},
{"BEQ",			"CEX EQ,#1,#0", "twoInstruc"},
{"BZ",			"CEX EQ,#1,#0", "twoInstruc"},
{"BNE",			"CEX NE,#1,#0", "twoInstruc"},
{"BNZ",			"CEX NE,#1,#0", "twoInstruc"},
{"BGE",			"CEX GE,#1,#0", "twoInstruc"},
{"BLT",			"CEX LT,#1,#0", "twoInstruc"},
{"BC",			"CEX CS,#1,#0", "twoInstruc"},
{"BNC",			"CEX CC,#1,#0", "twoInstruc"},
{"BN",			"CEX MI,#1,#0", "twoInstruc"},
{"CALL",		"BL", 			"postArg"},
{"PULL",		"LD R6+,", 		"postArg"},
{"PUSH",		"ST", 			"preArg"},
{"RET",			"MOV R5,R7", 	"noOp"},
{"JUMP",		"MOV", 			"preArg"},
{"CLR.B",		"MOVL #0,",		"postArg"},
{"CLR",			"MOVLZ #0,",	"postArg"},
{"CLR.W",		"MOVLZ #0,",	"postArg"}
};

/*
* This structure defines the possible components of the each assembly record
*/

typedef struct record
{
	char firstElement[MAX_CHAR];
	char secondElement[MAX_CHAR];
	char thirdElement[MAX_CHAR];
	char comment[MAX_CHAR];
}record;

int branchFlag = RESET;	//Flag for branch statements

int commentFlag = RESET; //Flag for line that is exclusively a comment

int noLabelFlag = RESET; //Flag for line with no label

int returnFlag = RESET;

/*
* The main function accepts 3 arguments. argv[0] is the name of the programme,
* argv[1] is the path to the input file, and argv[2] is the path to the output
* file. Each line of the input file is read by the readRecord function and
* returns a segmented XM2 record where each element is assigned its own
* variable within the greater XM2_record struct. This struct is sent to 
* updateRecord to be converted to the XM3 ISA. The function returns the same 
* type of struct it accepted by all the legacy XM2 components have been updated
* to XM3. If during the readRecord function it was discovered that a record
* contained only a comment, the commentFlag is set and the comment is printed.
* If the readRecord function finds that no label is set then the first element
* of the struct record is not printed. If the branchFlag is set by the previous
* record then the next record will have "BRA" printed along with the operand
* from the previous record to indicate where the assembly programme is
* branching to. When inputFile has been completely read, both the input and
* output files are closed and the programme exits. 
*/

int main(int argc, char** argv)
{	
	FILE* inputFile;
	
	FILE* outputFile;
	
	char input[MAX_CHAR];
	
	char unparsedRecord[MAX_CHAR];	
	
	record XM2_record, XM3_record;
	
/*
* If no input file path is detected then the programme quits
*/
		
	if(argv[INPUT_FILE_PATH] == NULL)
	{
		printf("No valid input file detected\n");
		
		exit(1);
	}
	else
	{
		inputFile = fopen(argv[INPUT_FILE_PATH], "r");
	}
	
/*
* If no output file path is detected then the programme quits
*/
	
	if(argv[OUTPUT_FILE_PATH] == NULL)
	{
		printf("No valid output file given\n");
		
		exit(1);
	}
	else
	{
		outputFile = fopen(argv[OUTPUT_FILE_PATH], "w");
	}

/*
* Each line of the inputFile is set as a string named "input"
*/

	while(fgets(input, MAX_CHAR, inputFile) != NULL)
	{	
	
/*
* If input is not NULL it is segmented by the readRecord function.
* The returned record datatype is modified by the updateRecord function.
*/

		if(input)
		{			
			XM2_record = readRecord(input);
			
			XM3_record = updateRecord(XM2_record);
			
			if(commentFlag == SET)
			{
				fprintf(outputFile, "%s", XM3_record.comment);
				
				commentFlag = RESET;
			}
			else
			{
				if(!noLabelFlag) 
				{
					fprintf(outputFile, "%s ", XM3_record.firstElement);
				}
				
				noLabelFlag = RESET;
				
				fprintf(outputFile, "%s", XM3_record.secondElement);
			
				if(!branchFlag) 
				{
					fprintf(outputFile, "%s ", XM3_record.thirdElement);
				}
					
				fprintf(outputFile, "%s\n", XM3_record.comment);
			}

		}
		else
		{
			fprintf(outputFile, "\n");
		}
		
		if(branchFlag)
		{
			fprintf(outputFile, "BRA %s\n", XM3_record.thirdElement);
			
			branchFlag = RESET;
		}

	}
	
	fclose(inputFile);
	
	fclose(outputFile);
	
	return 0;
}
/*
* This function accepts a single line from the input file at a time as a string
* and dissects that string into the fundamental components of an XM2 record.
* Initially, to determine if the entire line is a comment the string is
* stripped of any initial white spaces and the first character is compared to 
* the semi-colon. If matched, the commentFlag is set and the record returned.
* This means that the record contains only a comment and requires no further
* processing. Otherwise, the record is parsed for a comment. If two tokens
* can be created from the record then a comment is present. The comment is then
* saved to the comment element of the XM2_record datatype. The first token is 
* saved to a new string that is continuously tokenized where each successive
* token is saved to the next record element until no new tokens exist. This
* XM2_record is now complete and is returned to main. 
*/

struct record readRecord(char input[MAX_CHAR])
{
	record XM2_record = {NULL, NULL, NULL, NULL};
	
	char unparsedRecord[MAX_CHAR];
	
	char* token;
	
	char nospace[MAX_CHAR];
	
	char unmodifiedInput[MAX_CHAR];
	
	strcpy(unmodifiedInput, input);
	
	token = strtok(input, " ");
	
	strcpy(nospace, token);
	
/*
* A new token is created from the input string called "nospace" in which
* any initial whitespace is removed from the string. It the first character of
* this token is a semi-colon then the entire line is a comment and an
* untokenized version of the input string is set to the comment member of the
* XM2_record structure.
*/
	
	if(nospace[0] == ';')
	{		
		strcpy(XM2_record.comment, unmodifiedInput);
		
		commentFlag = SET;
	}
	else
	{

/*
* If the line is not entirely a comment then it is tokenized based on the
* semi-colon.
*/	
	
		token = strtok(unmodifiedInput, ";");
		
		strcpy(unparsedRecord, token); //Everything before the comment
		
		token = strtok(NULL, ";");
		
		if(token != NULL) //If comment is present, everything after the comment
		{
			strcpy(XM2_record.comment, ";");
			
			strcat(XM2_record.comment, token);
		}
/*
* The unparsedRecord is tokenized based on the presents of a space or tab. As
* long as a token is present, it will be assigned to the next element of the
* XM2_record.
*/
			
		token = strtok(unparsedRecord, " \t");
		
		if(token != NULL)
		{
			strcpy(XM2_record.firstElement, token);
			
			token = strtok(NULL, " \t");
		}
		if(token != NULL)
		{
			strcpy(XM2_record.secondElement, token);
			
			token = strtok(NULL, " \t");
		}
		if(token != NULL)
		{
			strcpy(XM2_record.thirdElement, token);

			token = strtok(NULL, " \t");
		}
	}
	return XM2_record;
}

/*
* This function accepts the segmented record created by the readRecord function
* and converts any of the legacy XM2 instructions to their XM3 equivelent. If 
* either the first OR second element of the XM2 record is matched to one of the
* legacy instruction from the known set defined in the instructionTable struct
* array, a series of statements compares the type of instruction. If the first
* second instruction is the one that is matched, the first element must be a 
* label and is recorded as such. Otherwise, the first element of the XM3_record
* is NOT set and the noLabelFlag is set. When the instruction is matched to an
* instruction from the instructionTable data type, the index the match was made
* at is used to define the type of instruction that was matched. If this type
* is "noOp" then the first element of the XM3_record is set to the instruction
* at the index and the record is fully updated. If the type is "twoInstruc"
* the secondElement of the XM3_record is set to the updated instruction while 
* the thirdElement is set to either the second or third element of the 
* XM2_record depending if there was a label or not. The branchLabel is also
* set. If the instruction type is "postArg", the XM3 instruction is copied to
* the secondElement of the XM3_record. IN the special case that the XM3
* instruction is "BL", a whitespace will be appended to the end of the
* secondElement. If a label is present, the thirdElement of the XM3_record
* will be set to the thirdElement of the XM2_record, otherwise the it will be
* set to the secondElement of the XM2_record. If the type is set to "preArg"
* the secondElement of the XM3_record is set and the thirdElement is set
* based on the presents of the label in the same manner in the previous
* instruction types. If the new XM3 instruction is "ST", ",-R6" will be
* appended to the end of the thirdElement. If the instruction is "MOV", ",R7"
* will be appeneded to the end of the thirdElement. Finally the comment from 
* the XM2_record is added to the comment element of the XM3_record and the 
* XM3_record is returned. If none of the instructions were matched in the 
* forloop all the elements of the XM2_record are copied to the XM3_record 
* and the XM3_record is returned. 
*/

struct record updateRecord(struct record XM2_record)
{
	record XM3_record = {NULL, NULL, NULL, NULL}; //Errors ensue without this

	for(int i = 0; i < TABLE_MAX; i++)
	{		

/*
* If either the first or the second element of the XM2_record matches an
* XM2_instruction from the lookup table, the following sequences will determine
* the required logic for assigning the rest of the elements. The logic is based
* on the "type" element of the lookup table that corresponds to the index where
* the instruction was matched. 
*/
	
		if((strstr(XM2_record.firstElement, table[i].XM2_instruction))	||
		  (strstr(XM2_record.secondElement, table[i].XM2_instruction)))
		{
		
/*
* If the secondElement is the instruction, the firstElement must be a label.
* If the label is present the firstElement of the XM2_record is copied to the
* firstElement of the XM3_record. Otherwise, set the noLabelFlag.
*/

			if(strstr(XM2_record.secondElement, table[i].XM2_instruction))
			{
				strcpy(XM3_record.firstElement, XM2_record.firstElement);
			}
			else
			{
				noLabelFlag = SET;
			}
/*
* If "noOp" is the type, then the secondElement of the XM3_record is set to the
* XM3 instruction.
*/

			if(strcmp(table[i].type, "noOp") == 0)
			{
				strcpy(XM3_record.secondElement, table[i].XM3_instruction);
			}
			
/*
* If "twoInstruc" is the type, the secondElement is set to the XM3 instruction
* and the thirdElement is set to next XM2 element. The branchFlag is also set.
*/
			
			else if(strcmp(table[i].type, "twoInstruc") == 0)
			{
				strcpy(XM3_record.secondElement, table[i].XM3_instruction);
				
				if(strstr(XM2_record.secondElement, table[i].XM2_instruction))
				{
					strcpy(XM3_record.thirdElement, XM2_record.thirdElement);
				}
				else //else no label is present.
				{
					strcpy(XM3_record.thirdElement, XM2_record.secondElement);
				}
				
				branchFlag = SET;
			}
			
/*
* If "postArg" is the type, the secondElement is set to the XM3 instruction
* and the thirdElement is set to next XM2 element. A space is added to the 
* secondElement if the instruction is "BL".
*/
			else if(strcmp(table[i].type, "postArg") == 0) 
			{
				
				strcpy(XM3_record.secondElement, table[i].XM3_instruction);
				
				if(strstr(XM3_record.secondElement, "BL"))
				{
					strcat(XM3_record.secondElement, " ");
				}
				
				if(strstr(XM2_record.secondElement, table[i].XM2_instruction))
				{
					strcpy(XM3_record.thirdElement, XM2_record.thirdElement);
				}
				else //else no label is present
				{
					strcpy(XM3_record.thirdElement, XM2_record.secondElement);
				}
			}
			else
			{	

/*
* If "preArg" is the type, the secondElement is set to the XM3 instruction
* and the thirdElement is set to next XM2 element. 
*/
		
				strcpy(XM3_record.secondElement, table[i].XM3_instruction);
				
				strcat(XM3_record.secondElement, " ");
				
				if(strstr(XM2_record.secondElement, table[i].XM2_instruction))
				{
					strcpy(XM3_record.thirdElement, XM2_record.thirdElement);
				}
				else //else no label is present
				{				
					strcpy(XM3_record.thirdElement, XM2_record.secondElement);
				}
				
				char* removeNewline; //token removes newline character
					
				removeNewline = strtok(XM3_record.thirdElement, "\n");
				
/* 
* Operands are appended to the end of the thirdElement of the XM3_record.
*/
			
				if(strstr(XM3_record.secondElement, "ST"))
				{					
					strcat(XM3_record.thirdElement, ",-R6");
				}
				else
				{
					strcat(XM3_record.thirdElement, ",R7");
				}
			}
			
			strcpy(XM3_record.comment, XM2_record.comment);
			
			return XM3_record;
		}
	}

/*
* If no match is made then all of XM2_record is copied to XM3_record.
*/

	strcpy(XM3_record.firstElement, XM2_record.firstElement);
	
	strcpy(XM3_record.secondElement, XM2_record.secondElement);
	
	strcat(XM3_record.secondElement, " ");
	
	strcpy(XM3_record.thirdElement, XM2_record.thirdElement);
	
	strcpy(XM3_record.comment, XM2_record.comment);
			
	return XM3_record;
}











