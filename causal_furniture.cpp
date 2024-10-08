#include "causal_furniture.h"
#include "beliefstate.h"
#include "utils.h"
#include <math.h>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;
using namespace UTILS;

CAUSAL_FURNITURE::CAUSAL_FURNITURE(std::string name = "kerosene_lamp", int numparts = 6, bool causal = true, std::vector<std::pair<std::string, std::string>> allpairs = {}, double conf=0.75, int er_num = 1)
:   furniture_name(name),
    numParts(numparts),
    causal(causal),
    allpairs(allpairs),
    conf(conf),
    er_num(er_num)
{
    NumActions = (numparts * (numparts - 1) / 2)+numparts;
    NumObservations = 2;
    if (furniture_name=="wall")
        names = { "backplate","lampbody","cord","lightbulb" };
    else if (furniture_name == "desk")
        names = { "cord","outlet", "base", "lightbulb", "pullchain", "shade" };
    else if (furniture_name == "flash")
        names = { "batteries", "switchandcircuit", "lens", "lightbulb" };
    else if(furniture_name == "kerosene")
        names = { "kerosenetank","burner", "chimney", "wick", "lighter" };
    else if (furniture_name == "scooter")
        names = { "neck","handle", "fender","fwheel","bwheel","deck","brake", "stand" };
    /*else if (furniture_name == "paddle")
        names = { "hull","handle","rudder","pedals","seats","drain","wheel","shaft" };*/
    else if (furniture_name == "pistol")
        names = { "frame","grip","triggerguard","barrel","hammer","sights","trigger","magazine","firingpin","cartridge","slide" };
    else if (furniture_name == "paddle")
        names = { "hull","handle","rudder","pedals","seats","drain","wheels","shaft" };
    else if (furniture_name == "cannon")
        names = { "barrel","trunnions","carriage","fuse","vent","gunpowder","chamber","sights" };
    else if (furniture_name == "sink")
        names = { "shutoff","watersupply","faucet","taps","sinktrap","tailpiece","drain","basin" };
    else if (furniture_name == "toilet")
        names = { "tank","supply","float","handle","chain","flapper","refill","bowl","seat","closet" };
    else if (furniture_name == "bicycle")
        names = { "frame","handle","brake","fork","fwheel","seat","rgears","fgears","pedals","crank","chain","bwheel" };
    else if (furniture_name == "tricycle")
        names = { "frame","handle","fork","pedals","front","seat","right","left" };
    else if (furniture_name == "standing_mixer")
        names = { "circuit","cord","switch","motor","fan","control","gear","beaters","housing","bowl","frame" };
    else if (furniture_name == "hand_mixer")
        names = { "handle","body","crank","gears","beater" };
    else
    {
        cout << "unknown object" << endl;
        exit(1);
    }
    cout << " Furniture name: " << furniture_name << " Parts: " << names[0];

    //RewardRange = 100;// NumActions / 4.0;
    //Discount = 0.95;
  // names = { "kerosenetank","burner", "chimney", "wick", "lighter" };
   // names = { "batteries", "switchandcircuit", "lens", "lightbulb"};
 //names = { "cord","outlet", "base", "lightbulb", "pullchain", "shade"};
  // names = { "backplate","lampbody","cord","lightbulb" };
  //  names = { "frame","grip","triggerguard","barrel","hammer","sights","trigger","magazine","firingpin","cartridge","slide" };
   // names = { "watersupply","taps","faucet","faucetaerator","basin", "shutoff", "tailpiece", "drain","sinktrap" };
    //cout << "numparts" << numParts << Discount <<endl;
   // cout << "all pair size" << allpairs.size() << endl;
    
}

STATE* CAUSAL_FURNITURE::Copy(const STATE& state) const
{
    assert(state.IsAllocated());
    const CAUSAL_FURNITURE_STATE& oldstate = safe_cast<const CAUSAL_FURNITURE_STATE&>(state);
    CAUSAL_FURNITURE_STATE* newstate = MemoryPool.Allocate();
    *newstate = oldstate;
    return newstate;
}

void CAUSAL_FURNITURE::Validate(const STATE& state) const
{
    /*const CAUSAL_FURNITURE_STATE& cfstate = safe_cast<const CAUSAL_FURNITURE_STATE&>(state);
    for (int i = 0; i < XSize * YSize; ++i)
    {
        if (bsstate.Cells(i).Diagonal && bsstate.Cells(i).Occupied)
        {
            DisplayState(bsstate, cout);
            assert(false);
        }
    }*/
}
STATE* CAUSAL_FURNITURE::CreateStartStateReal(int real) const
{
    CAUSAL_FURNITURE_STATE* cfstate = MemoryPool.Allocate();
    cfstate->numleft = 0;
    cfstate->unassembled.clear();
    for (int i = 0; i < NumActions; i++)
    {
        OBJECT_PARTS obj;
        obj.num = i;
        int n = numParts;
        //int x_ind = n - 2 - floor(sqrt(-8 * i + 4 * n * (n - 1) - 7) / 2.0 - 0.5);
        //int y_ind = i + x_ind + 1 - n * (n - 1) / 2 + (n - x_ind) * ((n - x_ind) - 1) / 2;
        int x_ind = (0.5 * (2 * n + 1 - sqrt(4 * n * n + 4 * n - 8 * i + 1)));
        int y_ind = (i - (x_ind * n - 0.5 * x_ind * x_ind - 0.5 * x_ind));
        std::string obj1name = names[x_ind];
        std::string obj2name = names[y_ind];
        std::pair<std::string, std::string> connection(obj1name, obj2name);
        if (isValid(connection))
        {
            obj.legit = 1;
        }
        else
        {
            obj.legit = 0;
        }
        //obj.legit = Random(2);
        if (obj.legit)
            cfstate->numleft++;
        obj.visited = false;
        cfstate->unassembled.push_back(obj);
    }

    return cfstate;
}

STATE* CAUSAL_FURNITURE::CreateStartState() const
{
    CAUSAL_FURNITURE_STATE* cfstate = MemoryPool.Allocate();
  
    cfstate->unassembled.clear();
    cfstate->numleft = 0;
    if (causal)
    {
       // cout << "CAUSAL";
       // int j = Random(NumActions);
            for (int i = 0; i < NumActions; i++)
            {
            //int i = Random(NumActions);
                OBJECT_PARTS obj;
                obj.num = i;
                int n = numParts;
                //int x_ind = n - 2 - floor(sqrt(-8 * i + 4 * n * (n - 1) - 7) / 2.0 - 0.5);
                //int y_ind = i + x_ind + 1 - n * (n - 1) / 2 + (n - x_ind) * ((n - x_ind) - 1) / 2;
                int x_ind = (0.5 * (2 * n + 1 - sqrt(4 * n * n + 4 * n - 8 * i + 1)));
                int y_ind = (i - (x_ind * n - 0.5 * x_ind * x_ind - 0.5 * x_ind));
                std::string obj1name = names[x_ind];
                std::string obj2name = names[y_ind];
                std::pair<std::string, std::string> connection(obj1name, obj2name);
                if (isCausal(connection))
                {
                    obj.legit = Bernoulli(conf);
                }
                else {
                    obj.legit = Bernoulli(1-conf);
                }
                if (obj.legit)
                    cfstate->numleft++;
                obj.visited = false;
                cfstate->unassembled.push_back(obj);
                
            }
            //cout << "start state" <<(cfstate->numleft) << endl;
        
        return cfstate;
    }
    else
    {
        //cout << " NOT in causal" << endl;
        for (int i = 0; i < NumActions; i++)
        {
            OBJECT_PARTS obj;
            obj.num = i;
            
            obj.legit = Random(2);
            
            if (obj.legit)
                cfstate->numleft++;
            obj.visited = false;
            cfstate->unassembled.push_back(obj);
        }
        return cfstate;
    }
}



void CAUSAL_FURNITURE::FreeState(STATE* state) const
{
    CAUSAL_FURNITURE_STATE* cfstate = safe_cast<CAUSAL_FURNITURE_STATE*>(state);
    MemoryPool.Free(cfstate);
}

bool CAUSAL_FURNITURE::Step(STATE& state, int action,
    int& observation, double& reward) const
{
    CAUSAL_FURNITURE_STATE& cfstate = safe_cast<CAUSAL_FURNITURE_STATE&>(state);

    
OBJECT_PARTS& obj = cfstate.unassembled[action];
if (obj.visited)
{
    reward = -10;
    observation = 0;
    assert(false);

}
else
{
    if (obj.legit)
    {
        reward = -1;
        //reward = 10;
      // observation = 1;
        cfstate.numleft = cfstate.numleft - 1;
    }
    else
    {
        reward = -1;
     // observation = 0;
    }
    obj.visited = true;
}
if (cfstate.numleft == 0)
{
    reward += NumActions;
    observation = 1;
    //cout << "num left " << cfstate.numleft << endl;
    return true;
}
else
{
    observation = 0;
    return false;
}
}
bool CAUSAL_FURNITURE::isValid(std::pair<std::string, std::string> connection) const
{
  //  std::vector<std::pair<std::string, std::string>> validconnections{ std::make_pair("backplate","lampbody"), std::make_pair("lampbody","lightbulb"), std::make_pair("cord","lightbulb") };
  //std::vector<std::pair<std::string, std::string>> validconnections{ std::make_pair("kerosenetank", "burner"), std::make_pair("burner","wick"), std::make_pair("lighter","wick"),std::make_pair("chimney","burner") };
  //std::vector<std::pair<std::string, std::string>> validconnections{ std::make_pair("batteries", "switchandcircuit"),std::make_pair("batteries", "lightbulb"), std::make_pair("switchandcircuit","lightbulb"), std::make_pair("lens","lightbulb") };
   //std::vector<std::pair<std::string, std::string>> validconnections{ std::make_pair("outlet", "cord"),std::make_pair("cord", "lightbulb"), std::make_pair("base","lightbulb"), std::make_pair("pullchain","lightbulb"), std::make_pair("shade","lightbulb") };
   // std::vector<std::pair<std::string, std::string>> validconnections{ std::make_pair("firingpin","slide"), std::make_pair("sights","slide"),std::make_pair("barrel","slide"),std::make_pair("trigger","hammer"),std::make_pair("hammer","slide"),std::make_pair("slide","frame"),std::make_pair("magazine","frame"),std::make_pair("trigger","frame"),std::make_pair("triggerguard","frame"),std::make_pair("grip","frame"),std::make_pair("cartridge","magazine") };
   // std::vector<std::pair<std::string, std::string>> validconnections{ std::make_pair("shutoff","watersupply"),std::make_pair("watersupply","faucet"), std::make_pair("taps","faucet"), std::make_pair("faucetaerator","faucet"), std::make_pair("sinktrap","tailpiece"), std::make_pair("tailpiece", "drain"), std::make_pair("drain", "basin") };

    //Trouble shooting. Valid connections are the damaged parts causing error.

   // std::vector<std::pair<std::string, std::string>> validconnections{ std::make_pair("kerosenetank", "kerosenetank"), std::make_pair("burner","wick"), std::make_pair("lighter","lighter") };
    std::vector<std::pair<std::string, std::string>> validconnections;
    if (furniture_name == "wall")
    {
        if (er_num == 1)
        {
            validconnections = { std::make_pair("lightbulb","lightbulb") };
        }
        else if (er_num == 2)
        {
            validconnections = { std::make_pair("cord","lightbulb") };
        }
        else if (er_num == 3)
        {
            validconnections = { std::make_pair("cord","cord") };
        }
        else
        {
            assert(false);
        }
    }
    if (furniture_name == "flash")
    {
        if (er_num == 1)
        {
            validconnections = { std::make_pair("batteries", "switchandcircuit") };
        }
        else if (er_num == 2)
        {
            validconnections = { std::make_pair("batteries", "lightbulb") };
        }
        else if (er_num == 3)
        {
            validconnections = { std::make_pair("switchandcircuit","lightbulb") };
        }
        else if (er_num == 4)
        {
            validconnections = { std::make_pair("switchandcircuit","switchandcircuit") };
        }
        else if (er_num == 5)
        {
            validconnections = { std::make_pair("batteries","batteries") };
        }
        else if (er_num == 6)
        {
            validconnections = { std::make_pair("lightbulb","lightbulb") };
        }
        else
        {
            assert(false);
        }
    }
    if (furniture_name == "kerosene")
    {
        if (er_num == 1)
        {
            validconnections = { std::make_pair("kerosenetank", "burner") };
        }
        else if (er_num == 2)
        {
            validconnections = { std::make_pair("burner","wick") };
        }
        else if (er_num == 3)
        {
            validconnections = { std::make_pair("lighter","wick") };
        }
        else if (er_num == 4)
        {
            validconnections = { std::make_pair("kerosenetank","lighter") };
        }
        else if (er_num == 5)
        {
            validconnections = { std::make_pair("kerosenetank","kerosenetank") };
        }
        else if (er_num == 6)
        {
            validconnections = { std::make_pair("burner","burner") };
        }
        else if (er_num == 7)
        {
            validconnections = { std::make_pair("lighter","lighter") };
        }
        else if (er_num == 8)
        {
            validconnections = { std::make_pair("wick","wick") };
        }
        else
        {
            assert(false);
        }
    }
    if (furniture_name == "desk")
    {
        if (er_num == 1)
        {
            validconnections = { std::make_pair("outlet", "cord") };
        }
        else if (er_num == 2)
        {
            validconnections = { std::make_pair("cord","lightbulb") };
        }
        else if (er_num == 3)
        {
            validconnections = { std::make_pair("pullchain","lightbulb") };
        }
        else if (er_num == 4)
        {
            validconnections = { std::make_pair("lightbulb","lightbulb") };
        }
        else if (er_num == 5)
        {
            validconnections = { std::make_pair("pullchain","pullchain") };
        }
        else if (er_num == 6)
        {
            validconnections = { std::make_pair("outlet","outlet") };
        }
        else if (er_num == 7)
        {
            validconnections = { std::make_pair("cord","cord") };
        }
        else
        {
            assert(false);
        }
    }

    if (furniture_name == "bicycle")
    {
        if (er_num == 1)
        {
            validconnections = { std::make_pair("pedals","crank")};
        }
        else if (er_num == 2)
        {
            validconnections = { std::make_pair("crank","fgears")};
        }
        else if (er_num == 3)
        {
            validconnections = { std::make_pair("fgears","chain") };
        }
        else if (er_num == 4)
        {
            validconnections = { std::make_pair("rgears","chain") };
        }
        else if (er_num == 5)
        {
            validconnections = { std::make_pair("rgears","bwheel") };
        }
        else if (er_num == 6)
        {
            validconnections = { std::make_pair("frame","rgears") };
        }
        else if (er_num == 7)
        {
            validconnections = { std::make_pair("bwheel","frame") };
        }
        else if (er_num == 8)
        {
            validconnections = { std::make_pair("fgears","frame") };
        }
        else if (er_num == 9)
        {
            validconnections = { std::make_pair("pedals","pedals") };
        }
        else if (er_num == 10)
        {
            validconnections = { std::make_pair("crank","crank") };
        }
        else if (er_num == 11)
        {
            validconnections = { std::make_pair("fgears","fgears") };
        }
        else if (er_num == 12)
        {
            validconnections = { std::make_pair("chain","chain") };
        }
        else if (er_num == 13)
        {
            validconnections = { std::make_pair("rgears","rgears") };
        }
        else if (er_num == 14)
        {
            validconnections = { std::make_pair("bwheel","bwheel") };
        }
        else if (er_num == 15)
        {
            validconnections = { std::make_pair("frame","frame") };
        }
        else
        {
            assert(false);
        }
    }

    if (furniture_name == "sink")
    {
        if (er_num == 1)
        {
            validconnections = { std::make_pair("taps","faucet") };
        }
        else if (er_num == 2)
        {
            validconnections = { std::make_pair("watersupply","faucet") };
        }
        else if (er_num == 3)
        {
            validconnections = { std::make_pair("shutoff","watersupply"), };
        }
        else if (er_num == 4)
        {
            validconnections = { std::make_pair("taps","taps") };
        }
        else if (er_num == 5)
        {
            validconnections = { std::make_pair("faucet","faucet") };
        }
        else if (er_num == 6)
        {
            validconnections = { std::make_pair("watersupply","watersupply") };
        }
        else if (er_num == 7)
        {
            validconnections = { std::make_pair("shutoff","shutoff") };
        }
        else
        {
            assert(false);
        }
    }

    if (furniture_name == "toilet")
    {
        if (er_num == 1)
        {
            validconnections = { std::make_pair("tank","handle") };
        }
        else if (er_num == 2)
        {
            validconnections = { std::make_pair("supply","tank") };
        }
        else if (er_num == 3)
        {
            validconnections = { std::make_pair("tank","flapper"), };
        }
        else if (er_num == 4)
        {
            validconnections = { std::make_pair("handle","chain") };
        }
        else if (er_num == 5)
        {
            validconnections = { std::make_pair("flapper","chain") };
        }
        else if (er_num == 6)
        {
            validconnections = { std::make_pair("tank","tank") };
        }
        else if (er_num == 7)
        {
            validconnections = { std::make_pair("handle","handle") };
        }
        else if (er_num == 8)
        {
            validconnections = { std::make_pair("supply","supply") };
        }
        else if (er_num == 9)
        {
            validconnections = { std::make_pair("chain","chain") };
        }
        else if (er_num == 10)
        {
            validconnections = { std::make_pair("flapper","flapper") };
        }
        else if (er_num == 11)
        {
            validconnections = { std::make_pair("float","supply") };
        }
        else if (er_num == 12)
        {
            validconnections = { std::make_pair("float","float") };
        }
        else
        {
            assert(false);
        }
    }
   // cout << "Error in : " << validconnections[0].first << "-" << validconnections[0].second << endl;
   // std::vector<std::pair<std::string, std::string>> validconnections{ std::make_pair("cord", "lightbulb") };
   // std::vector<std::pair<std::string, std::string>> validconnections{ std::make_pair("kerosenetank", "kerosenetank") };
   // std::vector<std::pair<std::string, std::string>> validconnections{ std::make_pair("batteries", "batteries") };
    auto it3 = std::find_if(validconnections.begin(), validconnections.end(), [&](std::pair<std::string, std::string> obj) { return ((obj.first == connection.first) && (obj.second == connection.second)); });
    auto it4 = std::find_if(validconnections.begin(), validconnections.end(), [&](std::pair<std::string, std::string> obj) { return ((obj.first == connection.second) && (obj.second == connection.first)); });
    if ((validconnections.end() == it3) && (validconnections.end() == it4))
        return false;
    else
        return true;
}

bool CAUSAL_FURNITURE::isCausal(std::pair<std::string, std::string> connection) const
{
    

    auto it3 = std::find_if(allpairs.begin(), allpairs.end(), [&](std::pair<std::string, std::string> obj) { return ((obj.first == connection.first) && (obj.second == connection.second)); });
    auto it4 = std::find_if(allpairs.begin(), allpairs.end(), [&](std::pair<std::string, std::string> obj) { return ((obj.first == connection.second) && (obj.second == connection.first)); });

    if ((allpairs.end() == it3) && (allpairs.end() == it4))
        return false;
    else
        return true;
}
bool CAUSAL_FURNITURE::LocalMove(STATE& state, const HISTORY& history,
    int stepObs, const STATUS& status) const
{
    CAUSAL_FURNITURE_STATE& cfstate = safe_cast<CAUSAL_FURNITURE_STATE&>(state);
    int ct = 1;
    while (ct <= NumActions)
    {
        ct += 1;
        int action = Random(NumActions);
        OBJECT_PARTS& obj = cfstate.unassembled[action];
        if (obj.visited)
        {
            //cout << "in visited, continuing" << endl;
            continue;
        }
        else
        {
            /*int chance;
            int n = numParts;
            int x_ind = n - 2 - floor(sqrt(-8 * action + 4 * n * (n - 1) - 7) / 2.0 - 0.5);
            int y_ind = action + x_ind + 1 - n * (n - 1) / 2 + (n - x_ind) * ((n - x_ind) - 1) / 2;
            std::string obj1name = names[x_ind];
            std::string obj2name = names[y_ind];
            std::pair<std::string, std::string> connection(obj1name, obj2name);*/
            /*if (isCausal(connection))
                chance = Bernoulli(0.75);
            else
                chance = Bernoulli(0.25);

            if (obj.legit == chance)
                continue;*/

            
            obj.legit = !obj.legit;
            if (obj.legit)
                cfstate.numleft++;
            else
                cfstate.numleft--;
            //return true;

            //cout << "history" << history.Size() << endl;
            for (int t = 0; t < history.Size(); ++t)

            {
                // Ensure that ships are consistent with observation history
                int a = history[t].Action;
                const OBJECT_PARTS& obj2 = cfstate.unassembled[a];

                assert(obj2.visited);
                if (history[t].Observation)
                {
                    if (obj2.legit != history[t].Observation)
                    {
                        //cout << "return false" << endl;
                        return false;
                    }
                }
            }
            //cout << "return true" << endl;
            return true;
        }


    }
    return false;
    
   
}


void CAUSAL_FURNITURE::GenerateLegal(const STATE& state, const HISTORY& history,
    vector<int>& legal, const STATUS& status) const
{
    const CAUSAL_FURNITURE_STATE& cfstate = safe_cast<const CAUSAL_FURNITURE_STATE&>(state);
    for (int a = 0; a < NumActions; ++a)
    {
        if (!cfstate.unassembled[a].visited)
        {
            legal.push_back(a);
        }
    }
    //bool diagonals = Knowledge.Level(status.Phase) == KNOWLEDGE::SMART;
    /*if (diagonals)
    {
        for (int a = 0; a < NumActions; ++a)
            if (!bsstate.Cells(a).Visited && !bsstate.Cells(a).Diagonal)
                legal.push_back(a);
    }
    else
    {*/
    /*for (int a = 0; a < NumActions; ++a)
    {
        // reference: https://stackoverflow.com/questions/27086195/linear-index-upper-triangular-matrix
        int n = numParts;
        int x_ind = n - 2 - floor(sqrt(-8 * a + 4 * n * (n - 1) - 7) / 2.0 - 0.5);
        int y_ind = a + x_ind + 1 - n * (n - 1) / 2 + (n - x_ind) * ((n - x_ind) - 1) / 2;
        std::string obj1name = names[x_ind];
        std::string obj2name = names[y_ind];

        auto it = std::find_if(cfstate.unassembled.begin(), cfstate.unassembled.end(), [&](OBJECT_PARTS obj) { return (obj.name == obj1name); });
        auto it2 = std::find_if(cfstate.unassembled.begin(), cfstate.unassembled.end(), [&](OBJECT_PARTS obj) { return obj2name == obj.name; });
        if ((cfstate.unassembled.end() != it) && (cfstate.unassembled.end() != it2))
        {
            if ((*it).ports_remaining > 0 && (*it2).ports_remaining > 0)
            {
                legal.push_back(a);
            }
            //cout << "legal" << a ;
        }
        //cout << endl;

    }
            //if (!bsstate.Cells(a).Visited)
                
    //}*/
}



void CAUSAL_FURNITURE::DisplayState(const STATE& state, ostream& ostr) const
{
    const CAUSAL_FURNITURE_STATE& cfstate = safe_cast<const CAUSAL_FURNITURE_STATE&>(state);
    
    std::string outstate[numParts+1][numParts+1];
    for (int x = 0; x < numParts+1; x++)
    {
        for (int y = 0; y < numParts+1; y++)
        {
            if ((x == 0) && (y == 0))
            {
                outstate[x][y] = "|";
            }
            else if (x == 0)
                outstate[x][y] = names[y - 1];
            else if (y == 0)
                outstate[x][y] = names[x - 1];
            else
                outstate[x][y] = ".";



        }
    }
    ostr << endl << "  ";
    for (int i = 0; i < NumActions; i++)
    {
        const OBJECT_PARTS& obj = cfstate.unassembled[i];
        
        int n = numParts;
       // int x_ind = n - 2 - floor(sqrt(-8 * i + 4 * n * (n - 1) - 7) / 2.0 - 0.5);
        //int y_ind = i + x_ind + 1 - n * (n - 1) / 2 + (n - x_ind) * ((n - x_ind) - 1) / 2;
        int x_ind = (0.5 * (2 * n + 1 - sqrt(4 * n * n + 4 * n - 8 * i + 1)));
        int y_ind = (i - (x_ind * n - 0.5 * x_ind * x_ind - 0.5 * x_ind));
        if (obj.visited && obj.legit)
            outstate[x_ind + 1][y_ind + 1] = "V + L";
        else if (obj.visited && !obj.legit)
            outstate[x_ind + 1][y_ind + 1] = "V + NL";
        else if (!obj.visited && obj.legit)
            outstate[x_ind + 1][y_ind + 1] = "NV + L";
        else if (!obj.visited && !obj.legit)
            outstate[x_ind + 1][y_ind + 1] = "NV + NL";

        
    }

    for (int x = 0; x < numParts + 1; x++)
    {
        for (int y = 0; y < numParts + 1; y++)
        {
            ostr << setw(10) << outstate[x][y] << ' ';
        }
        ostr << endl;
    }
    
    /*cout << "unassembled" << endl;
    for (int i = 0; i < cfstate.unassembled.size(); i++)
    {
        
        int num = cfstate.unassembled[i].num;
        DisplayAction(num, cout);
        cout << "legit " << cfstate.unassembled[i].legit << ", visited" <<  cfstate.unassembled[i].visited << endl;
    }*/
}
/*
void BATTLESHIP::DisplayObservation(const STATE& state, int observation, ostream& ostr) const
{
    if (observation)
        cout << "Hit\n";
    else
        cout << "Miss\n";
}

void BATTLESHIP::DisplayAction(int action, ostream& ostr) const
{
    COORD actionPos = COORD(action % XSize, action / XSize);

    ostr << endl << "  ";
    for (int x = 0; x < XSize; x++)
        ostr << setw(1) << x << ' ';
    ostr << "  " << endl;
    for (int y = YSize - 1; y >= 0; y--)
    {
        ostr << setw(1) << y << ' ';
        for (int x = 0; x < XSize; x++)
        {
            char c = ' ';
            if (actionPos == COORD(x, y))
                c = '@';
            ostr << c << ' ';
        }
        ostr << setw(1) << y << endl;
    }
    ostr << "  ";
    for (int x = 0; x < XSize; x++)
        ostr << setw(1) << x << ' ';
    ostr << "  " << endl;
}
*/
void CAUSAL_FURNITURE::DisplayBeliefs(const BELIEF_STATE& beliefState,
    ostream& ostr) const
{
    GRID<int> counts(numParts, numParts);
    counts.SetAllValues(0);

    for (int j = 0; j < beliefState.GetNumSamples(); j++)
    {
        const CAUSAL_FURNITURE_STATE* cfstate =
            safe_cast<const CAUSAL_FURNITURE_STATE*>(
                beliefState.GetSample(j));
        for (int i = 0; i < NumActions; i++)
        {
            const OBJECT_PARTS& obj = cfstate->unassembled[i];

            int n = numParts;
            int x_ind = (0.5 * (2 * n + 1 - sqrt(4 * n * n + 4 * n - 8 * i + 1)));
            int y_ind = (i - (x_ind * n - 0.5 * x_ind * x_ind - 0.5 * x_ind));

            counts(x_ind, y_ind) += obj.legit;
        }
    }

    
    for (int x = 0; x < numParts + 1; x++)
    {
        ostr << endl;
        for (int y = 0; y < numParts + 1; y++)
        {
            if ((x == 0) && (y == 0))
            {
                ostr.width(10);
                ostr.precision(2);
                ostr << fixed << "|";
            }
            else if (x == 0)
            {
                ostr.width(10);
                ostr.precision(2);
                ostr << fixed << names[y - 1] << " ";
            }
               
            else if (y == 0)
            {
                ostr.width(10);
                ostr.precision(2);
                ostr << fixed << names[x - 1]<< " ";
            }
            else
            {
                ostr.width(10);
                ostr.precision(2);
                ostr << fixed << (double)counts(x-1, y-1) / beliefState.GetNumSamples()<< " ";
            }



        }
        ostr << endl;
    }
}
void CAUSAL_FURNITURE::DisplayAction(int action, ostream& ostr) const
{


    int n = numParts;
    //int x_ind = n - 2 - floor(sqrt(-8 * action + 4 * n * (n - 1) - 7) / 2.0 - 0.5);
    //int y_ind = action + x_ind + 1 - n * (n - 1) / 2 + (n - x_ind) * ((n - x_ind) - 1) / 2;
    int x_ind = (0.5 * (2 * n + 1 - sqrt(4 * n * n + 4 * n - 8 * action + 1)));
    int y_ind = (action - (x_ind * n - 0.5 * x_ind * x_ind - 0.5 * x_ind));

    std::string obj1name = names[x_ind];
    std::string obj2name = names[y_ind];
    if (obj1name==obj2name)
        ostr << "replace " << obj1name << endl;
    else
        ostr << "inspect connection between " << obj1name << " and " << obj2name << endl;
}