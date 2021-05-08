#include "CodeGenerator.h"
void CodeGenerator::Generate(Parser::ParserNode* root, Lexer& lex)
{
	m_Lexer = &lex;
	Program(root->Childs.begin());
}

void CodeGenerator::Program(NodeIterator root)
{
	auto childs = root->Childs.begin();
	Skip(childs, 1);
	auto procedureIdentifier = childs;
	Skip(childs, 2);
	auto block = childs;
	Block(block);
}

void CodeGenerator::Block(NodeIterator root)
{
	auto childs = root->Childs.begin();
	Skip(childs, 1);
	StatementsList(childs);
}

void CodeGenerator::StatementsList(NodeIterator root)
{
	auto childs = root->Childs.begin();
	if (childs->Type == NodeType::Empty)
	{
		m_Code << "nop" << std::endl;
		return;
	}
	Statement(childs);
	Skip(childs, 1);
	StatementsList(childs);
}

void CodeGenerator::Statement(NodeIterator root)
{
	auto child = root->Childs.begin();
	if (child->Token.Type == Lexer::Keywords::LOOP)
	{
		GenerateLoopCode(child);
	}
	else if (child->Token.Type == Lexer::Keywords::CASE)
	{
		GenerateCaseCode(child);
	}
}

void CodeGenerator::GenerateCaseCode(NodeIterator first)
{
	Skip(first, 1);
	GenerateExpresionCode("eax",first);
	m_Code << "mov ebx, eax" << std::endl;
	Skip(first, 2);
	int caseId = m_CaseCount++;
	AlternativeList(first, caseId);
	m_Code << "CASE_END_" << caseId << ":" << std::endl;
}

void CodeGenerator::GenerateLoopCode(NodeIterator first)
{
	int loopId = m_LoopCount++;
	m_Code << "Loop_" << loopId << ":" << std::endl;
	Skip(first, 1);
	StatementsList(first);
	m_Code <<"jmp Loop_" << loopId << std::endl;
}

void CodeGenerator::GenerateExpresionCode(const char* reg,NodeIterator first)
{
	auto child = first->Childs.begin();
	auto multiplier = child->Childs.begin();
	m_Code << "mov " << reg << ", ";
	PrintValueToCode(multiplier->Token.Type);
	m_Code << std::endl;
	Skip(child,1);
	MultiplierList(reg, child);
}

void CodeGenerator::MultiplierList(const char* reg, NodeIterator root)
{
	auto child = root->Childs.begin();
	auto instruction = child->Childs.begin();
	if (child->Type == NodeType::Empty)
	{
		m_Code << "nop" << std::endl;
		return;
	}

	if (instruction->Token.Type == '*')
	{
		Skip(child, 1);
		auto multiplier = child->Childs.begin();
		m_Code << "mov ecx, ";
		PrintValueToCode(multiplier->Token.Type);
		m_Code << std::endl<< "mul ecx" << std::endl;
		
	}
	else if (instruction->Token.Type == '/')
	{
		Skip(child, 1);
		auto multiplier = child->Childs.begin();
		
		m_Code << "mov ecx, ";
		PrintValueToCode(multiplier->Token.Type);
		m_Code << std::endl << "div ecx" << std::endl;
	}
	else //MOD
	{
		Skip(child, 1);
		auto multiplier = child->Childs.begin();
		m_Code << "mov ecx, ";
		PrintValueToCode(multiplier->Token.Type);
		m_Code << std::endl
		<< "div ecx"<<std::endl
		<< "mov " << reg <<", edx" <<std::endl;
	}
	Skip(child, 1);
	MultiplierList(reg,child);
}

void CodeGenerator::AlternativeList(NodeIterator root,int caseEndId)
{
	auto childs = root->Childs.begin();
	if (childs->Type == NodeType::Empty)
	{
		m_Code << "nop" << std::endl;
		return;
	}
	Alternative(childs, caseEndId);
	Skip(childs, 1);
	AlternativeList(childs, caseEndId);
}

void CodeGenerator::Alternative(NodeIterator root, int caseEndId)
{
	auto childs = root->Childs.begin();
	auto caseId = m_CaseCount++;
	GenerateExpresionCode("eax", childs);
	m_Code << "cmp ebx,eax" << std::endl;
	m_Code << "jne " << "CASE_" << caseId << ":" << std::endl;
	Skip(childs, 3);
	StatementsList(childs);
	m_Code << "jmp CASE_END_" << caseEndId << std::endl;
	m_Code << "CASE_" << caseId << ":" << std::endl;
}

void CodeGenerator::Skip(NodeIterator& it, int count)
{
	for (size_t i = 0; i < count; i++)
	{
		it++;
	}
}

void CodeGenerator::PrintValueToCode(uint16_t type)
{
	if (Lexer::Token::IsConstant(type))
	{
		m_Code << m_Lexer->GetConstantValue(type);
	}
	else if(Lexer::Token::IsIdentifier(type))
	{
		m_Code << "[" << m_Lexer->GetIdentifierValue(type) << "]";
	}
}



