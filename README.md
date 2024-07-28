To compile:

g++ *.cpp  -lboost_program_options -g

To run (sample command):

./a.out --problem causal_furniture --furniture "flash" --number 4 --causal true --causalfilename test_troubleshooting/test_user_output.txt --disabletree 0 --usetransforms 1 --runs 50 --mindoubles 14 --maxdoubles 14 --confidence 0.9 --outputfile test_troubleshooting/flash_expert_vis_jul28_test_more
Descriptions:

--furniture - furniture name. It HAS TO be set to "flash", "wall", "kero" or "desk".

--number - number of parts in the furniture (wall lamp: 4, flashlight: 4, desk lamp: 6, kerosene lamp: 5 )

--causal - true if using causal graph, false otherwise

--causalfilename - Input file with causal embeddings for all users.

--disabletree - 0 if using POMCP, 1 if using RolloutSearch. Set to use since we use POMCP.

--runs - number of runs per # of simulations, reward will be the average across all runs. Set to 50

--mindoubles - minimum # of simulations in the experiment. Set to 14

--maxdoubles - maximum # of simulations in the experiment. Set to 14

--confidence - The confidence in causal input (between 0 and 1). Set to 0.9.

--outputfile- Path to output file.

To register a new object for troubleshooting other than the lamps, follow the steps below. For the lamps, it will work without any changes for all possible error locations just by updating the parameters while running. However for new objects, the following changes to the code have to be made:

1. In main.cpp in after block 197-204, add an if statement with the name of the furniture and set the maximum number of error locations possible for that object. Simulations will be run for all error locations at once.

2. Make sure the furniture name added in step 1 is the same name provided to the "--furniture" parameter while running the program.

3. The value of "maxerrors" for step 1 needs to be manually determine. Take the ground truth causal model for that object. Trim it at the observable malfunction point. In the trimmed model consider all possible nodes and connections. The total number of nodes in trimmed ground-truth causal models plus the total number of edges is the value for maxerrors for that object.

4. In causal_furniture.cpp in the constructor, after the block (ln: 22-29) add another if block adding the furniture name and names of all its parts. Note: Name has to be the same string used in step 1.

5. In causal_furniture.cpp in the isValid function, add an if block for the new furniture. This block sets up the simulation for each possible error location. There will be an inner if block for "er_num" going from 1 to maxerrors set in step 1. The value of "validconnections" determines the "true" location of error that the robot needs to discover. Since we are working with only one error right now, there will just be one pair in the vector. For each error number, validconnections will have a unique error. These errors will be the nodes and connections in trimmed ground-truth causal models for that object. For nodes, the pair will be the object part paired with itself.

Visualization:

1. To turn off visualizing belief state, comment out line 42 and 82b in mcts.cpp

2. In experiment.cpp turn off line 74 vand 78 to turn off printing actions and observations.

