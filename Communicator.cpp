#include "Communicator.h"

void connect_thread(int i ) 
{    
    pid_t pid;

    if ( (pipe(fdread[i]) < 0) || (pipe(fdwrite[i]) < 0) )
    {
        std::cerr << "PIPE ERROR" << std::endl;
        return ;
    }
    if ( (pid = fork()) < 0 )
    {
        std::cerr << "FORK ERROR" << std::endl;
        return;
    }   
    // parent process
    if (pid == 0)     // CHILD PROCESS
    {
        close(fdwrite[i][1]);
        close(fdread[i][0]);

        if (fdwrite[0] != STDIN_FILENO)
        {
            if (dup2(fdwrite[i][0], STDIN_FILENO) != STDIN_FILENO)
            {
                std::cerr << "dup2 error to stdin" << std::endl;
            }
            close(fdwrite[i][0]);
        }
        if (fdread[i][1] != STDOUT_FILENO)
        {
            if (dup2(fdread[i][1], STDOUT_FILENO) != STDOUT_FILENO)
            {
                std::cerr << "dup2 error to stdout" << std::endl;
            }
            close(fdread[i][1]);
        }
        if ( execlp(executes[i], (char *)0) < 0 )
        {
            std::cerr << "system error" << std::endl;
            return;
        }
        return;
    }
    else        // PARENT PROCESS
    {
        int MAXLINE = 15;
        char line[MAXLINE];
        int rv;
        // Close file dscriptors)
        close(fdwrite[i][0]);
        close(fdread[i][1]);
        
        //Create a string with CONNECT NUM_COLS NUM_ROWS
        char conn = CONNECT;
        std::ostringstream oss;
        oss << conn << " " << NUM_COLS << " "  << NUM_ROWS << " "  << num_Players << " " << i << std::endl;
        std::string var = oss.str();
        char server_msg[MAXLINE];
        strncpy(server_msg, var.c_str(), sizeof(server_msg));
      //  std::cout <<server_msg << std::endl;
        
        // Write to pipe
        if ( write(fdwrite[i][1], server_msg, strlen(server_msg) ) != strlen(server_msg) )
        {
            std::cerr << "WRITE ERROR FROM PIPE" << std::endl;
        }
       
        do  //WAITS FOR COMMUNICATION -- EWOULDBLOCK
        {
            rv = read(fdread[i][0], line, MAXLINE);
            if ( rv < 0 )
            {
                std::cerr << "READ ERROR FROM PIPE" << std::endl;
            }
            else if (rv == 0)
            {
                std::cerr << "Child Closed Pipe" << std::endl;
                return;
            }
            else
                line[rv] = '\0';
        }while ( rv  == -1 ) ; 

        
        std::cout << "Player number "<< i+1 <<" is:" << line << std::endl;
        if( strcmp (line, "CONNECTED") != 0)
        {
            std::cerr << "Connection error with player "<< i+1 << std::endl;
            exit(-1);
        }
        return;
    }
}

void send_Map(int i)
{
    int rv;
    int MAXLINE = NUM_COLS*NUM_ROWS;
    char line[MAXLINE];
    //Create a string with MAP world_map
    char conn = MAP;
    std::string var = "";
    var += conn;
    var += " ";
    
    for(int i = 0; i < NUM_COLS*NUM_ROWS; i++){
        if ( world_Map[i] == STONE_PUP)
            var += STONE;
        else
            var += world_Map[i]; 	
    }
    char server_msg[NUM_COLS*NUM_ROWS +3];
    strncpy(server_msg, var.c_str(), sizeof(server_msg));
    server_msg[sizeof(server_msg)-1] = '\n';
   //  std::cout <<server_msg << std::endl;
    if ( write(fdwrite[i][1], server_msg, strlen(server_msg) ) != strlen(server_msg) )
    {
        std::cerr << "WRITE ERROR FROM PIPE WHILE SENDING MAP" << std::endl;
    }
    do  //WAITS FOR COMMUNICATION -- EWOULDBLOCK
    {
        rv = read(fdread[i][0], line, 15);
        if ( rv < 0 )
        {
            std::cerr << "READ ERROR FROM PIPE WHILE SENDING MAP" << std::endl;
        }
        else if (rv == 0)
        {
            std::cerr << "Child "<< i+1<<" Closed Pipe WHILE SENDING MAP" << std::endl;
            return;
        }
        else
            line[rv] = '\0';
    }while ( rv  == -1 ) ; 
    
   // std::cout << "Player number "<< i+1 <<" is:" << line << std::endl;
    if( strcmp (line, "RECEIVED") != 0)
    {
        std::cerr << "Connection error with player "<< i+1 << std::endl;
        exit(-1);
    }
    
}

void get_Action(int i, std::vector<Player*> players, Map *map){
    int rv;
    int MAXLINE = 80;
    char line[MAXLINE];
    char server_msg[MAXLINE];
    //Create a string with P Player.x Player.y
    char conn = POSITIONS;
    std::ostringstream oss;
    oss << conn << " " ;
    for ( int j = 0; j < num_SPlayers; j++)
    {
        oss << " " << players[j]->get_mapX() << " "  << players[j]->get_mapY() << " " << players[j]->get_Range() << " " << players[j]->is_Alive() << " " << players[j]->get_Speed() << " " << players[j]->get_Team_Id();
    }
    oss << std::endl;
    std::string var = oss.str();
    strncpy(server_msg, var.c_str(), sizeof(server_msg));
    //std::cout <<server_msg << std::endl;
    if ( write(fdwrite[i][1], server_msg, strlen(server_msg) ) != strlen(server_msg) )
    {
        std::cerr << "WRITE ERROR FROM PIPE WHILE SENDING POSITIONS" << std::endl;
    }
    do  //WAITS FOR COMMUNICATION -- EWOULDBLOCK
    {
        rv = read(fdread[i][0], line, 2);
        if ( rv < 0 )
        {
            std::cerr << "READ ERROR FROM PIPE WHILE READING ACTION" << std::endl;
        }
        else if (rv == 0)
        {
            std::cerr << "Child " << i+1<<" Closed Pipe -function: get_Action()" << std::endl;
            exit(-1);
        }
        else
            line[rv] = '\0';
    }while ( rv  == -1 ) ; 
/*    if ( i == 0){
        std::cout << "Player number "<< i+1 <<" is:" << line << std::endl;
        std::cout << " " << players[i]->get_mapX() << " "  << players[i]->get_mapY() << " " << players[i]->get_Range() << " " << players[i]->is_Alive() << " " << players[i]->get_Speed() << " " << players[i]->get_Team_Id();
    }*/

    if( strcmp (line, "TIMEOUT") == 0)
    {
        std::cerr << " Player "<< i+1 << " timed out" << std::endl;
        players[i]->die();
    }
    bool updated = false;
    while ( !updated ){
        if ( map_updated )
        {
            switch (atoi(line)) 
            {
                    case LEFT:
                        if(players[i] != NULL && map->can_Move(players[i], -1) && players[i]->is_Alive())
                            players[i]->move(LEFT);
                        break;
                    case RIGHT:
                        if(players[i] != NULL && map->can_Move(players[i], 1) && players[i]->is_Alive())
                            players[i]->move(RIGHT);
                        break;
                    case UP:
                        if(players[i] != NULL && map->can_Move(players[i], -NUM_COLS) && players[i]->is_Alive())
                            players[i]->move(UP);
                        break;
                    case DOWN:
                        if(players[i] != NULL && map->can_Move(players[i], NUM_COLS) && players[i]->is_Alive())
                            players[i]->move(DOWN);
                        break;
                    case FIRE:
                        if(players[i] != NULL && map->can_Move(players[i], 0) && players[i]->is_Alive() && players[i]->can_Place()) 
                        {   // Can only plant 1 bomb per location 
                            Bomb* temp = new Bomb(players[i]);
                            map->add_bomb(temp);
                        }
                        break;
            }
            updated = true;
        }
    }
}
