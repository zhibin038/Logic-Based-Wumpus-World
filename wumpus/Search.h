// Search.h
//
// A* search for wumpus world navigation.

#ifndef SEARCH_H
#define SEARCH_H

#include <list>
#include "Action.h"
#include "Percept.h"
#include "Location.h"
#include "Orientation.h"
#include "WorldState.h"

class SearchState
{
public:
	SearchState (Location& loc, Orientation& ori, int dep, SearchState* par, Action act)
	{
		location.X = loc.X;
		location.Y = loc.Y;
		orientation = ori;
		depth = dep;
		parent = par;
		action = act;
		heuristic = 0;
		cost = 0;
	}

	bool operator== (const SearchState& state);

	Location location;
	Orientation orientation;
	int depth;
	int heuristic;
	int cost;
	SearchState* parent;
	Action action; // action used to get to this state from parent state
};

class SearchEngine
{
public:
	SearchEngine () {};
	~SearchEngine () {};
	SearchState* Search (SearchState* initialState, SearchState* goalState);
	SearchState* AStarSearch (SearchState* initialState, SearchState* goalState);
	bool GoalTest (SearchState* state, SearchState* goalState);
	SearchState* GetChildState (SearchState* state, Action action);
	int CityBlockDistance (Location& location1, Location& location2);
	void Clear();
	void AddSafeLocation (int x, int y);
	void RemoveSafeLocation (int x, int y);
	bool SafeLocation (int x, int y);
	bool Visited (SearchState* state);
	void AddToFrontierInOrder (SearchState* state);

	list<SearchState*> frontier;
	list<SearchState*> explored;
	list<Location> safeLocations;
	int nodeCount;
};

list<Location>::iterator findLocation (list<Location>::iterator first, list<Location>::iterator last, const Location& val);

#endif // SEARCH_H
