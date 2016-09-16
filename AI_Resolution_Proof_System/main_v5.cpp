#include <iostream>
#include <cstdlib>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>
#include <stack>

using namespace std;

// argv[1] should be the file name of the KB, Query and Result file.
// e.g. $ Prove.exe StarWar
// This is just an example. You don't need to modify this code.
// You can code whatever you want but your code should both follow I/O format and print execution time.
// BTW, please attach a Readme.txt for compilation.
// !, =, &, |, =>, <=>

class const_or_var
    {
    public:
        bool is_var; //TRUE = variable; FALSE = constant;
        vector<char> dependent_var_vector;
        string name;
    };

class clause
    {
    public:
        clause();
        bool is_inv; //TRUE = inverse;
        int type_of_clause; //0 = predicate; 1 = forall; 2 = exist; 3 = &; 4 = |; 5 = !; 6 = =>; 7 = <=>; 8 = []
        string clause_name;
        char quantify_var;
        clause *parent_c;
        vector<clause*> clause_vector;
        vector<const_or_var*> param_vector;
    private:
    };

clause::clause()
    {
    type_of_clause = -1;
    clause_name = "NONE";
    }

class predicate
    {
    public:
        predicate();
        string name;
        int num_var;
    };

predicate::predicate()
    {
    name = "";
    num_var = 1;
    }

class var_trans_row
    {
    public:
        char var_name;
        vector<char> dependent_var_vector;
    };

class resolution_proof_system
    {
    public:
        char fname[3][100];
        bool prove_or_not;

        resolution_proof_system();
        ~resolution_proof_system();
        bool parser_v2(char *argv[]);
        bool formal_output_1();
        bool formal_output_2();
        bool process_buffer();
        bool turn_into_CNF();
        bool FOL_resolution();
        void print_buffer();
        void print_FOL();
        void print_CNF();
        
    private:
        string output_filename;
        string neg_query;
        char *kb_buffer_temp;
        //int kb_size;
        //char *query_buffer;
        //char *query_inv_buffer;
        vector<char> query_buffer;
        vector<char> query_inv_buffer;
        //int query_size;
        //char *kb_buffer;
        vector<char> kb_buffer;
        int original_kb_size;
        //int num_of_KB_terms;
        vector<int> position_of_KB_terms;
        vector<char> var_vector;
        vector<string> const_vector;
        vector<predicate*> pred_vector;
        vector<clause*> FOL_vector;
        vector<clause*> FOL_vector_copy;
        vector<clause*> CNF_vector;
        //int position_of_neg_query;
        //vector<clause*> CNF_vector_copy;
        vector<var_trans_row*> var_trans_vector;

        int construct_clause_ds(int i, clause *parent_clause);
        void print_clause(clause *present_clause);
        void print_clause_to_file(clause *present_clause, ofstream &fout);
        bool find_and_replace_clause(clause *target_clause, clause *compared_clause, clause *new_clause);
        bool copy_clause(const clause *target_clause, clause *new_clause);
        /** Turn into CNF **/
        bool eliminate_brackets(clause *parent_clause);
        bool eliminate_iff(clause *parent_clause);
        bool eliminate_implications(clause *parent_clause);
        bool move_inv_inwards(clause *parent_clause);
        //bool standardize_var(clause *parent_clause, vector<char> accumulated_used_var_vector);
        bool skolemize(clause *parent_clause, vector<char> accumulated_depend_var_vector);
        bool drop_universal_quantifiers(clause *parent_clause);
        bool distribute_or_over_and(clause *parent_clause);
        bool make_CNF_vector(clause *parent_clause);
        bool we_need_a_better_ds(clause *parent_clause);
        /** Turn into CNF **/
        int resolve_two_clauses(const clause *clause_a, const clause *clause_b, clause *result_clause);
        bool resolvable_or_not(const clause *clause_a, const clause *clause_b);
        bool equal_or_included(const clause *clause_a, const clause *new_clause);
    };

resolution_proof_system::resolution_proof_system()
    {
    output_filename = "";
    }

resolution_proof_system::~resolution_proof_system()
    {

    }

bool resolution_proof_system::find_and_replace_clause(clause *target_clause, clause *compared_clause, clause *new_clause)
    {
    //cout << "~~~~~~~~~~~" << endl;
    bool found_or_not = false;
    int i = target_clause->clause_vector.size() - 1;
    //cout << "!!!!!!!!!!!!!11" << endl;
    while(i >= 0)
        {
        if(target_clause->clause_vector[i] == compared_clause)
            {
            found_or_not = true;
            target_clause->clause_vector[i] = new_clause;
            new_clause->parent_c = target_clause;
            break;
            }
        --i;
        }
    if(found_or_not == false)
        {
        //cout << "Fail to find the clause. " << endl;
        return false;
        }
    return true;
    }

bool resolution_proof_system::copy_clause(const clause *target_clause, clause *new_clause)
    {
    int i;
    clause *clause_temp;
    const_or_var *param_temp;

    new_clause->type_of_clause = target_clause->type_of_clause;
    new_clause->clause_name = target_clause->clause_name;
    new_clause->quantify_var = target_clause->quantify_var;
    i = 0;
    while(i < target_clause->param_vector.size())
        {
        param_temp = new const_or_var;
        param_temp->is_var = target_clause->param_vector[i]->is_var;
        param_temp->name = target_clause->param_vector[i]->name;
        new_clause->param_vector.push_back(param_temp);
        ++i;
        }
    i = 0;
    while(i < target_clause->clause_vector.size())
        {
        clause_temp = new clause;
        copy_clause(target_clause->clause_vector[i], clause_temp);
        new_clause->clause_vector.push_back(clause_temp);
        clause_temp->parent_c = new_clause;
        ++i;
        }
    return true;
    }

void resolution_proof_system::print_buffer()
    {
    int i;
    if(query_buffer.empty()!=true)
        {
        cout << "========== Query ==========" << endl;
        i = 0;
        while(i < query_buffer.size())
            {
            cout << query_buffer[i];
            ++i;
            }
        cout << endl << endl;
        }
    if(query_inv_buffer.empty()!=true)
        {
        cout << "========== Inverse of Query ==========" << endl;
        i = 0;
        while(i < query_inv_buffer.size())
            {
            cout << query_inv_buffer[i];
            ++i;
            }
        cout << endl << endl;
        }
    cout << "========== Variables ==========" << endl;
    i = 0;
    while(i < var_vector.size())
        {
        cout << var_vector[i] << ", ";
        ++i;
        }
    cout << endl << "========== Constants ==========" << endl;
    i = 0;
    while(i < const_vector.size())
        {
        cout << const_vector[i] << ", ";
        ++i;
        }
    cout << endl << "========== Predicates ==========" << endl;
    i = 0;
    while(i < pred_vector.size())
        {
        cout << pred_vector[i]->name << ", ";
        ++i;
        }
    cout << endl;
    return;
    }

void resolution_proof_system::print_clause(clause *present_clause)
    {
    int j;
    if(present_clause->type_of_clause == 0) //predicate
        {
        cout << present_clause->clause_name << "(";
        j = 0;
        while(j < present_clause->param_vector.size())
            {
            cout << present_clause->param_vector[j]->name << ",";
            ++j;
            }
        cout << ")";
        }
    else if(present_clause->type_of_clause == 1) //forall
        {
        cout << "Forall " << present_clause->quantify_var << " ";
        print_clause(present_clause->clause_vector[0]);
        }
    else if(present_clause->type_of_clause == 2) //exist
        {
        cout << "Exist " << present_clause->quantify_var << " ";
        print_clause(present_clause->clause_vector[0]);
        }
    else if(present_clause->type_of_clause == 3) //&
        {
        print_clause(present_clause->clause_vector[0]);
        //cout << "Vector size: " << present_clause->clause_vector.size();
        j = 1;
        while(j < present_clause->clause_vector.size())
            {
            cout << " & ";
            print_clause(present_clause->clause_vector[j]);
            ++j;
            }
        }
    else if(present_clause->type_of_clause == 4) // |
        {
        print_clause(present_clause->clause_vector[0]);
        j = 1;
        while(j < present_clause->clause_vector.size())
            {
            cout << " | ";
            print_clause(present_clause->clause_vector[j]);
            ++j;
            }
        }
    else if(present_clause->type_of_clause == 5) // !
        {
        cout << "! ";
        print_clause(present_clause->clause_vector[0]);
        }
    else if(present_clause->type_of_clause == 6) // =>
        {
        //cout << "Size of vector: " << present_clause->clause_vector.size() << endl;
        print_clause(present_clause->clause_vector[0]);
        cout << " => ";
        //cout << present_clause->clause_vector[1]->type_of_clause;
        print_clause(present_clause->clause_vector[1]);
        }
    else if(present_clause->type_of_clause == 7) // <=>
        {
        print_clause(present_clause->clause_vector[0]);
        cout << " <=> ";
        print_clause(present_clause->clause_vector[1]);
        }
    else if(present_clause->type_of_clause == 8) // []
        {
        cout << "[";
        print_clause(present_clause->clause_vector[0]);
        cout << "]";
        }
    else
        {
        //cout << "  Something is fucking wrong. Clause size " << present_clause->clause_vector.size();
        }
    return;
    }

void resolution_proof_system::print_clause_to_file(clause *present_clause, ofstream &fout)
    {
    int j;
    if(present_clause->type_of_clause == 0) //predicate
        {
        fout << present_clause->clause_name << "(";
        if(present_clause->param_vector.size() != 0)
            {
            if(present_clause->param_vector[0]->is_var == true)
                {
                fout << present_clause->param_vector[0]->name;
                }
            else
                {
                fout << "\"" << present_clause->param_vector[0]->name << "\"";
                }
            }
        j = 1;
        while(j < present_clause->param_vector.size())
            {
            if(present_clause->param_vector[j]->is_var == true)
                {
                fout << "," << present_clause->param_vector[j]->name;
                }
            else
                {
                fout << "," << "\"" << present_clause->param_vector[j]->name << "\"";
                }
            ++j;
            }
        fout << ")";
        }
    else if(present_clause->type_of_clause == 1) //forall
        {
        fout << "Forall " << present_clause->quantify_var << " ";
        print_clause_to_file(present_clause->clause_vector[0], fout);
        }
    else if(present_clause->type_of_clause == 2) //exist
        {
        fout << "Exist " << present_clause->quantify_var << " ";
        print_clause_to_file(present_clause->clause_vector[0], fout);
        }
    else if(present_clause->type_of_clause == 3) //&
        {
        print_clause_to_file(present_clause->clause_vector[0], fout);
        //cout << "Vector size: " << present_clause->clause_vector.size();
        j = 1;
        while(j < present_clause->clause_vector.size())
            {
            fout << " & ";
            print_clause_to_file(present_clause->clause_vector[j], fout);
            ++j;
            }
        }
    else if(present_clause->type_of_clause == 4) // |
        {
        print_clause_to_file(present_clause->clause_vector[0], fout);
        j = 1;
        while(j < present_clause->clause_vector.size())
            {
            fout << " | ";
            print_clause_to_file(present_clause->clause_vector[j], fout);
            ++j;
            }
        }
    else if(present_clause->type_of_clause == 5) // !
        {
        fout << "! ";
        print_clause_to_file(present_clause->clause_vector[0], fout);
        }
    else if(present_clause->type_of_clause == 6) // =>
        {
        //cout << "Size of vector: " << present_clause->clause_vector.size() << endl;
        print_clause_to_file(present_clause->clause_vector[0], fout);
        fout << " => ";
        //cout << present_clause->clause_vector[1]->type_of_clause;
        print_clause_to_file(present_clause->clause_vector[1], fout);
        }
    else if(present_clause->type_of_clause == 7) // <=>
        {
        print_clause_to_file(present_clause->clause_vector[0], fout);
        fout << " <=> ";
        print_clause_to_file(present_clause->clause_vector[1], fout);
        }
    else if(present_clause->type_of_clause == 8) // []
        {
        fout << "[";
        print_clause_to_file(present_clause->clause_vector[0], fout);
        fout << "]";
        }
    else
        {
        //cout << "  Something is fucking wrong. Clause size " << present_clause->clause_vector.size();
        }
    return;
    }

void resolution_proof_system::print_FOL()
    {
    int i;
    i = 0;
    while(i < FOL_vector.size())
        {
        print_clause(FOL_vector[i]);
        cout << endl;
        ++i;
        }
    return;
    }

void resolution_proof_system::print_CNF()
    {
    int i;
    i = 0;
    while(i < CNF_vector.size())
        {
        cout << "C" << i+1 << ": ";
        print_clause(CNF_vector[i]);
        cout << endl;
        ++i;
        }
    return;
    }


bool resolution_proof_system::parser_v2(char *argv[])
    {
    int i, debug;
    char temp_char;
    char *temp_char_ptr;
    temp_char_ptr = argv[1];
    
    snprintf(fname[0],100,"%s.KB",argv[1]);
    snprintf(fname[1],100,"%s.Query",argv[1]);
    snprintf(fname[2],100,"%s.Result",argv[1]);
    i = 0;
    while(temp_char_ptr[i]!='.')
        {
        output_filename = output_filename + temp_char_ptr[i];
        ++i;
        }
    output_filename = output_filename + ".Result";

    ifstream fin;
    fin.open(fname[0]);
    while(fin.get(temp_char))
        {
        //cout << "hi" << endl;
        kb_buffer.push_back(temp_char);
        }
    kb_buffer.push_back('\n');
    original_kb_size = kb_buffer.size();
    fin.close();

    /*if(kb_buffer.empty()!=true)
        {
        cout << "========== KB ==========" << endl;
        i = 0;
        while(i < kb_buffer.size())
            {
            cout << kb_buffer[i];
            ++i;
            }
        cout << endl << endl;
        }*/
    
    fin.open(fname[1]);
    query_inv_buffer.push_back('!');
    query_inv_buffer.push_back(' ');
    query_inv_buffer.push_back('[');
    while(fin.get(temp_char))
        {
        //debug = temp_char;
        //cout << "--" << temp_char << ": " << debug << "++" << endl;
        
        query_buffer.push_back(temp_char);
        query_inv_buffer.push_back(temp_char);
        }
    while( query_inv_buffer.back()==10 || query_inv_buffer.back()==13)
        {
        query_inv_buffer.pop_back();
        }
    query_inv_buffer.push_back(']');
    query_inv_buffer.push_back('\n');
    fin.close();

    //cout << "~~~ Start to put query_inv_buffer" << endl;

    /*while(kb_buffer.back() != 'F')
        {
        kb_buffer.pop_back();
        }*/

    i = 0;
    while(i < query_inv_buffer.size())
        {
        //debug = query_inv_buffer[i];
        //cout << "--" << query_inv_buffer[i] << ": " << debug << "++" << endl;
        kb_buffer.push_back(query_inv_buffer[i]);
        ++i;
        }
    kb_buffer.push_back('\n');
    return true;
    }

/*bool resolution_proof_system::formal_output_1()
    {
    ofstream fout;
    fout.open(output_filename.c_str(), ios::app);
    
    return true;
    }*/

bool resolution_proof_system::formal_output_1()
    {
    //string filename = "";
    int i, j;

    ofstream fout;
    fout.open(fname[2]);
    
    fout << "conversion start" << endl << endl;
    fout << "KB" << endl;
    fout << "{" << endl;

    //cout << 999 << endl;

    i = 0;
    j = 0;
    while(i < FOL_vector_copy.size())
        {
        //cout << i << endl;
        print_clause_to_file(FOL_vector_copy[i], fout);
        fout << endl << "to" << endl;
        while(j < position_of_KB_terms[i])
            {
            fout << "C" << j+1 << " ";
            print_clause_to_file(CNF_vector[j], fout);
            fout << endl;
            ++j;
            }
        ++i;
        }

    fout << "}" << endl << endl;


    fout << "Negated Query" << endl;
    fout << "{" << endl;
    //fout << neg_query;
    i = 0;
    while(i < query_inv_buffer.size()-1)
        {
        fout << query_inv_buffer[i];
        ++i;
        }
    fout << endl << "to" << endl;

    //cout << "?" << endl;
    
    while(j < CNF_vector.size())
        {
        fout << "C" << j+1 << " ";
        print_clause_to_file(CNF_vector[j], fout);
        fout << endl;
        ++j;
        }
    fout << "}" << endl << endl;
    fout << "conversion end" << endl << endl;
    fout.close();
    
    return true;
    }

/*bool resolution_proof_system::formal_output_2()
    {
    int i;

    ofstream fout;
    fout.open(output_filename.c_str(), ios::app);

    fout << "}" << endl << endl;
    fout << "conversion end" << endl;
    fout.close();
    return true;
    }*/

bool resolution_proof_system::formal_output_2()
    {
    ofstream fout;
    fout.open(fname[2], ios::app);
    fout << "proof & unification start" << endl << endl;
    fout << endl << "proof & unification end" << endl << endl;
    fout << "PROVE/DISPROVE:" << endl;
    if(prove_or_not==true)
        fout << "PROVE" << endl;
    else
        fout << "DISPROVE" << endl;
    fout.close();
    return true;
    }

bool resolution_proof_system::process_buffer()
    {
    string temp = "";
    clause *clause_temp;
    clause *clause_temp_2;
    clause *clause_temp_3;
    predicate *predicate_temp;
    int i, j, var_counter, debug, start_of_FOL;
    /** Go through "Variables: " **/
    //cout << 1 << endl;
    //cout << kb_buffer[0] << kb_buffer[1] << kb_buffer[2] << kb_buffer[3] << endl;
    i = 0;
    while(kb_buffer[i]!=' ')
        {
        //cout << kb_buffer[i] << endl;
        ++i;
        }
    //cout << endl;
    ++i;
    while(kb_buffer[i]!=10 && kb_buffer[i]!=13)//Before the endl
        {
        if(kb_buffer[i]!=',')
            var_vector.push_back(kb_buffer[i]);
        ++i;
        }
    ++i;//Go to next line
    /** Go through "Constants: " **/
    while(kb_buffer[i]!=' ')
        {
        //cout << kb_buffer[i];
        ++i;
        }
    ++i;
    while(kb_buffer[i]!=10 && kb_buffer[i]!=13)//Before the endl
        {
        //cout << kb_buffer[i] << endl;
        if(kb_buffer[i]=='"')
            {
            ++i;
            temp = "";
            while(kb_buffer[i]!='"')
                {
                temp = temp + kb_buffer[i];
                ++i;
                }
            const_vector.push_back(temp);
            }
        ++i;
        }
    ++i;//Go to next line
    /** Go through "Predicates: " **/
    while(kb_buffer[i]!=' ')
        {
        //cout << kb_buffer[i] << endl;
        ++i;
        }
    ++i; //escape the space after':'
    while(kb_buffer[i]!=10 && kb_buffer[i]!=13)
        {
        //cout << kb_buffer[i] << endl;
        if(kb_buffer[i]==',')
            ++i;
        predicate_temp = new predicate;
        while(kb_buffer[i]!='(')
            {
            //cout << kb_buffer[i] << endl;
            predicate_temp->name = predicate_temp->name + kb_buffer[i];
            ++i;
            }
        //cout << predicate_temp->name << endl;
        while(kb_buffer[i]!=')')
            {
            //cout << kb_buffer[i] << endl;
            if(kb_buffer[i]==',')
                ++predicate_temp->num_var;
            ++i;
            }
        pred_vector.push_back(predicate_temp);
        ++i;
        }
    /** Go to next line **/
    while(kb_buffer[i]==10 || kb_buffer[i]==13)
        {
        //cout << i << endl;
        ++i;
        }
    while(kb_buffer[i]!=10 && kb_buffer[i]!=13)//Go through "FOL start"
        {
        ++i;
        }
    /** Go to next line **/
    while(kb_buffer[i]==10 || kb_buffer[i]==13)
        ++i;
    start_of_FOL = i;
    temp = "";
    while(kb_buffer[i]!=10 && kb_buffer[i]!=13)//Get the next line
        {
        temp = temp + kb_buffer[i];
        ++i;
        }
    ++i;//Go to next line
    while(temp!="FOL end")
        {
        //debug = temp[0];
        //cin >> debug;
        //cout << "+++" << temp << "+++"<< endl;
        clause_temp = new clause;
        //clause_temp->type_of_clause = 0;
        construct_clause_ds(start_of_FOL, clause_temp);
        /*if(clause_temp->type_of_clause == 0)
            {
            clause_temp_2 = clause_temp;
            clause_temp = clause_temp->clause_vector[0];
            delete clause_temp_2;
            }*/
        FOL_vector.push_back(clause_temp);
        

        clause_temp_2 = new clause;
        construct_clause_ds(start_of_FOL, clause_temp_2);
        FOL_vector_copy.push_back(clause_temp_2);
        
        //start_of_FOL = i;
        //cout << i-1 << ": " << kb_buffer[i-1] << endl;
        //cout << i << ": " << kb_buffer[i] << endl;
        while(kb_buffer[i]==0 || kb_buffer[i]==10 || kb_buffer[i]==13)//Go to next line
            ++i;
        //cout << "SHIT: " << kb_buffer[i] << endl;
        start_of_FOL = i;

        temp = "";
        while(i<kb_buffer.size() && kb_buffer[i]!=0 && kb_buffer[i]!=10 && kb_buffer[i]!=13)
            {
            //cin >> debug;
            //cout << "KB Size:" << kb_size << endl;
            //debug = kb_buffer[i];
            //cout << i << ": " << debug << endl;
            temp = temp + kb_buffer[i];
            ++i;
            }
        /*while(kb_buffer[i]==0 || kb_buffer[i]==10 || kb_buffer[i]==13)//Go to next line
            ++i;*/
        //cout << "---" << temp << "---" << endl;
        }
    //num_of_KB_terms = FOL_vector.size();
    /** Process the query **/
    //cout << "~~~~~~~~~~~~~~~~~~~~!!!!!!!!!!!!!11" << endl;
    while(kb_buffer[i]==0 || kb_buffer[i]==10 || kb_buffer[i]==13 || kb_buffer[i]==' ' )//Go to next line
        ++i;
    /*start_of_FOL = i;

    temp = "";
    while(i<kb_buffer.size() && kb_buffer[i]!=0 && kb_buffer[i]!='\n' && kb_buffer[i]!=' ')
        {
        cout << kb_buffer[i] << endl;
        temp = temp + kb_buffer[i];
        ++i;
        }*/
    //cout << ":)))" << endl;
    clause_temp = new clause;
    construct_clause_ds(i, clause_temp);
    FOL_vector.push_back(clause_temp);

    /*clause_temp_3 = new clause;
    construct_clause_ds(i, clause_temp_3);
    FOL_vector_copy.push_back(clause_temp_3);*/

    return true;
    }

int resolution_proof_system::construct_clause_ds(int i, clause *parent_clause)
    {
    // !, =, &, |, =>, <=>
    //0 = predicate; 1 = forall; 2 = exist; 3 = &; 4 = |; 5 = !; 6 = =>; 7 = <=>; 8 = []
    bool flag_1;
    int start, j, debug, debug2;
    string temp, param_name, temp_clause_name;
    clause *clause_temp;
    const_or_var *param_temp;

    //start = i;
    while(kb_buffer[i]!=10 && kb_buffer[i]!=13 && kb_buffer[i]!=']')
        {
        //cin >> debug2;
        debug = kb_buffer[i];
        //cout << "++++++" << debug << "++++++" << endl;
        if(kb_buffer[i] == ' ')
            ++i;
        start = i;
        temp = "";
        while(kb_buffer[i]!=' ' && kb_buffer[i]!=']' && kb_buffer[i]!=10 && kb_buffer[i]!=13)
            {
            temp = temp + kb_buffer[i];
            ++i;
            }
        //cout << "-" << temp << endl;
        //0 = predicate; 1 = forall; 2 = exist; 3 = &; 4 = |; 5 = !; 6 = =>; 7 = <=>; 8 = []
        if(temp == "Forall")
            {
            //cout << "Forall" << endl;
            if(parent_clause->type_of_clause==1 || parent_clause->type_of_clause==2 || parent_clause->type_of_clause==8)
                {
                clause_temp = new clause;
                clause_temp->type_of_clause = 1;
                ++i;
                clause_temp->quantify_var = kb_buffer[i];
                ++i; // skip the space
                i = construct_clause_ds(i, clause_temp);
                parent_clause->clause_vector.push_back(clause_temp);
                }
            else
                {
                parent_clause->type_of_clause = 1;
                ++i; // skip the space
                parent_clause->quantify_var = kb_buffer[i];
                ++i; // skip the space
                i = construct_clause_ds(i, parent_clause);
                }
            }
        else if(temp == "Exist")
            {
            //cout << "Exist" << endl;
            if(parent_clause->type_of_clause==1 || parent_clause->type_of_clause==2 || parent_clause->type_of_clause==8)
                {
                clause_temp = new clause;
                clause_temp->type_of_clause = 2;
                ++i;
                clause_temp->quantify_var = kb_buffer[i];
                ++i; // skip the space
                i = construct_clause_ds(i, clause_temp);
                parent_clause->clause_vector.push_back(clause_temp);
                }
            else
                {
                parent_clause->type_of_clause = 2;
                ++i; // skip the space
                parent_clause->quantify_var = kb_buffer[i];
                ++i; // skip the space
                i = construct_clause_ds(i, parent_clause);
                }
            }
        else if(temp == "!")
            {
            /*parent_clause->type_of_clause = 5;
            ++i;
            clause_temp = new clause;
            i = construct_clause_ds(i, clause_temp);
            parent_clause->clause_vector.push_back(clause_temp);*/
            //cout << "Paren clause type: " << parent_clause->type_of_clause << endl;
            if(parent_clause->type_of_clause==1 || parent_clause->type_of_clause==2 || parent_clause->type_of_clause==3 || parent_clause->type_of_clause==4 || parent_clause->type_of_clause==5 || parent_clause->type_of_clause==6 || parent_clause->type_of_clause==7 || parent_clause->type_of_clause==8)
                {
                clause_temp = new clause;
                clause_temp->type_of_clause = 5;
                //clause_temp->clause_vector.push_back(parent_clause->clause_vector.back());
                //parent_clause->clause_vector.pop_back();
                i = construct_clause_ds(i+1, clause_temp);
                parent_clause->clause_vector.push_back(clause_temp);
                }
            /*else if(parent_clause->type_of_clause==8)
                {
                clause_temp = new clause;
                clause_temp->type_of_clause = 5;
                i = construct_clause_ds(i+1, clause_temp);
                parent_clause->clause_vector.push_back(clause_temp);
                }*/
            else
                {
                parent_clause->type_of_clause = 5;
                i = construct_clause_ds(i+1, parent_clause);
                }
            }
        else if(temp[0] == '[')
            {
            if(parent_clause->type_of_clause == -1)
                parent_clause->type_of_clause = 8;
            /*clause_temp = new clause;
            clause_temp->type_of_clause = 8;
            parent_clause->clause_vector.push_back(clause_temp);
            parent_clause = clause_temp;*/
            /*if(parent_clause->type_of_clause==3 || parent_clause->type_of_clause==4 || parent_clause->type_of_clause==5 || parent_clause->type_of_clause==6 || parent_clause->type_of_clause==7)
                {
                clause_temp = new clause;
                clause_temp->type_of_clause = 8;
                i = construct_clause_ds(start + 1, clause_temp);
                ++i;
                parent_clause->clause_vector.push_back(clause_temp);
                parent_clause = clause_temp;
                }
            else
                {

                }*/
            //parent_clause->type_of_clause = 8;
            clause_temp = new clause;
            clause_temp->type_of_clause = 8;
            i = construct_clause_ds(start + 1, clause_temp);
            ++i;
            parent_clause->clause_vector.push_back(clause_temp);
            }
        else if(temp == "&")
            {
            if(parent_clause->type_of_clause==0 || parent_clause->type_of_clause==5)
                {
                clause_temp = new clause;
                clause_temp->type_of_clause = parent_clause->type_of_clause;
                if(parent_clause->type_of_clause==0)
                    {
                    clause_temp->clause_name = parent_clause->clause_name;
                    j = 0;
                    while(j < parent_clause->param_vector.size())
                        {
                        clause_temp->param_vector.push_back(parent_clause->param_vector[j]);
                        ++j;
                        }
                    }
                j = 0;
                while(j < parent_clause->clause_vector.size())
                    {
                    clause_temp->clause_vector.push_back(parent_clause->clause_vector[j]);
                    ++j;
                    }
                parent_clause->clause_vector.clear();
                parent_clause->clause_vector.push_back(clause_temp);
                parent_clause->type_of_clause = 3;
                i = construct_clause_ds(i+1, parent_clause);
                }
            else if(parent_clause->type_of_clause==4 || parent_clause->type_of_clause==6 || parent_clause->type_of_clause==7 || parent_clause->type_of_clause==8)
                {
                clause_temp = new clause;
                clause_temp->type_of_clause = 3;
                clause_temp->clause_vector.push_back(parent_clause->clause_vector.back());
                parent_clause->clause_vector.pop_back();
                i = construct_clause_ds(i+1, clause_temp);
                parent_clause->clause_vector.push_back(clause_temp);
                }
            else
                {
                parent_clause->type_of_clause = 3;
                i = construct_clause_ds(i+1, parent_clause);
                }
            //if(parent_clause->type_of_clause!=4 && parent_clause->type_of_clause!=6 && parent_clause->type_of_clause!=7)
            //parent_clause->type_of_clause = 3;
            //clause_temp = new clause;
            //i = construct_clause_ds(i+1, clause_temp);
            //i = construct_clause_ds(i+1, parent_clause);
            //cout << parent_clause->clause_vector.size() << ", " << parent_clause->clause_vector[parent_clause->clause_vector.size()-1]->clause_name << endl;
            //parent_clause->clause_vector.push_back(clause_temp);
            }
        else if(temp == "|")
            {
            if(parent_clause->type_of_clause==0 || parent_clause->type_of_clause==3 || parent_clause->type_of_clause==5)
                {
                clause_temp = new clause;
                clause_temp->type_of_clause = parent_clause->type_of_clause;
                if(parent_clause->type_of_clause==0)
                    {
                    //cout << "Clause name (OR): " << parent_clause->clause_name << endl;
                    clause_temp->clause_name = parent_clause->clause_name;
                    //cout << clause_temp->clause_name << endl;
                    //cout << parent_clause->clause_vector.size() << endl;
                    //cout << parent_clause->param_vector.size() << endl;
                    j = 0;
                    while(j < parent_clause->param_vector.size())
                        {
                        clause_temp->param_vector.push_back(parent_clause->param_vector[j]);
                        ++j;
                        }
                    }
                j = 0;
                while(j < parent_clause->clause_vector.size())
                    {
                    clause_temp->clause_vector.push_back(parent_clause->clause_vector[j]);
                    ++j;
                    }
                parent_clause->clause_vector.clear();
                parent_clause->clause_vector.push_back(clause_temp);
                parent_clause->type_of_clause = 4;
                i = construct_clause_ds(i+1, parent_clause);
                }
            else if(parent_clause->type_of_clause==6 || parent_clause->type_of_clause==7 || parent_clause->type_of_clause==8)
                {
                clause_temp = new clause;
                clause_temp->type_of_clause = 4;
                clause_temp->clause_vector.push_back(parent_clause->clause_vector.back());
                parent_clause->clause_vector.pop_back();
                i = construct_clause_ds(i+1, clause_temp);
                parent_clause->clause_vector.push_back(clause_temp);
                }
            else
                {
                parent_clause->type_of_clause = 4;
                i = construct_clause_ds(i+1, parent_clause);
                }
            }
        else if(temp == "=>")
            {
            //cout << "=>" << endl;
            if(parent_clause->type_of_clause==0 || parent_clause->type_of_clause==3 || parent_clause->type_of_clause==4 || parent_clause->type_of_clause==5)
                {
                clause_temp = new clause;
                clause_temp->type_of_clause = parent_clause->type_of_clause;
                if(parent_clause->type_of_clause==0)
                    {
                    clause_temp->clause_name = parent_clause->clause_name;
                    j = 0;
                    while(j < parent_clause->param_vector.size())
                        {
                        clause_temp->param_vector.push_back(parent_clause->param_vector[j]);
                        ++j;
                        }
                    }
                j = 0;
                while(j < parent_clause->clause_vector.size())
                    {
                    clause_temp->clause_vector.push_back(parent_clause->clause_vector[j]);
                    ++j;
                    }
                parent_clause->clause_vector.clear();
                parent_clause->clause_vector.push_back(clause_temp);
                parent_clause->type_of_clause = 6;
                i = construct_clause_ds(i+1, parent_clause);
                }
            else if(parent_clause->type_of_clause==7 || parent_clause->type_of_clause==8)
                {
                clause_temp = new clause;
                clause_temp->type_of_clause = 6;
                clause_temp->clause_vector.push_back(parent_clause->clause_vector.back());
                parent_clause->clause_vector.pop_back();
                i = construct_clause_ds(i+1, clause_temp);
                parent_clause->clause_vector.push_back(clause_temp);
                }
            else
                {
                parent_clause->type_of_clause = 6;
                i = construct_clause_ds(i+1, parent_clause);
                }
            //parent_clause->type_of_clause = 6;
            //clause_temp = new clause;
            //i = construct_clause_ds(i+1, clause_temp);
            //i = construct_clause_ds(i+1, parent_clause);
            //parent_clause->clause_vector.push_back(clause_temp);
            }
        else if(temp == "<=>")
            {
            if(parent_clause->type_of_clause==0 || parent_clause->type_of_clause==3 || parent_clause->type_of_clause==4 || parent_clause->type_of_clause==5 || parent_clause->type_of_clause==6)
                {
                clause_temp = new clause;
                clause_temp->type_of_clause = parent_clause->type_of_clause;
                if(parent_clause->type_of_clause==0)
                    {
                    clause_temp->clause_name = parent_clause->clause_name;
                    j = 0;
                    while(j < parent_clause->param_vector.size())
                        {
                        clause_temp->param_vector.push_back(parent_clause->param_vector[j]);
                        ++j;
                        }
                    }
                j = 0;
                while(j < parent_clause->clause_vector.size())
                    {
                    clause_temp->clause_vector.push_back(parent_clause->clause_vector[j]);
                    ++j;
                    }
                parent_clause->clause_vector.clear();
                parent_clause->clause_vector.push_back(clause_temp);
                parent_clause->type_of_clause = 7;
                i = construct_clause_ds(i+1, parent_clause);
                }
            else if(parent_clause->type_of_clause==8)
                {
                clause_temp = new clause;
                clause_temp->type_of_clause = 7;
                clause_temp->clause_vector.push_back(parent_clause->clause_vector.back());
                parent_clause->clause_vector.pop_back();
                i = construct_clause_ds(i+1, clause_temp);
                parent_clause->clause_vector.push_back(clause_temp);
                }
            else
                {
                parent_clause->type_of_clause = 7;
                i = construct_clause_ds(i+1, parent_clause);
                }
            }
        else
            {
            flag_1 = true;
            if(parent_clause->type_of_clause==-1 || parent_clause->type_of_clause==1 || parent_clause->type_of_clause==2)
                {
                flag_1 = false;
                clause_temp = parent_clause;
                }
            else
                {
                clause_temp = new clause;
                }
            clause_temp->type_of_clause = 0;
            j = 0;
            temp_clause_name = "";
            while(temp[j] != '(')
                {
                temp_clause_name = temp_clause_name + temp[j];
                ++j;
                }
            /*if(temp_clause_name == "Kills")
                 cout << "AHHHHHHHH" << endl;*/
            clause_temp->clause_name = temp_clause_name;
            //cout << temp_clause_name << endl;
            ++j;
            while(j < temp.size())
                {
                if(temp[j] == ')')
                    break;
                if(temp[j] == ',')
                    ++j;
                if(temp[j] == '"') // A constant
                    {
                    param_temp = new const_or_var;
                    param_temp->is_var = false;
                    ++j;
                    /** Get the name of the constant **/
                    param_name = "";
                    while(j<temp.size() && temp[j]!='"')
                        {
                        param_name = param_name + temp[j];
                        ++j;
                        }
                    ++j;
                    param_temp->name = param_name;
                    /*if(param_name == "Kitty")
                        cout << "~~~AHHHHHHHH" << endl;*/
                    //cout << "Constant: " << param_name << endl;
                    clause_temp->param_vector.push_back(param_temp);
                    //parent_clause->param_vector.push_back(param_temp);
                    }
                else // A variable
                    {
                    param_temp = new const_or_var;
                    param_temp->is_var = true;
                    param_temp->name = temp[j];
                    //cout << "Var: " << temp[j] << endl;
                    /*if(temp[j] == 'Y')
                        cout << "~~~AHHHHHHHH" << endl;*/
                    clause_temp->param_vector.push_back(param_temp);
                    //parent_clause->param_vector.push_back(param_temp);
                    ++j;
                    }
                }
            //cout << "HI" << endl;
            if(flag_1 == true)
                {
                //cout << "Push " << clause_temp->clause_name << ", back to " << parent_clause->type_of_clause << endl;
                parent_clause->clause_vector.push_back(clause_temp);
                }
            }
        //debug = kb_buffer[i];
        //cout << "======" << debug << "======" << endl;
        //++i;
        //debug = kb_buffer[i];
        //cout << "======" << debug << "======" << endl;
        }
    return i;
    }

bool resolution_proof_system::turn_into_CNF()
    {
    // !, =, &, |, =>, <=>
    // 0 = predicate; 1 = forall; 2 = exist; 3 = &; 4 = |; 5 = !; 6 = =>; 7 = <=>; 8 = []
    vector<char> useless_vector;
    /*cout << endl
         << "*********************************" << endl
         << "*****Before turning into CNF*****" << endl
         << "*********************************" << endl;
    print_FOL();*/
    int i;
    clause *clause_temp;
    clause *clause_temp_2;
    
    i = 0;
    while(i < FOL_vector.size())
        {
        FOL_vector[i]->parent_c = NULL;
        eliminate_brackets(FOL_vector[i]);
        //eliminate_iff(FOL_vector[i]);
        //move_inv_inwards(FOL_vector[i]);
        ++i;
        }

    /*cout << endl
         << "*****After eliminating brackets*****" << endl << endl;
    print_FOL();*/

    i = 0;
    while(i < FOL_vector.size())
        {
        FOL_vector[i]->parent_c = NULL;
        eliminate_iff(FOL_vector[i]);
        //eliminate_iff(FOL_vector[i]);
        //move_inv_inwards(FOL_vector[i]);
        ++i;
        }

    /*cout << endl
         << "*****After eliminating iff*****" << endl << endl;
    print_FOL();*/

    i = 0;
    while(i < FOL_vector.size())
        {
        FOL_vector[i]->parent_c = NULL;
        //eliminate_iff(FOL_vector[i]);
        eliminate_implications(FOL_vector[i]);
        //move_inv_inwards(FOL_vector[i]);
        ++i;
        }

    /*cout << endl
         << "*****After eliminating implications*****" << endl << endl;
    print_FOL();*/

    i = 0;
    while(i < FOL_vector.size())
        {
        FOL_vector[i]->parent_c = NULL;
        move_inv_inwards(FOL_vector[i]);
        ++i;
        }
    /*cout << endl
         << "*****After moving inv inwards *****" << endl << endl;
    print_FOL();*/

    i = 0;
    while(i < FOL_vector.size())
        {
        FOL_vector[i]->parent_c = NULL;
        skolemize(FOL_vector[i], useless_vector);
        ++i;
        }
    /*cout << endl
         << "*****After skolemizing*****" << endl << endl;
    print_FOL();*/

    i = 0;
    while(i < FOL_vector.size())
        {
        FOL_vector[i]->parent_c = NULL;
        drop_universal_quantifiers(FOL_vector[i]);
        ++i;
        }
    /*cout << endl
         << "*****After dropping universal quantifiers*****" << endl << endl;
    print_FOL();*/

    i = 0;
    while(i < FOL_vector.size())
        {
        FOL_vector[i]->parent_c = NULL;
        distribute_or_over_and(FOL_vector[i]);
        ++i;
        }
    /*out << endl
         << "*****After distributing OR over AND*****" << endl << endl;
    print_FOL();*/

    i = 0;
    while(i < FOL_vector.size()-1)
        {
        FOL_vector[i]->parent_c = NULL;
        make_CNF_vector(FOL_vector[i]);
        position_of_KB_terms.push_back(CNF_vector.size());
        ++i;
        }

    FOL_vector[i]->parent_c = NULL;
    make_CNF_vector(FOL_vector[i]);
    /*cout << endl
         << "*****This is our CNF vector*****" << endl << endl;
    print_CNF();*/
    //cout << "HI" << endl;

    i = 0;
    while(i < CNF_vector.size())
        {
        clause_temp = new clause;
        copy_clause(CNF_vector[i], clause_temp);
        clause_temp->parent_c = NULL;
        CNF_vector[i] = clause_temp;

        /*clause_temp_2 = new clause;
        copy_clause(CNF_vector[i], clause_temp_2);
        clause_temp_2->parent_c = NULL;
        CNF_vector_copy.push_back(clause_temp_2);*/
        //we_need_a_better_ds(CNF_vector[i]);
        ++i;
        }

    formal_output_1();

    i = 0;
    while(i < CNF_vector.size())
        {
        we_need_a_better_ds(CNF_vector[i]);
        ++i;
        }
    //cout << endl
         //<< "*****This is our CNF vector*****" << endl << endl;
    //print_CNF();
    return true;
    }

bool resolution_proof_system::eliminate_brackets(clause *parent_clause)
    {
    clause *clause_temp;
    int i;

    if(parent_clause->type_of_clause == 0)
        {
        return true;
        }
    else if(parent_clause->type_of_clause == 8)
        {
        clause_temp = parent_clause->clause_vector[0];
        if(parent_clause->parent_c == NULL)
            {
            i = 0;
            while(i < FOL_vector.size())
                {
                if(FOL_vector[i] == parent_clause)
                    {
                    FOL_vector[i] = clause_temp;
                    clause_temp->parent_c = NULL;
                    delete parent_clause;
                    parent_clause = NULL;
                    break;
                    }
                ++i;
                }
            }
        else
            {
            if(find_and_replace_clause(parent_clause->parent_c, parent_clause, clause_temp))
                {
                delete parent_clause;
                parent_clause = NULL;
                eliminate_brackets(clause_temp);
                }
            else
                return false;
            }
        }
    else
        {
        i = 0;
        while(i < parent_clause->clause_vector.size())
            {
            parent_clause->clause_vector[i]->parent_c = parent_clause;
            eliminate_brackets(parent_clause->clause_vector[i]);
            ++i;
            }
        }
    return true;
    }

bool resolution_proof_system::eliminate_iff(clause *parent_clause)
    {
    int i;
    clause *temp_clause_1;
    clause *temp_clause_2;
    clause *temp_clause_3;
    clause *temp_clause_4;
    clause *temp_clause_5;
    clause *temp_clause_6;

    if(parent_clause->type_of_clause == 0)
        {
        return true;
        }
    else if(parent_clause->type_of_clause!=7 && parent_clause->type_of_clause!=0)
        {
        i = 0;
        while(i < parent_clause->clause_vector.size())
            {
            parent_clause->clause_vector[i]->parent_c = parent_clause;
            eliminate_iff(parent_clause->clause_vector[i]);
            ++i;
            }
        }
    else // This clause is an iff clause
        {
        parent_clause->type_of_clause = 3;
        /** Copy clause[0] to two new clauses, copy clause[1] to two new clauses **/
        temp_clause_1 = new clause;
        temp_clause_2 = new clause;
        temp_clause_3 = new clause;
        temp_clause_4 = new clause;
        copy_clause(parent_clause->clause_vector[0], temp_clause_1);
        copy_clause(parent_clause->clause_vector[1], temp_clause_2);
        copy_clause(parent_clause->clause_vector[0], temp_clause_3);
        copy_clause(parent_clause->clause_vector[1], temp_clause_4);
        
        /** We need two inverse clauses **/
        temp_clause_5 = new clause;
        temp_clause_6 = new clause;
        temp_clause_5->type_of_clause = 5;
        temp_clause_6->type_of_clause = 5;

        temp_clause_5->clause_vector.push_back(temp_clause_1);
        temp_clause_1->parent_c = temp_clause_5;
        temp_clause_6->clause_vector.push_back(temp_clause_4);
        temp_clause_4->parent_c = temp_clause_6;

        /** Re-construct parent_clause->clause_vector[0], parent_clause->clause_vector[1] **/
        parent_clause->clause_vector[0]->type_of_clause = 4;
        parent_clause->clause_vector[1]->type_of_clause = 4;

        parent_clause->clause_vector[0]->clause_vector.clear();
        parent_clause->clause_vector[1]->clause_vector.clear();
        
        parent_clause->clause_vector[0]->param_vector.clear();
        parent_clause->clause_vector[1]->param_vector.clear();
        
        parent_clause->clause_vector[0]->clause_vector.push_back(temp_clause_5);
        parent_clause->clause_vector[0]->clause_vector.push_back(temp_clause_2);
        temp_clause_5->parent_c = parent_clause->clause_vector[0];
        temp_clause_2->parent_c = parent_clause->clause_vector[0];
        
        parent_clause->clause_vector[1]->clause_vector.push_back(temp_clause_3);
        parent_clause->clause_vector[1]->clause_vector.push_back(temp_clause_6);
        temp_clause_3->parent_c = parent_clause->clause_vector[1];
        temp_clause_6->parent_c = parent_clause->clause_vector[1];
        }
    return true;
    }

bool resolution_proof_system::eliminate_implications(clause *parent_clause)
    {
    int i;
    clause *temp_clause_1;
    clause *temp_clause_2;
    clause *temp_clause_3;

    if(parent_clause->type_of_clause == 0)
        {
        return true;
        }
    else if(parent_clause->type_of_clause!=6 && parent_clause->type_of_clause!=0)
        {
        i = 0;
        while(i < parent_clause->clause_vector.size())
            {
            parent_clause->clause_vector[i]->parent_c = parent_clause;
            eliminate_implications(parent_clause->clause_vector[i]);
            ++i;
            }
        }
    else // This clause is an implication
        {
        temp_clause_2 = parent_clause->clause_vector.back();
        
        temp_clause_1 = new clause;
        temp_clause_1->type_of_clause = 5; 
        if(parent_clause->clause_vector.size() == 2)
            {
            parent_clause->clause_vector[0]->parent_c = temp_clause_1;
            temp_clause_1->clause_vector.push_back(parent_clause->clause_vector[0]);
            }
        else if( parent_clause->clause_vector.size() > 2)
            {
            temp_clause_3 = new clause;
            temp_clause_3->type_of_clause = 6;
            i = 0;
            while( i < (parent_clause->clause_vector.size()-1) )
                {
                parent_clause->clause_vector[i]->parent_c = temp_clause_3;
                temp_clause_3->clause_vector.push_back(parent_clause->clause_vector[i]);
                ++i;
                }
            temp_clause_3->parent_c = temp_clause_1;
            temp_clause_1->clause_vector.push_back(temp_clause_3);
            }
        else
            {
            cout << "parent_clause->clause_vector.size() < 2" << endl;
            return false;
            }
        eliminate_implications(temp_clause_1);
        eliminate_implications(temp_clause_2);
        parent_clause->type_of_clause = 4;
        parent_clause->clause_vector.clear();
        temp_clause_1->parent_c = parent_clause;
        temp_clause_2->parent_c = parent_clause;
        parent_clause->clause_vector.push_back(temp_clause_1);
        parent_clause->clause_vector.push_back(temp_clause_2);
        }
    return true;
    }

bool resolution_proof_system::move_inv_inwards(clause *parent_clause)
    {
    int i;
    bool found_or_not;
    vector<clause*> clause_temp_vec;
    clause *grand_parent_clause;
    clause *clause_temp;
    clause *clause_temp_2;
    if(parent_clause->type_of_clause == 0)
        {
        return true;
        }
    else if(parent_clause->type_of_clause == 5) // We found an inverse clause, next we have to check its subsequent clause
        {
        clause_temp = parent_clause->clause_vector[0];
        clause_temp->parent_c = parent_clause;
        grand_parent_clause = parent_clause->parent_c;
        if(parent_clause->clause_vector.size()!=1)
            cout << "WARNING: parent_clause->clause_vector.size()!=1" << endl;
        if(clause_temp->type_of_clause == 5) // ! !
            {
            //cout << "OAOAOAOAOAO" << endl;
            /*GOGOGO*/
            if(grand_parent_clause == NULL)
                {
                i = 0;
                while(i < FOL_vector.size())
                    {
                    if(FOL_vector[i] == parent_clause)
                        {
                        FOL_vector[i] = clause_temp->clause_vector[0];
                        clause_temp->parent_c = NULL;
                        delete parent_clause;
                        parent_clause = NULL;
                        break;
                        }
                    ++i;
                    }
                }
            else
                {
                clause_temp_2 = clause_temp->clause_vector[0];
                //grand_parent_clause = parent_clause->parent_c;
                //find_and_replace_clause(grand_parent_clause, parent_clause, clause_temp_2);
                if(find_and_replace_clause(grand_parent_clause, parent_clause, clause_temp_2)==false)
                    {
                    cout << "Fail to link to parent clause. " << endl;
                    cout << grand_parent_clause->clause_vector.size() << ", " << parent_clause->clause_vector.size() << ", " << clause_temp_2->clause_vector.size() << endl;
                    return false;
                    }
                delete clause_temp;
                clause_temp = NULL;
                delete parent_clause;
                parent_clause = NULL;
                move_inv_inwards(clause_temp_2);
                }
            }
        else if(clause_temp->type_of_clause == 1) // Forall
            {
            if(clause_temp->clause_vector.size()!=1)
                {
                cout << "WARNING: clause_temp->clause_vector.size()!=1" << endl;
                }
            /*clause_temp->type_of_clause = 2;
            clause_temp_2 = new Clause;
            clause_temp_2->type_of_clause = 5;
            clause_temp_2->clause_vector.push_back(clause_temp->clause_vector[0]);
            clause_temp->clause_vector[0] = clause_temp_2;
            if(find_and_replace_clause(grand_parent_clause, parent_clause, clause_temp)==false)
                {
                cout << "Fail to link to parent clause. " << endl;
                return false;
                }
            delete parent_clause;
            move_inv_inwards(clause_temp_2->clause_vector[0]);*/
            parent_clause->type_of_clause = 2;
            parent_clause->quantify_var = clause_temp->quantify_var;
            clause_temp->type_of_clause = 5;
            clause_temp->quantify_var = ' ';
            move_inv_inwards(clause_temp);
            }
        else if(clause_temp->type_of_clause == 2) // Exists
            {
            if(clause_temp->clause_vector.size()!=1)
                {
                cout << "WARNING: clause_temp->clause_vector.size()!=1" << endl;
                }
            /*clause_temp->type_of_clause = 1;
            clause_temp_2 = new Clause;
            clause_temp_2->type_of_clause = 5;
            clause_temp_2->clause_vector.push_back(clause_temp->clause_vector[0]);
            clause_temp->clause_vector[0] = clause_temp_2;
            if(find_and_replace_clause(grand_parent_clause, parent_clause, clause_temp)==false)
                {
                cout << "Fail to link to parent clause. " << endl;
                return false;
                }
            delete parent_clause;
            move_inv_inwards(clause_temp_2->clause_vector[0]);*/
            parent_clause->type_of_clause = 1;
            parent_clause->quantify_var = clause_temp->quantify_var;
            clause_temp->type_of_clause = 5;
            clause_temp->quantify_var = ' ';
            move_inv_inwards(clause_temp);
            }
        else if(clause_temp->type_of_clause == 3) // &
            {
            parent_clause->type_of_clause = 4;
            i = 0;
            while(i < clause_temp->clause_vector.size())
                {
                clause_temp_2 = new clause;
                clause_temp_2->type_of_clause = 5;
                clause_temp_2->clause_vector.push_back(clause_temp->clause_vector[i]);
                clause_temp->clause_vector[i]->parent_c = clause_temp_2;
                clause_temp_vec.push_back(clause_temp_2);
                ++i;
                }
            delete clause_temp;
            parent_clause->clause_vector.clear();
            i = 0;
            while(i < clause_temp_vec.size())
                {
                parent_clause->clause_vector.push_back(clause_temp_vec[i]);
                clause_temp_vec[i]->parent_c = parent_clause;
                ++i;
                }
            clause_temp_vec.clear();
            move_inv_inwards(parent_clause);
            }
        else if(clause_temp->type_of_clause == 4) // |
            {
            //cout << "An OR here!" << endl;
            parent_clause->type_of_clause = 3;
            i = 0;
            while(i < clause_temp->clause_vector.size())
                {
                clause_temp_2 = new clause;
                clause_temp_2->type_of_clause = 5;
                clause_temp_2->clause_vector.push_back(clause_temp->clause_vector[i]);
                clause_temp->clause_vector[i]->parent_c = clause_temp_2;
                clause_temp_vec.push_back(clause_temp_2);
                ++i;
                }
            delete clause_temp;
            parent_clause->clause_vector.clear();
            i = 0;
            while(i < clause_temp_vec.size())
                {
                parent_clause->clause_vector.push_back(clause_temp_vec[i]);
                clause_temp_vec[i]->parent_c = parent_clause;
                ++i;
                }
            clause_temp_vec.clear();
            move_inv_inwards(parent_clause);
            }
        else if(clause_temp->type_of_clause == 8)
            {
            clause_temp->type_of_clause = 5;
            //cout << clause_temp->clause_name << endl;
            if(grand_parent_clause != NULL)
                find_and_replace_clause(grand_parent_clause, parent_clause, clause_temp);
            else
                {
                found_or_not = false;
                i = FOL_vector.size() - 1;
                while(i >= 0)
                    {
                    if(FOL_vector[i] == parent_clause)
                        {
                        FOL_vector[i] = clause_temp;
                        clause_temp->parent_c = NULL;
                        found_or_not = true;
                        break;
                        }
                    --i;
                    }
                if(found_or_not == false)
                    {
                    cout << "Cant find the corresponding FOL[i]" << endl;
                    return false;
                    }
                }
            //cout << clause_temp->clause_name << endl;
            delete parent_clause;
            parent_clause = NULL;
            move_inv_inwards(clause_temp);
            }
        /*else
            {
            cout << "Why are you still here? " << endl;
            cout << "   - " << clause_temp->clause_name << ", type: " << clause_temp->type_of_clause << endl;
            return false;
            }*/
        }
    else
        {
        i = 0;
        while(i < parent_clause->clause_vector.size())
            {
            move_inv_inwards(parent_clause->clause_vector[i]);
            ++i;
            }
        }
    return true;
    }

/*bool resolution_proof_system::standardize_var(clause *parent_clause, vector<char> accumulated_used_var_vector)
    {
    i = 0;
    if(parent_clause->type_of_clause == 0)
        {

        }
    return true;
    }*/

bool resolution_proof_system::skolemize(clause *parent_clause, vector<char> accumulated_depend_var_vector)
    {
    // You have to remember: 
    // 1. The list or variables that are going to be turned into constants
    // 2. The corresponding dependent variables of each existence variable
    vector<char> temp_depend_var_vector;
    var_trans_row *temp_var_trans_row;
    const_or_var *temp_param;
    clause *clause_temp;
    int i, j, k;

    if(parent_clause->type_of_clause == 0)
        {
        /** need to do something **/
        i = 0;
        while(i < parent_clause->param_vector.size())
            {
            temp_param = parent_clause->param_vector[i];
            if(temp_param->is_var == true)
                {
                j = 0;
                while(j < var_trans_vector.size())
                    {
                    /** Find the corresponding variable **/
                    if(var_trans_vector[j]->var_name == temp_param->name[0])
                        {
                        temp_param->is_var = false;
                        k = 0;
                        while(k < var_trans_vector[j]->dependent_var_vector.size())
                            {
                            temp_param->dependent_var_vector.push_back(var_trans_vector[j]->dependent_var_vector[k]);
                            ++k;
                            }
                        }
                    ++j;
                    }
                }
            ++i;
            }
        return true;
        }
    else if(parent_clause->type_of_clause == 1) // Forall
        {
        accumulated_depend_var_vector.push_back(parent_clause->quantify_var);
        skolemize(parent_clause->clause_vector[0], accumulated_depend_var_vector);
        }
    else if(parent_clause->type_of_clause == 2) // Exist
        {
        clause_temp = parent_clause->clause_vector[0];
        temp_var_trans_row = new var_trans_row;
        temp_var_trans_row->var_name = parent_clause->quantify_var;
        i = 0;
        while(i < accumulated_depend_var_vector.size())
            {
            temp_var_trans_row->dependent_var_vector.push_back(accumulated_depend_var_vector[i]);
            ++i;
            }
        var_trans_vector.push_back(temp_var_trans_row);
        if(parent_clause->parent_c == NULL)
            {
            i = 0;
            while(i < FOL_vector.size())
                {
                if(parent_clause == FOL_vector[i])
                    {
                    FOL_vector[i] = clause_temp;
                    clause_temp->parent_c = NULL;
                    break;
                    }
                ++i;
                }
            }
        else
            {
            if(find_and_replace_clause(parent_clause->parent_c, parent_clause, clause_temp) == false)
                {
                cout << "Fail to link to parent clause. " << endl;
                return false;
                }
            }
        delete parent_clause;
        skolemize(clause_temp, accumulated_depend_var_vector);
        }
    else
        {
        i = 0;
        while(i < parent_clause->clause_vector.size())
            {
            skolemize(parent_clause->clause_vector[i], accumulated_depend_var_vector);
            ++i;
            }
        }
    return true;
    }

bool resolution_proof_system::drop_universal_quantifiers(clause *parent_clause)
    {
    bool found_or_not;
    int i;
    clause *clause_temp;

    if(parent_clause->type_of_clause == 0)
        {
        return true;
        }
    else if(parent_clause->type_of_clause == 1) // Remove this parent clause
        {
        clause_temp = parent_clause->clause_vector[0];
        if(parent_clause->parent_c == NULL)
            {
            found_or_not = false;
            i = 0;
            while(i < FOL_vector.size())
                {
                if(FOL_vector[i] == parent_clause)
                    {
                    FOL_vector[i] = clause_temp;
                    clause_temp->parent_c = NULL;
                    found_or_not = true;
                    break;
                    }
                ++i;
                }
            if(found_or_not == false)
                {
                cout << "Cant find the corresponding FOL[i]" << endl;
                return false;
                }
            }
        else
            {
            find_and_replace_clause(parent_clause->parent_c, parent_clause, clause_temp);
            }
        delete parent_clause;
        parent_clause = NULL;
        drop_universal_quantifiers(clause_temp);
        }
    else
        {
        i = 0;
        while(i < parent_clause->clause_vector.size())
            {
            drop_universal_quantifiers(parent_clause->clause_vector[i]);
            ++i;
            }
        }
    return true;
    }

bool resolution_proof_system::distribute_or_over_and(clause *parent_clause)
    {
    bool found_or_not;
    int i, j, k;
    clause *and_clause;
    clause *clause_temp;
    vector<clause*> copy_clause_vector;
    vector<clause*> new_or_clause_vector;

    if(parent_clause->type_of_clause == 0)
        {
        return true;
        }
    else if(parent_clause->type_of_clause == 4)
        {
        i = 0;
        while(i < parent_clause->clause_vector.size())
            {
            distribute_or_over_and(parent_clause->clause_vector[i]);
            ++i;
            }

        found_or_not = false;
        i = 0;
        while(i < parent_clause->clause_vector.size())
            {
            if(parent_clause->clause_vector[i]->type_of_clause == 3)
                {
                found_or_not = true;
                /** Need some works **/
                and_clause = parent_clause->clause_vector[i];
                j = 0;
                while(j < and_clause->clause_vector.size())
                    {
                    k = 0;
                    while( k < parent_clause->clause_vector.size() )
                        {
                        if(k != i)
                            {
                            clause_temp = new clause;
                            copy_clause(parent_clause->clause_vector[k], clause_temp);
                            copy_clause_vector.push_back(clause_temp);
                            }
                        ++k;
                        }
                    clause_temp = new clause;
                    clause_temp->type_of_clause = 4;
                    clause_temp->clause_vector.push_back(and_clause->clause_vector[j]);
                    and_clause->clause_vector[j]->parent_c = clause_temp;
                    k = 0;
                    while(k < copy_clause_vector.size())
                        {
                        clause_temp->clause_vector.push_back(copy_clause_vector[k]);
                        copy_clause_vector[k]->parent_c = clause_temp;
                        ++k;
                        }
                    copy_clause_vector.clear();
                    new_or_clause_vector.push_back(clause_temp);
                    ++j;
                    }

                parent_clause->type_of_clause = 3;
                /** Remove the old clauses from parent clause **/
                j = 0;
                while(j < parent_clause->clause_vector.size())
                    {
                    delete parent_clause->clause_vector[j];
                    parent_clause->clause_vector[j] = NULL;
                    ++j;
                    }
                parent_clause->clause_vector.clear();
                /** Push back the newly generated clauses to parent clause **/
                j = 0;
                while(j < new_or_clause_vector.size())
                    {
                    parent_clause->clause_vector.push_back(new_or_clause_vector[j]);
                    new_or_clause_vector[j]->parent_c = parent_clause;
                    ++j;
                    }
                /** Check the newly generated clauses again **/
                j = 0;
                while(j < parent_clause->clause_vector.size())
                    {
                    distribute_or_over_and(parent_clause->clause_vector[j]);
                    ++j;
                    }
                break;
                }
            ++i;
            }
        }
    else
        {
        i = 0;
        while(i < parent_clause->clause_vector.size())
            {
            distribute_or_over_and(parent_clause->clause_vector[i]);
            ++i;
            }
        }
    return true;
    }

bool resolution_proof_system::make_CNF_vector(clause *parent_clause)
    {
    bool check_flag;
    int i;
    if(parent_clause->type_of_clause == 0 || parent_clause->type_of_clause == 5)
        {
        CNF_vector.push_back(parent_clause);
        }
    else if(parent_clause->type_of_clause == 3)
        {
        i = 0;
        while(i < parent_clause->clause_vector.size())
            {
            make_CNF_vector(parent_clause->clause_vector[i]);
            ++i;
            }
        }
    else if(parent_clause->type_of_clause == 4)
        {
        /*check_flag = true;
        i = 0;
        while(i < parent_clause->clause_vector.size())
            {
            if( parent_clause->clause_vector[i]->type_of_clause!=0 && parent_clause->clause_vector[i]->type_of_clause!=5 )
                {
                check_flag = false;
                cout << "An illegal clause inside the OR clause. " << endl;
                cout << "Clause size: " << parent_clause->clause_vector.size() << endl;
                cout << parent_clause->clause_vector[0]->clause_name << ", " << parent_clause->clause_vector[0]->clause_name << endl;
                cout << parent_clause->clause_vector[0]->type_of_clause << ", " << parent_clause->clause_vector[0]->type_of_clause << endl;
                break;
                }
            ++i;
            }*/
        CNF_vector.push_back(parent_clause);
        }
    else
        {
        cout << "An illegal clause. " << endl;
        return false;
        }
    return true;
    }

bool resolution_proof_system::we_need_a_better_ds(clause *parent_clause)
    {
    clause *clause_temp;
    int i;
    if(parent_clause->type_of_clause == 5)
        {
        clause_temp = parent_clause->clause_vector[0];
        clause_temp->is_inv = true;
        if(parent_clause->parent_c != NULL)
            {
            find_and_replace_clause(parent_clause->parent_c, parent_clause, clause_temp);
            }
        else
            {
            i = 0;
            while(i < CNF_vector.size())
                {
                if(CNF_vector[i] == parent_clause)
                    {
                    CNF_vector[i] = clause_temp;
                    clause_temp->parent_c = NULL;
                    break;
                    }
                ++i;
                }
            }
        delete parent_clause;
        parent_clause = NULL;
        }
    else if(parent_clause->type_of_clause == 4)
        {
        i = 0;
        while(i < parent_clause->clause_vector.size())
            {
            we_need_a_better_ds(parent_clause->clause_vector[i]);
            ++i;
            }
        }
    return true;
    }

bool resolution_proof_system::resolvable_or_not(const clause *clause_a, const clause *clause_b)
    {
    int i, j;
    clause *clause_temp_1;
    clause *clause_temp_2;
    i = clause_a->clause_vector.size() - 1;
    while(i >= 0)
        {
        clause_temp_1 = clause_a->clause_vector[i];

        j = clause_b->clause_vector.size() - 1;
        while(j >= 0)
            {
            //cout << 111111 << endl;
            clause_temp_2 = clause_b->clause_vector[j];
            //cout << "Type of C1: " << clause_temp_1->clause_name << endl;
            //cout << "Type of C2: " << clause_temp_2->clause_name << endl << endl; 
            if(clause_temp_1->clause_name == clause_temp_2->clause_name)
                {
                //cout << 2222222 << endl;
                if(clause_temp_1->is_inv==true && clause_temp_2->is_inv==false)
                    {
                    //cout << "++++-----" << endl;
                    return true;
                    }
                else if(clause_temp_1->is_inv==false && clause_temp_2->is_inv==true)
                    {
                    //cout << "++++-----" << endl;
                    return true;
                    }
                }
            //cout << "33333" << endl;
            --j;
            }
        --i;
        }
    return false;
    }

int resolution_proof_system::resolve_two_clauses(const clause *clause_a, const clause *clause_b, clause *result_clause)
    {
    bool they_are_complement;
    bool they_are_same;
    int i, j, complement_counter;
    clause *clause_temp_1;
    clause *clause_temp_2;
    clause *new_clause;
    
    complement_counter = 0;
    i = 0;
    while(i < clause_a->clause_vector.size())
        {
        they_are_complement = false;
        clause_temp_1 = clause_a->clause_vector[i];
        
        j = 0;
        //cout << "AAA" << endl;
        while(j < clause_b->clause_vector.size())
            {
            //cout << "+++" << endl;
            //cout << clause_b->type_of_clause << endl;
            //cout << "---" << endl;
            if(clause_b->type_of_clause==0 && clause_b->clause_name=="NONE")
                return 0;
            clause_temp_2 = clause_b->clause_vector[j];
            if((clause_temp_1->is_inv==clause_temp_2->is_inv) && (clause_temp_1->clause_name==clause_temp_2->clause_name))
                {
                new_clause = new clause;
                //cout << 111 << endl;
                copy_clause(clause_temp_1, new_clause);
                //cout << 222 << endl;
                new_clause->parent_c = result_clause;
                result_clause->clause_vector.push_back(new_clause);
                break;
                }
            else if((clause_temp_1->is_inv!=clause_temp_2->is_inv) && (clause_temp_1->clause_name==clause_temp_2->clause_name))
                {
                ++complement_counter;
                they_are_complement = true;
                break;
                }
            ++j;
            }
        //cout << "BBB" << endl;
        if(they_are_complement == true)
            {
            if(complement_counter > 1)
                {
                //cout << "BBB" << endl;
                return 2;
                }
            }
        else
            {
            new_clause = new clause;
            copy_clause(clause_temp_1, new_clause);
            new_clause->parent_c = result_clause;
            result_clause->clause_vector.push_back(new_clause);
            }
        //cout << "BBB" << endl;
        ++i;
        }

    i = 0;
    while(i < clause_b->clause_vector.size())
        {
        they_are_same = false;
        they_are_complement = false;
        clause_temp_1 = clause_b->clause_vector[i];

        j = 0;
        while(j < clause_a->clause_vector.size())
            {
            clause_temp_2 = clause_a->clause_vector[j];
            if((clause_temp_1->is_inv==clause_temp_2->is_inv) && (clause_temp_1->clause_name==clause_temp_2->clause_name))
                {
                they_are_same = true;
                break;
                }
            else if((clause_temp_1->is_inv!=clause_temp_2->is_inv) && (clause_temp_1->clause_name==clause_temp_2->clause_name))
                {
                they_are_complement = true;
                break;
                }
            ++j;
            }
        if((they_are_complement==false) && (they_are_same==false))
            {
            new_clause = new clause;
            copy_clause(clause_temp_1, new_clause);
            result_clause->clause_vector.push_back(new_clause);
            }
        ++i;
        }

    if(result_clause->clause_vector.size()==0)
        return 0;
    else
        return 1;
    }

bool resolution_proof_system::equal_or_included(const clause *clause_a, const clause *new_clause)
    {
    bool same_clause_flag;
    int i, j;
    clause *clause_temp;
    if(clause_a->type_of_clause==0 && new_clause->type_of_clause==4) //0 4
        {
        i = new_clause->clause_vector.size() - 1;
        while(i >= 0)
            {
            clause_temp = new_clause->clause_vector[i];
            if(clause_a->is_inv != clause_temp->is_inv)
                {
                --i;
                continue;
                }   
            if(clause_a->clause_name != clause_temp->clause_name)
                {
                --i;
                continue;
                }
            return true; // Dont need to push back new_clause
            --i;
            }
        return false;
        }
    else
        {
        /*cout << "+++++" << endl;
        cout << clause_a->type_of_clause << endl;
        cout << ", " <<  new_clause->type_of_clause << endl;
        cout << "-----" << endl;*/
        if(clause_a->type_of_clause==0 && new_clause->type_of_clause==0) // 0 0
            {
            //cout << "2222+" << endl;
            if(clause_a->is_inv != new_clause->is_inv)
                {
                //cout << 2222 << endl;
                return false;
                }
            if(clause_a->clause_name != new_clause->clause_name)
                {
                //cout << 2222 << endl;
                return false;
                }
            //cout << 2222 << endl;
            return true;
            }
        else if(clause_a->type_of_clause==4 && new_clause->type_of_clause==0) // 4 0
            {
            //cout << 3333 << endl;
            return false;
            }
        else if(clause_a->type_of_clause==4 && new_clause->type_of_clause==4) // 4 4, fucking compilcated
            {
            //cout << "4444+" << endl;
            if(clause_a->clause_vector.size() > new_clause->clause_vector.size())
                {
                //cout << 4444 << endl;
                return false;
                }
            else
                {
                /**  **/
                //cout << 1111 << endl;
                i = clause_a->clause_vector.size() - 1;
                while(i >= 0)
                    {
                    //cout << "++" << endl;
                    same_clause_flag = false;
                    clause_temp = clause_a->clause_vector[i];
                    j = new_clause->clause_vector.size() - 1;
                    while(j >= 0)
                        {
                        if((clause_temp->is_inv==new_clause->clause_vector[j]->is_inv) && (clause_temp->clause_name==new_clause->clause_vector[j]->clause_name))
                            {
                            same_clause_flag = true;
                            break;
                            }
                        --j;
                        }
                    //cout << "--" << endl;
                    if(same_clause_flag == false)
                        {
                        //cout << 4444 << endl;
                        return false;
                        }
                    --i;
                    }
                //cout << 4444 << endl;
                return true;
                }
            }
        else
            {
            //cout << "WHAT type is this?" << endl;
            return false;
            }
        }
    return true;
    }

bool resolution_proof_system::FOL_resolution()
    {
    bool resolvability_flag;
    bool equal_or_included_flag;
    bool new_clause_flag;
    bool same_clause_flag;
    int i, j, m, n, resolve_result;
    int temp_CNF_vector_size;
    clause *new_clause;
    clause *temp_clause_1;
    clause *temp_clause_2;
    vector<clause*> new_clause_vector;
    //vector<clause*> temp_vector;
    ofstream fout;
    fout.open(fname[2], ios::app);
    fout << "proof & unification start" << endl << endl;
    

    while(true)
        { 
        //cout << "++" << endl;
        i = 0;
        while(i < CNF_vector.size() - 1)
            {
            j = i + 1;
            while(j < CNF_vector.size())
                {
                //cout << ":-)" << endl;
                
                /*cout << "========" << endl;
                cout << "Is (C" << i << ")"; 
                print_clause(CNF_vector[i]);
                cout << " and (C" << j << ")";
                print_clause(CNF_vector[j]);
                cout << " resolvable?" << endl;
                cout << "========" << endl << endl;*/

                resolvability_flag = resolvable_or_not(CNF_vector[i], CNF_vector[j]);
                //cout << ":-|" << endl;
                if(resolvability_flag == true)
                    {
                    new_clause = new clause;
                    new_clause->type_of_clause = 4;
                    //cout << ":-)" << endl;
                    resolve_result = resolve_two_clauses(CNF_vector[i], CNF_vector[j], new_clause);
                    //cout << "XDDDD" << endl;
                    if(resolve_result == 0) // a NULL clause
                        {
                        // KB entails the query
                        fout << endl << "proof & unification end" << endl << endl;
                        fout << "PROVE/DISPROVE:" << endl;
                        fout << "DISPROVE" << endl;
                        fout.close();
                        return true;
                        }
                    else if(resolve_result == 1) // a new clause is generated
                        {
                        //cout << "+++++" << endl;
                        //cout << new_clause->clause_vector.size() << endl;
                        //cout << "-----" << endl;
                        new_clause_vector.push_back(new_clause);
                        }
                    }
                else
                    {
                    //cout << "~~" << endl;
                    ++j;
                    continue;
                    }

                
                //cout << "--" << endl;
                ++j;
                }
            ++i;
            }
        //cout << "==" << endl;

        /** check if all new clauses are repeated **/
        new_clause_flag = false;
        temp_CNF_vector_size = CNF_vector.size() - 1;
        i = new_clause_vector.size() - 1;
        while(i >= 0)
            {
            //cout << 111 << endl;
            j = temp_CNF_vector_size;
            while(j >= 0)
                {
                //cout << 111 << endl;
                equal_or_included_flag = equal_or_included(CNF_vector[j], new_clause_vector[i]);
                //cout << 222 << endl;
                if(equal_or_included_flag == false)
                    {
                    //cout << "ZZZZZ" << endl;
                    if(CNF_vector[j]->clause_vector.size() > new_clause_vector[i]->clause_vector.size())
                        {
                        //cout << 111 << endl;
                        m = new_clause_vector[i]->clause_vector.size() - 1;
                        //cout << 222 << endl;
                        while(m >= 0)
                            {
                            //cout << 333 << endl;
                            same_clause_flag = false;
                            temp_clause_1 = new_clause_vector[i]->clause_vector[m];
                            n = CNF_vector[j]->clause_vector.size() - 1;
                            
                            //cout << endl << 444 << endl;
                            while(n >= 0)
                                {
                                //cout << endl << "~~~~~~~~~~~~~~~~~~~~" << endl; 
                                //cout << "J: " << j << ", N: " << n << endl;
                                //cout << n << endl;
                                temp_clause_2 = CNF_vector[j]->clause_vector[n];
                                //cout << CNF_vector[j]->clause_name << endl;
                                //if(temp_clause_2 == NULL)
                                    //cout << "Its FUCKING NULL" << endl;
                                //cout << temp_clause_1->type_of_clause << endl;
                                //cout << temp_clause_2->type_of_clause << endl;
                                if((temp_clause_1->is_inv==temp_clause_2->is_inv) && (temp_clause_1->clause_name==temp_clause_2->clause_name))
                                    {
                                    same_clause_flag = true;
                                    break;
                                    }
                                --n;
                                }
                            //cout << 555 << endl;
                            if(same_clause_flag == false)
                                break;
                            --m;
                            }
                        if(same_clause_flag == true)
                            {
                            /* Could be bug here */
                            /*cout << "=====$$$$$$$======" << endl;
                            cout << "Clause " << j << ": ";
                            print_clause(CNF_vector[j]);
                            cout << endl;*/
                            delete CNF_vector[j];
                            //cout << "J: " << j << endl;
                            CNF_vector[j] = new_clause_vector[i];
                            /*cout << "Is replaced as: ";
                            print_clause(CNF_vector[j]);
                            cout << "=====$$$$$$$======" << endl << endl;*/
                            }
                        else
                            {
                            new_clause_flag = true;
                            
                            CNF_vector.push_back(new_clause_vector[i]);
                            //temp_vector.push_back(new_clause_vector[i]);

                            /*cout << "====================" << endl;
                            print_clause(new_clause_vector[i]);
                            cout << " is pushed. " << endl;
                            cout << "====================" << endl << endl;*/

                            //cout << endl << "++++++" << endl;
                            //cout << "i: " << i << ", " << new_clause_vector[i]->clause_vector.size() << endl;
                            //cout << "Size: " << CNF_vector.size() << ", " << CNF_vector.back()->type_of_clause << endl;
                            //cout << "------" << endl;
                            //if(CNF_vector.size() == 94)
                                //cout << "Fuck I am here" << endl;
                            }
                        }
                    else
                        {
                        /** Fucking bug here **/
                        new_clause_flag = true;
                        CNF_vector.push_back(new_clause_vector[i]);
                        
                        /*cout << "=======~~~~~~=======" << endl;
                        print_clause(new_clause_vector[i]);
                        cout << " is pushed. " << endl;
                        cout << "=======~~~~~~=======" << endl << endl;*/

                        //cout << endl << "++++++" << endl;
                        //cout << "i: " << i << ", " << new_clause_vector[i]->clause_vector.size() << endl;
                        //cout << "Size: " << CNF_vector.size() << ", " << CNF_vector.back()->type_of_clause << endl;
                        //cout << "------" << endl;
                        /*if(CNF_vector.size() == 94)
                            {
                            cout << endl << "+++++++++++++++++++++++++++" << endl; 
                            cout << CNF_vector[93]->clause_name << endl;
                            cout << CNF_vector[93]->clause_vector.size() << endl;
                            cout << "Fuck I am here" << endl;
                            }*/
                        }
                    //cout << "QQQQQ" << endl;
                    }
                else
                    {
                    //BUG here?
                    //cout << "zzzzz" << endl;
                    delete new_clause_vector[i];
                    new_clause_vector[i] = NULL;
                    break;
                    //cout << "qqqqq" << endl;
                    }
                --j;
                }
            --i;
            //cout << 222 << endl;
            }
        //cout << "--" << endl;
        new_clause_vector.clear();
        if(new_clause_flag == false)
            {
            fout << endl << "proof & unification end" << endl << endl;
            fout << "PROVE/DISPROVE:" << endl;
            fout << "DISPROVE" << endl;
            fout.close();
            return false;
            }
        /*i = 0;
        while(i < CNF_vector.size())
            {
            cout << "+++" << endl;
            print_clause(CNF_vector[i]);
            cout << "---" << endl << endl;
            ++i;
            }*/
        }

    }

int main(int argc, char *argv[])
    {
    if(argc == 1)
        cout << "Need to input the file name. " << endl;

    resolution_proof_system obj;
    obj.parser_v2(argv);
    //cout << "HI~" << endl;
    //obj.merge_kb_and_query_inv();
    obj.process_buffer();
    //cout << "HI" << endl;
    //obj.print_buffer();
    obj.turn_into_CNF();
    if(obj.FOL_resolution()==true)
        {
        obj.prove_or_not = true;
        //cout << "PROVE" << endl;
        }
    else
        {
        obj.prove_or_not = false;
        //cout << "DISPROVE" << endl;
        }
    //obj.formal_output_2();
    return 0;
    }
