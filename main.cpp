#include "Lexer/Lexer.h"
#include "Parser/Parser.h"
#include <fstream>
#include <iostream>
#include <iomanip>

void PrintTree(const Parser::ParserNode* root, int depth, const Lexer& lex, std::ostream& stream);
void GenerateSyntaxTreeMarkdown(const Parser::ParserNode* root, const Lexer& lex, std::ostream& stream);
bool CompareFiles(std::string path1, std::string path2);

int main(int argc, const char* argv[])
{
	if (argc == 0)
	{
		return 0;
	}
	Lexer lex;
	Parser pars;
	std::fstream file;
	std::string path;
	for (size_t i = 1; i < argc; i++)
	{
		if (i == 16)
		{
			auto t = 1;
		}
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
				if (lex.IsHaveError())
				{
					file << lex.GetError().str();
				}
				if (pars.IsHaveError())
				{
					file << pars.GetError();
				}
				else
				{
					PrintTree(pars.GetTree(), 0, lex, file);
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
void PrintTree(const Parser::ParserNode* root,int depth,const Lexer& lex, std::ostream& stream)
{
	for (size_t i = 0; i < 2 * depth; i++)
	{
		stream << '.';
	}

	if (root->Type == NodeType::Token)
	{
		stream << root->Token.Type << " ";
		lex.PrintValueToStream(stream, root->Token.Type);
		stream << std::endl;
	}
	else
	{
		stream << NodeTypeToString(root->Type) << std::endl;
	}

	for (auto& child : root->Childs)
	{
		PrintTree(&child, depth + 1, lex,stream);
	}
}
void GenerateSyntaxTreeMarkdown(const Parser::ParserNode* root, const Lexer& lex,std::ostream& stream)
{
	stream << "[";
	if (root->Type == NodeType::Token)
	{
		stream << root->Token.Type << " ";
		lex.PrintValueToStream(stream, root->Token.Type);
	}
	else
	{
		stream << NodeTypeToString(root->Type);
	}
	for (auto child : root->Childs)
	{
		GenerateSyntaxTreeMarkdown(&child, lex, stream);
	}
	stream << "]";
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
