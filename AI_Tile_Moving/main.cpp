#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <queue>
#include <time.h>
#include <iomanip>
#include <math.h>

/** index 0 start from top left corner **/
/** 0123 **/
/** 4567 **/
/** 89ab **/
/** cdef **/

using namespace std;

class axis
    {
    public:
        int x, y;
    };

class bfs_obj
    {
    public:
        int next_direction;
        vector<int> direction_list;
        axis recent_position[4];
    };

class tile_moving
    {
    public:
        queue<bfs_obj*> bfs_node;
        void initialization();
        bool depth_first();
        bool breadth_first();
        void output_backtrace_result();


    private:
        int debug;
        axis tiles_position[4];
        axis goal_position[4];
        vector<vector<char> > grid_pattern;
        bool used_pattern[16][16][16][16];

        void move_agent_block(axis (&position_arr)[4], int direction);
        bool check_goal(axis now[4]);
        void swap_tiles_in_grid(char &a, char &b);
        void swap_tiles_position(axis &a, axis &b);
        void recover_tiles_position(axis record[4]);

        /** for debugging purpose **/
        void output_grid_pattern();
        void output_tiles_position();
    };

void tile_moving::output_tiles_position()
    {
    cout << "A(" << tiles_position[0].x << ", " << tiles_position[0].y << "), "
         << "B(" << tiles_position[1].x << ", " << tiles_position[1].y << "), "
         << "C(" << tiles_position[2].x << ", " << tiles_position[2].y << "), "
         << "agent(" << tiles_position[3].x << ", " << tiles_position[3].y << "), " << endl;
    }

void tile_moving::output_backtrace_result()
    {
    cout << "A(" << goal_position[0].x << ", " << goal_position[0].y << "), "
         << "B(" << goal_position[1].x << ", " << goal_position[1].y << "), "
         << "C(" << goal_position[2].x << ", " << goal_position[2].y << "), "
         << "agent(" << goal_position[3].x << ", " << goal_position[3].y << "), " << endl;
    }

void tile_moving::output_grid_pattern()
    {
    int i, j;
    i = 0;
    while(i<=3)
        {
        j = 0;
        while(j<=3)
            {
            cout << setw(2) << grid_pattern[j][i] << ",";
            ++j;
            }
        cout << endl;
        ++i;
        }
    }

bool tile_moving::breadth_first()
    {
    int i, temp;
    int a, b, c, d;
    bfs_obj* this_move;
    bfs_obj* next_move;

    /** record the initial pattern **/
    a = tiles_position[0].x + tiles_position[0].y * 4;
    b = tiles_position[1].x + tiles_position[1].y * 4;
    c = tiles_position[2].x + tiles_position[2].y * 4;
    d = tiles_position[3].x + tiles_position[3].y * 4;
    used_pattern[a][b][c][d] = true;

    /** move left **/
    if(this_move->direction_list.size()!=0)
        {
            {

            }
        }
    next_move = new bfs_obj;
    next_move->next_direction = 2;
    next_move->direction_list.push_back(2);
    i = 3;
    while(i>=0)
        {
        next_move->recent_position[i].x = tiles_position[i].x;
        next_move->recent_position[i].y = tiles_position[i].y;
        --i;
        }
    bfs_node.push(next_move);

    /** move up **/
    next_move = new bfs_obj;
    next_move->next_direction = 3;
    next_move->direction_list.push_back(3);
    i = 3;
    while(i>=0)
        {
        next_move->recent_position[i].x = tiles_position[i].x;
        next_move->recent_position[i].y = tiles_position[i].y;
        --i;
        }
    bfs_node.push(next_move);

    while(bfs_node.empty()!=true)
        {
        this_move = bfs_node.front();
        /** make the next move **/
        move_agent_block(this_move->recent_position, this_move->next_direction);
        /** check if this pattern have already appeared **/
        a = this_move->recent_position[0].x + this_move->recent_position[0].y * 4;
        b = this_move->recent_position[1].x + this_move->recent_position[1].y * 4;
        c = this_move->recent_position[2].x + this_move->recent_position[2].y * 4;
        d = this_move->recent_position[3].x + this_move->recent_position[3].y * 4;
        if(used_pattern[a][b][c][d]!=true)
            {
            /** the pattern is new **/
            used_pattern[a][b][c][d]=true;
            /** check if we reach the goal **/
            i = 3;
            while(i>=0)
                {
                if(this_move->recent_position[i].x!=goal_position[i].x || this_move->recent_position[i].y!=goal_position[i].y)
                    break;
                else
                    --i;
                }
            /*cout << this_move->recent_position[0].x << ", " << this_move->recent_position[0].y << endl;
            cout << this_move->recent_position[1].x << ", " << this_move->recent_position[1].y << endl;
            cout << this_move->recent_position[2].x << ", " << this_move->recent_position[2].y << endl;
            cout << this_move->recent_position[3].x << ", " << this_move->recent_position[3].y << endl;*/
            if(i==-1)/** we found the solution **/
                {
                i = this_move->direction_list.size() - 1;
                output_backtrace_result();
                while(i >= 0)
                    {
                    cout << this_move->direction_list[i] << endl;
                    temp = (this_move->direction_list[i]+2) % 4;
                    move_agent_block(goal_position, temp);/** move in reverse direction **/
                    output_backtrace_result();
                    --i;
                    }
                return true;
                }
            else/** haven't reached the goal **/
                {
                if(this_move->recent_position[3].x!=0)
                    {
                    /** can move left **/
                    next_move = new bfs_obj;
                    next_move->next_direction = 2;
                    i = 3;
                    while(i>=0)
                        {
                        next_move->recent_position[i].x = this_move->recent_position[i].x;
                        next_move->recent_position[i].y = this_move->recent_position[i].y;
                        --i;
                        }
                    next_move->direction_list = this_move->direction_list;
                    next_move->direction_list.push_back(2);
                    bfs_node.push(next_move);
                    }
                if(this_move->recent_position[3].x!=3)
                    {
                    /** can move right **/
                    next_move = new bfs_obj;
                    next_move->next_direction = 0;
                    i = 3;
                    while(i>=0)
                        {
                        next_move->recent_position[i].x = this_move->recent_position[i].x;
                        next_move->recent_position[i].y = this_move->recent_position[i].y;
                        --i;
                        }
                    next_move->direction_list = this_move->direction_list;
                    next_move->direction_list.push_back(0);
                    bfs_node.push(next_move);
                    }
                if(this_move->recent_position[3].y!=0)
                    {
                    /** can move up **/
                    next_move = new bfs_obj;
                    next_move->next_direction = 3;
                    i = 3;
                    while(i>=0)
                        {
                        next_move->recent_position[i].x = this_move->recent_position[i].x;
                        next_move->recent_position[i].y = this_move->recent_position[i].y;
                        --i;
                        }
                    next_move->direction_list = this_move->direction_list;
                    next_move->direction_list.push_back(3);
                    bfs_node.push(next_move);
                    }
                if(this_move->recent_position[3].y!=3)
                    {
                    /** can move down **/
                    next_move = new bfs_obj;
                    next_move->next_direction = 1;
                    i = 3;
                    while(i>=0)
                        {
                        next_move->recent_position[i].x = this_move->recent_position[i].x;
                        next_move->recent_position[i].y = this_move->recent_position[i].y;
                        --i;
                        }
                    next_move->direction_list = this_move->direction_list;
                    next_move->direction_list.push_back(1);
                    bfs_node.push(next_move);
                    }
                }
            }
        delete this_move;
        bfs_node.pop();
        }
    return false;
    }

bool tile_moving::depth_first()
    {
    //output_grid_pattern();
    //output_tiles_position();
    int i, j, temp, k;
    int a, b, c, d;
    char neighbor;
    bool make_any_move_flag;
    axis prev_position[4];

    //cin >> debug;
    //cout << "debug" << endl;
    //k = debug;
    //debug++;

    if(check_goal(tiles_position))
        {
        return true;
        }
    else
        {
        /** record the pattern before move to next level **/
        i = 3;
        while(i >= 0)
            {
            prev_position[i].x = tiles_position[i].x;
            prev_position[i].y = tiles_position[i].y;
            //cout << "prev x: " << prev_position[i].x << endl;
            //cout << "prev y: " << prev_position[i].y << endl;
            --i;
            }
        /** try to move in every direction **/
        make_any_move_flag = false;
        i = 3;
        temp = rand();
        temp = abs(temp);
        //temp = 5;
        //cout << "temp: " << temp << endl;
        while(i >= 0)
            {
            temp = temp % 4;
            //cout << "temp in while: " << temp << endl;
            switch(temp)
                {
                case 0:/** move LEFT **/
                    if(prev_position[3].x != 0)
                        {
                        //cout << "can move left" << endl;
                        //neighbor = grid_pattern[prev_position[3].x-1][prev_position[3].y];
                        //if(neighbor != ' ')/** we are swapping a named tile **/
                        /*    swap_tiles_position(tiles_position[neighbor-65], tiles_position[3]);
                        else
                            tiles_position[3].x = tiles_position[3].x - 1;
                        swap_tiles_in_grid(grid_pattern[prev_position[3].x-1][prev_position[3].y], grid_pattern[prev_position[3].x][prev_position[3].y]);
                        */
                        move_agent_block(tiles_position, 2);

                        a = tiles_position[0].x + tiles_position[0].y * 4;
                        b = tiles_position[1].x + tiles_position[1].y * 4;
                        c = tiles_position[2].x + tiles_position[2].y * 4;
                        d = tiles_position[3].x + tiles_position[3].y * 4;

                        if(used_pattern[a][b][c][d]==false)
                            {
                            used_pattern[a][b][c][d] = true;
                            //make_any_move_flag = true;
                            if(depth_first())
                                {
                                cout << "move left" << endl;
                                move_agent_block(goal_position, 0);
                                return true;
                                }
                            else/** this is a wrong branch, recover everything we changed **/
                                {
                                //cout << "Recover from move left!" << endl;
                                recover_tiles_position(prev_position);
                                //swap_tiles_in_grid(grid_pattern[prev_position[3].x-1][prev_position[3].y], grid_pattern[prev_position[3].x][prev_position[3].y]);
                                used_pattern[a][b][c][d] = false;
                                }
                            }
                        else/** this move is done before, recover everything we changed **/
                            {
                            //cout << "This pattern has appeared before! (left)" << endl;
                            recover_tiles_position(prev_position);
                            //swap_tiles_in_grid(grid_pattern[prev_position[3].x-1][prev_position[3].y], grid_pattern[prev_position[3].x][prev_position[3].y]);
                            used_pattern[a][b][c][d] = false;
                            }
                        }
                    break;
                case 1:/** move UP **/
                    if(prev_position[3].y != 0)
                        {
                        //cout << "can move up" << endl;
                        //neighbor = grid_pattern[prev_position[3].x][prev_position[3].y-1];
                        //if(neighbor != ' ')/** we are swapping a named tile **/
                        /*    swap_tiles_position(tiles_position[neighbor-65], tiles_position[3]);
                        else
                            tiles_position[3].y = tiles_position[3].y - 1;

                        swap_tiles_in_grid(grid_pattern[prev_position[3].x][prev_position[3].y-1], grid_pattern[prev_position[3].x][prev_position[3].y]);
                        */
                        move_agent_block(tiles_position, 3);

                        a = tiles_position[0].x + tiles_position[0].y * 4;
                        b = tiles_position[1].x + tiles_position[1].y * 4;
                        c = tiles_position[2].x + tiles_position[2].y * 4;
                        d = tiles_position[3].x + tiles_position[3].y * 4;

                        if(used_pattern[a][b][c][d]==false)
                            {
                            used_pattern[a][b][c][d] = true;
                            //make_any_move_flag = true;
                            if(depth_first())
                                {
                                cout << "move up" << endl;
                                move_agent_block(goal_position, 1);
                                return true;
                                }
                            else/** this is a wrong branch, recover everything we changed **/
                                {
                                //cout << "Recover from move up!" << endl;
                                recover_tiles_position(prev_position);
                                //swap_tiles_in_grid(grid_pattern[prev_position[3].x][prev_position[3].y-1], grid_pattern[prev_position[3].x][prev_position[3].y]);
                                used_pattern[a][b][c][d] = false;
                                }
                            }
                        else/** this move is done before, recover everything we changed **/
                            {
                            //cout << "This pattern has appeared before! (up)" << endl;
                            recover_tiles_position(prev_position);
                            //swap_tiles_in_grid(grid_pattern[prev_position[3].x][prev_position[3].y-1], grid_pattern[prev_position[3].x][prev_position[3].y]);
                            used_pattern[a][b][c][d] = false;
                            }
                        }
                    break;
                case 2:/** move RIGHT **/
                    if(prev_position[3].x != 3)
                        {
                        //cout << "can move right" << endl;
                        //neighbor = grid_pattern[prev_position[3].x+1][prev_position[3].y];
                        //if(neighbor != ' ')/** we are swapping a named tile **/
                        /*    swap_tiles_position(tiles_position[neighbor-65], tiles_position[3]);
                        else
                            tiles_position[3].x = tiles_position[3].x + 1;
                        swap_tiles_in_grid(grid_pattern[prev_position[3].x+1][prev_position[3].y], grid_pattern[prev_position[3].x][prev_position[3].y]);
                        */
                        move_agent_block(tiles_position, 0);

                        a = tiles_position[0].x + tiles_position[0].y * 4;
                        b = tiles_position[1].x + tiles_position[1].y * 4;
                        c = tiles_position[2].x + tiles_position[2].y * 4;
                        d = tiles_position[3].x + tiles_position[3].y * 4;

                        if(used_pattern[a][b][c][d]==false)
                            {
                            used_pattern[a][b][c][d] = true;
                            //make_any_move_flag = true;
                            if(depth_first())
                                {
                                cout << "move right" << endl;
                                move_agent_block(goal_position, 2);
                                return true;
                                }
                            else/** this is a wrong branch, recover everything we changed **/
                                {
                                //cout << "Recover from move right!" << endl;
                                recover_tiles_position(prev_position);
                                //swap_tiles_in_grid(grid_pattern[prev_position[3].x+1][prev_position[3].y], grid_pattern[prev_position[3].x][prev_position[3].y]);
                                used_pattern[a][b][c][d] = false;
                                }
                            }
                        else/** this move is done before, recover everything we changed **/
                            {
                            //cout << "This pattern has appeared before! (right)" << endl;
                            recover_tiles_position(prev_position);
                            //swap_tiles_in_grid(grid_pattern[prev_position[3].x+1][prev_position[3].y], grid_pattern[prev_position[3].x][prev_position[3].y]);
                            used_pattern[a][b][c][d] = false;
                            }
                        }
                    break;
                case 3:/** move DOWN **/
                    if(prev_position[3].y != 3)
                        {
                        //cout << "can move down" << endl;
                        //neighbor = grid_pattern[prev_position[3].x][prev_position[3].y+1];
                        //if(neighbor != ' ')/** we are swapping a named tile **/
                        /*    swap_tiles_position(tiles_position[neighbor-65], tiles_position[3]);
                        else
                            tiles_position[3].y = tiles_position[3].y + 1;
                        swap_tiles_in_grid(grid_pattern[prev_position[3].x][prev_position[3].y+1], grid_pattern[prev_position[3].x][prev_position[3].y]);
                        */
                        move_agent_block(tiles_position, 1);

                        a = tiles_position[0].x + tiles_position[0].y * 4;
                        b = tiles_position[1].x + tiles_position[1].y * 4;
                        c = tiles_position[2].x + tiles_position[2].y * 4;
                        d = tiles_position[3].x + tiles_position[3].y * 4;

                        if(used_pattern[a][b][c][d]==false)
                            {
                            used_pattern[a][b][c][d] = true;
                            //make_any_move_flag = true;
                            if(depth_first())
                                {
                                cout << "move down" << endl;
                                move_agent_block(goal_position, 3);
                                return true;
                                }
                            else/** this is a wrong branch, recover everything we changed **/
                                {
                                //cout << "Recover from move down!" << endl;
                                recover_tiles_position(prev_position);
                                //swap_tiles_in_grid(grid_pattern[prev_position[3].x][prev_position[3].y+1], grid_pattern[prev_position[3].x][prev_position[3].y]);
                                }
                            }
                        else/** this move is done before, recover everything we changed **/
                            {
                            //cout << "This pattern has appeared before! (down)" << endl;
                            recover_tiles_position(prev_position);
                            //swap_tiles_in_grid(grid_pattern[prev_position[3].x][prev_position[3].y+1], grid_pattern[prev_position[3].x][prev_position[3].y]);
                            used_pattern[a][b][c][d] = false;
                            }
                        }
                    break;
                default:
                    cout << "ERRORRRRRRRRRRRRR" << endl;
                    break;
                }
            --i;
            ++temp;
            //cout << "i: " << i << " , ";
            //cout << "temp: " << temp << endl;
            }
        //if(make_any_move_flag == false)
            //{
            //cout << "NO move can be made" << endl;
            //return false;
            //}
        }
    //cout << k << endl;
    return false;
    }

void tile_moving::initialization()
    {
    debug = 0;

    vector<char> row;
    int i;
    int a, b, c, d;

    grid_pattern.push_back(row);
    grid_pattern.push_back(row);
    grid_pattern.push_back(row);
    grid_pattern.push_back(row);

    i = 0;
    while(i<=3)
        {
        grid_pattern[i].push_back(' ');
        grid_pattern[i].push_back(' ');
        grid_pattern[i].push_back(' ');
        grid_pattern[i].push_back(' ');
        ++i;
        }
    grid_pattern[0][3] = 'A';
    grid_pattern[1][3] = 'B';
    grid_pattern[2][3] = 'C';
    grid_pattern[3][3] = 'z';
    //cout << "After initialization" << endl;
    //cout << "Row: " << grid_pattern.size();
    //cout << "Col: " << grid_pattern[grid_pattern.size()-1].size();
    //output_grid_pattern();

    tiles_position[0].x = 0;
    tiles_position[0].y = 3;

    tiles_position[1].x = 1;
    tiles_position[1].y = 3;

    tiles_position[2].x = 2;
    tiles_position[2].y = 3;

    tiles_position[3].x = 3;
    tiles_position[3].y = 3;

    /** set goal position **/
    goal_position[0].x = 1;
    goal_position[0].y = 1;

    goal_position[1].x = 1;
    goal_position[1].y = 2;

    goal_position[2].x = 1;
    goal_position[2].y = 3;

    goal_position[3].x = 3;
    goal_position[3].y = 3;

    /*goal_position[0].x = 0;
    goal_position[0].y = 3;

    goal_position[1].x = 1;
    goal_position[1].y = 3;

    goal_position[2].x = 2;
    goal_position[2].y = 2;

    goal_position[3].x = 3;
    goal_position[3].y = 3;*/

    a = 15;
    while(a >= 0)
        {
        b = 15;
        while(b >= 0)
            {
            c = 15;
            while(c >= 0)
                {
                d = 15;
                while(d >= 0)
                    {
                    used_pattern[a][b][c][d] = false;
                    --d;
                    }
                --c;
                }
            --b;
            }
        --a;
        }
    used_pattern[12][13][14][15] = true;
    }

bool tile_moving::check_goal(axis now[4])
    {
    int i;
    i = 3;
    while(i >= 0)
        {
        //cout << "i: " << i << endl;
        //cout << "(" << now[i].x << ", " << now[i].y << ")" << ",  " << endl;
        //cout << "(" << goal_position[i].x << ", " << goal_position[i].y << ")" << ",  ";
        if((now[i].x!=goal_position[i].x) || (now[i].y!=goal_position[i].y))
            {
            //cout << "Conflict!" << endl;
            return false;
            }
        --i;
        }
    //cout << endl;
    return true;
    }

void tile_moving::move_agent_block(axis (&position_arr)[4], int direction)
    {
    //goal_position[3].x
    int i;
    axis next_posit;

    if(direction==0)/** move right **/
        {
        next_posit.x = position_arr[3].x + 1;
        next_posit.y = position_arr[3].y;
        }
    else if(direction==1)/** move down **/
        {
        next_posit.x = position_arr[3].x;
        next_posit.y = position_arr[3].y + 1;
        }
    else if(direction==2)/** move left **/
        {
        next_posit.x = position_arr[3].x - 1;
        next_posit.y = position_arr[3].y;
        }
    else/** move up **/
        {
        next_posit.x = position_arr[3].x;
        next_posit.y = position_arr[3].y - 1;
        }

    i = 2;
    while(i >= 0)
        {
        /** if the position of agent block is same as A or B or C **/
        /** These two blocks have to swap their axis **/
        if(next_posit.x==position_arr[i].x && next_posit.y==position_arr[i].y)
            {
            position_arr[i].x = position_arr[3].x;
            position_arr[i].y = position_arr[3].y;
            break;
            }
        --i;
        }
    position_arr[3].x = next_posit.x;
    position_arr[3].y = next_posit.y;
    return;
    }

void tile_moving::swap_tiles_in_grid(char &a, char &b)
    {
    char temp;
    temp = a;
    a = b;
    b = temp;
    return;
    }

void tile_moving::swap_tiles_position(axis &a, axis &b)
    {
    axis temp;
    temp.x = a.x;
    temp.y = a.y;
    a.x = b.x;
    a.y = b.y;
    b.x = temp.x;
    b.y = temp.y;
    return;
    }

void tile_moving::recover_tiles_position(axis record[4])
    {
    int i = 3;
    while(i>=0)
        {
        tiles_position[i].x = record[i].x;
        tiles_position[i].y = record[i].y;
        --i;
        }
    }

int main(int argc, char *argv[])
    {
    tile_moving obj;

    srand(time(NULL));

    obj.initialization();
    if(!obj.breadth_first())
        cout << "No Solution" << endl;
    else
        {
        obj.output_backtrace_result();
        cout << "Solution found" << endl;
        }
    return 0;
    }
