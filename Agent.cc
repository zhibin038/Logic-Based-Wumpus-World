// Agent.cc
//
// HW2

#include <iostream>
#include <stdlib.h>
#include "Agent.h"

using namespace std;

Agent::Agent ()
{
	// HW2
	currentState.worldSize = 0; // unknown
	currentState.wumpusLocation = Location(0,0); // unknown
	currentState.goldLocation = Location(0,0); // unknown

	// HW3
	InitializeKB();
}

Agent::~Agent ()
{

}

void Agent::Initialize ()
{
	// Initialize current state
	currentState.agentLocation = Location(1,1);
	currentState.agentOrientation = RIGHT;
	currentState.agentAlive = true;
	currentState.agentHasArrow = true;
	currentState.agentHasGold = false;
	currentState.agentInCave = true;
	currentState.wumpusAlive = true;
	// Clear out any leftover actions
	actionList.clear();
	while(!stack_location.empty())
		stack_location.pop();
	stack_location.push(Location(1,1));
	lastAction = CLIMB; // dummy

	// HW2
	searchEngine.RemoveSafeLocation(currentState.wumpusLocation.X, currentState.wumpusLocation.Y);
}

Action Agent::Process (Percept& percept)
{
	Action action;

	UpdateState(percept);

	if(actionList.empty())
	{
		if (percept.Glitter) 
		{
			actionList.push_back(GRAB);
		} 
		else if (currentState.agentHasGold && (currentState.agentLocation == Location(1,1))) 
		{
			actionList.push_back(CLIMB);
		} 
		else if (currentState.agentHasGold) 
		{		
			Location location;
			location.X = 1;
			location.Y = 1;
			FindPathToLocation(location);
		}
		else if (percept.Stench && currentState.agentHasArrow) 
		{
			actionList.push_back(SHOOT);
		}
		else 
		{
			label1:
			Location l = stack_location.top();
			stack_location.pop();
			string str = "safe_" + my_to_string(l.X) + "_" + my_to_string(l.Y);
			Sentence s(str);
			bool result = KB.Ask(s);
			if(result)
			{
				switch (currentState.agentOrientation)
				{
					case RIGHT: 
					{
						stack_location.push(Location(l.X,l.Y-1));
						stack_location.push(Location(l.X,l.Y+1));
						stack_location.push(Location(l.X+1,l.Y));
						break;
					}
					case UP:
					{
						stack_location.push(Location(l.X-1,l.Y));
						stack_location.push(Location(l.X+1,l.Y));
						stack_location.push(Location(l.X,l.Y+1));
						break;
					}
					case LEFT: 
					{
						stack_location.push(Location(l.X,l.Y-1));
						stack_location.push(Location(l.X,l.Y+1));
						stack_location.push(Location(l.X-1,l.Y));
						break;
					}
					case DOWN:
					{
						stack_location.push(Location(l.X-1,l.Y));
						stack_location.push(Location(l.X+1,l.Y));
						stack_location.push(Location(l.X,l.Y-1));
						break;
					}
				}
				if(l == Location(1,1))
					goto label1;
				FindPathToLocation(l);
			}
		}
	}
	action = actionList.front();
	actionList.pop_front();
	lastAction = action;
	return action;

	/*if (actionList.empty())
	{
		if (percept.Glitter) {
			actionList.push_back(GRAB);
		} else if (currentState.agentHasGold && (currentState.agentLocation == Location(1,1))) {
			actionList.push_back(CLIMB);
		} else if ((! (currentState.goldLocation == Location(0,0))) && (! currentState.agentHasGold)) { // HW2
			// If know gold location, but don't have it, then find path to it
			FindPathToLocation(currentState.goldLocation);
		} else if (currentState.agentHasGold) { // HW2
			// If have gold, then find path to (1,1)
			Location location;
			location.X = 1;
			location.Y = 1;
			FindPathToLocation(location);
		} else if (percept.Stench && currentState.agentHasArrow) {
			actionList.push_back(SHOOT);
		} else if (percept.Bump) {
			action = (Action) ((rand() % 2) + 1); // random turn
			actionList.push_back(action);
		} else {
			// Random move
			action = (Action) (rand() % 3);
			actionList.push_back(action);
		}
	}
	action = actionList.front();
	actionList.pop_front();
	lastAction = action;
	return action;*/

}

void Agent::GameOver (int score)
{

}

void Agent::UpdateState (Percept& percept)
{
	// Check if wumpus killed
	if (percept.Scream)
	{
		currentState.wumpusAlive = false;
		// HW2: Since only kill wumpus point-blank, we know its location is in front of agent
		currentState.wumpusLocation = currentState.agentLocation;
		switch (currentState.agentOrientation)
		{
			case RIGHT: currentState.wumpusLocation.X++; break;
			case UP: currentState.wumpusLocation.Y++; break;
			case LEFT: currentState.wumpusLocation.X--; break;
			case DOWN: currentState.wumpusLocation.Y--; break;
		}
	}
	// Check if have gold
	if (lastAction == GRAB)
	{
		currentState.agentHasGold = true;
		currentState.goldLocation = currentState.agentLocation; // HW2
	}
	// Check if used arrow
	if (lastAction == SHOOT)
	{
		currentState.agentHasArrow = false;
	}
	// Update orientation
	if (lastAction == TURNLEFT)
	{
		currentState.agentOrientation = (Orientation) ((currentState.agentOrientation + 1) % 4);
	}
	if (lastAction == TURNRIGHT)
	{
		currentState.agentOrientation = (Orientation) ((currentState.agentOrientation + 3) % 4);
	}
	// Update location
	if ((lastAction == GOFORWARD) && (! percept.Bump))
	{
		switch (currentState.agentOrientation)
		{
			case RIGHT: currentState.agentLocation.X++; break;
			case UP: currentState.agentLocation.Y++; break;
			case LEFT: currentState.agentLocation.X--; break;
			case DOWN: currentState.agentLocation.Y--; break;
		}
	}

	// HW2: Update world size
	if (currentState.agentLocation.X > currentState.worldSize)
	{
		currentState.worldSize = currentState.agentLocation.X;
	}
	if (currentState.agentLocation.Y > currentState.worldSize)
	{
		currentState.worldSize = currentState.agentLocation.Y;
	}

	// HW2: Update safe locations in search engine
	int x = currentState.agentLocation.X;
	int y = currentState.agentLocation.Y;
	searchEngine.AddSafeLocation(x,y);
	if ((! percept.Breeze) && ((! percept.Stench) || (! currentState.wumpusAlive)))
	{
		if (x > 1) searchEngine.AddSafeLocation(x-1,y);
		if (x < currentState.worldSize) searchEngine.AddSafeLocation(x+1,y); // worldSize=0 if unknown
		if (y > 1) searchEngine.AddSafeLocation(x,y-1);
		if (y < currentState.worldSize) searchEngine.AddSafeLocation(x,y+1); // worldSize=0 if unknown
	}


	if(percept.Stench)
	{
	   string str = "stench_" + my_to_string(currentState.agentLocation.X) + "_" + my_to_string(currentState.agentLocation.Y);
	   Sentence s(str);
	   KB.Tell(s);
	}
	else
	{
	   string str =  "~stench_" + my_to_string(currentState.agentLocation.X) + "_" + my_to_string(currentState.agentLocation.Y);
	   Sentence s(str);
	   KB.Tell(s);
	}

	if(percept.Breeze)
	{
	   string str = "breeze_" + my_to_string(currentState.agentLocation.X) + "_" + my_to_string(currentState.agentLocation.Y);
	   Sentence s(str);
	   KB.Tell(s);
	}
	else
	{
	   string str = "~breeze_" + my_to_string(currentState.agentLocation.X) + "_" + my_to_string(currentState.agentLocation.Y);
	   Sentence s(str);
	   KB.Tell(s);
	}

	// ----- HW3
	// Potentially add new wumpus/stench rules to KB... (todo)
	// Potentially add new pit/breeze rules to KB... (todo)
	// Add stench and breeze info to KB and try to find wumpus/pits
	/*if (currentState.wumpusLocation == Location(0,0))
	{
		// Add stench information to KB
		string str1 = "stench_" + my_to_string(currentState.agentLocation.X) + "_" + my_to_string(currentState.agentLocation.Y);
		if (! percept.Stench) {
			str1 = "~" + str1;
		}
		Sentence s1(str1);
		KB.Tell(s1);

		// Check if can prove wumpus location (below is too specific)
		Sentence s2("wumpus_1_2");
		if (KB.Ask(s2))
		{
			currentState.wumpusLocation = Location(1,2);
		}
		Sentence s3("wumpus_2_1");
		if (KB.Ask(s3))
		{
			currentState.wumpusLocation = Location(2,1);
		}
	}
	// Add breeze information to KB
	string str1 = "breeze_" + my_to_string(currentState.agentLocation.X) + "_" + my_to_string(currentState.agentLocation.Y);
	if (! percept.Breeze) {
		str1 = "~" + str1;
	}
	Sentence s1(str1);
	KB.Tell(s1);

	// Check if can prove any pit locations (below is too specific)
	Sentence s2("pit_1_2");
	if (KB.Ask(s2))
	{
		currentState.pitLocations.push_back(Location(1,2)); // do something with this information
	}
	Sentence s3("pit_2_1");
	if (KB.Ask(s3))
	{
		currentState.pitLocations.push_back(Location(2,1)); // do something with this information
	}*/

	// KB.Print(); // Uncomment to check KB at each turn

	// ----- End HW3 additions
}

// HW2: Use search to find sequence of actions from agent's current location to
// the given goalLocation, and add the actions to the actionList.
void Agent::FindPathToLocation (Location& goalLocation)
{
	SearchState* initialState = new SearchState (currentState.agentLocation, currentState.agentOrientation, 0, NULL, CLIMB);
	SearchState* goalState = new SearchState (goalLocation, currentState.agentOrientation, 0, NULL, CLIMB);
	SearchState* finalState = searchEngine.Search (initialState, goalState);
	// If solution found, retain actions
	if (finalState != NULL)
	{
		SearchState* tmpState = finalState;
		while (tmpState->parent != NULL)
		{
			actionList.push_front(tmpState->action);
			tmpState = tmpState->parent;
		}
	}
	searchEngine.Clear(); // deletes entire search tree, including initialState and finalState
	delete goalState;
}

// HW3
void Agent::InitializeKB ()
{
	// Start with some rules and facts near the (1,1) location
	/*Sentence s1("(stench_1_3 & stench_2_2 & stench_1_1) => wumpus_1_2");
	Sentence s2("(stench_3_1 & stench_2_2 & stench_1_1) => wumpus_2_1");
	Sentence s3("(breeze_1_3 & breeze_2_2 & breeze_1_1) => pit_1_2");
	Sentence s4("(breeze_3_1 & breeze_2_2 & breeze_1_1) => pit_2_1");
	Sentence s5("safe_1_1");
	Sentence s6("(pit_1_2 | wumpus_1_2) => ~safe_1_2");
	Sentence s7("(pit_2_1 | wumpus_2_1) => ~safe_2_1");
	KB.Tell(s1);
	KB.Tell(s2);
	KB.Tell(s3);
	KB.Tell(s4);
	KB.Tell(s5);
	KB.Tell(s6);
	KB.Tell(s7);*/
	string fileName("kb1.txt");
	KB.Read(fileName);
	KB.Print();
}


// HW3: utilities

#include <sstream>

string my_to_string (int x)
{
	ostringstream ss;
	ss << x;
	return ss.str();
}

