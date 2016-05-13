#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "Global_Vars.h"
#include "Player.h"

class Logger
{
public:
    
    /* Writes the log header*/
    void write_Log(); 
    /* Writes every state of the game*/
    void write_state(std::vector<Player*> &all_Players);
    /* Writes who won */
    void write_winner(int n);
    /* Reads the log header*/
    void read_log_header();
    /* Reads every state of the game and replays it*/
    void read_state(int state, std::vector<Player*> &all_Players);
    /* Gets current date to name the log file*/
    std::string get_date(void);


private:
    /* Saves the last_log to check if it's diferent*/
    std::string last_log;
    /* Saves map disposition and positions to navigate*/
    std::vector<std::string> lvls;
    std::vector<std::string> positions;
};

#endif