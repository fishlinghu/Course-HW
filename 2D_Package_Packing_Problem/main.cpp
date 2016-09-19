#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <cstring>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <queue>
#include <stack>
#include <climits>
#include <functional>
#include <locale>
#include <algorithm>
#include <time.h>

using namespace std;

class task
    {
    public:
        task();
        string test_name;
        string parent_core_name;
        int core_index;
        int length;
        int power;
        int start_time;
        int part;
        int start_bit;
        int end_bit;
        int tam_width;

        /** for bist **/
        bool bist;
        string rsc_name;

        /** for debug **/
        bool flag;
    };

task::task()
    {
    bist = false;
    rsc_name = "none";
    tam_width = 0;
    length = -1;
    start_time = -1;
    part = 1;
    power = -1;
    }

/***** core *****/

struct OrderByTime_SmallFirst
    {
    bool operator() (task const* a, task const* b) {return a->length > b->length;}
    };

struct OrderByBitxTime_SmallFirst
    {
    bool operator() (task const* a, task const* b) {return (a->length*a->tam_width) > (b->length*b->tam_width);}
    };

struct OrderByBit_SmallFirst
    {
    bool operator() (task const* a, task const* b) {return a->tam_width > b->tam_width;}
    };

struct OrderByPower_SmallFirst
    {
    bool operator() (task const* a, task const* b) {return a->power > b->power;}
    };

struct OrderByPowerxTime_SmallFirst
    {
    bool operator() (task const* a, task const* b) {return (a->length*a->power) > (b->length*b->power);}
    };

class core
    {
    public:
        core();
        string core_name;
        int core_index;
        int tam_width;
        int start_bit;
        int start_time;//debug purpose
        int total_ext_length;
        //ext list
        //priority_queue<task*, vector<task*>, OrderByTime1> ext_Stack;
        vector<task*> total_core_ext;
        //bist list
        //priority_queue<task*, vector<task*>, OrderByTime1> bist_Q;
        vector<task*> total_core_bist;
    };

core::core()
    {
    tam_width = -1;
    start_bit = -1;
    start_time = -1;
    total_ext_length = 0;
    }
/************************ precedence ************************/
class preced
    {
    public:
        string test_name;
        int time_constraint;
        int prev_count;
        vector<preced*> next;
    };

/************************ column and row ************************/
class row
    {
    public:
        row();
        int start_time;
        int bit_remain;
        int power_remain;
        int width_remain;
        int start_bit, end_bit;
    };

row::row()
    {
    start_time = -1;
    start_bit = -1;
    bit_remain = 0;
    width_remain = 0;
    power_remain = 0;
    }

class column
    {
    public:
        int start_time;
        int power_remain;
        int width;
        vector<row*> row_list;
        vector<column*> column_for_bist_list;
        map<string, string> used_rsc;
        map<int, int> used_core;
    };

/************************ main class ************************/
class ext_list
    {
    public:
        //priority_queue<task*, vector<task*>, OrderByTime1> ext_Stack;
        stack<task*> ext_Stack;
    };

class ext_sort_list
    {
    public:
        priority_queue<task*, vector<task*>, OrderByTime_SmallFirst> ext_sort_Q_time;
        priority_queue<task*, vector<task*>, OrderByBitxTime_SmallFirst> ext_sort_Q_bitxtime;
        priority_queue<task*, vector<task*>, OrderByBit_SmallFirst> ext_sort_Q_bit;
        priority_queue<task*, vector<task*>, OrderByPower_SmallFirst> ext_sort_Q_power;
        priority_queue<task*, vector<task*>, OrderByPowerxTime_SmallFirst> ext_sort_Q_powerxtime;
    };

struct OrderByTime3
    {
    bool operator() (core const* a, core const* b) {return a->total_ext_length < b->total_ext_length;}
    };

struct OrderByTAM
    {
    bool operator() (core const* a, core const* b) {return a->tam_width < b->tam_width;}
    };

class SoC_Scheduling
    {
    public:
        int min_scheduling_time;

        SoC_Scheduling();
        bool parser(const char* filename);
        void construct_precedence_ds();
        void construct_precedence_ds2();
        void construct_ext_ds();

        void construct_ext_ds_time();
        void construct_bist_ds_time();

        void construct_ext_ds_bitxtime();
        void construct_ext_ds_bit();

        void construct_ext_ds_power();
        void construct_bist_ds_power();

        void construct_ext_ds_powerxtime();
        void construct_bist_ds_powerxtime();

        void construct_core_list_copy();
        void tam_arrangement();
        void scheduling();
        void destruction();
        void save_solution();
        void output(const char* filename);

        void show_tam_arrangement();
        void show_input();
        void show_unused_task();
        void show_preced_constraint();

    private:
        void to_lower_case(string &in);
        void tam_loading_increment(int start, int width, int length, vector<int> &v);
        /**** core list ****/
        priority_queue<core*, vector<core*>, OrderByTime3> core_Q;
        priority_queue<core*, vector<core*>, OrderByTAM> core_Q_by_TAM;
        //priority_queue<core*, vector<core*>, OrderByTime4> core_Q_by_tam_bit;
        vector<core*> core_list;
        vector<core*> core_list_copy;

        /**** ext list ****/
        vector<ext_sort_list*> ext_sorting_head;
        vector<ext_list*> ext_list_head;

        /**** bist list ****/
        vector<task*> total_bist_list;
        stack<task*> total_bist_Stack;
        //priority_queue<task*, vector<task*>, OrderByTime1> total_bist_Stack;
        priority_queue<task*, vector<task*>, OrderByTime_SmallFirst> bist_time_sorting_Q;

        /**** precedence list ****/
        vector<vector<string> > precedence_list;
        map<string, preced*> preced_elements;

        //map<string, preced*> preced_first;
        //map<string, preced*> preced_other;

        //vector<preced*> orig_preced_first;
        //vector<preced*> orig_preced_other;

        /**** rsc list ****/
        vector<string> total_rsc_list;

        /**** for scheduling ****/
        vector<column*> col_list;

        int total_tam_width;
        int total_power;
        int total_time;
    };

SoC_Scheduling::SoC_Scheduling()
    {
    min_scheduling_time = INT_MAX;
    }

void SoC_Scheduling::show_preced_constraint()
    {
    map<string, preced*>::iterator it;
    for(it=preced_elements.begin();it!=preced_elements.end();++it)
        {
        cout << it->first << ": " << it->second->time_constraint << endl;
        }
    }

void SoC_Scheduling::show_unused_task()
    {
    int i;
    cout << "Unused ext test: " << endl;
    i = 0;
    while(i < total_tam_width)
        {
        while(ext_list_head[i]->ext_Stack.empty()!=true)
            {
            cout << ext_list_head[i]->ext_Stack.top()->test_name << ", ";
            cout << "prev_count: " << preced_elements[ext_list_head[i]->ext_Stack.top()->test_name]->prev_count
                 << ", time constraint: " << preced_elements[ext_list_head[i]->ext_Stack.top()->test_name]->time_constraint << endl;
            ext_list_head[i]->ext_Stack.pop();
            }
        ++i;
        }
    cout << endl << endl << "Unused BIST test: " << endl;
    while(total_bist_Stack.empty()!=true)
        {
        cout << total_bist_Stack.top()->test_name << ", ";
        total_bist_Stack.pop();
        }
    }

void SoC_Scheduling::show_tam_arrangement()
    {
    int i = core_list.size() - 1;
    while(i>=0)
        {
        cout << core_list[i]->core_name << ": " << endl
             << "(" << core_list[i]->start_bit << ", " << core_list[i]->start_bit + core_list[i]->tam_width - 1
             << "), total ext length: " << core_list[i]->total_ext_length
             << ", TAM width: " << core_list[i]->tam_width << endl << endl;
        --i;
        }
    }

void SoC_Scheduling::show_input() //bug, dont use this
    {
    core* temp;
    task* temp_task;
    int i, j;
    i = 0;
    cout << "Precedence: " << endl;
    while(i<precedence_list.size())
        {
        j = 0;
        while(j<precedence_list[i].size())
            {
            cout << precedence_list[i][j] << " > ";
            ++j;
            }
        cout << endl;
        ++i;
        }
    cout << endl;
    while(!core_Q.empty())
        {
        temp = core_Q.top();
        cout << endl << temp->core_name << endl;
        j = 0;
        while(j < temp->total_core_ext.size())
            {
            temp_task = temp->total_core_ext[j];
            cout << temp_task->test_name << " "
                 << temp_task->length << " "
                 << temp_task->power << " "
                 << temp_task->part << endl;
            ++j;
            }
        j = 0;
        while(j < temp->total_core_bist.size())
            {
            temp_task = temp->total_core_bist[j];
            cout << temp_task->test_name << " "
                 << temp_task->length << " "
                 << temp_task->power << " "
                 << temp_task->part << " "
                 << temp_task->rsc_name << endl;
            ++j;
            }
        core_Q.pop();
        }
    int total_bist_length = 0;
    i = 0;
    while(i < total_bist_list.size())
        {
        total_bist_length+=total_bist_list[i]->length;
        ++i;
        }
    cout << endl
         << "***************" << endl
         << endl;
    cout << "Num of BIST: " << total_bist_list.size() << endl;
    cout << "Total BIST length: " << total_bist_length << endl;
    }

void SoC_Scheduling::output(const char* filename)
    {
    int i, j;
    string out_filename = filename;
    out_filename.replace(out_filename.end()-4, out_filename.end(), "scheduling");
    ofstream fout;
    fout.open(out_filename.c_str());
    fout << "Schedule" << endl;
    fout << "begin" << endl;
    fout << "Test_time " << min_scheduling_time << endl;
    //cout << "Scheduling Time: " << min_scheduling_time << endl;
    i = 0;
    while(i<core_list_copy.size())
        {
        if(core_list_copy[i]->tam_width!=0)
            {
            fout << "TAM_assignment " << core_list_copy[i]->core_name
                 << " [" << core_list_copy[i]->start_bit+core_list_copy[i]->tam_width-1
                 << ":" << core_list_copy[i]->start_bit << "]" << endl;
            }
        ++i;
        }
    i = 0;
    while(i<core_list_copy.size())
        {
        j = 0;
        while(j<core_list_copy[i]->total_core_bist.size())
            {
            fout << "BIST " << core_list_copy[i]->core_name << " "
                 << core_list_copy[i]->total_core_bist[j]->test_name
                 << " (" << core_list_copy[i]->total_core_bist[j]->start_time << ", "
                 << core_list_copy[i]->total_core_bist[j]->start_time+core_list_copy[i]->total_core_bist[j]->length-1
                 << ")";
            //if(core_list_copy[i]->total_core_bist[j]->flag)
                //fout << " -Longest BIST in the column";
            fout << endl;
            ++j;
            }
        j = 0;
        while(j<core_list_copy[i]->total_core_ext.size())
            {
            fout << "External " << core_list_copy[i]->core_name << " "
                 << core_list_copy[i]->total_core_ext[j]->test_name
                 << " (" << core_list_copy[i]->total_core_ext[j]->start_time << ", "
                 << core_list_copy[i]->total_core_ext[j]->start_time+core_list_copy[i]->total_core_ext[j]->length-1
                 << ")";
            //if(core_list_copy[i]->total_core_ext[j]->flag)
                //fout << " -found an existing slot";
            fout << endl;
            ++j;
            }
        ++i;
        }
    fout << "end" << endl;
    fout.close();
    }

void SoC_Scheduling::save_solution()
    {
    int i, j;
    i = 0;
    while(i < core_list.size())
        {
        core_list_copy[i]->start_bit = core_list[i]->start_bit;
        j = 0;
        while(j < core_list[i]->total_core_ext.size())
            {
            core_list_copy[i]->total_core_ext[j]->start_time = core_list[i]->total_core_ext[j]->start_time;
            //core_list_copy[i]->total_core_ext[j]->flag = core_list[i]->total_core_ext[j]->flag;
            ++j;
            }
        j = 0;
        while(j < core_list[i]->total_core_bist.size())
            {
            core_list_copy[i]->total_core_bist[j]->start_time = core_list[i]->total_core_bist[j]->start_time;
            //core_list_copy[i]->total_core_bist[j]->flag = core_list[i]->total_core_bist[j]->flag;
            ++j;
            }
        ++i;
        }
    }

void SoC_Scheduling::scheduling()
    {
    bool break_loop_flag;
    bool column_width_is_BIST_width;
    bool violating_power_constraint;
    bool find_unfit_flag;
    bool precedence_flag;
    bool find_a_place;
    bool bist_find_a_place;
    int useless;
    int i, j, k, m, n;
    int power_constraint;
    int min_index, max_index, max_time;
    long long int min_time;
    row* row_temp;
    row* new_row_temp;
    column* column_temp;
    column* column_temp2;
    column* column_for_bist_temp;
    task* ext_temp;
    task* bist_temp;
    preced* preced_next;
    preced* preced_temp;
    stack<task*> ext_buffer_Stack;
    stack<task*> bist_buffer_Stack;
    vector<task*> ext_list_for_column;
    map<string, string>::iterator it;

    i = 0;
    power_constraint = total_power;

    while(true)
        {
        //cout << "i: " << i << endl;
        violating_power_constraint = false;
        if(!ext_list_head[i]->ext_Stack.empty())
            {
            find_unfit_flag = false;
            /**** traverse ext start from bit i until we find a unfit ext ****/
            /**** empty means that we put all the list of placeable ext into the schedule ****/
            //if(ext_list_head[i]->ext_Stack.top()->test_name=="test_58_3_4" || ext_list_head[i]->ext_Stack.top()->test_name=="test_81_3_1")
                //{
                //if(preced_elements[ext_list_head[i]->ext_Stack.top()->test_name]->prev_count==0)
                    //cout << "Find!" << endl;
                //}
            while(find_unfit_flag==false && ext_list_head[i]->ext_Stack.empty()==false)
                {
                //cout << "Inner loop" << endl;
                ext_temp = ext_list_head[i]->ext_Stack.top();
                //if(ext_temp->test_name=="test_58_3_4" || ext_temp->test_name=="test_81_3_1")
                    //cout << ext_temp->test_name << endl;
                /*cout << "test name: " << ext_list_head[i]->ext_Stack.top()->test_name << endl
                     << "core start bit: " << core_list[ext_list_head[i]->ext_Stack.top()->core_index]->start_bit << endl
                     << "test start bit: " << ext_list_head[i]->ext_Stack.top()->start_bit << endl
                     << "i: " << i << endl;*/
                //cin >> useless;
                if(preced_elements.count(ext_temp->test_name)==0 || preced_elements[ext_temp->test_name]->prev_count==0)
                    {
                    /** pass the precedence test **/
                    precedence_flag = true;
                    }
                else
                    {
                    /** fail the precedence test **/
                    /*if(ext_temp->test_name=="test_58_3_4" || ext_temp->test_name=="test_81_3_1")
                        {
                        cout << "Prev counter: " << preced_elements[ext_temp->test_name]->prev_count
                             << ", Time Constraint: " << preced_elements[ext_temp->test_name]->time_constraint << endl;
                        }*/
                    ext_buffer_Stack.push(ext_temp);
                    ext_list_head[i]->ext_Stack.pop();
                    find_unfit_flag = false;
                    precedence_flag = false;
                    }

                if(precedence_flag)
                    {
                    //if(ext_temp->test_name=="test_58_3_4" || ext_temp->test_name=="test_81_3_1")
                        //cout << "OK" << endl;
                    find_unfit_flag = true;/** If this flag remain true after traversing all the rows, the task needs to be put in the new column **/
                    find_a_place = false;/** this flag is used for the column-searching iteration below **/
                    /**** see if this ext can fit into a row, traverse the column ****/
                    j = 0;
                    while(j<col_list.size() && find_a_place==false)
                        {
                        k = 0;
                        while(k < col_list[j]->row_list.size())
                            {
                            row_temp = col_list[j]->row_list[k];
                            if(preced_elements.count(ext_temp->test_name)!=0 && (row_temp->start_time<preced_elements[ext_temp->test_name]->time_constraint||preced_elements[ext_temp->test_name]->time_constraint==-1))
                                {
                                /** violating the time constraint **/
                                ++k;
                                continue;
                                }
                            if(row_temp->start_bit<=ext_temp->start_bit && row_temp->end_bit>=ext_temp->end_bit && row_temp->width_remain>=ext_temp->length && row_temp->power_remain>=ext_temp->power)
                                {
                                /**** OK! The task can be placed in a existing row ****/
                                //if(ext_temp->test_name=="test_36_1_1" || ext_temp->test_name=="test_59_3_5")
                                    //cout << ext_temp->test_name << " placed in the existing row!" << endl;
                                //ext_temp->flag = true;

                                find_a_place = true;
                                find_unfit_flag = false;

                                ext_temp->start_time = row_temp->start_time;
                                /*cout << "Found a place in existing slot" << endl
                                     << "====" << ext_temp->test_name
                                     << ", core " << ext_temp->core_index + 1
                                     << ", from " << ext_temp->start_time << " to " << ext_temp->start_time + ext_temp->length - 1 << endl << endl;*/

                                /**** top ****/
                                if(row_temp->end_bit > ext_temp->end_bit)
                                    {
                                    new_row_temp = new row;
                                    new_row_temp->start_bit = ext_temp->end_bit + 1;
                                    new_row_temp->end_bit = row_temp->end_bit;
                                    new_row_temp->start_time = row_temp->start_time;
                                    new_row_temp->power_remain = row_temp->power_remain - ext_temp->power;
                                    new_row_temp->width_remain = ext_temp->length;
                                    col_list[j]->row_list.push_back(new_row_temp);
                                    }

                                /**** right ****/
                                row_temp->width_remain = row_temp->width_remain - ext_temp->length;
                                row_temp->start_time = row_temp->start_time + ext_temp->length;

                                /**** update the precedence chain ****/
                                if(preced_elements.count(ext_temp->test_name)>0)
                                    {
                                    preced_temp = preced_elements[ext_temp->test_name];
                                    m = preced_temp->next.size() - 1;
                                    while(m>=0)
                                        {
                                        preced_next = preced_temp->next[m];
                                        preced_next->prev_count--;
                                        if(preced_next->time_constraint < (ext_temp->start_time+ext_temp->length))
                                            preced_next->time_constraint = ext_temp->start_time + ext_temp->length;
                                        --m;
                                        }
                                    }
                                ext_list_head[i]->ext_Stack.pop();
                                break;
                                }
                            ++k;
                            }
                        ++j;
                        }
                    /**** finish traversing all the existing rows or already find a place to put the task ****/
                    if(find_unfit_flag==true)
                        {
                        /**** the ext task can't be placed in any of the existing row ****/
                        /**** will break the while loop after executing if ****/
                        power_constraint = power_constraint - ext_temp->power;
                        //cout << "power_constraint = " << power_constraint;
                        //cout << ext_temp->test_name << ", power: " << ext_temp->power << endl;
                        /**** still some space ****/
                        if(power_constraint>=0)
                            {
                            ext_list_for_column.push_back(ext_temp);
                            /*cout << ext_list_head[i]->ext_Stack.top()->test_name << endl;
                            cout << i << endl;*/
                            ext_list_head[i]->ext_Stack.pop();
                            /**** put back the constraint violating ext ****/
                            while(!ext_buffer_Stack.empty())
                                {
                                ext_list_head[i]->ext_Stack.push(ext_buffer_Stack.top());
                                ext_buffer_Stack.pop();
                                }
                            /**** tam bits are occupied, jump to the next available bit ****/
                            //cout << "i before: " << i << endl;
                            i = i + ext_temp->tam_width - 1;

                            /*if(i>total_tam_width)
                                {
                                cout << ext_temp->test_name << endl;
                                cout << "i after: " << i << endl;
                                cout << ext_temp->test_name
                                     << "'s width: " << ext_temp->tam_width
                                     << ", start bit: " << ext_temp->start_bit << endl;
                                }*/
                            }
                        /**** no enough space ****/
                        else
                            {
                            //cout << "power_constraint = " << power_constraint << endl;
                            //cout << ext_temp->test_name << ", power: " << ext_temp->power << endl;
                            //cout << "# of ext in the column = " << ext_list_for_column.size() << endl;
                            //if(ext_temp->test_name=="test_58_3_4" || ext_temp->test_name=="test_81_3_1")
                                //cout << "No Space" << endl;
                            ext_buffer_Stack.push(ext_temp);
                            violating_power_constraint = true;
                            power_constraint = total_power;
                            ext_list_head[i]->ext_Stack.pop();
                            }
                        }
                    //else
                        //{
                        //ext_list_head[i]->ext_Stack.pop();
                        //}
                    }
                }

            }
        //else
            //cout << "Bit " << i << " stack is empty!" << endl;
        while(ext_buffer_Stack.empty()!=true)
            {
            ext_list_head[i]->ext_Stack.push(ext_buffer_Stack.top());
            //cout << ext_buffer_Stack.top()->test_name << endl;
            //cout << i << endl;
            ext_buffer_Stack.pop();
            }
        ++i;
        /**** after a complete loop, no ext can be placed, end of ext placement ****/
        if(i==total_tam_width && ext_list_for_column.empty()==true)
            {
            break_loop_flag = true;
            n = total_tam_width - 1;
            while(n>=0)
                {
                if(ext_list_head[n]->ext_Stack.empty()!=true)
                    {
                    violating_power_constraint = false;
                    i = 0;
                    break_loop_flag = false;
                    break;
                    }
                --n;
                }
            if(break_loop_flag || total_bist_Stack.empty()!=true)
                break;
            }
        /**** construct the column ****/
        if(violating_power_constraint==true || i==total_tam_width)
            {
            column_width_is_BIST_width = false;
            power_constraint = total_power;
            min_index = -1;
            max_index = -1;
            min_time = INT_MAX;
            max_time = -1;
            column_temp = new column;
            column_temp->start_time = total_time;
            //if(total_time==-1)
                //cout << "&&&&&&&&&&&&&&&" << endl;
            //cout << "Constructing a column" << endl;
            /**** traverse the vector of ext ****/
            j = ext_list_for_column.size() - 1;

            while(j>=0)
                {
                /**** record the used core ****/
                column_temp->used_core[ext_list_for_column[j]->core_index] = ext_list_for_column[j]->core_index;
                /**** record the time for each ext ****/
                ext_list_for_column[j]->start_time = column_temp->start_time;
                //ext_list_for_column[j]->flag = false;
                //if(total_time==-1)
                    //cout << "!!!!!!!!!!!!!!!!!" << endl;
                /*cout << "====" << ext_list_for_column[j]->test_name
                     << ", core " << ext_list_for_column[j]->core_index + 1
                     << ", from " << ext_list_for_column[j]->start_time
                     << " to " << ext_list_for_column[j]->start_time + ext_list_for_column[j]->length - 1 << endl;*/
                /**** calculated the remaining power for the column ****/
                power_constraint = power_constraint - ext_list_for_column[j]->power;
                /**** record the longest and the shortest ext task ****/
                if(ext_list_for_column[j]->length > max_time)
                    {
                    max_index = j;
                    max_time = ext_list_for_column[j]->length;
                    }
                if(ext_list_for_column[j]->length < min_time)
                    {
                    min_index = j;
                    min_time = ext_list_for_column[j]->length;
                    }
                if(preced_elements.count(ext_list_for_column[j]->test_name)>0)
                    {
                    preced_temp = preced_elements[ext_list_for_column[j]->test_name];
                    m = preced_temp->next.size() - 1;
                    while(m>=0)
                        {
                        preced_next = preced_temp->next[m];
                        preced_next->prev_count--;
                        if(preced_next->time_constraint < (ext_list_for_column[j]->start_time+ext_list_for_column[j]->length))
                            preced_next->time_constraint = ext_list_for_column[j]->start_time + ext_list_for_column[j]->length;
                        --m;
                        }
                    }
                --j;
                }
            /**** insert BIST ****/
            /**** first try to insert a long BIST ****/
            while(total_bist_Stack.empty()!=true && total_bist_Stack.top()->length>max_time)
                {
                bist_temp = total_bist_Stack.top();
                /**** check the precedence constraint ****/
                if(preced_elements.count(bist_temp->test_name)!=0)
                    {
                    if(preced_elements[bist_temp->test_name]->prev_count!=0 || total_time<preced_elements[bist_temp->test_name]->time_constraint)
                        {
                        bist_buffer_Stack.push(bist_temp);
                        total_bist_Stack.pop();
                        continue;
                        }
                    }

                if(bist_temp->power<=power_constraint && column_temp->used_core.count(bist_temp->core_index)==0)
                    {
                    if(bist_temp->rsc_name=="none" || column_temp->used_rsc.count(bist_temp->rsc_name)==0)
                        {
                        /**** ok the BIST can be placed ****/
                        column_width_is_BIST_width = true;
                        //bist_temp->flag = true;
                        bist_temp->start_time = total_time;
                        //cout << bist_temp->test_name << ", start time: " << bist_temp->start_time << endl;
                        power_constraint = power_constraint - bist_temp->power;
                        //power_remain_back = power_constraint;
                        max_time = bist_temp->length;
                        column_temp->used_core[bist_temp->core_index] = bist_temp->core_index;
                        /*cout << "++++" << bist_temp->test_name
                             << ", core " << bist_temp->core_index + 1
                             << ", from " << bist_temp->start_time
                             << " to " << bist_temp->start_time + bist_temp->length - 1
                             << ", rsc: " << bist_temp->rsc_name << endl;
                        cout << "    ~used rsc are: ";*/
                        //for(it=column_temp->used_rsc.begin();it!=column_temp->used_rsc.end();++it)
                            //{
                            //cout << it->first << ", ";
                            //}
                        //cout << endl;
                        if(bist_temp->rsc_name!="none")
                            column_temp->used_rsc[bist_temp->rsc_name] = bist_temp->rsc_name;
                        if(preced_elements.count(bist_temp->test_name)>0)
                            {
                            preced_temp = preced_elements[bist_temp->test_name];
                            m = preced_temp->next.size() - 1;
                            while(m>=0)
                                {
                                preced_next = preced_temp->next[m];
                                preced_next->prev_count--;
                                if(preced_next->time_constraint < (bist_temp->start_time+bist_temp->length))
                                    preced_next->time_constraint = bist_temp->start_time + bist_temp->length;
                                --m;
                                }
                            }
                        total_bist_Stack.pop();
                        break;
                        }
                    else
                        {
                        /**** violating the constraints ****/
                        bist_buffer_Stack.push(bist_temp);
                        total_bist_Stack.pop();
                        }
                    }
                else
                    {
                    /**** violating the constraints ****/
                    bist_buffer_Stack.push(bist_temp);
                    total_bist_Stack.pop();
                    }
                }

            /**** put back the unused BIST ****/
            while(bist_buffer_Stack.empty()!=true)
                {
                total_bist_Stack.push(bist_buffer_Stack.top());
                bist_buffer_Stack.pop();
                }

            /**** construct the new column for bist ****/
            column_for_bist_temp = new column;
            column_for_bist_temp->start_time = column_temp->start_time;
            //cout << column_for_bist_temp->start_time << endl;
            column_for_bist_temp->used_core = column_temp->used_core;
            column_for_bist_temp->used_rsc = column_temp->used_rsc;
            column_for_bist_temp->width = min_time;
            column_for_bist_temp->power_remain = power_constraint;
            column_temp->column_for_bist_list.push_back(column_for_bist_temp);

            /**** try to insert any BIST test shorter than the shortest ext ****/
            while(total_bist_Stack.empty()!=true)
                {
                find_a_place = false;
                bist_temp = total_bist_Stack.top();
                j = 0;
                while(j < column_temp->column_for_bist_list.size())
                    {
                    column_temp2 = column_temp->column_for_bist_list[j];
                    if(bist_temp->length<=column_temp2->width)
                        {
                        if(preced_elements.count(bist_temp->test_name)!=0)
                            {
                            if(preced_elements[bist_temp->test_name]->prev_count!=0)
                                break;
                            if(column_temp2->start_time < preced_elements[bist_temp->test_name]->time_constraint)
                                {
                                ++j;
                                continue;
                                }
                            }
                        if(bist_temp->power<=column_temp2->power_remain && column_temp2->used_core.count(bist_temp->core_index)==0)
                            {
                            if(bist_temp->rsc_name=="none" || column_temp2->used_rsc.count(bist_temp->rsc_name)==0)
                                {
                                /**** ok the BIST can be placed ****/
                                find_a_place = true;
                                bist_find_a_place = true;
                                bist_temp->start_time = column_temp2->start_time;
                                /*cout << "++++" << bist_temp->test_name
                                     << ", core " << bist_temp->core_index + 1
                                     << ", from " << bist_temp->start_time
                                     << " to " << bist_temp->start_time + bist_temp->length - 1
                                     << ", rsc: " << bist_temp->rsc_name << endl;
                                cout << "    ~used rsc are: ";
                                for(it=column_temp2->used_rsc.begin();it!=column_temp2->used_rsc.end();++it)
                                    {
                                    cout << it->first << ", ";
                                    }
                                cout << endl;*/
                                /**** construct the new right column for bist ****/
                                column_for_bist_temp = new column;
                                column_for_bist_temp->start_time = column_temp2->start_time + bist_temp->length;
                                column_for_bist_temp->used_core = column_temp2->used_core;
                                column_for_bist_temp->used_rsc = column_temp2->used_rsc;
                                if(bist_temp->rsc_name!="none")
                                    column_for_bist_temp->used_rsc[bist_temp->rsc_name] = bist_temp->rsc_name;
                                column_for_bist_temp->width = column_temp2->width - bist_temp->length;
                                column_for_bist_temp->power_remain = column_temp2->power_remain;
                                column_temp->column_for_bist_list.push_back(column_for_bist_temp);

                                /**** old column adjustment ****/
                                column_temp2->width = bist_temp->length;
                                column_temp2->used_core[bist_temp->core_index] = bist_temp->core_index;
                                column_temp2->power_remain = column_temp2->power_remain - bist_temp->power;
                                if(bist_temp->rsc_name!="none")
                                    column_temp2->used_rsc[bist_temp->rsc_name] = bist_temp->rsc_name;

                                /**** precedence adjustment ****/
                                if(preced_elements.count(bist_temp->test_name)>0)
                                    {
                                    preced_temp = preced_elements[bist_temp->test_name];
                                    m = preced_temp->next.size() - 1;
                                    while(m>=0)
                                        {
                                        preced_next = preced_temp->next[m];
                                        preced_next->prev_count--;
                                        if(preced_next->time_constraint < (bist_temp->start_time+bist_temp->length))
                                            preced_next->time_constraint = bist_temp->start_time + bist_temp->length;
                                        --m;
                                        }
                                    }
                                break;
                                }
                            }
                        }
                    ++j;
                    }
                if(find_a_place==false)
                    bist_buffer_Stack.push(bist_temp);
                total_bist_Stack.pop();
                }
            /**** put back the unused BIST ****/
            while(bist_buffer_Stack.empty()!=true)
                {
                total_bist_Stack.push(bist_buffer_Stack.top());
                bist_buffer_Stack.pop();
                }
            /**** destruction of the bist columns ****/
            j = column_temp->column_for_bist_list.size() - 1;
            while(j>=0)
                {
                delete column_temp->column_for_bist_list[j];
                --j;
                }
            column_temp->column_for_bist_list.clear();
            /**** rows construction a LOT of BUG HERE ****/
            //cout << 1 << endl;
            if(column_width_is_BIST_width==true)//longest test is BIST
                {
                //cout << "Case BIST Longest: " << endl;
                j = ext_list_for_column.size();
                if(j!=0)
                    {
                    power_constraint = power_constraint / j;
                    }
                --j;
                while(j>=0)
                    {
                    //cout << "row " << j << ": ";
                    row_temp = new row;
                    if(j==0)
                        {
                        row_temp->start_bit = 0;
                        }
                    else
                        {
                        row_temp->start_bit = ext_list_for_column[j]->start_bit;
                        }

                    if(j==(ext_list_for_column.size()-1))
                        {
                        row_temp->end_bit = total_tam_width - 1;
                        }
                    else
                        {
                        row_temp->end_bit = ext_list_for_column[j+1]->start_bit - 1;
                        }
                    //cout << "Start bit " << row_temp->start_bit << ", End bit " << row_temp->end_bit << endl;
                    row_temp->start_time = total_time + ext_list_for_column[j]->length;
                    row_temp->width_remain = max_time - ext_list_for_column[j]->length;
                    row_temp->power_remain = ext_list_for_column[j]->power + power_constraint;
                    column_temp->row_list.push_back(row_temp);
                    --j;
                    }
                }
            else//longest test is external test
                {
                //cout << "Case Ext Longest: " << endl;
                j = ext_list_for_column.size() - 1;
                if(j!=0)
                    power_constraint = power_constraint / j;

                while(j>=0)
                    {
                    //cout << "row " << j << ": ";
                    row_temp = new row;
                    if(j==0)
                        {
                        row_temp->start_bit = 0;
                        }
                    else if(j==max_index+1)
                        {
                        row_temp->start_bit = ext_list_for_column[j-1]->end_bit + 1;
                        }
                    else
                        {
                        row_temp->start_bit = ext_list_for_column[j]->start_bit;
                        }

                    if(j==(ext_list_for_column.size()-1))
                        {
                        row_temp->end_bit = total_tam_width - 1;
                        }
                    else
                        {
                        row_temp->end_bit = ext_list_for_column[j+1]->start_bit - 1;
                        }
                    //cout << "Start bit " << row_temp->start_bit << ", End bit " << row_temp->end_bit << endl;
                    row_temp->width_remain = max_time - ext_list_for_column[j]->length;
                    if(row_temp->width_remain==0)
                        {
                        //cout << " --This is the longest ext!" << endl;
                        delete row_temp;
                        }
                    else
                        {
                        row_temp->start_time = total_time + ext_list_for_column[j]->length;
                        row_temp->power_remain = ext_list_for_column[j]->power + power_constraint;
                        column_temp->row_list.push_back(row_temp);
                        }
                    --j;
                    }
                }
            //cout << endl;
            //cout << 2 << endl;
            /**** initializing for the next round of insertion ****/
            //if(max_time==-1)
                //cout << "*******************" << endl;
            total_time = total_time + max_time;
            if(max_time==-1)
                cout << "###################" << endl;
            col_list.push_back(column_temp);
            //cout << endl;
            power_constraint = total_power;
            //if(power_constraint==-1)
                //cout << "FUUUUUUUUUUUCK" << endl;
            i = 0;
            ext_list_for_column.clear();
            }
        }
    /**** finish the rest ****/
    //show_unused_task();
    if(total_bist_Stack.empty()!=true)
        {
	column_temp = NULL;
        //cout << "Hi" << endl;
        power_constraint = total_power;
        while(true && total_bist_Stack.empty()!=true)
            {
            bist_temp = total_bist_Stack.top();
            if(preced_elements.count(bist_temp->test_name)!=0 && preced_elements[bist_temp->test_name]->prev_count!=0)
                {
                bist_buffer_Stack.push(bist_temp);
                total_bist_Stack.pop();
                }
            else
                {
                /**** ok the BIST can be placed ****/
                bist_temp->start_time = total_time;
                power_constraint = power_constraint - bist_temp->power;
                /**** modify the precedence list ****/
                if(preced_elements.count(bist_temp->test_name)>0)
                    {
                    preced_temp = preced_elements[bist_temp->test_name];
                    m = preced_temp->next.size() - 1;
                    while(m>=0)
                        {
                        preced_next = preced_temp->next[m];
                        preced_next->prev_count--;
                        if(preced_next->time_constraint < (bist_temp->start_time+bist_temp->length))
                            preced_next->time_constraint = bist_temp->start_time + bist_temp->length;
                        --m;
                        }
                    }
                /**** construct a new column ****/
                /*cout << "Constructing a column (back)" << endl;
                cout << "++++" << bist_temp->test_name
                     << ", core " << bist_temp->core_index + 1
                     << ", from " << bist_temp->start_time
                     << " to " << bist_temp->start_time + bist_temp->length - 1
                     << ", rsc: " << bist_temp->rsc_name << endl;*/
                column_temp = new column;
                column_temp->start_time = total_time;
                column_temp->width = bist_temp->length;
                column_temp->used_core[bist_temp->core_index] = bist_temp->core_index;
                column_temp->power_remain = power_constraint;
                if(bist_temp->rsc_name!="none")
                    column_temp->used_rsc[bist_temp->rsc_name] = bist_temp->rsc_name;

                total_time = total_time + bist_temp->length;
                total_bist_Stack.pop();
                break;
                }
            }
        while(total_bist_Stack.empty()!=true)
            {
            bist_temp = total_bist_Stack.top();
            if(preced_elements.count(bist_temp->test_name)!=0 && preced_elements[bist_temp->test_name]->prev_count!=0)
                {
                bist_buffer_Stack.push(bist_temp);
                total_bist_Stack.pop();
                }
            else if(column_temp->used_core.count(bist_temp->core_index)==0 && (bist_temp->rsc_name=="none" || column_temp->used_rsc.count(bist_temp->rsc_name)==0) && bist_temp->power<power_constraint)
                {
                /**** ok the BIST can be placed ****/
                bist_temp->start_time = column_temp->start_time;
                /*cout << "++++" << bist_temp->test_name
                     << ", core " << bist_temp->core_index + 1
                     << ", from " << bist_temp->start_time
                     << " to " << bist_temp->start_time + bist_temp->length - 1
                     << ", rsc: " << bist_temp->rsc_name << endl;
                cout << "    ~used rsc are: ";
                for(it=column_temp->used_rsc.begin();it!=column_temp->used_rsc.end();++it)
                    {
                    cout << it->first << ", ";
                    }
                cout << endl;*/
                /**** modify the precedence list ****/
                if(preced_elements.count(bist_temp->test_name)>0)
                    {
                    preced_temp = preced_elements[bist_temp->test_name];
                    m = preced_temp->next.size() - 1;
                    while(m>=0)
                        {
                        preced_next = preced_temp->next[m];
                        preced_next->prev_count--;
                        if(preced_next->time_constraint < (bist_temp->start_time+bist_temp->length))
                            preced_next->time_constraint = bist_temp->start_time + bist_temp->length;
                        --m;
                        }
                    }
                /**** modify the column ****/
                column_temp->used_core[bist_temp->core_index] = bist_temp->core_index;
                column_temp->power_remain = column_temp->power_remain - bist_temp->power;
                if(bist_temp->rsc_name!="none")
                    column_temp->used_rsc[bist_temp->rsc_name] = bist_temp->rsc_name;

                total_bist_Stack.pop();
                }
            else
                {
                bist_buffer_Stack.push(bist_temp);
                total_bist_Stack.pop();
                }
            }
        //cout << endl;
        /**** put back the unused bist ****/
        while(bist_buffer_Stack.empty()!=true)
            {
            total_bist_Stack.push(bist_buffer_Stack.top());
            bist_buffer_Stack.pop();
            }
        delete column_temp;
        //show_unused_task();
        scheduling();
        }
    }

void SoC_Scheduling::destruction()
    {
    int i, j;
    column* column_temp;
    //cout << total_time << endl;
    if(total_time < min_scheduling_time)
        {
        min_scheduling_time = total_time;
        save_solution();
        }

    i = col_list.size() - 1;
    while(i>=0)
        {
        column_temp = col_list[i];
        j = column_temp->row_list.size() - 1;
        while(j>=0)
            {
            delete column_temp->row_list[j];
            --j;
            }
        column_temp->row_list.clear();
        delete col_list[i];
        --i;
        }
    col_list.clear();
    }

void SoC_Scheduling::construct_ext_ds()
    {
    int i = total_tam_width;
    ext_list* temp;
    ext_sort_list* temp2;
    while(i>0)
        {
        temp = new ext_list;
        ext_list_head.push_back(temp);
        temp2 = new ext_sort_list;
        ext_sorting_head.push_back(temp2);
        --i;
        }
    }

void SoC_Scheduling::construct_ext_ds_time()
    {
    int i, j;
    i = core_list.size() - 1;
    while(i>=0)
        {
        j = core_list[i]->total_core_ext.size() - 1;
        //cout << core_list[i]->core_name << "'s start bit: " << core_list[i]->start_bit << endl;
        while(j>=0)
            {
            //core_list[i]->total_core_ext[j]->flag = false;
            core_list[i]->total_core_ext[j]->start_time = -1;
            core_list[i]->total_core_ext[j]->start_bit = core_list[i]->start_bit;
            core_list[i]->total_core_ext[j]->end_bit = core_list[i]->start_bit + core_list[i]->tam_width - 1;
            ext_sorting_head[core_list[i]->start_bit]->ext_sort_Q_time.push(core_list[i]->total_core_ext[j]);
            --j;
            }
        --i;
        }
    i = total_tam_width - 1;
    while(i>=0)
        {
        while(ext_sorting_head[i]->ext_sort_Q_time.empty()!=true)
            {
            ext_list_head[i]->ext_Stack.push(ext_sorting_head[i]->ext_sort_Q_time.top());
            ext_sorting_head[i]->ext_sort_Q_time.pop();
            }
        --i;
        }
    }

void SoC_Scheduling::construct_ext_ds_bitxtime()
    {
    int i, j;
    i = core_list.size() - 1;
    while(i>=0)
        {
        j = core_list[i]->total_core_ext.size() - 1;
        //cout << core_list[i]->core_name << "'s start bit: " << core_list[i]->start_bit << endl;
        while(j>=0)
            {
            //core_list[i]->total_core_ext[j]->flag = false;
            core_list[i]->total_core_ext[j]->start_time = -1;
            core_list[i]->total_core_ext[j]->start_bit = core_list[i]->start_bit;
            core_list[i]->total_core_ext[j]->end_bit = core_list[i]->start_bit + core_list[i]->tam_width - 1;
            ext_sorting_head[core_list[i]->start_bit]->ext_sort_Q_bitxtime.push(core_list[i]->total_core_ext[j]);
            --j;
            }
        --i;
        }
    i = total_tam_width - 1;
    while(i>=0)
        {
        while(ext_sorting_head[i]->ext_sort_Q_bitxtime.empty()!=true)
            {
            ext_list_head[i]->ext_Stack.push(ext_sorting_head[i]->ext_sort_Q_bitxtime.top());
            ext_sorting_head[i]->ext_sort_Q_bitxtime.pop();
            }
        --i;
        }
    }

void SoC_Scheduling::construct_bist_ds_time()
    {
    int i;
    i = total_bist_list.size() - 1;
    while(i>=0)
        {
        total_bist_list[i]->start_time = -1;
        bist_time_sorting_Q.push(total_bist_list[i]);
        --i;
        }
    while(bist_time_sorting_Q.empty()!=true)
        {
        total_bist_Stack.push(bist_time_sorting_Q.top());
        bist_time_sorting_Q.pop();
        }
    }

void SoC_Scheduling::construct_ext_ds_bit()
    {
    int i, j;
    i = core_list.size() - 1;
    while(i>=0)
        {
        j = core_list[i]->total_core_ext.size() - 1;
        //cout << core_list[i]->core_name << "'s start bit: " << core_list[i]->start_bit << endl;
        while(j>=0)
            {
            //core_list[i]->total_core_ext[j]->flag = false;
            core_list[i]->total_core_ext[j]->start_time = -1;
            core_list[i]->total_core_ext[j]->start_bit = core_list[i]->start_bit;
            core_list[i]->total_core_ext[j]->end_bit = core_list[i]->start_bit + core_list[i]->tam_width - 1;
            ext_sorting_head[core_list[i]->start_bit]->ext_sort_Q_bit.push(core_list[i]->total_core_ext[j]);
            --j;
            }
        --i;
        }
    i = total_tam_width - 1;
    while(i>=0)
        {
        while(ext_sorting_head[i]->ext_sort_Q_bit.empty()!=true)
            {
            ext_list_head[i]->ext_Stack.push(ext_sorting_head[i]->ext_sort_Q_bit.top());
            ext_sorting_head[i]->ext_sort_Q_bit.pop();
            }
        --i;
        }
    }

void SoC_Scheduling::construct_ext_ds_power()
    {
    int i, j;
    i = core_list.size() - 1;
    while(i>=0)
        {
        j = core_list[i]->total_core_ext.size() - 1;
        //cout << core_list[i]->core_name << "'s start bit: " << core_list[i]->start_bit << endl;
        while(j>=0)
            {
            //core_list[i]->total_core_ext[j]->flag = false;
            core_list[i]->total_core_ext[j]->start_time = -1;
            core_list[i]->total_core_ext[j]->start_bit = core_list[i]->start_bit;
            core_list[i]->total_core_ext[j]->end_bit = core_list[i]->start_bit + core_list[i]->tam_width - 1;
            ext_sorting_head[core_list[i]->start_bit]->ext_sort_Q_power.push(core_list[i]->total_core_ext[j]);
            --j;
            }
        --i;
        }
    i = total_tam_width - 1;
    while(i>=0)
        {
        while(ext_sorting_head[i]->ext_sort_Q_power.empty()!=true)
            {
            ext_list_head[i]->ext_Stack.push(ext_sorting_head[i]->ext_sort_Q_power.top());
            ext_sorting_head[i]->ext_sort_Q_power.pop();
            }
        --i;
        }
    }

void SoC_Scheduling::construct_ext_ds_powerxtime()
    {
    int i, j;
    i = core_list.size() - 1;
    while(i>=0)
        {
        j = core_list[i]->total_core_ext.size() - 1;
        //cout << core_list[i]->core_name << "'s start bit: " << core_list[i]->start_bit << endl;
        while(j>=0)
            {
            //core_list[i]->total_core_ext[j]->flag = false;
            core_list[i]->total_core_ext[j]->start_time = -1;
            core_list[i]->total_core_ext[j]->start_bit = core_list[i]->start_bit;
            core_list[i]->total_core_ext[j]->end_bit = core_list[i]->start_bit + core_list[i]->tam_width - 1;
            ext_sorting_head[core_list[i]->start_bit]->ext_sort_Q_powerxtime.push(core_list[i]->total_core_ext[j]);
            --j;
            }
        --i;
        }
    i = total_tam_width - 1;
    while(i>=0)
        {
        while(ext_sorting_head[i]->ext_sort_Q_powerxtime.empty()!=true)
            {
            ext_list_head[i]->ext_Stack.push(ext_sorting_head[i]->ext_sort_Q_powerxtime.top());
            ext_sorting_head[i]->ext_sort_Q_powerxtime.pop();
            }
        --i;
        }
    }

void SoC_Scheduling::construct_precedence_ds()
    {
    /**** construct the permanent string list of precedence ****/
    preced* temp;
    preced* prev;
    int i, j;
    i = 0;
    while(i < precedence_list.size())
        {
        /**** head task ****/
        if(preced_elements.count(precedence_list[i][0])==0)
            {
            /**** a new element ****/
            temp = new preced;
            temp->test_name = precedence_list[i][0];
            preced_elements[precedence_list[i][0]] = temp;
            }
        prev = preced_elements[precedence_list[i][0]];

        /**** following task ****/
        j = 1;
        while(j < precedence_list[i].size())
            {
            if(preced_elements.count(precedence_list[i][j])==0)
                {
                /**** a new element ****/
                temp = new preced;
                temp->test_name = precedence_list[i][j];
                preced_elements[precedence_list[i][j]] = temp;
                }
            prev->next.push_back(preced_elements[precedence_list[i][j]]);
            prev = preced_elements[precedence_list[i][j]];
            ++j;
            }
        ++i;
        }
    }

void SoC_Scheduling::construct_precedence_ds2()
    {
    total_time = 0;
    int i;
    map<string, preced*>::iterator it;
    preced* temp;
    /**** initialization ****/
    for(it=preced_elements.begin();it!=preced_elements.end();++it)
        {
        temp = it->second;
        temp->prev_count = 0;
        temp->time_constraint = -1;
        }
    /**** update the prev_count ****/
    for(it=preced_elements.begin();it!=preced_elements.end();++it)
        {
        temp = it->second;
        i = temp->next.size() - 1;
        while(i>=0)
            {
            temp->next[i]->prev_count++;
            --i;
            }
        }
    /**** update the time_constraint ****/
    for(it=preced_elements.begin();it!=preced_elements.end();++it)
        {
        temp = it->second;
        if(temp->prev_count==0)
            {
            temp->time_constraint = 0;
            }
        }
    }

void SoC_Scheduling::tam_arrangement()
    {
    int i, j, k;
    int space, num_of_rest, index, max_loading, temp_max_loading;
    queue<core*> core_temp_Q;
    vector<int> tam_bit_loading(total_tam_width, 0);
    vector<core*> first_part;
    vector<core*> second_part;
    core* core_temp;
    /**** high block arrangement ****/
    core_temp = core_Q_by_TAM.top();
    core_temp_Q.push(core_temp);
    core_Q_by_TAM.pop();
    core_temp->start_bit = 0;
    tam_loading_increment(0, core_temp->tam_width, core_temp->total_ext_length, tam_bit_loading);
    space = total_tam_width - core_temp->tam_width;
    core_temp = core_Q_by_TAM.top();
    while(core_temp->tam_width >= space)
        {
        core_temp_Q.push(core_temp);
        core_Q_by_TAM.pop();
        core_temp->start_bit = 0;
        tam_loading_increment(0, core_temp->tam_width, core_temp->total_ext_length, tam_bit_loading);
        space = total_tam_width - core_temp->tam_width;
        core_temp = core_Q_by_TAM.top();
        }

    /**** low block arrangement ****/
    while(core_Q_by_TAM.empty()!=true && core_Q_by_TAM.top()->tam_width>total_tam_width/11)
        {
        first_part.push_back(core_Q_by_TAM.top());
        core_temp_Q.push(core_Q_by_TAM.top());
        core_Q_by_TAM.pop();
        }
    while(core_Q_by_TAM.empty()!=true && core_Q_by_TAM.top()->tam_width>0)
        {
        second_part.push_back(core_Q_by_TAM.top());
        core_temp_Q.push(core_Q_by_TAM.top());
        core_Q_by_TAM.pop();
        }
    num_of_rest = first_part.size();
    while(num_of_rest!=0)
        {
        /**** randomly pick a core and take the last element to front ****/
        index = rand()%num_of_rest;
        core_temp = first_part[index];
        first_part[index] = first_part[num_of_rest-1];

        /**** first frame, put the core just under the roof ****/
        i = core_temp->tam_width;
        max_loading = 0;
        while(i>0)
            {
            if(tam_bit_loading[total_tam_width-i] > max_loading)
                max_loading = tam_bit_loading[total_tam_width-i];
            --i;
            }
        core_temp->start_bit = total_tam_width - core_temp->tam_width;

        /**** find a best frame(minimum max loading) for the core ****/
        /**** traverse the TAM bit ****/
        i = total_tam_width - core_temp->tam_width - 1;
        while(i>=0)
            {
            /**** find max loading of the frame ****/
            temp_max_loading = 0;
            k = i;
            /**** traverse the frame ****/
            while(k < i+core_temp->tam_width)
                {
                if(tam_bit_loading[k] > temp_max_loading)
                    temp_max_loading = tam_bit_loading[k];
                ++k;
                }
            if(temp_max_loading < max_loading)
                {
                max_loading = temp_max_loading;
                core_temp->start_bit = i;
                }
            --i;
            }
        /**** frame decided, update the TAM loading vector ****/
        tam_loading_increment(core_temp->start_bit, core_temp->tam_width, core_temp->total_ext_length, tam_bit_loading);
        --num_of_rest;
        }
    /**** do the same thing on second part (totally same code) ****/
    num_of_rest = second_part.size();
    while(num_of_rest!=0)
        {
        /**** take the last element to front ****/
        index = rand()%num_of_rest;
        core_temp = second_part[index];
        second_part[index] = second_part[num_of_rest-1];

        /**** first frame, put the core just under the roof ****/
        i = core_temp->tam_width;
        max_loading = 0;
        while(i>0)
            {
            if(tam_bit_loading[total_tam_width-i] > max_loading)
                max_loading = tam_bit_loading[total_tam_width-i];
            --i;
            }
        core_temp->start_bit = total_tam_width - core_temp->tam_width;

        /**** find a best frame(minimum max loading) for the core ****/
        /**** traverse the TAM bit ****/
        i = total_tam_width - core_temp->tam_width - 1;
        while(i>=0)
            {
            /**** find max loading of the frame ****/
            temp_max_loading = 0;
            k = i;
            /**** traverse the frame ****/
            while(k<i+core_temp->tam_width)
                {
                if(tam_bit_loading[k] > temp_max_loading)
                    temp_max_loading = tam_bit_loading[k];
                ++k;
                }
            if(temp_max_loading < max_loading)
                {
                max_loading = temp_max_loading;
                core_temp->start_bit = i;
                }
            --i;
            }
        /**** frame decided, update the TAM loading vector ****/
        tam_loading_increment(core_temp->start_bit, core_temp->tam_width, core_temp->total_ext_length, tam_bit_loading);
        --num_of_rest;
        }
    tam_bit_loading.clear();
    first_part.clear();
    second_part.clear();
    while(!core_temp_Q.empty())
        {
        core_Q_by_TAM.push(core_temp_Q.front());
        core_temp_Q.pop();
        }
    }

void SoC_Scheduling::construct_core_list_copy()
    {
    int i, j;
    core* core_temp;
    task* task_temp;
    i = 0;
    while(i < core_list.size())
        {
        core_temp = new core;
        core_temp->core_name = core_list[i]->core_name;
        core_temp->tam_width = core_list[i]->tam_width;
        j = 0;
        /**** copy the ext task vector ****/
        while(j < core_list[i]->total_core_ext.size())
            {
            task_temp = new task;
            task_temp->test_name = core_list[i]->total_core_ext[j]->test_name;
            task_temp->length = core_list[i]->total_core_ext[j]->length;
            core_temp->total_core_ext.push_back(task_temp);
            ++j;
            }
        /**** copy the bist vector ****/
        j = 0;
        while(j < core_list[i]->total_core_bist.size())
            {
            task_temp = new task;
            task_temp->test_name = core_list[i]->total_core_bist[j]->test_name;
            task_temp->length = core_list[i]->total_core_bist[j]->length;
            core_temp->total_core_bist.push_back(task_temp);
            ++j;
            }
        core_list_copy.push_back(core_temp);
        ++i;
        }
    }

bool SoC_Scheduling::parser(const char* filename)
    {
    int flag = 0;//-1 = bist;0 = nothing;1 = ext
    int core_counter = 0;
    string temp = "";
    ifstream fin;
    fin.open(filename);
    vector<string> temp_preced;
    //precedence* temp_preced;
    core* temp_core;
    task* temp_task;
    fin >> temp;
    while(fin.peek()!=EOF)
        {
        if(temp[0]=='#')
            {
	    getline(fin, temp);
	    fin >> temp;
	    }
        else
            {
            to_lower_case(temp);
            if(temp=="system")
                {
                //cout << temp << endl;
                while(temp!="end")
                    {
                    //cout << "Sys" << endl;
                    if(temp[0]=='#')
                        getline(fin, temp);
                    else
                        {
                        to_lower_case(temp);
                        if(temp=="tam_width")
                            {
                            fin >> total_tam_width;
                            //cout << "tam width: " << total_tam_width << endl;
                            fin >> temp;
                            }
                        else if(temp=="power")
                            {
                            fin >> total_power;
                            //cout << "power: " << total_power << endl;
                            fin >> temp;
                            }
                        else if(temp=="precedence")
                            {
                            fin >> temp;
                            temp_preced.push_back(temp);
                            while(fin.peek()!=10)
                                {
                                fin >> temp;// '<_ '
                                fin >> temp;
                                temp_preced.push_back(temp);
                                }
                            precedence_list.push_back(temp_preced);
                            temp_preced.clear();
                            }
                        else if(temp=="resource")
                            {
                            //cout << "resource: ";
                            fin >> temp;
                            while(fin.peek()!=10)
                                {
                                total_rsc_list.push_back(temp);
                                //cout << temp << " ";
                                fin >> temp;
                                }
                            //cout << endl;
                            }
                        else
                            fin >> temp;
                        }
                    to_lower_case(temp);
                    }
                }
            else if(temp=="core")
                {
                temp_core = new core;
                fin >> temp_core->core_name;
                temp_core->core_index = core_counter;
                while(temp!="end")
                    {
                    //cout << "core: " << temp << endl;
                    if(temp[0]=='#')
                        getline(fin, temp);
                    else
                        {
                        to_lower_case(temp);
                        if(temp=="tam_width")
                            {
                            fin >> temp_core->tam_width;
                            fin >> temp;
                            }
                        else if(temp=="external")
                            {
                            temp_task = new task;
                            temp_task->core_index = core_counter;
                            temp_task->tam_width = temp_core->tam_width;
                            fin >> temp_task->test_name;

                            //cout << temp_ext->test_name << endl;
                            fin >> temp;
                            to_lower_case(temp);
                            while(temp!="external" && temp!="bist" && temp!="end")
                                {
                                if(temp=="length")
                                    {
                                    fin >> temp_task->length;
                                    temp_core->total_ext_length = temp_core->total_ext_length + temp_task->length;
                                    }
                                else if(temp=="power")
                                    {
                                    fin >> temp_task->power;
                                    }
                                else if(temp=="partition")
                                    {
                                    fin >> temp_task->part;
                                    }
                                fin >> temp;
                                to_lower_case(temp);
                                }
                            //finish ext construction
                            //temp_core->ext_Stack.push(temp_task);
                            temp_core->total_core_ext.push_back(temp_task);
                            }
                        else if(temp=="bist")
                            {
                            temp_task = new task;
                            temp_task->bist = true;
                            temp_task->core_index = core_counter;
                            fin >> temp_task->test_name;

                            fin >> temp;
                            to_lower_case(temp);
                            while(temp!="external" && temp!="bist" && temp!="end")
                                {
                                if(temp=="length")
                                    {
                                    fin >> temp_task->length;
                                    }
                                else if(temp=="power")
                                    {
                                    fin >> temp_task->power;
                                    }
                                else if(temp=="partition")
                                    {
                                    fin >> temp_task->part;
                                    }
                                else if(temp=="resource")
                                    {
                                    fin >> temp_task->rsc_name;
                                    }
                                fin >> temp;
                                to_lower_case(temp);
                                }
                            //finish bist construction
                            temp_core->total_core_bist.push_back(temp_task);
                            total_bist_list.push_back(temp_task);
                            //total_bist_Stack.push(temp_task);
                            }
                        else
                            fin >> temp;
                        }
                    to_lower_case(temp);
                    }
                //finish core construction
                core_list.push_back(temp_core);
                ++core_counter;
                //core_Q.push(temp_core);
                core_Q_by_TAM.push(temp_core);
                }
            else
                fin >> temp;
            }
        }
    fin.close();
    return true;
    }

/****** small functions ******/

void SoC_Scheduling::to_lower_case(string &in)
    {
    int i = in.size() - 1;
    while(i>=0)
        {
        in[i] = tolower(in[i]);
        --i;
        }
    return;
    }

void SoC_Scheduling::tam_loading_increment(int start, int width, int length, vector<int> &v)
    {
    while(width>0)
        {
        v[start] = v[start] + length;
        ++start;
        --width;
        }
    }

int main(int argc, char* argv[])
    {
    int i, temp;
    SoC_Scheduling obj;

    srand((unsigned)time(NULL));

    if(obj.parser(argv[1]))
        {
        //obj.show_input();
        obj.construct_core_list_copy();
        obj.construct_precedence_ds();
        obj.construct_ext_ds();
        //obj.show_input();
        i = 0;
        while(i<1000)
            {
	//cout << i << " ";
            obj.construct_precedence_ds2();
            obj.tam_arrangement();
            //obj.show_tam_arrangement();
            obj.construct_ext_ds_time();
            //obj.construct_ext_ds_bitxtime();
            //obj.construct_ext_ds_bit();
            //obj.construct_ext_ds_powerxtime();
            //obj.construct_ext_ds_power();
            obj.construct_bist_ds_time();
            //cout << "HI" << endl;
            //cout << i << endl;
            obj.scheduling();
            //cout << i << endl;
            //obj.show_preced_constraint();
            //obj.show_unused_task();
            //cin >> temp;
            obj.destruction();
            ++i;
            }
        i = 0;
        while(i<1000)
            {
            obj.construct_precedence_ds2();
            obj.tam_arrangement();
            obj.construct_ext_ds_bitxtime();
            obj.construct_bist_ds_time();
            obj.scheduling();
            obj.destruction();
            ++i;
            }
        obj.output(argv[1]);
        }
    return 0;
    }
