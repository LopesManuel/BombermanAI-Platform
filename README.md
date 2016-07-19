#Bomberman as an Artificial Intelligence Platform (BAIP)

The system was developed for Unix systems using the Simple DirectMedia Layer (SDL). It allows the user to interface with BAIP using the keyboard, sending information to the screen, and emulating the game simulation. Every simulation done in the platform can also be seen as a standard reinforcement learning problem, the user just needs to keep an accumulated score. Each state consists of a game frame, represented in a 2D array of characters, the dimensions of this array varies with the window screen width and height, and with the sprite size, by default the dimensions give origin to a 20 width and 15 height array. The action space consists of 6 discrete movements ( up, down, left, right, place bomb, wait), the agents move one grid cell per movement.

In order for the simulation to be perceptible for the human eye the agents are slowed down to 5 actions per second, making it possible for a user to play using the manual controls against his/hers own AI agent. There is also the option of not showing the simulation on screen, making the agent fast as they possible can be.

BAIP can be used with any type of artificial intelligence, it passes all the information to the agents, so if a user wants to create a RL agent he needs to give a reward at each time-step based on the information received from the platform, and then store score or points between frames. An episode begins on the first frame after the start command, and terminates when the game ends. A game ends when only one agent is alive or if the simulation has run more than the predefined number of maximum states allowed per simulation. This predefined maximum number of frames was set to prevent simulations to run for ever.

It also offers more functionalities in order to aid the development of new agents. We will now present the list of all the commands accepted:

    -m , adds a player controlled by the user to the simulation, the sum of the total players needs to be greater than 2 and less or equal to 4.
    -n <number of agents>, sets the number of AI agents, the sum of the total players needs to be greater than 2 and less or equal to 4.
    -p <agents>, followed by the agents executable command, defines which agents are going to play.
    -l <map>, flag to define the map that we are going to use in the simulation, map is the path to the map text file.
    -replay <log file>, loads log file to screen and starts replay mode.
    -seed <number>, sets random seed to number.
    -log, records simulation to log file, under the Log directory.
    -f, does not draw the simulation making it faster to run, useful when we have to run several simulations.
    -learning <posX , posY>, activates the learning mode and sets the objective to <posX , posY>

Using the commands shown previously we now present how to run different simulations:

./BAIP -l Lvls/lvl0.txt -n 2 -p plannerK2 dummy

./BAIP -l Lvls/lvl3.txt -learning 19 14 -n 2 -p qlearnerE qlearner

The first command executes a normal game simulation where the last agent standing will be the winner, the latter shows a simulation for RL agent, where their objective is to arrive to the position (19,14).
