#pragma once
#include <istream>
#include <array>
#include <string>
#include <vector>
#include <sstream>

class Lexer
{
public:
	struct Token
	{
		enum Type
		{
			Keyword = 401,
			Constant = 501,
			Identifier = 1001,
		};
		uint16_t Type;
		uint32_t Row;
		uint32_t Column;
		Token() = default;
		Token(uint16_t type, uint32_t row, uint32_t column)
			:Type(type), Row(row), Column(column) {}
		static bool IsDelimiter(uint16_t type);
		static bool IsKeyword(uint16_t type);
		static bool IsConstant(uint16_t type);
		static bool IsIdentifier(uint16_t type);
	};
	enum CharType
	{
		Space = 0,
		Digit = 1,
		Letter = 2,
		Delimiter = 3,
		Comment = 4,
		NewLine = 5,
		WierdTokenStart = 6,
		Forbidden = 7,
	};
	enum Keywords
	{
		PROGRAM = Token::Type::Keyword, 
		BEGIN, 
		END, 
		LOOP, 
		ENDLOOP, 
		CASE, 
		OF, 
		MOD, 
		ENDCASE,
		LABEL,
	};
public:
	using TokenTable = std::vector<Token>;
	using ErrorMessage = std::stringstream;
	Lexer();
	void Process(std::istream& stream);
	const TokenTable& GetTokenTable() const { return m_Tokens; }
	const ErrorMessage& GetError() const { return m_Error; }
	void ClearData();
	bool IsHasError() { return m_Error.tellp() > 0; }
	const std::string& GetKeywordValue(uint16_t type) const { return m_KeyWords[type % Token::Keyword]; };
	const std::string& GetIdentifierValue(uint16_t type) const { return m_Identifiers[type % Token::Identifier]; };
	const unsigned long GetConstantValue(uint16_t type) const { return m_Constants[type % Token::Constant]; };
	const char GetDelimiterValue(uint16_t type) const { return type; };
	std::ostream& PrintValueToStream(std::ostream& stream,uint16_t token_type) const;
private:
	uint8_t GetCharType(char c) const { return m_CharCodes[c]; };
	bool NextChar();
	void IllegalCharacterError();
	void IllegalCharacterError(uint32_t row, uint32_t column,char ch);
	void ConstantOverflow(uint32_t column);
	void UnexpectedCommentaryEnd();
	uint16_t GetIndentifierType(bool& is_new, bool& is_keyword);
	uint16_t GetConstantType(bool& is_new, unsigned long value);
	uint16_t GetDelimiterType();
	bool TryConverConstant(unsigned long& value);
private:
	std::istream* m_Stream = nullptr;
	std::string m_CurrentToken;
	char m_CurrentChar;
	std::array<uint8_t, 128> m_CharCodes = { Forbidden };
	uint32_t m_CurrentRow = 0;
	uint32_t m_CurrentColumn = 0;
	std::vector<Token> m_Tokens;
	std::stringstream m_Error;
	std::array<std::string, 10> m_KeyWords = {"PROGRAM","BEGIN","END","LOOP","ENDLOOP","CASE","OF","MOD","ENDCASE","LABEL"};
	std::array<char, 7> m_Delimiters = { '*','/',';',':','.' ,'\\',',' };
	std::vector<std::string> m_Identifiers;
	std::vector<unsigned long> m_Constants;
};