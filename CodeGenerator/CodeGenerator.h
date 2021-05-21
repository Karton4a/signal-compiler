#pragma once
#include "../Parser/Parser.h"
#include "../Lexer/Lexer.h"
#include <sstream>

class CodeGeneratorException : public std::exception
{
public:
	CodeGeneratorException(uint32_t line, uint32_t column) 
	{
		m_Error << "Code Generator: Error (line " << line << ", column " << column << "): Program name does not allowed in expressions";
	}
	virtual std::string Message() { return m_Error.str(); };
protected:
	std::stringstream m_Error;

};

class CodeGenerator
{
public:
	void Generate(Parser::ParserNode* root,Lexer& lex);
	std::string GetCode() { return m_Code.str(); }
	bool IsHasError() { return m_Error.size() != 0; }
	const std::string& GetError() { return m_Error; }
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
	void PrintValueToCode(const Lexer::Token& type);
private:
	std::string m_Error;
	uint16_t m_LoopCount = 0;
	uint16_t m_CaseCount = 0;
	std::stringstream m_Code;
	Lexer* m_Lexer;
	std::string m_ProgramIdentifier;
};