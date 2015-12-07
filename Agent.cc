// Agent.cc
//
// HW2

#include <iostream>
#include <stdlib.h>
#include "Agent.h"

using namespace std;
map<std::string,float> create_map()
    {
      map<std::string,float> m;
      string s= "0_0_0";
        m[s] = 0.0;
      return m;
    }
map<std::string,int> create_table()
    {
      map<std::string,int> m;
      string s= "0_0_0";
        m[s] = 0;
      return m;
    }


Agent::Agent ()
{
	// HW2
	currentState.worldSize = 4; // unknown
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
	whole_last_action =-1;
	lastAction = CLIMB; // dummy
	previous_state = currentState;

	// HW2
	searchEngine.RemoveSafeLocation(currentState.wumpusLocation.X, currentState.wumpusLocation.Y);
}

float Agent::Reward( WorldState previous_state,Action lastAction)
{
	if(  !currentState.agentHasGold && lastAction == GRAB )
		return 500.0;
    if(currentState.agentHasGold && currentState.agentLocation == Location(1,1) && lastAction==CLIMB)
        return 1000.0;
	if(	lastAction == SHOOT)
		return -10.0;
	if( !currentState.wumpusAlive)
		return -1000.0;
	else
		return 0;
}

 map<std::string,float> Agent::Q_table =  create_map();
map<std::string,int> Agent::frequency_table = create_table();

int Agent::UpdateQ_table( )
{
    float Q_sa =0;
    ostringstream convert1,convert2,convert3,convert4;
    convert1 << previous_state.agentLocation.X;
    convert2 << previous_state.agentLocation.Y;
    convert3 << whole_last_action;
    if(whole_last_action == -1)
    return 0;
    string golad_state;
    if( previous_state.agentHasGold )
            golad_state = "_gold_";
    else
            golad_state = "_~gold_";
    string pre_state_action_pair ( convert1.str() + "_" + convert2.str() + golad_state  + convert3.str());
    std::map<std::string,float>::iterator it= Agent::Q_table.find(pre_state_action_pair);
        if(it != Q_table.end())
            Q_sa =Q_table[pre_state_action_pair];
        Q_sa = Q_sa + ALPHA * (Reward(previous_state ,lastAction) + 0.85* MaxQvalue() - Q_sa );
		Q_table[pre_state_action_pair] = Q_sa;
    cout<< "update " <<pre_state_action_pair << " value :" << Q_sa << endl;
    return 0;
}


Action Agent::Process (Percept& percept)
{
	Action action;

    if(percept.Glitter)
        cout << "here~~~~~~~~~~~~~~~~~~~~"<<endl;

    UpdateState(percept);

	if (actionList.empty())
	{
		//update Q-talbe but do not generate actions assume best actions
        UpdateQ_table();
        int action_code;
            //choosing the max value action or random
            int Max = MaxAction( currentState.agentLocation,previous_state );
            if( Max != -1)
                action_code = random();
            if( frequency_counts( previous_state ) < 10)
                 action_code = random();

            int valid_action=1;
            while( valid_action )
            {
                valid_action = Generate_Action(action_code);
                if(valid_action)
                    action_code = random();
            }

		if (percept.Glitter && !currentState.agentHasGold )
		 {
            actionList.pop_front();
            actionList.clear();
			actionList.push_back(GRAB);
			action_code = 5;
            }
        whole_last_action = action_code;
        previous_state = currentState;
    }
	action = actionList.front();
	int k = actionList.size();
	actionList.pop_front();
	lastAction = action;
	//Print_Table();
	return action;
}

int  Agent::frequency_counts( WorldState state)
{
    ostringstream convert1,convert2,convert3;
    convert1 << state.agentLocation.X;
    convert2 << state.agentLocation.Y;

    string  golad_state;
    if( currentState.agentHasGold )
            golad_state = "_gold_";
    else
            golad_state = "_~gold_";
    string state_s ( convert1.str() + "_" + convert2.str()  + golad_state  );
    std::map<std::string,int>::iterator it= frequency_table.find(state_s);
     if(it != frequency_table.end())
        {   frequency_table[state_s] = frequency_table[state_s] + 1;}
    else
        frequency_table[state_s] =1;
    return frequency_table[state_s];
}


Action Agent::random()
{   Action action;
    int k = rand();
   //s cout << "each rand = "<< k <<flush;
     action = (Action) (k % 4);

    return action;
}

float Agent::MaxQvalue ( )
{
    ostringstream convert1,convert2, convert3;
    convert1 << currentState.agentLocation.X;
    convert2 << currentState.agentLocation.Y;
    string  golad_state;
    if( currentState.agentHasGold )
            golad_state = "_gold_";
    else
            golad_state = "_~gold_";


    string state_s  ( convert1.str() + "_" + convert2.str() + golad_state  );

    float Max=-9999;
    for(int i=0; i < 5 ; i++)
    {
        ostringstream action_str;
        action_str << i;
        string action(action_str.str());
        string state_action_pair = state_s + action;
        std::map<std::string,float>::iterator it= Q_table.find(state_action_pair);
        if(it != Q_table.end())
          {
                float temp = Q_table[state_action_pair];
                if( Max < temp)
                    Max = temp;
          }
    }
    if(Max==-9999)
        return 0;
    return Max;
}

int Agent::Generate_Action(int action_code)
{
    cout << "action code:" << action_code <<endl;
     Location new_location;
    if(action_code == 0)  //go to right
    {
         new_location.X = currentState.agentLocation.X+1;
         new_location.Y =currentState.agentLocation.Y;
         if(new_location.X > currentState.worldSize)
            return 1;
    }
     if(action_code == 1) //go to up
    {
         new_location.X = currentState.agentLocation.X;
         new_location.Y =currentState.agentLocation.Y+1;
          if(new_location.Y > currentState.worldSize)
            return 1;
    }
     if(action_code == 2)  //go to left
    {

         new_location.X = currentState.agentLocation.X-1;
         new_location.Y =currentState.agentLocation.Y;
         if(new_location.X <=0)
            return 1;
    }
     if(action_code == 3 )   //go to down
    {
         new_location.X = currentState.agentLocation.X;
         new_location.Y =currentState.agentLocation.Y-1;
          if(new_location.Y <= 0)
            return 1;
    }
    FindPathToLocation(new_location);
    return 0;
}

int Agent::MaxAction (Location state , WorldState previous_state)
{
    ostringstream convert1,convert2,convert3;
    convert1 << state.X;
    convert2 << state.Y;
    string  golad_state;
    if( currentState.agentHasGold )
            golad_state = "_gold_";
    else
            golad_state = "_~gold_";

    string state_s ( convert1.str() + "_" + convert2.str() + golad_state );

    bool found=0;
    float Max = -1000;
    for(int i=0; i < 3 ; i++)
    {
        ostringstream action_str;
        action_str << i;
        string action(action_str.str());
        string state_action_pair = state_s + action;
        std::map<std::string,float>::iterator it= Q_table.find(state_action_pair);
        if(it != Q_table.end())
          {

                float temp = Q_table[state_action_pair];
                if( Max < temp)
                {       found =true;
                        Max = temp;
                }
               // cout << state_action_pair << " value: " << temp <<endl;
          }
    }
    if(found)
        return Max;
    return -1;
}

void Agent::Print_Table()
{
    cout << "Q-value table" <<endl;
    typedef std::map<std::string, float>::iterator it_type;
    int counter=0;
    for(it_type iterator1 = Q_table.begin(); iterator1 != Q_table.end(); iterator1++)
    {
        cout<<iterator1->first;
        cout << " value:" << iterator1->second << "   " <<flush;
        if(counter % 2 ==0)
        cout << endl;
        counter ++;
    }
    cout << endl <<"frequency table" <<endl;
    for( std::map<std::string, int>::iterator iterator1 = frequency_table.begin(); iterator1 != frequency_table.end(); iterator1++)
    {
        cout<<iterator1->first;
        cout << " couter:" << iterator1->second << "   ";
        if(counter % 2 ==0)
        cout << endl;
        counter ++;
    }

}

void Agent::GameOver (int score, Percept percept)
{

    UpdateQ_table();
    Print_Table();
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
	Sentence s1("(stench_1_3 & stench_2_2 & stench_1_1) => wumpus_1_2");
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
	KB.Tell(s7);
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

