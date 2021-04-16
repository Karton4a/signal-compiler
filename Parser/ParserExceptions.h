#pragma once
#include "../Lexer/Lexer.h"

#include <cstdint>
#include <exception>
#include <string>

class ParserException : public std::exception
{
public:
	ParserException(uint32_t line, uint32_t column);
	virtual std::string Message(const Lexer& lex) { return m_Error.str(); };
private:
	std::string m_ErrorString;
protected:
	std::stringstream m_Error;

};

class TypeException : public ParserException
{
public:
	TypeException(const char* type, Lexer::Token actual)
		:m_ExpectedType(type), m_Token(actual), ParserException(actual.Row, actual.Column) {};
	virtual std::string Message(const Lexer& lex) override;
private:
	Lexer::Token m_Token;
	const char* m_ExpectedType;
};

class ValueException : public ParserException
{
public:
	ValueException(std::string expected, const Lexer::Token actual)
		:m_ExpectedValue(expected), m_Token(actual), ParserException(actual.Row, actual.Column) { }
	ValueException(char expected, const Lexer::Token actual)
		:m_ExpectedValue(), m_Token(actual), ParserException(actual.Row, actual.Column)
	{
		m_ExpectedValue.push_back(expected);
	}
	virtual std::string Message(const Lexer& lex) override;
private:
	std::string m_ExpectedValue;
	Lexer::Token m_Token;
};

class UnexpectedEndException : public ParserException
{
public:
	UnexpectedEndException(uint32_t line, uint32_t column)
		:ParserException(line, column) 
	{
		m_Error << "Unexpected program end" << std::endl;
	};
};
class UnexpectedGrammarException : public ParserException
{
public:
	UnexpectedGrammarException(const char* _where, uint32_t line, uint32_t column)
		:ParserException(line, column)
	{
		m_Error << "Unexpected grammar in " << _where << std::endl;
	}
};