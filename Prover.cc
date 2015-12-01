// Prover.cc
//
// Propositional logic theorem prover.
//
// Version 1.1 (10-16-2015). Written by Larry Holder.

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "Prover.h"

using namespace std;


//----- Clause -----

void Clause::AddLiteral (Literal& literal, bool positive)
{
	/*
	if (positive) {
		positiveLiterals.push_back (literal);
	} else {
		negativeLiterals.push_back (literal);
	}
	*/
	AddNewLiteral(literal,positive);
}

void Clause::AddLiterals (Clause* clause)
{
	/*
	list<Literal>::iterator itr;
	for (itr = clause->positiveLiterals.begin(); itr != clause->positiveLiterals.end(); itr++)
	{
		AddLiteral(*itr,true);
	}
	for (itr = clause->negativeLiterals.begin(); itr != clause->negativeLiterals.end(); itr++)
	{
		AddLiteral(*itr,false);
	}
	*/
	AddNewLiterals(clause);
}

void Clause::RemoveLiteral (Literal& literal, bool positive)
{
	if (positive) {
		positiveLiterals.remove (literal);
	} else {
		negativeLiterals.remove (literal);
	}
}

void Clause::AddNewLiteral (Literal& literal, bool positive)
{
	if (positive) {
		if (find(positiveLiterals.begin(), positiveLiterals.end(), literal) == positiveLiterals.end())
			positiveLiterals.push_back (literal);
	} else {
		if (find(negativeLiterals.begin(), negativeLiterals.end(), literal) == negativeLiterals.end())
			negativeLiterals.push_back (literal);
	}
}

void Clause::AddNewLiterals (Clause* clause)
{
	list<Literal>::iterator itr;
	for (itr = clause->positiveLiterals.begin(); itr != clause->positiveLiterals.end(); itr++)
	{
		AddNewLiteral(*itr,true);
	}
	for (itr = clause->negativeLiterals.begin(); itr != clause->negativeLiterals.end(); itr++)
	{
		AddNewLiteral(*itr,false);
	}
}

// Removes redundant literals.
void Clause::Simplify ()
{
	positiveLiterals.sort();
	positiveLiterals.unique();
	negativeLiterals.sort();
	negativeLiterals.unique();
}

// Returns true if clause contains A and ~A
bool Clause::Tautology ()
{
	list<Literal>::iterator itr;
	for (itr = negativeLiterals.begin(); itr!= negativeLiterals.end(); itr++)
	{
		if (find (positiveLiterals.begin(), positiveLiterals.end(), (*itr)) != positiveLiterals.end())
			return true;
	}
	return false;
}

void Clause::Print()
{
	bool first = true;
	list<Literal>::iterator itr;
	cout << "(" << id << ") ";
	for (itr = positiveLiterals.begin(); itr != positiveLiterals.end(); itr++)
	{
		if (! first) cout << " | ";
		cout << (*itr);
		first = false;
	}
	for (itr = negativeLiterals.begin(); itr != negativeLiterals.end(); itr++)
	{
		if (! first) cout << " | ";
		cout << "~" << (*itr);
		first = false;
	}
	if (first)
		cout << "empty Clause";
}

void Clause::Clear()
{
	negativeLiterals.clear();
	positiveLiterals.clear();
}

bool Clause::Empty()
{
	if (negativeLiterals.empty() && positiveLiterals.empty())
		return true;
	else return false;
}

bool Clause::operator== (const Clause& clause)
{
	if (negativeLiterals.size() != clause.negativeLiterals.size())
		return false;
	if (positiveLiterals.size() != clause.positiveLiterals.size())
		return false;
	return Subsumes(clause);
}

// Returns true if every literal in this clause occurs in the given clause.
bool Clause::Subsumes (const Clause& clause)
{
	list<Literal>::iterator itr;
	for (itr = negativeLiterals.begin(); itr!= negativeLiterals.end(); itr++)
	{
		if (find (clause.negativeLiterals.begin(), clause.negativeLiterals.end(), (*itr)) == clause.negativeLiterals.end())
			return false;
	}
	for (itr = positiveLiterals.begin(); itr!= positiveLiterals.end(); itr++)
	{
		if (find (clause.positiveLiterals.begin(), clause.positiveLiterals.end(), (*itr)) == clause.positiveLiterals.end())
			return false;
	}
	return true;
}


// ----- ClauseList -----

ClauseList::ClauseList (Sentence& sentence)
{
	ParseTree tree (sentence);
	ConvertToClauses(tree.root);
	tree.Clear();
}

void ClauseList::ConvertToClauses(ParseTreeNode* node)
{
	Clause* clause;
	if (node != NULL) {
		if (node->element == "&") {
			ConvertToClauses(node->left);
			ConvertToClauses(node->right);
		} else if (node->element == "|") {
			clause = new Clause;
			ConvertToClause (node->left, clause);
			ConvertToClause (node->right, clause);
			AddClause(clause);
		} else if (node->element == "~") {
			clause = new Clause;
			clause->AddLiteral(node->left->element, false);
			AddClause(clause);
		} else {
			clause = new Clause;
			clause->AddLiteral(node->element, true);
			AddClause(clause);
		}
	}
}

void ClauseList::ConvertToClause (ParseTreeNode* node, Clause* clause)
{
	if (node->element == "|") {
		ConvertToClause (node->left, clause);
		ConvertToClause (node->right, clause);
	} else if (node->element == "~") {
		clause->AddLiteral(node->left->element, false);
	} else {
		clause->AddLiteral(node->element, true);
	}
}

void ClauseList::AddClause (Clause* clause)
{
	clauseList.push_back(clause);
}

void ClauseList::AddClauses (ClauseList& clauses)
{
	Clause* clause;
	list<Clause*>::iterator itr;
	for (itr = clauses.clauseList.begin(); itr != clauses.clauseList.end(); itr++)
	{
		clause = *itr;
		clauseList.push_back(clause);
	}
}

bool ClauseList::ContainsEmptyClause()
{
	Clause* clause;
	list<Clause*>::iterator itr;
	for (itr = clauseList.begin(); itr != clauseList.end(); itr++)
	{
		clause = *itr;
		if (clause->Empty())
			return true;
	}
	return false;
}

// Returns true if clause actually added, i.e., new.
// New clauses subsumed by clauses already on the list are not added.
// Clauses not added are deleted.
bool ClauseList::AddNewClause (Clause* clause)
{
	bool addedNew = false;

	if (Member(clause))
	{
		delete clause;
	} else {
		clauseList.push_back(clause);
		addedNew = true;
	}
	return addedNew;
}

// Returns true if any clauses actually added, i.e., new.
// Clauses not added are deleted.
bool ClauseList::AddNewClauses (ClauseList& clauses)
{
	Clause* clause;
	list<Clause*>::iterator itr;
	bool addedNew = false;

	for (itr = clauses.clauseList.begin(); itr != clauses.clauseList.end(); itr++)
	{
		clause = *itr;
		if (AddNewClause(clause)) {
			addedNew = true;
		}
	}
	return addedNew;
}

// Returns true if a clause in the clauseList subsumes the given clause.
bool ClauseList::Member (Clause* clause)
{
	Clause* clause1;
	list<Clause*>::iterator itr;
	for (itr = clauseList.begin(); itr != clauseList.end(); itr++)
	{
		clause1 = *itr;
		if (clause1->Subsumes(*clause))
			return true;
	}
	return false;
}

void ClauseList::Negate (ClauseList& negatedClauses)
{
	Clause* clause;
	Clause* negClause;
	Clause* newClause;
	ClauseList negatedClauses1;
	ClauseList newNegatedClauses;
	list<Clause*>::iterator itrC;
	list<Clause*>::iterator itrCN;
	list<Literal>::iterator itrL;

	itrC = clauseList.begin();
	if (itrC != clauseList.end())
	{
		// Special case for first clause
		clause = *itrC;
		for (itrL = clause->negativeLiterals.begin(); itrL!= clause->negativeLiterals.end(); itrL++)
		{
			newClause = new Clause;
			newClause->AddLiteral(*itrL,true);
			negatedClauses1.AddClause(newClause);
		}
		for (itrL = clause->positiveLiterals.begin(); itrL!= clause->positiveLiterals.end(); itrL++)
		{
			newClause = new Clause;
			newClause->AddLiteral(*itrL,false);
			negatedClauses1.AddClause(newClause);
		}
		// Permute rest of clauses
		for (itrC++; itrC != clauseList.end(); itrC++)
		{
			clause = *itrC;
			newNegatedClauses.clauseList.clear();
			for (itrL = clause->negativeLiterals.begin(); itrL!= clause->negativeLiterals.end(); itrL++)
			{
				for (itrCN = negatedClauses1.clauseList.begin(); itrCN != negatedClauses1.clauseList.end(); itrCN++)
				{
					negClause = *itrCN;
					newClause = new Clause;
					newClause->AddLiterals(negClause);
					newClause->AddLiteral(*itrL,true);
					newNegatedClauses.AddClause(newClause);
				}
			}
			for (itrL = clause->positiveLiterals.begin(); itrL!= clause->positiveLiterals.end(); itrL++)
			{
				for (itrCN = negatedClauses1.clauseList.begin(); itrCN != negatedClauses1.clauseList.end(); itrCN++)
				{
					negClause = *itrCN;
					newClause = new Clause;
					newClause->AddLiterals(negClause);
					newClause->AddLiteral(*itrL,false);
					newNegatedClauses.AddClause(newClause);
				}
			}
			negatedClauses1.Clear();
			negatedClauses1.AddClauses(newNegatedClauses);
		}
		negatedClauses.AddClauses(negatedClauses1);
		negatedClauses1.clauseList.clear();
	}
}

// Removes all clauses in clauseList matching given clause.
void ClauseList::RemoveClause (Clause* clause)
{
	if (DEBUG) {
		cout << "Removing clause: ";
		clause->Print();
		cout << endl;
	}
	Clause* clause1;
	list<Clause*>::iterator itr = clauseList.begin();
	while (itr != clauseList.end())
	{
		clause1 = *itr;
		if (*clause == *clause1) {
			itr = clauseList.erase(itr);
		} else itr++;
	}
}

// If a clause contains a tautology (A v ~A), then remove it.
// If one clause subsumes another clause, then remove the second clause.
// E.g., if (AvB) and (AvBvC) are on the list, then remove (AvBvC).
// Not terribly efficient, but the clause lists for which this is called are typically small.
void ClauseList::Simplify ()
{
	Clause* clause1;
	Clause* clause2;
	list<Clause*>::iterator itr1;
	list<Clause*>::iterator itr2;
	bool simplified = false;

	// First, simplify each individual clause
	// For now, assume all clauses are already simplified.
	/*
	for (itr1 = clauseList.begin(); itr1 != clauseList.end(); itr1++) {
		clause1 = *itr1;
		clause1->Simplify();
	}
	*/

	// Second, check for tautologies and subsumptions
	while (! simplified) {
		simplified = true;
		for (itr1 = clauseList.begin(); itr1 != clauseList.end(); itr1++) {
			clause1 = *itr1;
			// Remove clause if tautology - contains (A v ~A)
			if (clause1->Tautology()) {
				simplified = false;
				break;
			}
			// Remove any clause subsumed by clause1
			for (itr2 = clauseList.begin(); itr2 != clauseList.end(); itr2++) {
				clause2 = *itr2;
				if ((clause1 != clause2) && (clause1->Subsumes(*clause2))) {
					simplified = false;
					break;
				}
			}
			if (! simplified) {
				itr1 = itr2;
				break;
			}
		}
		if (! simplified)
			clauseList.erase(itr1);
	}
}

void ClauseList::Clear (bool inKB)
{
	Clause* clause;
	list<Clause*>::iterator itr;
	for (itr = clauseList.begin(); itr != clauseList.end(); itr++)
	{
		clause = *itr;
		if (inKB || (! clause->inKB))
		{
			clause->Clear();
			delete clause;
		}
	}
	clauseList.clear();
}

void ClauseList::Print ()
{
	Clause* clause;
	list<Clause*>::iterator itr;
	for (itr = clauseList.begin(); itr != clauseList.end(); itr++)
	{
		clause = *itr;
		clause->Print();
		cout << endl;
	}
}


// ----- KnowledgeBase -----

KnowledgeBase::KnowledgeBase ()
{
	numKBClauses = 0;
	numWorkingClauses = 0;
}

bool KnowledgeBase::Ask (Sentence& query)
{
	cout << "Prove: " << query << endl;
	ClauseList queryClauses (query);
	bool result = Ask(queryClauses);
	queryClauses.Clear();
	if (result)
		cout << "Found proof.\n";
	else cout << "No proof found.\n";
	return result;
}

bool KnowledgeBase::Ask (ClauseList& queryClauses)
{
	ClauseList negatedQueryClauses;
	ClauseList workingClauses;
	bool result;

	queryClauses.Negate(negatedQueryClauses);
	negatedQueryClauses.Simplify();
	numWorkingClauses = numKBClauses;
	NumberWorkingClauses(negatedQueryClauses);
	if (DEBUG) {
		negatedQueryClauses.Print();
		cout << endl;
	}
	workingClauses.AddClauses(negatedQueryClauses);
	workingClauses.AddClauses(clausesKB);
	result = PLResolution(workingClauses);
	workingClauses.Clear();
	return result;
}

void KnowledgeBase::Tell (Sentence& sentence)
{

	ClauseList clauses (sentence);
	clauses.Simplify();
	Tell(clauses);
}

void KnowledgeBase::Tell (ClauseList& clauses)
{
	Clause* clause;
	list<Clause*>::iterator itr;
	for (itr = clauses.clauseList.begin(); itr != clauses.clauseList.end(); itr++)
	{
		clause = *itr;
		if (clausesKB.AddNewClause (clause)) {
			clause->inKB = true;
			clause->id = ++numKBClauses;
		}
	}
}

void KnowledgeBase::Remove (Sentence& sentence)
{
	ClauseList clauses (sentence);
	clauses.Simplify();
	Remove(clauses);
}

void KnowledgeBase::Remove (ClauseList& clauses)
{
	Clause* clause;
	list<Clause*>::iterator itr;
	for (itr = clauses.clauseList.begin(); itr != clauses.clauseList.end(); itr++)
	{
		clause = *itr;
		clausesKB.RemoveClause (clause);
	}
}

void KnowledgeBase::Read (string& fileName)
{
	string line;
	Sentence sentence;
	ifstream inFile (fileName.c_str());

	if (inFile.is_open())
	{
		while (getline (inFile, line))
		{
			if (line[0] != '%') {
				sentence = (Sentence) line;
				Tell(sentence);
			}
		}
		inFile.close();
	} else {
		cout << "Cannot read from file " << fileName << endl;
	}
}

void KnowledgeBase::Print ()
{
	cout << "Knowledge Base:\n";
	clausesKB.Print();
	cout << endl;
}

void KnowledgeBase::Clear ()
{
	clausesKB.Clear(true);
	numKBClauses = 0;
	numWorkingClauses = 0;
}

bool KnowledgeBase::PLResolution (ClauseList& clauses)
{
	ClauseList resolvents;
	ClauseList newClauses;
	list<Clause*>::iterator itrC1;
	list<Clause*>::iterator itrC2;
	Clause* clause1;
	Clause* clause2;
	int numIterations = 0;

	while (true)
	{
		numIterations++;
		if (DEBUG) {
			cout << "PLResolution (iteration " << numIterations << ", #clauses " << clauses.clauseList.size() << "):\n\n";
		}
		newClauses.clauseList.clear();
		for (itrC1 = clauses.clauseList.begin(); itrC1 != clauses.clauseList.end(); itrC1++)
		{
			clause1 = *itrC1;
			itrC2 = itrC1;
			for (itrC2++; itrC2 != clauses.clauseList.end(); itrC2++)
			{
				clause2 = *itrC2;
				if ((! clause1->inKB) || (! clause2->inKB)) { // set-of-support strategy
					resolvents.clauseList.clear();
					PLResolve (clause1, clause2, resolvents);
					if (resolvents.ContainsEmptyClause())
					{
						newClauses.Clear();
						resolvents.Clear();
						return true;
					}
					// newClauses.AddNewClauses(resolvents);
					newClauses.AddClauses(resolvents); // Will Simplify once all added
				}
			}
		}
		newClauses.Simplify(); // new addition
		if (! clauses.AddNewClauses(newClauses))
		{
			// No new clauses added, so cannot prove
			return false;
		} else clauses.Simplify(); // new addition
	}
}

void KnowledgeBase::PLResolve (Clause* clause1, Clause* clause2, ClauseList& resolvents)
{
	Clause* newClause1;
	Clause* newClause2;
	list<Literal>::iterator itr1;
	list<Literal>::iterator itr2;

	for (itr1 = clause1->negativeLiterals.begin(); itr1 != clause1->negativeLiterals.end(); itr1++)
	{
		for (itr2 = clause2->positiveLiterals.begin(); itr2 != clause2->positiveLiterals.end(); itr2++)
		{
			if (*itr1 == *itr2)
			{
				newClause1 = new Clause;
				newClause1->AddNewLiterals(clause1);
				newClause1->RemoveLiteral(*itr1, false);
				newClause2 = new Clause;
				newClause2->AddNewLiterals(clause2);
				newClause2->RemoveLiteral(*itr2, true);
				newClause1->AddNewLiterals(newClause2);
				delete newClause2;
				if (newClause1->Tautology()) {
					delete newClause1;
				} else {
					newClause1->id = ++numWorkingClauses;
					resolvents.AddClause(newClause1);
				}
			}
		}
	}
	for (itr1 = clause1->positiveLiterals.begin(); itr1 != clause1->positiveLiterals.end(); itr1++)
	{
		for (itr2 = clause2->negativeLiterals.begin(); itr2 != clause2->negativeLiterals.end(); itr2++)
		{
			if (*itr1 == *itr2)
			{
				newClause1 = new Clause;
				newClause1->AddNewLiterals(clause1);
				newClause1->RemoveLiteral(*itr1, true);
				newClause2 = new Clause;
				newClause2->AddNewLiterals(clause2);
				newClause2->RemoveLiteral(*itr2, false);
				newClause1->AddNewLiterals(newClause2);
				delete newClause2;
				if (newClause1->Tautology()) {
					delete newClause1;
				} else {
					newClause1->id = ++numWorkingClauses;
					resolvents.AddClause(newClause1);
				}
			}
		}
	}
	if (DEBUG) {
		if (! resolvents.clauseList.empty())
		{
			cout << "Resolve:\n";
			clause1->Print();
			cout << endl;
			clause2->Print();
			cout << endl;
			cout << "Resolvents:\n";
			resolvents.Print();
			cout << endl;
		}
	}
}

// Numbers each clause starting at numClauses+1; updates numClauses.
void KnowledgeBase::NumberWorkingClauses(ClauseList& clauses)
{
	Clause* clause;
	list<Clause*>::iterator itr;
	for (itr = clauses.clauseList.begin(); itr != clauses.clauseList.end(); itr++)
	{
		clause = *itr;
		clause->id = ++numWorkingClauses;
	}
}


//----- ParseTreeNode class -----

ParseTreeNode::ParseTreeNode (const string& e, ParseTreeNode* l, ParseTreeNode* r)
{
	element = e;
	left = l;
	right = r;
}


//----- ParseTree class -----

ParseTree::ParseTree (Sentence& sentence)
{
	root = ParseSentence(sentence);
	ConvertToCNF(root,1);
	ConvertToCNF(root,2);
	ConvertToCNF(root,3);
	ConvertToCNF(root,4);
}

ParseTreeNode* ParseTree::ParseSentence (Sentence& sentence)
{
	string tmpStr = sentence;
	tmpStr.erase (remove_if (tmpStr.begin(), tmpStr.end(), ::isspace), tmpStr.end());
	if (tmpStr.length() == 0) // blank sentence string
		return NULL;
	return ParseSentence1 (tmpStr);
}

ParseTreeNode* ParseTree::ParseSentence1 (string& sentence)
// Assumes no whitespace in sentence string. Infix notation.
{
	int sentenceLength;
	OpPair opPair;
	PLOperator op;
	int opIndex;
	ParseTreeNode* node;
	string sentenceLeft;
	string sentenceRight;

	sentenceLength = sentence.length();
	opPair = FindHighestPrecedenceOp(sentence);
	op = opPair.first;
	opIndex = opPair.second;
	switch (op) {
		case OP_NOT:
			sentenceLeft = sentence.substr(1,string::npos);
			node = new ParseTreeNode("~", ParseSentence1(sentenceLeft), NULL);
			break;
		case OP_AND:
			sentenceLeft = sentence.substr(0,opIndex);
			sentenceRight = sentence.substr(opIndex+1, string::npos);
			node = new ParseTreeNode("&", ParseSentence1(sentenceLeft), ParseSentence1(sentenceRight));
			break;
		case OP_OR:
			sentenceLeft = sentence.substr(0,opIndex);
			sentenceRight = sentence.substr(opIndex+1, string::npos);
			node = new ParseTreeNode("|", ParseSentence1(sentenceLeft), ParseSentence1(sentenceRight));
			break;
		case OP_IMPLIES:
			sentenceLeft = sentence.substr(0,opIndex);
			sentenceRight = sentence.substr(opIndex+2, string::npos);
			node = new ParseTreeNode("=>", ParseSentence1(sentenceLeft), ParseSentence1(sentenceRight));
			break;
		case OP_BICONDITIONAL:
			sentenceLeft = sentence.substr(0,opIndex);
			sentenceRight = sentence.substr(opIndex+3, string::npos);
			node = new ParseTreeNode("<=>", ParseSentence1(sentenceLeft), ParseSentence1(sentenceRight));
			break;
		case OP_PAREN:
			// Sentence enclosed in paired parentheses; strip them
			sentenceLeft = sentence.substr(1,sentenceLength-2);
			node = ParseSentence1(sentenceLeft);
			break;
		case OP_NONE:
			// Assume atomic sentence
			node = new ParseTreeNode(sentence, NULL, NULL);
			break;
		case OP_ERROR:
			cout << "Error parsing sentence (" << opIndex << "): " << sentence << endl;
			node = NULL;
	}
	return node;
}

OpPair ParseTree::FindHighestPrecedenceOp (string& sentence)
{
	int parenLevel = 0;
	int index;
	int sentenceLength = sentence.length();
	OpPair result;

	result.first = OP_NONE;
	result.second = 0;
	for (index = 0; index < sentenceLength; index++)
	{
		// Process "("
		if (sentence[index] == '(') {
			parenLevel++;
			if (OP_PAREN > result.first) {
				result.first = OP_PAREN;
				result.second = index;
			}
		}
		// Process ")"
		if (sentence[index] == ')') {
			parenLevel--;
			if (parenLevel < 0) {
				result.first = OP_ERROR;
				result.second = index;
				break;
			}
		}
		// Process NOT "~"
		if ((parenLevel == 0) && (sentence[index] == '~')) {
			if (OP_NOT > result.first) {
				result.first = OP_NOT;
				result.second = index;
			}
		}
		// Process AND "&"
		if ((parenLevel == 0) && (sentence[index] == '&')) {
			if (OP_AND > result.first) {
				result.first = OP_AND;
				result.second = index;
			}
		}
		// Process OR "|"
		if ((parenLevel == 0) && (sentence[index] == '|')) {
			if (OP_OR > result.first) {
				result.first = OP_OR;
				result.second = index;
			}
		}
		// Process IMPLIES "=>"
		if ((parenLevel == 0) && (index < (sentenceLength-1)) && (sentence[index] == '=') && (sentence[index+1] == '>')) {
			if (OP_IMPLIES > result.first) {
				result.first = OP_IMPLIES;
				result.second = index;
			}
		}
		// Process BICONDITIONAL "=>"
		if ((parenLevel == 0) && (index < (sentenceLength-2)) && (sentence[index] == '<') && (sentence[index+1] == '=') && (sentence[index+2] == '>')) {
			if (OP_BICONDITIONAL > result.first) {
				result.first = OP_BICONDITIONAL;
				result.second = index;
			}
		}
	}
	return result;
}

void ParseTree::ConvertToCNF (ParseTreeNode* node, int caseNum)
{
	ParseTreeNode* tmpNode1;
	ParseTreeNode* tmpNode2;

	if (node != NULL) {
		switch (caseNum) {
		case 1:
			if (node->element == "<=>") {
				tmpNode1 = new ParseTreeNode("=>", node->left, node->right);
				tmpNode2 = new ParseTreeNode("=>", Copy(node->right), Copy(node->left));
				node->element = "&";
				node->left = tmpNode1;
				node->right = tmpNode2;
			}
			ConvertToCNF(node->left, caseNum);
			ConvertToCNF(node->right, caseNum);
			break;
		case 2:
			if (node->element == "=>") { // Case 2
				tmpNode1 = new ParseTreeNode("~", node->left, NULL);
				node->left = tmpNode1;
				node->element = "|";
			}
			ConvertToCNF(node->left, caseNum);
			ConvertToCNF(node->right, caseNum);
			break;
		case 3:
			if (node->element == "~") { // assume node->left exists
				tmpNode1 = node->left;
				if (tmpNode1->element == "~") { // Case 3a
					tmpNode2 = tmpNode1->left;
					node->element = tmpNode2->element;
					node->left = tmpNode2->left;
					node->right = tmpNode2->right;
					delete tmpNode1;
					delete tmpNode2;
					ConvertToCNF(node, caseNum);
				}
				if (tmpNode1->element == "&") { // Case 3b
					node->element = "|";
					tmpNode1->element = "~";
					tmpNode2 = new ParseTreeNode("~", tmpNode1->right, NULL);
					tmpNode1->right = NULL;
					node->right = tmpNode2;
				}
				if (tmpNode1->element == "|") { // Case 3c
					node->element = "&";
					tmpNode1->element = "~";
					tmpNode2 = new ParseTreeNode("~", tmpNode1->right, NULL);
					tmpNode1->right = NULL;
					node->right = tmpNode2;
				}
			}
			ConvertToCNF(node->left, caseNum);
			ConvertToCNF(node->right, caseNum);
			break;
		case 4:
			ConvertToCNF(node->left, caseNum);
			ConvertToCNF(node->right, caseNum);
			if (node->element == "|") {
				tmpNode1 = node->left;
				tmpNode2 = node->right;
				if (tmpNode2->element == "&") { // Case 4a
					tmpNode1 = new ParseTreeNode("|", node->left, tmpNode2->left);
					tmpNode2->element = "|";
					tmpNode2->left = Copy(node->left);
					node->element = "&";
					node->left = tmpNode1;
					ConvertToCNF(node->left, caseNum);
					ConvertToCNF(node->right, caseNum);
				} else if (tmpNode1->element == "&") { // Case 4b
					tmpNode2 = new ParseTreeNode("|", tmpNode1->right, node->right);
					tmpNode1->element = "|";
					tmpNode1->right = Copy(node->right);
					node->element = "&";
					node->right = tmpNode2;
					ConvertToCNF(node->left, caseNum);
					ConvertToCNF(node->right, caseNum);
				}
			}
			break;
		}
	}
}

ParseTreeNode* ParseTree::Copy (ParseTreeNode* node)
{
	ParseTreeNode* newNode = NULL;
	if (node != NULL)
	{
		newNode = new ParseTreeNode(node->element, Copy(node->left), Copy(node->right));
	}
	return newNode;
}

void ParseTree::Print ()
{
	cout << "Parse Tree:\n";
	Print1(0,root);
}

void ParseTree::Print1 (int tab, ParseTreeNode* node)
{
	if (node != NULL) {
		for (int i = 0; i < tab; i++)
			cout << " ";
		cout << node->element << endl;
		Print1(tab+2, node->left);
		Print1(tab+2, node->right);
	}
}

void ParseTree::Clear()
{
	Clear1(root);
	root = NULL;
}

void ParseTree::Clear1(ParseTreeNode* node)
{
	if (node != NULL) {
		Clear1 (node->left);
		Clear1 (node->right);
		delete node;
	}
}

