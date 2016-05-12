#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "Global_Vars.h"
#include "Player.h"

class Logger
{
public:
    

    /**/
    void write_Log();
    void write_state(std::vector<Player*> &all_Players);
    void read_state(int state, std::vector<Player*> &all_Players);
    std::string get_date(void);
    void read_log_header();

private:

    std::string last_log;
    std::vector<std::string> lvls;
    std::vector<std::string> positions;
};

#endif