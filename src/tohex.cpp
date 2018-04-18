//---------------------------------------------------------------------------
// ToHex
// Convert binary file to C/C++ header file containing source file as a char array.
// Originally Written by Sergey Vasyutin (svpro [at] outlook.com).
// Modified by Arjan Kuijpers 2018
//---------------------------------------------------------------------------

#include <stdio.h>
#include <string>
#include <algorithm>

#include <iostream>
#include <fstream>

//---------------------------------------------------------------------------
void Usage(void)
{
printf(
"ToHex. Convert binary file to C/C++ header file containing source file as a char array.\n"
"Usage:\n"
"   tohex <source filename>\n\n"
"The generated source code is written to the standard output stream.");
}

//---------------------------------------------------------------------------
int main(int Argc_, char *Argv_[])
{
long Size;
FILE *fp;
int SymbolInStringCount = 0;
int c;
int Count = 0;

if(Argc_ < 2) {
   Usage();
   return 1;
   }

std::string FileName(Argv_[1]);

fp = fopen(FileName.c_str(), "rb");
if(!fp) {
	printf("Error opening file '%s'.", FileName.c_str());
	return 1;
	}

std::ofstream outputFile;
outputFile.open(FileName + ".h", std::ios::binary & std::ios::out);
if (!outputFile.is_open())
{
	fclose(fp);
	return 2;
}


fseek(fp, 0, SEEK_END);
Size = ftell(fp);
fseek(fp, 0, SEEK_SET);

// Extracting filename without extension.
struct THelper {
	static bool isSeparator(const char C_) {return C_ == '\\' || C_ == '/';} 
	};
std::string::reverse_iterator it = std::find_if(FileName.rbegin(), FileName.rend(), 
	THelper::isSeparator);
//
std::string::iterator Start = it.base();
std::string::iterator Last = std::find(Start, FileName.end(), '.');
FileName.assign(Start, Last);
//
printf("/* This file was generated automatically. Do not edit. */\n"
   "#ifndef DATA_DECLARATION_ONLY\n\n");

outputFile << "/* This file was generated automatically. Do not edit. */\n"
"#ifndef DATA_DECLARATION_ONLY\n\n";

outputFile << "unsigned char " << FileName.c_str() << "[" << Size << "] = {";


printf("unsigned char %s[%ld] = {", FileName.c_str(), Size);
while((c = fgetc(fp)) != EOF) {
   Count++;
   if(SymbolInStringCount == 16) SymbolInStringCount = 0;
   if (!SymbolInStringCount) { printf("\n\t"); outputFile << "\n\t"; }
   outputFile << "0x" << std::hex << c;
   outputFile << ((Count == Size) ? ' ' : ',');
   printf("0x%02x%c ", c, (Count == Size) ? ' ' : ',');
   SymbolInStringCount++;
   }

outputFile << "\n\t};\n#else\n";
printf("\n\t};\n#else\n");
outputFile << "extern unsigned char " << FileName.c_str() << "[" << std::dec << Size << "];\n";
printf("extern unsigned char %s[%ld];\n", FileName.c_str(), Size);
printf("#endif\n\n/* EOF */\n");

outputFile << "#endif\n\n/* EOF */\n";
outputFile.close();

return 0;
}
