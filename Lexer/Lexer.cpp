#include "Lexer.h"

Lexer::Lexer()
{
	//m_Identifiers.resize(1000);
	m_CharCodes.fill(CharType::Forbidden);
	m_CharCodes[8] = CharType::Space;
	m_CharCodes[13] = CharType::Space;
	m_CharCodes[32] = CharType::Space;
	m_CharCodes['\t'] = CharType::Space;
	m_CharCodes['\n'] = CharType::NewLine;
	m_CharCodes['('] = CharType::Comment;
	m_CharCodes['#'] = CharType::WierdTokenStart;
	for (char delim : m_Delimiters)
	{
		m_CharCodes[delim] = CharType::Delimiter;
	}
	for (size_t i = 65; i <= 90; i++)
	{
		m_CharCodes[i] = CharType::Letter;
	}
	for (size_t i = 48; i <= 57; i++)
	{
		m_CharCodes[i] = CharType::Digit;
	}
}

void Lexer::Process(std::istream& stream)
{
	m_Stream = &stream;
	NextChar();
	m_CurrentRow = 1;
	m_CurrentColumn = 1;
	uint32_t start_col = 0;
	while (!m_Stream->eof())
	{
		switch (GetCharType(m_CurrentChar))
		{
		case CharType::Digit:
		{
			start_col = m_CurrentColumn;
			do
			{
				m_CurrentToken.push_back(m_CurrentChar);

			} while (NextChar() && GetCharType(m_CurrentChar) == CharType::Digit);

			unsigned long value = 0;
			if (TryConverConstant(value))
			{
				bool is_new = false;
				auto type = GetConstantType(is_new,value);
				if (is_new)
				{
					m_Constants.push_back(value);
				}
				m_Tokens.emplace_back(type, m_CurrentRow, start_col);
			}
			else
			{
				ConstantOverflow(start_col);
			}
			m_CurrentToken.clear();
			break;
		}
		case CharType::Letter:
		{
			start_col = m_CurrentColumn;
			do
			{
				m_CurrentToken.push_back(m_CurrentChar);

			} while (NextChar() && (GetCharType(m_CurrentChar) == CharType::Letter || GetCharType(m_CurrentChar) == CharType::Digit));
			bool is_new = false, is_keyword = false;
			auto type = GetIndentifierType(is_new, is_keyword);
			if (!is_keyword && is_new)
			{
				m_Identifiers.push_back(m_CurrentToken);
			}
			m_Tokens.emplace_back(type, m_CurrentRow, start_col);
			m_CurrentToken.clear();
			break;
		}
		case CharType::Delimiter:
			m_Tokens.emplace_back(GetDelimiterType(), m_CurrentRow, m_CurrentColumn);
			NextChar();
			break;
		case CharType::NewLine:
			NextChar();
			m_CurrentColumn = 1;
			m_CurrentRow++;
			break;
		case CharType::Space:
			do {} while (NextChar() && GetCharType(m_CurrentChar) == CharType::Space);
			break;
		case CharType::Comment:
			if (!NextChar())
			{
				IllegalCharacterError(m_CurrentRow, m_CurrentColumn - 1, '(');
				break;
			}
			if (m_CurrentChar != '*')
			{
				IllegalCharacterError(m_CurrentRow,m_CurrentColumn-1,'(');
				break;
			}
			
			do
			{
				do 
				{
					if (!NextChar())
					{
						UnexpectedCommentaryEnd();
						break;
					}
					if (GetCharType(m_CurrentChar) == CharType::NewLine)
					{
						m_CurrentColumn = 0;
						m_CurrentRow++;
					}
				} while (m_CurrentChar != '*');

			} while (NextChar() && m_CurrentChar != ')');
			NextChar();
			break;
		case CharType::WierdTokenStart:
		{
			start_col = m_CurrentColumn;
			m_CurrentToken.push_back(m_CurrentChar);
			while (NextChar() && GetCharType(m_CurrentChar) == CharType::Digit)
			{
				m_CurrentToken.push_back(m_CurrentChar);
			} 
			if (m_CurrentToken.size() == 1)
			{
				IllegalCharacterError(m_CurrentRow, m_CurrentColumn - 1, '#');
				m_CurrentToken.clear();
				NextChar();
				break;
			}
			do
			{
				if (m_CurrentChar != '-')
				{
					break;
				}
				m_CurrentToken.push_back(m_CurrentChar);
				bool entered = false;
				while (NextChar() && GetCharType(m_CurrentChar) == CharType::Digit)
				{
					m_CurrentToken.push_back(m_CurrentChar);
					entered = true;
				}
				if (!entered)
				{
					break;
				}

			} while (true);

			bool is_new = false;
			auto type = GetWierdTokenType(is_new);
			if (is_new)
			{
				m_WierdTokens.push_back(m_CurrentToken);
			}
			m_Tokens.emplace_back(type, m_CurrentRow, start_col);
			m_CurrentToken.clear();
		}
			break;
		default:
			IllegalCharacterError();
			NextChar();
			break;
		}
	}

}

void Lexer::ClearData()
{
	m_Tokens.clear();
	m_Error.clear();
	m_CurrentToken.clear();
	m_Constants.clear();
	m_Identifiers.clear();
	m_Error.clear();
}

std::ostream& Lexer::PrintValueToStream(std::ostream& stream, uint16_t token_type) const
{
	if (Lexer::Token::IsConstant(token_type))
	{
		stream << GetConstantValue(token_type);
	}
	else if (Lexer::Token::IsIdentifier(token_type))
	{
		stream << GetIdentifierValue(token_type);
	}
	else if (Lexer::Token::IsDelimiter(token_type))
	{
		stream << GetDelimiterValue(token_type);
	}
	else if (Lexer::Token::IsKeyword(token_type))
	{
		stream << GetKeywordValue(token_type);
	}
	return stream;
}

bool Lexer::NextChar()
{
	m_CurrentChar = m_Stream->get(); 
	if (!m_Stream->eof())
	{
		m_CurrentColumn++;
		return true;
	}
	return false;
}

void Lexer::IllegalCharacterError()
{
	IllegalCharacterError(m_CurrentRow, m_CurrentColumn, m_CurrentChar);
}

void Lexer::IllegalCharacterError(uint32_t row, uint32_t column, char ch)
{
	m_Error << "Lexer: Error (line "
		<< row << ", column "
		<< column
		<< "): Illegal character '"
		<< ch
		<< "' detected"
		<< std::endl;
}

void Lexer::ConstantOverflow(uint32_t column)
{
	m_Error << "Lexer: Error (line "
		<< m_CurrentRow << ", column "
		<< column
		<< "): Constant overflow : " << m_CurrentToken << std::endl;
}

void Lexer::UnexpectedCommentaryEnd()
{
	m_Error << "Lexer: Error (line "
		<< m_CurrentRow << ", column "
		<< m_CurrentColumn
		<< "): Unexpected commentary end"<< std::endl;
}

uint16_t Lexer::GetIndentifierType(bool& is_new,bool& is_keyword)
{
	is_new = true;
	is_keyword = false;
	auto it = std::find(m_KeyWords.begin(), m_KeyWords.end(), m_CurrentToken);
	if (it != m_KeyWords.end())
	{
		is_keyword = true;
		return Token::Keyword + std::distance(m_KeyWords.begin(), it);
	}

	auto it2 = std::find(m_Identifiers.begin(), m_Identifiers.end(), m_CurrentToken);

	if (it2 != m_Identifiers.end())
	{
		is_new = false;
		return Token::Identifier + std::distance(m_Identifiers.begin(),it2);
	}
	return Token::Identifier + m_Identifiers.size();
}

uint16_t Lexer::GetConstantType(bool& is_new,unsigned long value)
{
	is_new = true;
	auto it = std::find(m_Constants.begin(), m_Constants.end(), value);
	if (it != m_Constants.end())
	{
		is_new = false;
		return Token::Constant + std::distance(m_Constants.begin(),it);
	}
	return Token::Constant + m_Constants.size();
}

uint16_t Lexer::GetDelimiterType()
{
	return (uint16_t)m_CurrentChar;
}

uint16_t Lexer::GetWierdTokenType(bool& is_new)
{
	is_new = true;
	auto it = std::find(m_WierdTokens.begin(), m_WierdTokens.end(), m_CurrentToken);
	if (std::find(m_WierdTokens.begin(), m_WierdTokens.end(), m_CurrentToken) != m_WierdTokens.end())
	{
		is_new = false;
		return Token::WierdToken + std::distance(m_WierdTokens.begin(), it);
	}

	return Token::WierdToken + m_WierdTokens.size();
}

bool Lexer::TryConverConstant(unsigned long& value)
{
	try
	{
		value = std::stoul(m_CurrentToken);
		return true;
	}
	catch (const std::out_of_range& e)
	{
		return false;
	}
}

bool Lexer::Token::IsDelimiter(uint16_t type)
{
	return type <= (uint8_t)~0;
}

bool Lexer::Token::IsKeyword(uint16_t type)
{
	return type >= Keyword && type < Constant;
}

bool Lexer::Token::IsConstant(uint16_t type)
{
	return type >= Constant && type < Identifier;
}

bool Lexer::Token::IsIdentifier(uint16_t type)
{
	return type >= Identifier && type < WierdToken;
}

bool Lexer::Token::IsWierdToken(uint16_t type)
{
	return type >= WierdToken;
}
