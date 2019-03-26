/******************************************************************************/
/*!
\file   rummikub.cpp
\author Devone Reynolds
\par    email: mtae000@gmail.com
\brief  
    This file contains the implementation of a Rummikub Solver that would find whether or not it was possible to play a hand using legal runs and groups.
*/
/******************************************************************************/
#include <algorithm> //sort

RummiKub::RummiKub()
{
}

void RummiKub::Add(Tile const &tile)
{
    //Add the tile to the container
    _hand.push_back(tile);
}

void RummiKub::Solve()
{
    // Algorithm:
    // brute-force recursion: for each next tile you have the following options
    // 1) add it to an existing run with the same color as tile and tile's denomination is not yet in the run
    // 2) add it to an existing group with the same denomiation as tile and tile's color is not yet in the group
    // 3) create a new run
    // 4) create a new group
    //DebugHand();
    //Loop through each tile
    //Possiblly copy each card
    std::vector<Tile> a = _hand;
    //Sort the containter
    std::sort(a.begin(), a.end(), TileComparator);
    SolveRec(a);
}

std::vector<std::vector<Tile>> RummiKub::GetGroups() const
{
    return _groups;
}

std::vector<std::vector<Tile>> RummiKub::GetRuns() const
{
    return _runs;
}

bool RummiKub::SolveRec(std::vector<Tile> &hand)
{
    // base case
    // out of tiles -
    // return true if yest and exit
    if (hand.empty())
    {

        return isLegal();
    }

    Tile tile = hand.back();
    std::vector<Action> actions = GetOptions(tile);

    hand.pop_back();

    size_t aSize = actions.size();
    for (Action &action : actions)
    {
        //A series of actions are legal if there are 3 or more actions
        if (aSize)
        {
            //Execute based off of the mode in the action
            switch (action._mode)
            {
            //Add New Run
            case Action::ADD_RUN:
                _runs.push_back(std::vector<Tile>{action._tile});
                break;

            //Add New Group
            case Action::ADD_GROUP:
                _groups.push_back(std::vector<Tile>{action._tile});
                break;

            //Add to Existing Run
            case Action::EX_RUN:
                _runs[action.index].push_back(action._tile);
                break;

            //Add to Existing Group
            case Action::EX_GROUP:
                _groups[action.index].push_back(action._tile);
                break;

            //Invalid actions do nothing
            default:
                break;
            }
        }

        //Recursive call, if solved return true
        if (SolveRec(hand))
        {
            return true;
        }
        //Otherwise undo the action
        else
        {
            //Execute based off of the mode in the action
            switch (action._mode)
            {
            //Add New Run
            case Action::ADD_RUN:
                _runs.pop_back();
                break;

            //Add New Group
            case Action::ADD_GROUP:
                _groups.pop_back();
                break;

            //Add to Existing Run
            case Action::EX_RUN:
                _runs[action.index].pop_back();
                break;

            //Add to Existing Group
            case Action::EX_GROUP:
                _groups[action.index].pop_back();
                break;

            //Invalid actions do nothing
            default:
                break;
            }
        }
    }

    hand.push_back(tile);
    return false;
}

bool RummiKub::isLegal()
{
    bool correct = true;
    //Check each group
    for (std::vector<Tile> const &g : _groups)
    {
        //Check each tile in that group
        for (Tile const &t : g)
        {
            if (g.size() < 3 || g.size() > 4)
            {
                return false;
            }
            std::vector<Tile>::iterator it = std::find_if(_hand.begin(), _hand.end(),
                                                          [&t](Tile const &t1) {
                                                              return t.color == t1.color && t.denomination == t1.denomination;
                                                          });
            if (it == _hand.end())
            {
                return false;
            }
        }
    }

    for (std::vector<Tile> const &r : _runs)
    {
        if (r.size() < 3)
        {
            return false;
        }

        //Check each tile in that group
        for (Tile const &t : r)
        {
            std::vector<Tile>::iterator it = std::find_if(_hand.begin(), _hand.end(),
                                                          [&t](Tile const &t1) {
                                                              return t.color == t1.color && t.denomination == t1.denomination;
                                                          });
            if (it == _hand.end())
            {
                return false;
            }
        }

        int counts[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

        Color color = r[0].color;
        
        for (unsigned long i = 0; i < r.size(); ++i)
        {
            if (r[i].color != color)
            {
                return false;
                //correct = false;
            }
            ++counts[r[i].denomination];
        }

        for (int const &c : counts)
        {
            if (c > 1)
            {
                return false;
                //correct = false;
            }
        }

        if (correct)
        {
            int len = 0;
            for (int b : counts)
            {
                if (b)
                { // is tile
                    if (len > 0)
                    { // continued run
                        ++len;
                    }
                    else
                    { // new run started
                        len = 1;
                    }
                }
                else
                { // no tile
                    if (len > 0)
                    { // run ended
                        if (len < 3)
                        {
                            return false;
                            //correct = false;
                        }
                        len = 0; // not really necessary, see "len=1" above
                    }
                    else
                    { // no tile and no run going
                    }
                }
            }
            //check the length of the last run
            if (counts[12] && len < 3)
            {
                return false;
                //correct = false;
            }
        }

        
    }

    //Debug();
    return true;
}

std::vector<Action> RummiKub::GetOptions(const Tile &current)
{
    // 1) add it to an existing run with the same color as tile and tile's denomination is not yet in the run
    // 2) add it to an existing group with the same denomiation as tile and tile's color is not yet in the group
    // 3) create a new run
    // 4) create a new group
    std::vector<Action> a;
    int index = 0;

    //Determine if a run already exist that the tile can be added to
    for (std::vector<Tile> &r : _runs)
    {
        bool isRunValid = true;
        //Check each tile in that run
        for (Tile const &t : r)
        {

            //Check if current tile can be added to that run
            if ((current.denomination == t.denomination) || (current.color != t.color))
            {
                isRunValid = false;
                break;
            }
        }

        if (isRunValid)
        {
            //Add an action to add to group
            a.push_back(Action{current, &_runs, index, Action::EX_RUN});
        }

        ++index;
    }

    //Reset the index
    index = 0;

    //Determine if a group already exist that the tile can be added to
    for (std::vector<Tile> &g : _groups)
    {
        bool isGroupValid = true;

        //Check each tile in that group
        for (Tile const &t : g)
        {

            //Check if current tile can be added to that group
            if (current.color == t.color || current.denomination != t.denomination)
            {
                isGroupValid = false;
                break;
            }
        }

        if (isGroupValid)
        {
            //Add an action to add to group
            a.push_back(Action{current, &_groups, index, Action::EX_GROUP});
        }

        ++index;
    }

    //Create a new run
    a.push_back(Action{current, &_runs, -1, Action::ADD_RUN});

    //Create a new group
    a.push_back(Action{current, &_groups, -1, Action::ADD_GROUP});

    return a;
}

//Prints out each of the runs and groups
void RummiKub::Debug()
{
    std::cout << "DEBUG~~~~~~~~";
    int counter = 0;
    //Runs
    for (const std::vector<Tile> &r : _runs)
    {

        std::cout << std::endl
                  << "Run " << counter << std::endl;
        for (const Tile &t : r)
        {
            std::cout << t;
        }
        ++counter;
    }

    counter = 0;
    //Groups
    for (const std::vector<Tile> &g : _groups)
    {
        std::cout << std::endl
                  << "Group " << counter << std::endl;
        for (const Tile &t : g)
        {
            std::cout << t;
        }
        ++counter;
    }
    std::cout << std::endl
              << "DEBUG~~~~~~~~" << std::endl;
}

void RummiKub::DebugHand()
{
    std::cout << "DEBUG-HAND~~~~~~~~" << std::endl;
    ;
    //Runs

    for (const Tile &t : _hand)
    {
        std::cout << t;
    }
}