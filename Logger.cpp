#include "Logger.h"

void Logger::write_Log()
{
    int serial_number = rand() % 9999;
    char buffer [33];
    std::sprintf(buffer,"%d",serial_number); 

    std::string path = "Logs/log_";
    path += buffer;
    path += "_";
    path += get_date();
    std::cout << path << std::endl;
    log_data.open(path,std::fstream::out );
    log_data << "--------------------- LOG "<<  serial_number <<" ---------------------- \n" ;
    log_data << "Number of players : " << num_Players << "\n";    
    log_data << "Number of agents  : " << connected << "\n";  
    log_data << "Manual player id  : " << manual_Player_id << "\n";  
    log_data << "Screen height     : " << SCREEN_HEIGHT <<  " \t  Screen width    : " << SCREEN_WIDTH << "\n";    
    log_data << "Numbr of collumns : " << NUM_COLS       << " \t  Numbr of rows   : " << NUM_ROWS << std::endl;    
    log_data << "Map               : " << level << "\n";    
    log_data << "-----------------------------------------------------\n";    
}
void Logger::write_state(std::vector<Player*> &all_Players)
{
    state++;
    std::string current_log = "";
    for(int i = 0; i < NUM_COLS*NUM_ROWS; i++){
        current_log += world_Map[i]; 	
    }
    current_log += "\n";
    current_log += "P ";
    for ( int j = 0; j < num_SPlayers; j++)
    {
       current_log += std::to_string(all_Players[j]->get_mapX()); 
       current_log += " ";
       current_log += std::to_string(all_Players[j]->get_mapY());
       current_log += " ";
       current_log += std::to_string(all_Players[j]->get_Range());
       current_log += " ";
       current_log += std::to_string(all_Players[j]->is_Alive());
       current_log += " ";
       current_log += std::to_string(all_Players[j]->get_Speed());
       current_log += " ";
       current_log += std::to_string(all_Players[j]->get_Team_Id());
       current_log += " ";
    }
    current_log += "\n";
    if ( current_log.compare(last_log) ){
        last_log = current_log;
        log_data << last_log;
    }
}
void Logger::write_winner(int winner)
{
    log_data << "-----------------------------------------------------\n";    
    if ( winner != -1)
        log_data << " Player : " << winner+1 << " won!!";
    else
        log_data << " Draw !!";
}
std::string Logger::get_date(void)
{
   time_t now;
   char the_date[12];

   the_date[0] = '\0';

   now = time(NULL);

   if (now != -1)
   {
      std::strftime(the_date, 12, "%d_%m_%Y", gmtime(&now));
   }

   return std::string(the_date);
}


void Logger::read_state( int state , std::vector<Player*> &all_Players)
{
    if ( state < lvls.size() ){
        int count = 0; 
        for (int y = 0; y < NUM_ROWS; y++) 
        {
            for (int x = 0; x < NUM_COLS; x++) 
            {                  
                world_Map[ mIndex(x,y)] = lvls[state][count];
                count++;
            }
        }
        std::istringstream iss(positions[state]);
        std::string ignore, px, py, r, al, sp, te;
        iss >> ignore;
        for (int i = 0; i < num_Players; i++) 
        {
            iss >> px; // x position 
            iss >> py; // y position
            iss >> r; // ranges
            iss >> al; // is player i alive?
            iss >> sp; // player speed
            iss >> te; // player's team
            all_Players[i]->set_position(atoi(px.c_str()), atoi(py.c_str()));
            if ( al.compare("0") == 0 )
                all_Players[i]->die();
            else if (  !all_Players[i]->is_Alive() )
                all_Players[i]->resurect();
        }
    }
    else{
        state = lvls.size()-1;
    }
}

void Logger::read_log_header()
{
    std::string output;
    for (int i = 0; i < 8 ; i++){
        std::getline(log_data,output);
        std::cout << output << std::endl;
        std::istringstream iss(output);

        if ( i == 1){
            std::string word;
            while(iss >> word) {
                if ( word.compare(":") == 0 ){
                    iss >> word;
                    num_Players = atoi(word.c_str());
                }
            }            
        }
        else if ( i == 6){
            std::string word;
            while(iss >> word) {
                if ( word.compare(":") == 0 ){
                    iss >> word;
                    level = (char*) malloc(sizeof(char) * word.length());
                    sprintf(level, "%s", word.c_str());
                }
            }  
        }
    }
    while(std::getline(log_data,output)) // To get you all the lines.
    {
        if (  output.compare("-----------------------------------------------------") == 0 )
        {
            std::getline(log_data,output);
            std::istringstream iss(output);
            std::string word;
            while(iss >> word) {
                if ( word.compare(":") == 0 ){
                    iss >> word;
                    std::cout << "Player " << atoi(word.c_str()) << " won!" << std::endl;
                }
                else if ( word.compare("Draw") == 0 ){
                    std::cout << "Draw!"<< std::endl;
                }
            }            
        }
        else
        {
            /*     MAP     */
            lvls.push_back(output);
            /*     PLAYERS     */
            std::getline(log_data,output);
            positions.push_back(output);
        }
    }
}