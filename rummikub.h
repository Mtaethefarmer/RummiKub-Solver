/******************************************************************************/
/*!
\file   rummikub.h
\author Devone Reynolds
\par    email: mtae000@gmail.com
\brief  
    This file contains the implementation of a Rummikub Solver that would find whether or not it was possible to play a hand using legal runs and groups.
*/
/******************************************************************************/
#ifndef RUMMIKUB_H
#define RUMMIKUB_H
#include <fstream>
#include <vector>
#include <iostream>
#include <set>

enum Color { Red, Green, Blue, Yellow };

struct Tile {
    int     denomination;
    Color   color;
};

struct TileComp
{
    bool operator()(Tile a, Tile b) const
    {   
            return a.denomination < b.denomination;
    }
}TileComparator;

struct Action
{
    enum Mode{ADD_RUN, ADD_GROUP, EX_RUN, EX_GROUP};

    //Tile in question
    Tile _tile;

    //Existing tiles
    std::vector<std::vector<Tile>> * _tiles;

    int index;

    //What mode to perfom
    Mode _mode; 
};

std::ostream& operator<<(std::ostream& os, Tile const& t) {
    os << "{ "<< t.denomination << ",";
    switch ( t.color ) {
        case Red:      os << "R"; break;
        case Green:    os << "G"; break;
        case Blue:     os << "B"; break;
        case Yellow:   os << "Y"; break;
    }
    os << " }";
    return os;
}

class RummiKub {
    public:
        RummiKub(); // empty hand
        
        void Add( Tile const& ); // add a tile to hand

        void Solve(); // solve

        // get solution - groups
        std::vector< std::vector< Tile > > GetGroups() const;

        // get solution - runs
        std::vector< std::vector< Tile > > GetRuns() const;

        // if both vectors are empty - no solution possible
    private: 

    bool SolveRec(std::vector<Tile>& hand);  

    //Return if both the groups and runs are following the rules legally
    bool isLegal();

    //Calclates the number of actions that can be produced given a hand
    std::vector<Action> GetOptions(const Tile& current);

    //Prints out each of the runs and groups
    void Debug();
    void DebugHand();

    //Container of tiles
    std::vector<Tile> _hand;

    std::vector<std::vector<Tile>> _groups;
    std::vector<std::vector<Tile>> _runs;
};

#include "rummikub.cpp"
#endif
