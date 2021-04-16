#include "Lexer/Lexer.h"
#include "Parser/Parser.h"
#include <fstream>
#include <iostream>
#include <iomanip>

void PrintTree(const Parser::ParserNode* root, int depth, const Lexer& lex);

int main(int argc, const char* argv[])
{
	if (argc == 0)
	{
		return 0;
	}
	Lexer lex;
	std::fstream file;
	std::string path;
	for (size_t i = 1; i < argc; i++)
	{
		path.assign(argv[i]);
		file.open(path + "/input.sig");
		if (file.is_open())
		{
			lex.Process(file);
			file.close();

			file.open(path + "/generated.txt", std::fstream::out);
			if (file.is_open())
			{
				file << lex.GetError().str();
				for (const auto& token : lex.GetTokenTable())
				{
					using Token =  Lexer::Token;
					file << token.Row << std::setw(4) << token.Column << std::setw(6) << token.Type << "  ";
					if (Token::IsConstant(token.Type))
					{
						file << lex.GetConstantValue(token.Type);
					}
					else if (Token::IsDelimiter(token.Type))
					{
						file << lex.GetDelimiterValue(token.Type);
					}
					else if (Token::IsKeyword(token.Type))
					{
						file << lex.GetKeywordValue(token.Type);
					}
					else if(Token::IsIdentifier(token.Type))
					{
						file << lex.GetIdentifierValue(token.Type);
					}
					else
					{
						file << lex.GetWierdTokenType(token.Type);
					}
					file << std::endl;
				}
				file.close();
				lex.ClearData();
			}
		}
	}
	Parser pars;
	file.open("tests/parser.sig");
	if (file.is_open())
	{
		lex.Process(file);
		pars.Parse(lex);
		if (pars.IsHaveError())
		{
			std::cout << pars.GetError();
		}
		else
		{
			PrintTree(pars.GetTree(),0,lex);
		}

	}
	return 0;
}
void PrintTree(const Parser::ParserNode* root,int depth,const Lexer& lex)
{
	for (size_t i = 0; i < 2 * depth; i++)
	{
		std::cout << '.';
	}

	if (root->Type == NodeType::Token)
	{
		std::cout  << root->Token.Type << " ";
		lex.PrintValueToStream(std::cout, root->Token.Type);
		std::cout << std::endl;
	}
	else
	{
		std::cout << NodeTypeToString(root->Type) << std::endl;
	}

	for (auto child : root->Childs)
	{
		PrintTree(child, depth + 1, lex);
	}
}