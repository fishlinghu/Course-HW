#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <queue>
#include <stdio.h>
#include <string>
#include <math.h>
#include <iomanip>
#include <map>

using namespace std;

class x_and_v
    {
    public:
        x_and_v();
        bool choose;
        int level;
        int from_clause;
        int x;
        int v;
    };

x_and_v::x_and_v()
    {
    choose = false;
    }

class variable
    {
    public:
        int level;
        int var_name, clause_name;
        int complement;//-1=inverse
        int value;//-1=false
        bool watched;//false=not watched, true=watched
        variable(int name1, int name2, int cmp);
    };

variable::variable(int name1, int name2, int cmp)
    {
    var_name = name1;
    clause_name = name2;
    complement = cmp;
    value = 2;
    }

class var_index
    {
    public:
        var_index();
        bool assigned;
        int value;
        int level;
        int from_clause;
        int total_num;
        int pos_num;
        int neg_num;
        vector<variable*> same_var;
    };

var_index::var_index()
    {
    assigned = false;
    value = 0;
    level = 0;
    from_clause = -1;
    total_num = 0;
    pos_num = 0;
    neg_num = 0;
    }

class clause_index
    {
    public:
        vector<variable*> same_clause;
        map<int, variable*> unwatched_list;
        map<int, variable*> watched_list;
        map<int, variable*> solved_list;
        bool solve_or_not;//0=unsolved, 1=solved
        clause_index();
    };

clause_index::clause_index()
    {
    solve_or_not = false;
    }

class SAT_solver
    {
    public:
        SAT_solver();
        //~SAT_solver();
        void first_UIP(int x, int v, int clause_name, int level);
        void parser(const char* filename);
        void input_data();
        void initialize_watched_list();
        bool branching_heuristic(int level);
        bool DPLL(int level);
        void output(const char* filename, bool sat);

        map<int, x_and_v> value_to_be_assigned;

    private:
        vector<vector<x_and_v> > this_level_assignments;
        vector<var_index*> column;
        vector<clause_index*> row;
        int NCB_level;
        int next_level;
        int numofvar, numofclause;
        bool add_ncb_assignment;
        bool solved_test();
        bool set_variable_value(int level);
        x_and_v ncb_assignment;
        void backtrace(vector<x_and_v>& assignments_of_BCP);


    };

SAT_solver::SAT_solver()
    {
    NCB_level = -1;
    next_level = 1;
    }

void SAT_solver::output(const char* filename, bool sat)
    {
    int i = 1;
    string out_filename = filename;
    out_filename.replace(out_filename.end()-3, out_filename.end(), "sat");
    ofstream fout;
    fout.open(out_filename.c_str());
    if(sat)
        {
        fout << "s SATISFIABLE" << endl << "v ";
        while(i<=numofvar)
            {
            if(column[i]->same_var[0]->value < 0)
                fout << "-" << i << " ";
            else
                fout << i << " ";
            ++i;
            }
        fout << 0 << endl;
        }
    else
        {
        fout << "s UNSATISFIABLE" << endl;
        }
    fout.close();
    }

bool SAT_solver::solved_test()
    {
    int i;
    i = numofclause - 1;
    while(i>=0)
        {
        if(row[i]->solved_list.size()==0)
            return false;
        else
            --i;
        }
    return true;
    }

bool SAT_solver::branching_heuristic(int level)
    {
    x_and_v temp;
    int i, max_num = -1, max_num_index, flag = 0;
    i = numofvar;
    while(i>0)//branching heuristic
        {
        if(!column[i]->assigned)//choose from the unassigned variables
            {
            //cout << "X" << i << " is _" << endl;
            if(column[i]->total_num > max_num)
                {
                ++flag;
                max_num_index = i;
                max_num = column[i]->total_num;
                }
            }
        //else
            //cout << "X" << i << " is V" << endl;
        --i;
        }
    if(flag==0)
        {
        //cout << "nothing to choose from" << endl;
        return false;
        }
    temp.choose = true;
    temp.x = max_num_index;
    if(column[max_num_index]->pos_num > column[max_num_index]->neg_num)
        temp.v = 1;
    else
        temp.v = -1;
    temp.level = level;
    temp.from_clause = -1;//not a BCP assignment
    if(value_to_be_assigned.count(temp.x)==0)
        value_to_be_assigned[temp.x] = temp;
    /*******heuristically make an assignment*******/
    //cout << endl << "choose X" << temp.x << " = " << temp.v << endl;
    return true;
    }

bool SAT_solver::set_variable_value(int level)
    {
    x_and_v temp;
    int x, v;
    int i;
    int j;
    variable* present;
    variable* clause_variable;
    x = value_to_be_assigned.begin()->second.x;
    v = value_to_be_assigned.begin()->second.v;
    column[x]->from_clause = value_to_be_assigned.begin()->second.from_clause;
    column[x]->level = level;
    column[x]->assigned = true;
    column[x]->value = v;
    //cout << "X" << x << " = " << v << "(" << column[x]->from_clause << "), ";
    i = column[x]->same_var.size() - 1;
    while(i>=0)
        {
        present = column[x]->same_var[i];
        present->level = level;
        present->value = v;
        if(row[present->clause_name]->unwatched_list.count(present->var_name)!=0)
            row[present->clause_name]->unwatched_list.erase(present->var_name);

        /***the clause is still unsolved***/
        if(row[present->clause_name]->solved_list.empty())
            {
            //cout << "clause: " << present->clause_name << ", X" << present->var_name << " = " << v << endl;
            /***the clause is solved, dynamic adjust branching heuristic parameters***/
            if(v * present->complement == 1)
                {
                //cout << "clause " << present->clause_name << " is solved!" << endl;
                row[present->clause_name]->solved_list[x] = present;
                //cout << row[present->clause_name].solved_list[x]->var_name << endl;
                j = row[present->clause_name]->same_clause.size() - 1;
                while(j>=0)
                    {
                    clause_variable = row[present->clause_name]->same_clause[j];
                    if(clause_variable->complement==-1)
                        {
                        --column[clause_variable->var_name]->neg_num;
                        }
                    else
                        {
                        --column[clause_variable->var_name]->pos_num;
                        }
                    --column[clause_variable->var_name]->total_num;
                    --j;
                    }
                }
            /***after the assignment, the clause is still unsolved, and the assignment is on the watched variable***/
            else if(present->watched)
                {
                /***unwatched_list is not NULL, so just change the watched variable***/
                if(!row[present->clause_name]->unwatched_list.empty())
                    {
                    row[present->clause_name]->watched_list[row[present->clause_name]->unwatched_list.begin()->second->var_name] = row[present->clause_name]->unwatched_list.begin()->second;
                    //cout << row[present->clause_name]->unwatched_list[0]->var_name << endl;
                    row[present->clause_name]->unwatched_list.begin()->second->watched = true;

                    //cout << row[present->clause_name]->unwatched_list[0]->var_name << endl;
                    present->watched = false;
                    row[present->clause_name]->watched_list.erase(present->var_name);
                    //row[present->clause_name]->unwatched_list.push_back(present);
                    }
                /***there's no any unwatched variable***/
                else
                    {
                    /***BCP!! one of watched variables is unassigned, an unit clause***/
                    if(row[present->clause_name]->watched_list.begin()->second->value==2 && row[present->clause_name]->watched_list.rbegin()->second->value!=2)
                        {
                        /*cout << "in clause " << present->clause_name << endl;
                        cout << "X" << row[present->clause_name]->watched_list.begin()->second->var_name << " is unassigned" << endl;
                        cout << "Unit clause" << endl;*/
                        temp.level = level;
                        temp.from_clause = present->clause_name;
                        temp.v = row[present->clause_name]->watched_list.begin()->second->complement;
                        temp.x = row[present->clause_name]->watched_list.begin()->second->var_name;
                        //cout << "BCP->X" << temp.x << " = " << temp.v << endl;
                        /***the assignment already exist***/
                        if(value_to_be_assigned.count(temp.x)>0)
                            {
                            /***Conflict!! Different from previous assignment***/
                            if(value_to_be_assigned[temp.x].v!=temp.v)
                                {
                                //cout << "conflict11----clause " << present->clause_name << endl;
                                first_UIP(temp.x, temp.v, present->clause_name, level);
                                value_to_be_assigned.erase(x);
                                return false;
                                }
                            }
                        /***Totally new assignment, save it***/
                        else
                            {
                            //cout << "---BCP assignment: X" << temp.x << " = " << temp.v << endl;
                            value_to_be_assigned[temp.x]=temp;
                            }
                        }
                    /***BCP!! the other watched variables is unassigned, an unit clause, same structure as above***/
                    else if(row[present->clause_name]->watched_list.rbegin()->second->value==2 && row[present->clause_name]->watched_list.begin()->second->value!=2)
                        {
                        /*cout << "in clause " << present->clause_name << endl;
                        cout << "X" << row[present->clause_name]->watched_list.rbegin()->second->var_name << " is unassigned" << endl;
                        cout << "Unit clause" << endl;*/
                        temp.level = level;
                        temp.from_clause = present->clause_name;
                        temp.v = row[present->clause_name]->watched_list.rbegin()->second->complement;
                        temp.x = row[present->clause_name]->watched_list.rbegin()->second->var_name;
                        //cout << "BCP->X" << temp.x << " = " << temp.v << endl;
                        if(value_to_be_assigned.count(temp.x)>0)//the assignment already exist
                            {
                            if(value_to_be_assigned[temp.x].v!=temp.v)
                                {
                                //cout << "conflict12----clause " << present->clause_name << endl;
                                first_UIP(temp.x, temp.v, present->clause_name, level);
                                value_to_be_assigned.erase(x);
                                return false;
                                }
                            }
                        else//new assignment, save it
                            {
                            //cout << "---BCP assignment: X" << temp.x << " = " << temp.v << endl;
                            value_to_be_assigned[temp.x]=temp;
                            }
                        }
                    ///Conflict!! none of the two watched literals' is solved or unassigned
                    else if(row[present->clause_name]->watched_list.rbegin()->second->value!=2 && row[present->clause_name]->watched_list.begin()->second->value!=2)//conflict
                        {
                        //out << temp.x << endl;
                        //cout << temp.v << endl;
                        //cout << "conflict2----clause " << present->clause_name << endl;
                        //first_UIP(-5, -5, present->clause_name, level);
                        value_to_be_assigned.erase(x);

                        return false;
                        }
                    }
                }
            }
        else
            {
            if(v * present->complement == 1)
                row[present->clause_name]->solved_list[x] = present;
            }
        //cout << x << " set to " << v << endl;
        --i;
        }
    //cout << x << endl;
    //cout << "=====" << value_to_be_assigned.begin()->first << endl;
    value_to_be_assigned.erase(x);
    //cout << value_to_be_assigned.size() << endl;
    return true;
    }

void SAT_solver::first_UIP(int x, int v, int clause_name, int level)
    {
    bool flag;
    int i;
    int temp;
    int other_clause;
    int second_level;
    clause_index* clause_temp;
    variable* var_temp;
    variable* second_level_var;
    variable* first_level_var;
    map<int, variable*> new_clause;
    map<int, variable*>::iterator it;
    queue<int> same_level;
    queue<int> temp_Q;
    flag = false;
    //cout << 1 << endl;
    if(value_to_be_assigned.count(x)>0)
        {
        //cout << 1 << endl;
        //cout << "x " << x << endl;
        other_clause = value_to_be_assigned[x].from_clause;
        if(other_clause!=-1)
        {
        flag = true;
        //cout << other_clause << endl;
        i = row[other_clause]->same_clause.size() - 1;
        //cout << 2 << endl;

        while(i >= 0)
            {
            if(row[other_clause]->same_clause[i]->var_name != x)
                {
                new_clause[row[other_clause]->same_clause[i]->var_name] = row[other_clause]->same_clause[i];
                if(row[other_clause]->same_clause[i]->level == level)
                    same_level.push(row[other_clause]->same_clause[i]->var_name);
                }
            --i;
            }
        /*cout << endl << "========violate previous assignment=========" << endl;
        cout << other_clause << endl;
        for(it=new_clause.begin();it!=new_clause.end();it++)
            {
            if(it->second->complement == -1)
                cout << "-";
            cout << "x" << it->first << " ";
            }
        cout << endl;*/
        //cout << 3 << endl;
        i = row[clause_name]->same_clause.size() - 1;
        while(i >= 0)
            {
            if(new_clause.count(row[clause_name]->same_clause[i]->var_name)==0 && row[clause_name]->same_clause[i]->var_name!=x)
                {
                new_clause[row[clause_name]->same_clause[i]->var_name] = row[clause_name]->same_clause[i];
                if(row[clause_name]->same_clause[i]->level == level)
                    same_level.push(row[clause_name]->same_clause[i]->var_name);
                }
            --i;
            }
        //cout << 4 << endl;
        /*cout << endl << "=================" << endl;
        cout << clause_name << endl;
        for(it=new_clause.begin();it!=new_clause.end();it++)
            {
            if(it->second->complement == -1)
                cout << "-";
            cout << "x" << it->first << " ";
            }
        cout << endl;*/
        }
        }
    else
        {
        //cout << 10 << endl;
        flag = false;
        i = row[clause_name]->same_clause.size() - 1;
        //cout << 11 << endl;
        while(i >= 0)
            {
            if(row[clause_name]->same_clause[i]->level == level)
                {
                //cout << column[row[clause_name]->same_clause[i]->var_name]->from_clause << endl;
                same_level.push(row[clause_name]->same_clause[i]->var_name);
                new_clause[row[clause_name]->same_clause[i]->var_name] = row[clause_name]->same_clause[i];
                }
            else
                {
                new_clause[row[clause_name]->same_clause[i]->var_name] = row[clause_name]->same_clause[i];
                }
            --i;
            }
        //cout << 12 << endl;
        /*cout << endl << "=================" << endl;
        for(it=new_clause.begin();it!=new_clause.end();it++)
            {
            if(it->second->complement == -1)
                cout << "-";
            cout << "x" << it->first << " ";
            }
        cout << endl;*/
        }
    //cout << 2 << endl;
    //cout << "End of ini" << endl;
    while(same_level.size() > 1)
        {
        //cout << same_level.front() << endl;
        flag = true;
        other_clause = column[same_level.front()]->from_clause;

        //cout << other_clause << endl;
        //cout << "same level: ";
        while(other_clause==-1)
            {
            temp_Q.push(same_level.front());
            same_level.pop();
            if(same_level.empty())
                break;
            other_clause = column[same_level.front()]->from_clause;
            //cout << "X" << same_level.front();
            //cout << ", " << other_clause << endl;
            }
        //cout << "X" << same_level.front();

        if(other_clause==-1)
            {
            flag = false;
            break;
            }

        while(!temp_Q.empty())
            {
            same_level.push(temp_Q.front());
            temp_Q.pop();
            }
        //cout << "X" << same_level.front();
        //cout << other_clause << endl;
        i = row[other_clause]->same_clause.size() - 1;

        //cout << other_clause << endl;
        ///==================
        //int temp;
        //cin >> temp;
        ///==================
        /*cout << endl << "=================" << endl;
        cout << other_clause << endl;
        cout << "resolve " << same_level.front() << endl;
        for(it=new_clause.begin();it!=new_clause.end();it++)
            {
            if(it->second->complement == -1)
                cout << "-";
            cout << "x" << it->first << " ";
            }
        cout << endl;*/
        while(i >= 0)
            {
            new_clause.erase(same_level.front());
            if(new_clause.count(row[other_clause]->same_clause[i]->var_name)==0 && row[other_clause]->same_clause[i]->var_name!=same_level.front())
                {
                new_clause[row[other_clause]->same_clause[i]->var_name] = row[other_clause]->same_clause[i];
                if(row[other_clause]->same_clause[i]->level == level)
                    same_level.push(row[other_clause]->same_clause[i]->var_name);
                }
            if(new_clause.size() > 10)
                {
                flag = false;
                break;
                }
            --i;
            }
        //cout << same_level.size() << endl;
        same_level.pop();
        //cout << same_level.size() << endl;
        }
    //cout << 3 << endl;
    if(flag && new_clause.size()>1)
        {
        //cout << 1 << endl;
        second_level = 0;
        clause_temp = new clause_index;
        //cout << 1 << endl;
        for(it=new_clause.begin();it!=new_clause.end();it++)
            {
            var_temp = new variable(it->second->var_name, numofclause, it->second->complement);
            var_temp->level = it->second->level;
            var_temp->value = it->second->value;
            clause_temp->same_clause.push_back(var_temp);
            column[var_temp->var_name]->same_var.push_back(var_temp);
            if(var_temp->level == column[same_level.front()]->level)
                first_level_var = var_temp;
            if(var_temp->level>second_level && var_temp->level!=level)
                {
                second_level = var_temp->level;
                second_level_var = var_temp;
                }
            }
        //cout << 2 << endl;
        //NCB_level = second_level - 1;
        numofclause++;
        first_level_var->watched = true;
        second_level_var->watched = true;
        clause_temp->watched_list[first_level_var->var_name] = first_level_var;
        clause_temp->watched_list[second_level_var->var_name] = second_level_var;
        clause_temp->solved_list[first_level_var->var_name] = first_level_var;
        //cout << clause_temp->unwatched_list.size() << endl;
        //cout << 3 << endl;
        ncb_assignment.x = first_level_var->var_name;
        ncb_assignment.v = first_level_var->complement*(-1);
        ncb_assignment.from_clause = numofclause - 1;
        ncb_assignment.level = level;
        //cout << 4 << endl;
        row.push_back(clause_temp);
        add_ncb_assignment = true;
        //cout << row.size() << endl;
        //cout << numofclause << endl;
        /*cout << endl << "=================" << endl;
        for(it=new_clause.begin();it!=new_clause.end();it++)
            {
            if(it->second->complement == -1)
                cout << "-";
            cout << "x" << it->first << "(" << it->second->level << ")" << " ";
            }
        cout << endl;*/
        }
    //cout << 4 << endl;
    new_clause.clear();
    }

void SAT_solver::backtrace(vector<x_and_v>& assignments_of_BCP)
    {
    int i, j, k, temp_clause_name;
    variable* clause_variable;
    i = assignments_of_BCP.size() - 1;
    while(i>=0)
        {
        //cout << "backtrace: X" << assignments_of_BCP[i].x << " = " << assignments_of_BCP[i].v << endl;
        j = column[assignments_of_BCP[i].x]->same_var.size() - 1;
        column[assignments_of_BCP[i].x]->assigned = false;
        column[assignments_of_BCP[i].x]->level = 0;
        column[assignments_of_BCP[i].x]->from_clause = -1;
        while(j>=0)
            {
            column[assignments_of_BCP[i].x]->same_var[j]->value = 2;
            column[assignments_of_BCP[i].x]->same_var[j]->level = 0;
            temp_clause_name = column[assignments_of_BCP[i].x]->same_var[j]->clause_name;
            if(row[temp_clause_name]->watched_list.count(assignments_of_BCP[i].x)==0)
                {
                row[temp_clause_name]->unwatched_list[assignments_of_BCP[i].x] = column[assignments_of_BCP[i].x]->same_var[j];
                column[assignments_of_BCP[i].x]->same_var[j]->watched = false;
                }
            if(row[temp_clause_name]->solved_list.erase(assignments_of_BCP[i].x))
                {
                //cout << "In clause " << temp_clause_name << endl;
                if(row[temp_clause_name]->solved_list.size()==0)
                    {
                    //cout << temp_clause_name << " is not solved yet" << endl;
                    k = row[temp_clause_name]->same_clause.size() - 1;
                    while(k>=0)
                        {
                        clause_variable = row[temp_clause_name]->same_clause[k];
                        if(clause_variable->complement==-1)
                            {
                            ++column[clause_variable->var_name]->neg_num;
                            }
                        else
                            {
                            ++column[clause_variable->var_name]->pos_num;
                            }
                        ++column[clause_variable->var_name]->total_num;
                        //cout << clause_variable->var_name << " total num = " << column[clause_variable->var_name].total_num << endl;
                        --k;
                        }
                    }
                }
            --j;
            }
        assignments_of_BCP.erase(assignments_of_BCP.begin()+i);
        --i;
        }
    /*cout << endl << "After backtracking: " << endl;
    i = 1;
    while(i<=numofvar)
        {
        cout << "" << i << " ";
        if(column[i]->assigned==false)
            cout << "_ ";
        else
            cout << column[i]->value << " ";
        i++;
        }
    cout << endl;*/
    assignments_of_BCP.clear();
    }

bool SAT_solver::DPLL(int level)
    {
    int temp;
    int i;
    bool flag = false;
    x_and_v next_assignment;
    map<int, variable*>::iterator it;
    //cin >> temp;

    //cout << endl << "=============level: " << level << "===============" << endl;
    //cout << "Assignment: ";

    //add_ncb_assignment = false;
    /*if(level!=next_level)
        {
        cout << "NCB" << endl;
        backtrace(this_level_assignments[level]);
        return false;
        }*/

    while(!value_to_be_assigned.empty())
        {
        //cout << 1 << endl;
        add_ncb_assignment = false;
        this_level_assignments[level].push_back(value_to_be_assigned.begin()->second);
        //i = row.size() - 1;
        /*while(i>=0)
            {
            cout << i << ": ";
            cout << row[i]->watched_list.size() << ", ";
            cout << row[i]->unwatched_list.size() << endl;
            cout << i << endl;
            for(it=row[i]->watched_list.begin();it!=row[i]->watched_list.end();++i)
                {
                cout << "X" << it->first << ", ";
                }
            --i;
            }*/
        //cout << "X" << value_to_be_assigned.begin()->second.x << "=" << value_to_be_assigned.begin()->second.v << " ";
        //cout << value_to_be_assigned.size() << endl;
        //cout << value_to_be_assigned.begin()->second.x << " " << value_to_be_assigned.rbegin()->second.x << endl;
        if(!set_variable_value(level))
            {
            //cout << "-----"<< endl;
            /*cout << "CONFLICT!" << endl;
            cout << "This level assignment: ";
            i = this_level_assignments.size() - 1;
            while(i>=0)
                {
                cout << "X" << this_level_assignments[i].x << "=" << this_level_assignments[i].v << ", ";
                --i;
                }
            cout << endl;*/
            //cout << "conflict" << endl;
            value_to_be_assigned.clear();
            if(add_ncb_assignment==true)
                {
                value_to_be_assigned[ncb_assignment.x] = ncb_assignment;
                //cout << value_to_be_assigned.size() << endl;
                add_ncb_assignment = false;
                }
            flag = true;
            break;
            }
        }
    /*cout << endl << endl;
    i = 1;
    while(i<=numofvar)
        {
        cout << "" << i << " ";
        if(column[i]->assigned==false)
            cout << "_ ";
        else
            cout << column[i]->value << " ";
        i++;
        }
    cout << endl;*/
    //cout << "leave loop" << endl;
    if(solved_test())
        {
        cout << "SOLVED!!\n";
        return true;
        }
    /*choose a variable and assign value here x_next, v_next*/
    if(flag==true || branching_heuristic(level+1)==false)
        {
        //cout << "CONFLICT!!" << endl;
        /*cout << "This level assignment: ";
        i = this_level_assignments.size() - 1;
        while(i>=0)
            {
            cout << "X" << this_level_assignments[i].x << "=" << this_level_assignments[i].v << ", ";
            --i;
            }
        cout << endl;*/

        next_level = level - 1;
        backtrace(this_level_assignments[level]);
        return false;
        }
    else
        {
        //cout << endl;
        next_assignment = value_to_be_assigned.begin()->second;
        //this_level_assignments.push_back(next_assignment);
        //cout << "next assignment: X" << next_assignment.x << " = " << next_assignment.v << endl;
        next_level = level + 1;
        if(DPLL(level+1))
            return true;
        else
            {
            //backtrace(this_level_assignments[level+1]);

            //cout << "level" << endl;
                //cout << 3 << endl;
            next_assignment.v = (next_assignment.v) * (-1);
            value_to_be_assigned[next_assignment.x] = next_assignment;
            //cout << "try the other way: X" << next_assignment.x << " = " << next_assignment.v << endl;
            if(DPLL(level+1))
                return true;
            else
                {
                backtrace(this_level_assignments[level]);
                return false;
                }
            }
        }
    }

void SAT_solver::initialize_watched_list()//can pre-process here
    {
    int i = numofclause - 1;
    while(i>=0)
        {
        if(row[i]->same_clause.size()>1)
            {
            row[i]->watched_list[row[i]->same_clause[0]->var_name] = row[i]->same_clause[0];
            row[i]->watched_list[row[i]->same_clause[1]->var_name] = row[i]->same_clause[1];
            row[i]->same_clause[0]->watched = true;
            row[i]->same_clause[1]->watched = true;
            for(int j=2;j<row[i]->same_clause.size();j++)
                {
                row[i]->unwatched_list[row[i]->same_clause[j]->var_name] = row[i]->same_clause[j];
                }
            }
        else
            {
            row[i]->watched_list[row[i]->same_clause[0]->var_name] = row[i]->same_clause[0];
            row[i]->same_clause[0]->watched = true;
            }
        --i;
        }
    return;
    }

void SAT_solver::parser(const char* filename)
    {
    var_index* col_temp;
    clause_index* row_temp;
    char useless;
    string comment;
    variable* temp;
    int i, var;

    ifstream fin;
    fin.open(filename);

    useless = fin.peek();
    while(useless=='c')
        {
        getline(fin, comment);
        useless = fin.peek();
        }
    while(useless!='f')
        fin >> useless;
    fin >> numofvar >> numofclause;
    cout << "# of variables = " << numofvar << endl;
    cout << "# of clauses = " << numofclause << endl;

    i = numofvar;
    while(i >= 0)
        {
        col_temp = new var_index;
        column.push_back(col_temp);
        --i;
        }

    i = numofclause - 1;
    while(i >= 0)
        {
        row_temp = new clause_index;
        row.push_back(row_temp);
        --i;
        }
    this_level_assignments.resize(numofvar+1);

    //column = new var_index[numofvar+1];
    //row = new clause_index[numofclause];

    i = 0;
    while(i<numofclause)
        {
        fin >> var;

        while(var!=0)
            {
            //cout << "new var = " << var << ", ";
            if(var<0)
                {
                temp = new variable(abs(var), i, -1);
                ++column[temp->var_name]->neg_num;
                }
            else
                {
                temp = new variable(var, i, 1);
                ++column[temp->var_name]->pos_num;
                }
            //cout << i << "-";
            //cout << temp->var_name << "-";
            ++column[temp->var_name]->total_num;
            column[temp->var_name]->same_var.push_back(temp);//put same variable in the same column
            //cout << "comp = " << column[temp->var_name].same_var[0]->complement << ", ";
            row[i]->same_clause.push_back(temp);//put variables in the same clause in the same line
            //cout << row[i].same_clause[0]->var_name << " ";
            fin >> var;
            }
        //cout << endl;
        ++i;
        }
    }

void SAT_solver::input_data()
    {
    int i, j, k;
    for(i=0;i<row.size();i++)
        {
        k = row[i]->same_clause.size();
        j = 0;
        while(j<k)
            {
            if(row[i]->same_clause[j]->complement==-1)
                cout << "-";
            cout << row[i]->same_clause[j]->var_name << " ";
            ++j;
            }
        cout << endl;
        }
    }

int main(int argc, char* argv[])
    {
    SAT_solver obj;
    x_and_v initial;
    obj.parser(argv[1]);
    //could do some pre-processing here
    obj.initialize_watched_list();
    //obj.input_data();
    //cout << initial.x << ", " << initial.v << endl;
    obj.output(argv[1], obj.DPLL(1));
    //obj.input_data();
    return 0;
    }
