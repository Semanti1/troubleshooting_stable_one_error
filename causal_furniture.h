#ifndef CAUSAL_FURNITURE_H
#define CAUSAL_FURNITURE_H

#include "simulator.h"
#include "grid.h"
#include <list>
#include <string>
struct OBJECT_PARTS
{
    
    //int ports_remaining;
    //std::string name;
    int num;
    bool legit;
    bool visited;
};

class CAUSAL_FURNITURE_STATE : public STATE
{
public:

    
    //std::vector<std::pair<std::string,std::string>> connections;
    std::vector<OBJECT_PARTS> unassembled;
    int numleft;
    //std::string furniture_name;
};

class CAUSAL_FURNITURE : public SIMULATOR
{
public:

    CAUSAL_FURNITURE(std::string name, int numparts, bool causal, std::vector<std::pair<std::string, std::string>> allpairs, double conf, int er_num);

    virtual STATE* Copy(const STATE& state) const;
    virtual void Validate(const STATE& state) const;
    //STATE* CreateStartState(int real) const;
    virtual STATE* CreateStartState() const;
    virtual STATE* CreateStartStateReal(int real) const;
    virtual void FreeState(STATE* state) const;
    virtual bool Step(STATE& state, int action, 
        int& observation, double& reward) const;
        
    void GenerateLegal(const STATE& state, const HISTORY& history,
        std::vector<int>& legal, const STATUS& status) const;
    virtual bool LocalMove(STATE& state, const HISTORY& history,
        int stepObs, const STATUS& status) const;

    virtual void DisplayBeliefs(const BELIEF_STATE& beliefState,
        std::ostream& ostr) const;
    virtual void DisplayState(const STATE& state, std::ostream& ostr) const;
    /*virtual void DisplayObservation(const STATE& state, int observation, std::ostream& ostr) const;*/
    virtual void DisplayAction(int action, std::ostream& ostr) const;
    std::vector<std::pair<std::string, std::string>> allpairs;
    //std::vector<std::pair<std::string, std::string>> found;
private:

    /*bool Collision(const BATTLESHIP_STATE& bsstate, const SHIP& ship) const;
    void MarkShip(BATTLESHIP_STATE& bsstate, const SHIP& ship) const;
    void UnmarkShip(BATTLESHIP_STATE& bsstate, const SHIP& ship) const;
    void UpdateLegal(BATTLESHIP_STATE& bsstate) const;
    bool MoveShips(BATTLESHIP_STATE& bsstate) const;
    bool SwitchTwoShips(BATTLESHIP_STATE& bsstate) const;
    bool SwitchThreeShips(BATTLESHIP_STATE& bsstate) const;
    
    int XSize, YSize;
    int MaxLength, TotalRemaining;*/
    //STATE* CreateStartStateReal() const;
    //std::vector<std::pair<std::string, std::string>> allpairs ;
    bool isValid(std::pair<std::string, std::string> connection) const;
    bool isCausal(std::pair<std::string, std::string> connection) const;
    std::string furniture_name;
    int numParts;
    bool causal;
    double conf;
    int er_num;
    //string causalfilename;
    std::vector<std::string> names;
    std::vector<int> numPorts;
    mutable MEMORY_POOL<CAUSAL_FURNITURE_STATE> MemoryPool;
};

#endif // BATTLESHIP_H
