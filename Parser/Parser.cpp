#include "Parser.h"
#include "ParserExceptions.h"

void Parser::Parse(const Lexer& lexer)
{
	auto table = lexer.GetTokenTable();
	if (table.empty()) return;

	m_CurrentToken = table.cbegin();
	m_End = table.cend();
	m_Root = ParserNode(NodeType::SignalProgram);
	try
	{
		Program(&m_Root);
	}
	catch (ParserException& err)
	{
		m_Error = err.Message(lexer);
	}
}

void Parser::Program(ParserNode* _root)
{
	ParserNode* root = PushType(_root, NodeType::Program);
	if (m_CurrentToken->Type != Lexer::Keywords::PROGRAM)
	{
		throw ValueException("PROGRAM",*m_CurrentToken); // TODO maybe shoud use lexer keyword search 
	}
	PushCurrentToken(root);
	ProcedureIdentifier(root);
	if (m_CurrentToken->Type != ';')
	{
		throw ValueException(';', *m_CurrentToken);
	}
	PushCurrentToken(root);
	Block(root);
	if (m_CurrentToken->Type != '.')
	{
		throw ValueException('.', *m_CurrentToken);
	}
	root->Childs.emplace_back(*m_CurrentToken);
}

void Parser::ProcedureIdentifier(ParserNode* _root)
{
	ParserNode* root = PushType(_root, NodeType::ProcedureIdentifier);
	Identifier(root);
}

void Parser::Identifier(ParserNode* _root)
{
	ParserNode* root = PushType(_root, NodeType::Identifier);
	if (Lexer::Token::IsIdentifier(m_CurrentToken->Type))
	{
		PushCurrentToken(root);
	}
	else throw TypeException(NodeTypeToString(NodeType::Identifier),*m_CurrentToken);
}

void Parser::Block(ParserNode* _root)
{
	ParserNode* root = PushType(_root, NodeType::Block);
	if (m_CurrentToken->Type != Lexer::Keywords::BEGIN)
	{
		throw ValueException("BEGIN", *m_CurrentToken);
	}
	PushCurrentToken(root);
	bool is_empty = StatementsList(root);
	if (m_CurrentToken->Type != Lexer::Keywords::END)
	{
		if (is_empty)
		{
			throw ValueException("END", *m_CurrentToken);
		}
		throw UnexpectedGrammarException(NodeTypeToString(NodeType::Block), m_CurrentToken->Row, m_CurrentToken->Column);
	}
	PushCurrentToken(root);
}

bool Parser::StatementsList(ParserNode* _root)
{
	ParserNode* root = PushType(_root, NodeType::StatementsList);
	while (Statement(root)) {};
	if (root->Childs.empty())
	{
		PushType(root, NodeType::Empty);
		return true;
	}
	return false;
}

bool Parser::Statement(ParserNode* _root)
{
	if (m_CurrentToken->Type == Lexer::Keywords::LOOP)
	{
		ParserNode* root = PushType(_root, NodeType::Statement);
		PushCurrentToken(root);
		bool is_empty = StatementsList(root);
		if (m_CurrentToken->Type != Lexer::Keywords::ENDLOOP)
		{
			if (is_empty)
			{
				throw ValueException("ENDLOOP", *m_CurrentToken);
			}
			throw UnexpectedGrammarException(NodeTypeToString(NodeType::StatementsList), m_CurrentToken->Row, m_CurrentToken->Column);
		}
		PushCurrentToken(root);
		if (m_CurrentToken->Type != ';')
		{
			throw ValueException(';', *m_CurrentToken);
			//throw "expected one get another";
		}
		PushCurrentToken(root);
	}
	else if (m_CurrentToken->Type == Lexer::Keywords::CASE)
	{
		ParserNode* root = PushType(_root, NodeType::Statement);
		PushCurrentToken(root);
		Expresion(root);
		if (m_CurrentToken->Type != Lexer::Keywords::OF)
		{
			throw UnexpectedGrammarException(NodeTypeToString(NodeType::Expresion), m_CurrentToken->Row, m_CurrentToken->Column);
		}
		PushCurrentToken(root);
		bool is_empty = AlternativesList(root);
		if (m_CurrentToken->Type != Lexer::Keywords::ENDCASE)
		{
			if (is_empty)
			{
				throw ValueException("ENDCASE",*m_CurrentToken);
			}
			throw UnexpectedGrammarException(NodeTypeToString(NodeType::AlternativesList), m_CurrentToken->Row, m_CurrentToken->Column);
		}
		PushCurrentToken(root);
		if (m_CurrentToken->Type != ';')
		{
			throw ValueException(';', *m_CurrentToken);
		}
		PushCurrentToken(root);
	}
	else
	{
		return false;
	}
	return true;
}

bool Parser::Expresion(ParserNode* _root)
{
	ParserNode* root = PushType(_root, NodeType::Expresion);
	if (!Multiplier(root))
	{
		throw TypeException(NodeTypeToString(NodeType::Multiplier), *m_CurrentToken);
	}
	MultipliersList(root);
	return true;
}

void Parser::MultipliersList(ParserNode* _root)
{
	ParserNode* root = PushType(_root, NodeType::MultipliersList);
	while (MultiplicationInstruction(root))
	{
		if (!Multiplier(root))
		{
			throw TypeException(NodeTypeToString(NodeType::Multiplier), *m_CurrentToken);
		}
	}
	if (root->Childs.empty())
	{
		PushType(root, NodeType::Empty);
	}
}

bool Parser::MultiplicationInstruction(ParserNode* _root)
{
	if (m_CurrentToken->Type == '*' || m_CurrentToken->Type == '/' || m_CurrentToken->Type == Lexer::Keywords::MOD)
	{
		ParserNode* root = PushType(_root, NodeType::MultiplicationInstruction);
		PushCurrentToken(root);
		return true;
	}
	return false;
}

bool Parser::Multiplier(ParserNode* _root)
{
	if (Lexer::Token::IsConstant(m_CurrentToken->Type) || Lexer::Token::IsIdentifier(m_CurrentToken->Type))
	{
		ParserNode* root = PushType(_root, NodeType::Multiplier);
		PushCurrentToken(root);
		return true;
	}
	return false;
}

bool Parser::AlternativesList(ParserNode* _root)
{
	ParserNode* root = PushType(_root, NodeType::AlternativesList);
	while (Alternative(root)){}
	if (root->Childs.empty())
	{
		PushType(root, NodeType::Empty);
		return true;
	}
	return false;
}

bool Parser::Alternative(ParserNode* _root)
{
	ParserNode* root = PushType(_root, NodeType::Alternative);
	try
	{
		Expresion(root);
	}
	catch (ParserException&)
	{
		_root->Childs.pop_back();
		return false;
	}
	//if (!Expresion(root)) return false;
	//<expression> : / <statementslist> \ */

	if (m_CurrentToken->Type != ':')
	{
		throw ValueException(':',*m_CurrentToken);
	}
	PushCurrentToken(root);
	if (m_CurrentToken->Type != '/')
	{
		throw ValueException('/', *m_CurrentToken);
	}
	PushCurrentToken(root);
	bool is_empty = StatementsList(root);
	if (m_CurrentToken->Type != '\\')
	{
		if (is_empty)
		{
			throw ValueException('\\', *m_CurrentToken);
		}
		throw UnexpectedGrammarException(NodeTypeToString(NodeType::StatementsList),m_CurrentToken->Row,m_CurrentToken->Column);
	}
	PushCurrentToken(root);
	return true;
}

Parser::ParserNode* Parser::PushType(ParserNode* _node, NodeType type)
{
	_node->Childs.emplace_back(type);
	return &_node->Childs.back();
}

void Parser::NextToken()
{
	if (m_CurrentToken != m_End)
	{
		m_PreviousToken = m_CurrentToken;
		m_CurrentToken++;
	}
	else throw UnexpectedEndException(m_PreviousToken->Row,m_PreviousToken->Column);
}

void Parser::PushCurrentToken(ParserNode* _root)
{
	_root->Childs.emplace_back(*m_CurrentToken);
	NextToken();
}

const char* NodeTypeToString(NodeType type)
{
	switch (type)
	{
	case NodeType::SignalProgram: return "<signal-program>";
	case NodeType::Program: return "<program>";
	case NodeType::Block: return "<block>";
	case NodeType::StatementsList: return "<statements-list>";
	case NodeType::Statement: return "<statement>";
	case NodeType::AlternativesList: return "<alternatives-list>";
	case NodeType::Alternative: return "<alternative>";
	case NodeType::Expression: return "<expression>";
	case NodeType::MultipliersList: return "<multipliers-list>";
	case NodeType::MultiplicationInstruction: return "<multiplication-instruction>";
	case NodeType::Multiplier: return "<multiplier>";
	case NodeType::Identifier: return "<identifier>";
	case NodeType::ProcedureIdentifier: return "<procedure-identifier>";
	case NodeType::Expresion: return "<expresion>";
	case NodeType::Empty: return "<empty>";
	case NodeType::Token: return "<token>";
	default: return "<unknown>";
	}
}
