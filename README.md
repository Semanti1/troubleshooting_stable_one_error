To compile:

g++ *.cpp  -lboost_program_options -g

To run (sample command):

./a.out --problem causal_furniture --furniture "kerosene lamp" --number 6 --causal false --disabletree 0 --usetransforms 1 --runs 25 --mindoubles 8 --maxdoubles 16

Descriptions:

--furniture - furniture name

--number - number of parts in the furniture

--causal - true if using causal graph, false otherwise

--disabletree - 0 if using POMCP, 1 if using RolloutSearch

--runs - number of runs per # of simulations, reward will be the average across all runs

--mindoubles - minimum # of simulations in the experiment

--maxdoubles - maximum # of simulations in the experiment.
