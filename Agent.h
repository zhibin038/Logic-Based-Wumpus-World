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
#include <sstream>
#include <map>
#define   ALPHA 0.3
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
	Action random();
	int MaxAction (Location state , Action &act, WorldState previous_state);
	SearchEngine searchEngine;
	void FindPathToLocation (Location& goalLocation);
    static map<std::string,float> Q_table;// HW3
    static map<std::string,int> frequency_table;
	KnowledgeBase KB;
	void InitializeKB();
	float Reward( WorldState previous_state,Action lastAction) ;
	float MaxQvalue ( );
	int frequency_counts( WorldState state);
	int UpdateQ_table(Location s,WorldState previous_state );
    void Print_Table();
};

map<std::string,float> create_map();

// HW3 utilities
string my_to_string(int x);

#endif // AGENT_H
