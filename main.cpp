#include "battleship.h"
#include "mcts.h"
#include "network.h"
#include "pocman.h"
#include "rocksample.h"
#include "tag.h"
#include "causal_furniture.h"
#include "findit.h"
#include "findit_improved.h"
#include "experiment.h"
#include <boost/program_options.hpp>
#include <iostream>
#include <fstream>
#include <random>
#include <sstream>
using namespace std;
using namespace boost::program_options;

void UnitTests()
{
    cout << "Testing UTILS" << endl;
    UTILS::UnitTest();
    cout << "Testing COORD" << endl;
    COORD::UnitTest();
    cout << "Testing MCTS" << endl;
    MCTS::UnitTest();
}

void disableBufferedIO(void)
{
    setbuf(stdout, NULL);
    setbuf(stdin, NULL);
    setbuf(stderr, NULL);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
}

int main(int argc, char* argv[])
{
    //UTILS::mt(time(nullptr));
   
    MCTS::PARAMS searchParams;
    EXPERIMENT::PARAMS expParams;
    SIMULATOR::KNOWLEDGE knowledge;
    string problem, outputfile, policy, furniture;
    
    //std::vector<std::pair<std::string, std::string>> causalconnections{};
    string causalfilename = "";
    int size, number, treeknowledge = 1, rolloutknowledge = 1, smarttreecount = 10;
    double smarttreevalue = 1.0;
    double confidence = 0.75;
    bool causal = false;
    options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("test", "run unit tests")
        ("problem", value<string>(&problem), "problem to run")
        ("outputfile", value<string>(&outputfile)->default_value("output.txt"), "summary output file")
        ("policy", value<string>(&policy), "policy file (explicit POMDPs only)")
        ("size", value<int>(&size), "size of problem (problem specific)")
        ("number", value<int>(&number), "number of elements in problem (problem specific)")
        ("furniture", value<string>(&furniture), "furniture name (problem specific)")
        ("causal", value<bool>(&causal), "causal (problem specific)")
        ("causalfilename", value<string>(&causalfilename), "causal connections file name (problem specific)")
        ("confidence", value<double>(&confidence), "causal connections file name (problem specific)")
        ("timeout", value<double>(&expParams.TimeOut), "timeout (seconds)")
        ("mindoubles", value<int>(&expParams.MinDoubles), "minimum power of two simulations")
        ("maxdoubles", value<int>(&expParams.MaxDoubles), "maximum power of two simulations")
        ("runs", value<int>(&expParams.NumRuns), "number of runs")
        ("accuracy", value<double>(&expParams.Accuracy), "accuracy level used to determine horizon")
        ("horizon", value<int>(&expParams.UndiscountedHorizon), "horizon to use when not discounting")
        ("num steps", value<int>(&expParams.NumSteps), "number of steps to run when using average reward")
        ("verbose", value<int>(&searchParams.Verbose), "verbosity level")
        ("autoexploration", value<bool>(&expParams.AutoExploration), "Automatically assign UCB exploration constant")
        ("exploration", value<double>(&searchParams.ExplorationConstant), "Manual value for UCB exploration constant")
        ("usetransforms", value<bool>(&searchParams.UseTransforms), "Use transforms")
        ("transformdoubles", value<int>(&expParams.TransformDoubles), "Relative power of two for transforms compared to simulations")
        ("transformattempts", value<int>(&expParams.TransformAttempts), "Number of attempts for each transform")
        ("userave", value<bool>(&searchParams.UseRave), "RAVE")
        ("ravediscount", value<double>(&searchParams.RaveDiscount), "RAVE discount factor")
        ("raveconstant", value<double>(&searchParams.RaveConstant), "RAVE bias constant")
        ("treeknowledge", value<int>(&knowledge.TreeLevel), "Knowledge level in tree (0=Pure, 1=Legal, 2=Smart)")
        ("rolloutknowledge", value<int>(&knowledge.RolloutLevel), "Knowledge level in rollouts (0=Pure, 1=Legal, 2=Smart)")
        ("smarttreecount", value<int>(&knowledge.SmartTreeCount), "Prior count for preferred actions during smart tree search")
        ("smarttreevalue", value<double>(&knowledge.SmartTreeValue), "Prior value for preferred actions during smart tree search")
        ("disabletree", value<bool>(&searchParams.DisableTree), "Use 1-ply rollout action selection")
        ;

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    if (vm.count("help"))
    {
        cout << desc << "\n";
        return 1;
    }

    if (vm.count("problem") == 0)
    {
        cout << "No problem specified" << endl;
        return 1;
    }

    if (vm.count("test"))
    {
        cout << "Running unit tests" << endl;
        UnitTests();
        return 0;
    }

    SIMULATOR* real = 0;
    SIMULATOR* simulator = 0;

    if (problem == "battleship")
    {
        real = new BATTLESHIP(size, size, number);
        simulator = new BATTLESHIP(size, size, number);
    }
    else if (problem == "pocman")
    {
        real = new FULL_POCMAN;
        simulator = new FULL_POCMAN;
    }
    else if (problem == "network")
    {
        real = new NETWORK(size, number);
        simulator = new NETWORK(size, number);
    }
    else if (problem == "rocksample")
    {
        real = new ROCKSAMPLE(size, number);
        simulator = new ROCKSAMPLE(size, number);
    }
    else if (problem == "tag")
    {
        real = new TAG(number);
        simulator = new TAG(number);
    }
    else if (problem == "findit")
    {
        real = new FINDIT(size, number);
        simulator = new FINDIT(size, number);
    }
    else if (problem == "findit_improved")
    {
        real = new FINDIT_IMPROVED(size, number);
        simulator = new FINDIT_IMPROVED(size, number);
    }
    else if (problem == "causal_furniture")
    {
        //cout << "numparts" << numParts << endl;
        std::vector<std::vector<std::pair<std::string, std::string>>> allusers;
        std::vector<std::pair<std::string, std::string>> allpairs;
        string line, pair, conn;
        std::pair<std::string, std::string> connection;
        std::vector<std::string> parts;
       // cout << "args" << number << " " << causalfilename << endl;
       // cout << "confidence: " << confidence << endl;
        ifstream file;
        file.open(causalfilename);
        cout << file.is_open() << endl;
        //vector<int> seeds{ 17137, 26103, 11742, 18272, 1110, 6285, 22505, 1031, 4415, 19261, 3606, 9529, 2561, 30592, 28988, 30929, 29960, 25304, 6771, 431, 4074, 29305, 14792, 16926, 9287, 13699, 1478, 16277, 28209, 1337, 12803, 23492, 11032, 8689, 31787, 1612, 19482, 1844, 9552, 2669, 29555, 29993, 28114, 15, 27712, 18628, 12313, 11631, 24290, 13783, 10119, 1306, 9832, 1046, 14089, 16067, 19431, 17534, 9495, 28029, 3428, 16539, 15269, 20793, 10835, 27814, 17032, 9504, 23970, 12042, 15639, 1679, 10991, 7255, 24345, 4039, 243, 1372, 14922, 1214, 23854, 21408, 5848, 5271, 4791, 9537, 13681, 10061, 9987, 19328, 16376, 21757, 461, 18182, 29236, 20949, 17862, 30210, 30284, 4059 };
        vector<int> seeds{ 30869, 715, 22791, 6221, 2760, 21579, 7500, 1067, 12820, 24094, 9764, 13159, 9991, 3885, 13252, 6713, 12016, 7676, 14546, 16480, 269, 11274, 22807, 16252, 31569, 26503, 17273, 16536, 26314, 15836, 10656, 19596, 15553, 25182, 7740, 16873, 16536, 19794, 4979, 29537, 2178, 9948, 14951, 14178, 28938, 7862, 22368, 14825, 10066, 18902, 19654, 15042, 24903, 16359, 10416, 30262, 7183, 15237, 13713, 26794, 20, 23158, 18352, 22806, 6629, 20365, 2033, 27250, 16993, 22197, 1876, 3082, 23648, 16567, 6461, 20024, 7897, 8597, 3303, 23648, 505, 8147, 21529, 22001, 5006, 2189, 11476, 7796, 25387, 3718, 1741, 696, 1979, 15247, 22040, 13307, 31881, 8642, 14535, 28469, 23877, 18852, 17253};
        while (getline(file, line))
        {
           // cout << "hello" << line << endl;
            allpairs.clear();
            if (line[line.size() - 1] == '\r')
                line.erase(line.size() - 1);
            istringstream str(line);
            while (getline(str, pair, '|'))
            {
                //cout << "pair" << pair << endl;
                
                istringstream substr(pair);
                parts.clear();
                while (getline(substr, conn, ','))
                {
                    //cout << conn << endl;
                    parts.push_back(conn);

                }
                    //cout << "parts " << parts.size() << endl;
                    allpairs.push_back(make_pair(parts[0], parts[1]));
                
            }
            allusers.push_back(allpairs);
            //cout << "all pairs " << allpairs.size() << endl;
        }
        //real = new CAUSAL_FURNITURE(furniture, number, causal, allusers[0]);
        //simulator = new CAUSAL_FURNITURE(furniture, number, causal, allusers[0]);
       // cout << "all " << allusers.size() << endl;
        string outputfname;
        int maxerrors = 0;
            if (furniture == "wall")
                maxerrors = 3;
        if (furniture == "flash")
            maxerrors = 6;
        if (furniture == "kerosene")
            maxerrors = 8;
        if (furniture == "desk")
            maxerrors = 7;
        if (furniture == "bicycle")
            maxerrors = 15;
        if (furniture == "sink")
            maxerrors = 7;
        if (furniture == "toilet")
            maxerrors = 12;
        //std::vector<std::vector<std::pair<std::string, std::string>>> allusers2 = allusers;
        for (int i = 0; i < allusers.size(); i++)
        {
            for (int j = 1; j <= maxerrors; j++)
            {
                cout << "user " << i + 1 << ". Error: "<< j<< endl;
                //srand(time(NULL)+rand());
                srand(seeds[i]);
                outputfname = outputfile;
                real = new CAUSAL_FURNITURE(furniture, number, causal, allusers[i], confidence, j);
                simulator = new CAUSAL_FURNITURE(furniture, number, causal, allusers[i], confidence,j);
                simulator->SetKnowledge(knowledge);
                //cout << "knowledge" << knowledge.TreeLevel << knowledge.RolloutLevel << endl;
                /*if (i == 0)
                {
                    EXPERIMENT experiment(*real, *simulator, outputfile, expParams, searchParams);
                }
                else
                {*/
                //EXPERIMENT* experiment = new EXPERIMENT(*real, *simulator, outputfile, expParams, searchParams);
            //}
                outputfname = outputfname + "user_" + to_string((i + 1)) + "_err_ "+to_string(j)+ ".txt";
                cout << outputfname << endl;
                EXPERIMENT experiment(*real, *simulator, outputfname, expParams, searchParams);
                experiment.DiscountedReturn();
                delete real;
                delete simulator;
                //delete experiment;
                SIMULATOR* real = 0;
                SIMULATOR* simulator = 0;
            }
        }
        return 0;

        //simulator = new CAUSAL_FURNITURE("kerosene lamp", 6);
    }
    else 
    {
        cout << "Unknown problem" << endl;
        exit(1);
    }


    simulator->SetKnowledge(knowledge);
    EXPERIMENT experiment(*real, *simulator, outputfile, expParams, searchParams);
    experiment.DiscountedReturn();

    delete real;
    delete simulator;
    return 0;
}
