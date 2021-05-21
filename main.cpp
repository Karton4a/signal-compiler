#include "Lexer/Lexer.h"
#include "Parser/Parser.h"
#include "CodeGenerator/CodeGenerator.h"
#include <fstream>
#include <iostream>
#include <iomanip>

bool CompareFiles(std::string path1, std::string path2);

int main(int argc, const char* argv[])
{
	if (argc == 0)
	{
		return 0;
	}
	Lexer lex;
	Parser pars;
	CodeGenerator generator;
	std::fstream file;
	std::string path;
	std::fstream file2;
	for (size_t i = 1; i < argc; i++)
	{
		path.assign(argv[i]);
		file.open(path + "/input.sig");
		if (file.is_open())
		{
			lex.Process(file);
			pars.Parse(lex);
			file.close();

			file.open(path + "/generated.txt", std::fstream::out);
			if (file.is_open())
			{
				if (lex.IsHasError())
				{
					file << lex.GetError().str();
				}
				if (pars.IsHasError())
				{
					file << pars.GetError();
				}
				else
				{
					generator.Generate(pars.GetTree(), lex);
					if (generator.IsHasError())
					{
						file << generator.GetError();
					}
					else
					{
						file << generator.GetCode();
					}
				}
				file.close();
				lex.ClearData();
				if (CompareFiles(path + "/generated.txt", path + "/expected.txt"))
				{
					std::cout << "\033[1;32m" << "Test " << i << " passed" << "\033[0m";
				}
				else
				{
					std::cout << "\033[1;31m"  << "Test " << i << " failed" << "\033[0m";
				}
				std::cout << std::endl;
			}
		}
	}
	return 0;
}

bool CompareFiles(std::string path1, std::string path2)
{
	std::fstream file1(path1);
	std::fstream file2(path2);
	if (!file1.is_open() || !file2.is_open()) return false;
	while (true)
	{
		if (file1.eof() && file2.eof()) return true;
		if (file1.eof() || file2.eof()) return false;
		if (file1.get() != file2.get()) return false;
	}
}
