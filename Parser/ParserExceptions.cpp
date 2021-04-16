#include "ParserExceptions.h"

ParserException::ParserException(uint32_t line, uint32_t column)
{
	m_Error << "Parser: Error (line " << line << ", column " << column << "): ";
}

std::string TypeException::Message(const Lexer& lex)
{
	m_Error << "'"<< m_ExpectedType <<"' expected but '";
	lex.PrintValueToStream(m_Error, m_Token.Type);
	m_Error << "' found" << std::endl;
	return m_Error.str();
}

std::string ValueException::Message(const Lexer& lex)
{
	m_Error << "'" << m_ExpectedValue << "’ expected but '";
	lex.PrintValueToStream(m_Error, m_Token.Type);
	m_Error << "' found" << std::endl;
	return m_Error.str();
}
