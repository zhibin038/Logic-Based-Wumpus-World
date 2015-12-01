// Agent.h
//
// HW1/2 + starting point for HW3

#ifndef AGENT_H
#define AGENT_H

#include "Action.h"
#include "Percept.h"
#include "WorldState.h" // HW1
#include "Search.h" // HW2
#include "Prover.h" // HW3

class Agent
{
public:
	Agent ();
	~Agent ();
	void Initialize ();
	Action Process (Percept& percept);
	void GameOver (int score);

	// HW1
	void UpdateState (Percept& percept);
	WorldState currentState;
	list<Action> actionList;
	Action lastAction;

	// HW2
	SearchEngine searchEngine;
	void FindPathToLocation (Location& goalLocation);

	// HW3
	KnowledgeBase KB;
	void InitializeKB();
};

// HW3 utilities
string my_to_string(int x);

#endif // AGENT_H
