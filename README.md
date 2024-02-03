# RubikAI2
Optimized training code of "Self-Supervision is All You Need for Solving Rubik's Cube"
On each step:
20M random cube permutations with different amounts of moves are generated with C++ and pybind11.
Then a neural network consisting of 25 branches is trained to output the previous move from a given state of the cube.
If the cube is mixed enough, the previous move should be unpredictable, however, the predictions of the neural network are biased toward simpler states and can be used to guide a beam search to solve the cube even if it's thoroughly mixed.
It can be observed that the neural network can infer the previous move better than random even after 15 moves.


