#pragma once
#include "../Lexer/Lexer.h"

#include <vector>
#include <list>
#include <sstream>

enum class NodeType
{
	SignalProgram,
	Program,
	Block,
	StatementsList,
	Statement,
	AlternativesList,
	Alternative,
	Expression,
	MultipliersList,
	MultiplicationInstruction,
	Multiplier,
	Identifier,
	ProcedureIdentifier,
	Expresion,
	Declarations,
	LabelDeclarations,
	LabelsList,
	Empty,
	Token,
};
const char* NodeTypeToString(NodeType type);


class Parser
{
public:
	using ErrorMessage = std::string;
	struct ParserNode
	{
		ParserNode() = default;
		ParserNode(NodeType type)
			:Type(type),Token() {}

		ParserNode(const Lexer::Token token)
			:Type(NodeType::Token),Token(token) {}

		NodeType Type;
		Lexer::Token Token;
		std::list<ParserNode> Childs;
	};
	void Parse(const Lexer& lexer);
	ParserNode* GetTree() { return &m_Root; }
	bool IsHaveError() { return !m_Error.empty(); }
	const ErrorMessage& GetError() const { return m_Error; }
private:
	void Program(ParserNode* _root);
	void ProcedureIdentifier(ParserNode* _root);
	void Identifier(ParserNode* _root);
	void Block(ParserNode* _root);
	void Declarations(ParserNode* _root);
	void LabelDecl(ParserNode* _root);
	void LabelsList(ParserNode* _root);
	bool StatementsList(ParserNode* _root);
	bool Statement(ParserNode* _root);
	bool Expresion(ParserNode* _root);
	void MultipliersList(ParserNode* _root);
	bool MultiplicationInstruction(ParserNode* _root);
	bool Multiplier(ParserNode* _root);
	bool AlternativesList(ParserNode* _root);
	bool Alternative(ParserNode* _root);

	ParserNode* PushType(ParserNode* node, NodeType type);
	void NextToken();
	void PushCurrentToken(ParserNode* _root);
private:
	Lexer::TokenTable::const_iterator m_CurrentToken;
	Lexer::TokenTable::const_iterator m_PreviousToken;
	Lexer::TokenTable::const_iterator m_End;
	ParserNode m_Root;
	std::string m_Error;
};