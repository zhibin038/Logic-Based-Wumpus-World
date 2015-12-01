// Prover.h
//
// Propositional logic theorem prover.
//
// Version 1.1 (10-16-2015). Written by Larry Holder.

#ifndef PROVER_H
#define PROVER_H

#define DEBUG false

#include <list>
#include <string>

using namespace std;

typedef string Literal;
typedef string Sentence;

//----- Sentence Parsing -----

// Below should be in precedence order
enum PLOperator {OP_NONE, OP_PAREN, OP_NOT, OP_AND, OP_OR, OP_IMPLIES, OP_BICONDITIONAL, OP_ERROR};

typedef pair<PLOperator,int> OpPair;

class ParseTreeNode {
public:
	ParseTreeNode (const string& e, ParseTreeNode* l, ParseTreeNode* r);
	string element; // Either parentheses, logical operator, or atomic sentence
	ParseTreeNode* left;
	ParseTreeNode* right;
};

class ParseTree {
public:
	ParseTree(Sentence& sentence);
	ParseTreeNode* ParseSentence(Sentence& sentence);
	ParseTreeNode* ParseSentence1 (string& sentence);
	OpPair FindHighestPrecedenceOp (string& sentence);
	void ConvertToCNF (ParseTreeNode* node, int caseNum);
	ParseTreeNode* Copy (ParseTreeNode* node);
	void Print();
	void Print1 (int tab, ParseTreeNode* node);
	void Clear();
	void Clear1 (ParseTreeNode* node);
	ParseTreeNode* root;
};


//----- Propositional Logic classes -----

class Clause {
public:
	Clause() {inKB = false; id = 0;}
	void AddLiteral (Literal& literal, bool positive);
	void AddLiterals (Clause* clause);
	void RemoveLiteral (Literal& literal, bool positive);
	void AddNewLiteral (Literal& literal, bool positive);
	void AddNewLiterals (Clause* clause);
	void Simplify ();
	bool Tautology ();
	bool Empty();
	void Clear();
	void Print();
	bool operator== (const Clause& clause);
	bool Subsumes (const Clause& clause);
	list<Literal> positiveLiterals;
	list<Literal> negativeLiterals;
	bool inKB;
	int id;
};

class ClauseList {
public:
	ClauseList() {}
	ClauseList (Sentence& sentence);
	void ConvertToClauses(ParseTreeNode* node);
	void ConvertToClause (ParseTreeNode* node, Clause* clause);
	void AddClause (Clause* clause);
	void AddClauses (ClauseList& clauses);
	bool ContainsEmptyClause();
	bool AddNewClause (Clause* clause);
	bool AddNewClauses (ClauseList& clauses);
	bool Member (Clause* clause);
	void Negate (ClauseList& negatedClauses);
	void RemoveClause (Clause* clause);
	void Simplify ();
	void Clear(bool inKB = false);
	void Print();
	list<Clause*> clauseList;
};

class KnowledgeBase {
public:
	KnowledgeBase ();
	bool Ask (Sentence& query);
	void Tell (Sentence& sentence);
	void Remove (Sentence& sentence);
	void Read (string& fileName);
	void Print();
	void Clear ();
	bool PLResolution(ClauseList& clauseList);
	void PLResolve (Clause* clause1, Clause* clause2, ClauseList& resolvents);
	// Auxiliary versions of above methods
	bool Ask (ClauseList& queryClauses);
	void Tell (ClauseList& clauses);
	void Remove (ClauseList& clauses);
	void NumberWorkingClauses(ClauseList& clauses);
	ClauseList clausesKB;
	int numKBClauses;
	int numWorkingClauses;
};

#endif
