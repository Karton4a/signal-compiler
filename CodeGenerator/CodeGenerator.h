#pragma once
#include "../Parser/Parser.h"
#include "../Lexer/Lexer.h"
#include <sstream>
class CodeGenerator
{
public:
	void Generate(Parser::ParserNode* root,Lexer& lex);
	std::string GetCode() { return m_Code.str(); }
private:
	using NodeIterator = std::list<Parser::ParserNode>::iterator;
	void Program(NodeIterator root);
	void Block(NodeIterator root);
	void StatementsList(NodeIterator root);
	void Statement(NodeIterator root);
	void GenerateCaseCode(NodeIterator first);
	void GenerateLoopCode(NodeIterator first);
	void GenerateExpresionCode(const char* reg,NodeIterator first);
	void MultiplierList(const char* reg, NodeIterator root);
	void AlternativeList(NodeIterator root,int caseEndId);
	void Alternative(NodeIterator root, int caseEndId);
	void Skip(NodeIterator& it,int count);
	void PrintValueToCode(uint16_t type);
private:
	uint16_t m_LoopCount = 0;
	uint16_t m_CaseCount = 0;
	std::stringstream m_Code;
	Lexer* m_Lexer;
};