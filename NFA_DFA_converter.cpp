#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <vector>
#include <queue>
#include <map>
#include <sys/time.h>
#include <iomanip>

using namespace std;

class entry
	{
	public:
		entry(int n);
		int fill_transit_states_collection(multimap<int,int> &zero_transit_map, multimap<int,int> &one_transit_map, vector<entry*> &entry_table, queue<entry*> &new_entry_Q, int entry_counter);
		int idx;
		vector<int> NFA_states_collection;
		vector<int> zero_transit_states_collection;
		vector<int> one_transit_states_collection;
		entry* zero_transit_set;
		entry* one_transit_set;
		int zero_next;
		int one_next;
	private:
		// void add_transit_states(int n);
	};

entry::entry(int n)
	{
	idx = n;
	return;
	}

int entry::fill_transit_states_collection(multimap<int,int> &zero_transit_map, multimap<int,int> &one_transit_map, vector<entry*> &entry_table, queue<entry*> &new_entry_Q, int entry_counter)
	{
	entry* temp_entry;
	int i, j, temp_state;
	bool flag;
	i = 0;
	cout << "============ Entry " << idx << " ============" << endl;
	cout << " + NFA_states_collection: ";
	while ( i < NFA_states_collection.size() )
		{
		cout << NFA_states_collection[i] << ", ";
		for (multimap<int,int>::iterator it = zero_transit_map.equal_range( NFA_states_collection[i] ).first; it != zero_transit_map.equal_range( NFA_states_collection[i] ).second; ++it) 
			{
            // cout << it->first << "\t" << it->second << endl ;
			j = 0;
			flag = true;
			while( j < zero_transit_states_collection.size() )
				{
				if( zero_transit_states_collection[j] == it->second )
					{
					flag = false;
					break;
					}
				++j;
				}
			if( flag == true )
            	zero_transit_states_collection.push_back( it->second );
        	}

		for (multimap<int,int>::iterator it= one_transit_map.equal_range( NFA_states_collection[i] ).first; it != one_transit_map.equal_range( NFA_states_collection[i] ).second; ++it) 
			{
            // cout << it->first << "\t" << it->second << endl ;
            j = 0;
			flag = true;
			while( j < one_transit_states_collection.size() )
				{
				if( one_transit_states_collection[j] == it->second )
					{
					flag = false;
					break;
					}
				++j;
				}
			if( flag == true )
            	one_transit_states_collection.push_back( it->second );
        	}
		++i;
		}
	cout << endl;
	cout << " - Content of zero_transit_states_collection: ";
    j = 0;
	while( j < zero_transit_states_collection.size() )
		{
		cout << zero_transit_states_collection[j] << ", ";
		++j;
		}
	cout << endl;

	cout << " - Content of one_transit_states_collection: ";
    j = 0;
	while( j < one_transit_states_collection.size() )
		{
		cout << one_transit_states_collection[j] << ", ";
		++j;
		}
	cout << endl;
	// See if we need to put something in the entry table
	flag = true;
	i = 0;
	while ( i < entry_table.size() )
		{
		// cout << "Hi" << endl;
 		if( entry_table[i]->NFA_states_collection.size() == zero_transit_states_collection.size() )
			{
			// Possibly a same state
			flag = false;
			j = 0;
			while( j < entry_table[i]->NFA_states_collection.size() )
				{
				if( entry_table[i]->NFA_states_collection[j] !=  zero_transit_states_collection[j] )
					{
					// Not a same state
					flag = true;
					break;
					}
				++j;
				}
			if( flag == false )
				{
				// We got a same state here
				// cout << "Fuck off" << endl;
				zero_next = i;
				break;
				}
			}
		++i;
		}
	if(flag)
		{
		// We got a new state
		temp_entry = new entry(entry_counter);
		i = 0;
		while( i < zero_transit_states_collection.size() )
			{
			temp_entry->NFA_states_collection.push_back( zero_transit_states_collection[i] );
			cout << zero_transit_states_collection[i] << ",";
			++i;
			}
		cout << endl;
		entry_table.push_back( temp_entry );
		new_entry_Q.push( temp_entry );
		zero_next = entry_counter;
		++entry_counter;
		}
	// cout << "11111111111" << endl;
	flag = true;
	i = 0;
	while ( i < entry_table.size() )
		{
		if( entry_table[i]->NFA_states_collection.size() == one_transit_states_collection.size() )
			{
			flag = false;
			j = 0;
			while( j < entry_table[i]->NFA_states_collection.size() )
				{
				if( entry_table[i]->NFA_states_collection[j] !=  one_transit_states_collection[j] )
					{
					// Not a same state
					flag = true;
					break;
					}
				++j;
				}
			if( flag == false )
				{
				// We got a same state here
				// cout << "Fuck off" << endl;
				one_next = i;
				break;
				}
			}
		++i;
		}
	if(flag)
		{
		temp_entry = new entry(entry_counter);
		i = 0;
		while( i < one_transit_states_collection.size() )
			{
			temp_entry->NFA_states_collection.push_back( one_transit_states_collection[i] );
			cout << one_transit_states_collection[i] << ",";
			++i;
			}
		cout << endl;
		entry_table.push_back( temp_entry );
		new_entry_Q.push( temp_entry );
		one_next = entry_counter;
		++entry_counter;
		}
	// cout << "222222222222222222222222222" << endl;

	return entry_counter;
	}

int main()
	{
	multimap<int,int> zero_transit_map;
	multimap<int,int> one_transit_map;
	// multimap<int,int>::iterator it;

	entry* temp_entry;
	entry* entry_ptr;
	vector<entry*> entry_table;
	queue<entry*> new_entry_Q;
	int i, entry_counter, input_1, input_2, flag;

	// Complete the transition table
	cout << "Transit 0: " << endl;
	i = 0;
	while(i < 13)
		{
		cout << "Input" << i << endl;
		cin >> input_1 >> input_2;
		zero_transit_map.insert( pair<int,int>(input_1, input_2) );
		++i;
		}

	cout << "Transit 1: " << endl;
	i = 0;
	while(i < 13)
		{
		cout << "Input" << i << endl;
		cin >> input_1 >> input_2;
		one_transit_map.insert( pair<int,int>(input_1, input_2) );
		++i;
		}

	entry_counter = 0;
	temp_entry = new entry(entry_counter);
	temp_entry->NFA_states_collection.push_back(0);
	entry_table.push_back( temp_entry );
	++entry_counter;
	new_entry_Q.push(temp_entry);

	while( new_entry_Q.empty()!=true )
		{
		entry_ptr = new_entry_Q.front();
		// multimap<int,int> &zero_transit_map, multimap<int,int> &one_transit_map, vector<entry*> &entry_table, queue<entry*> &new_entry_Q, int entry_counter
		entry_counter = entry_ptr->fill_transit_states_collection(zero_transit_map, one_transit_map, entry_table, new_entry_Q, entry_counter);
		cout << entry_counter << endl;
		new_entry_Q.pop();
		}

	cout << endl << "Result:" << endl;
	cout << setw(5) << "State" << setw(4) << "0" << setw(4) << "1" << endl;
	cout << "-------------------------------------" << endl;
	i = 0;
	while( i < entry_table.size() )
		{
		cout << setw(5) << entry_table[i]->idx << setw(4) << entry_table[i]->zero_next << setw(4) << entry_table[i]->one_next << endl;
		++i;
		}

	i = 0;
	while( i < entry_table.size() )
		{
		delete entry_table[i];
		++i;
		}

	return 0;
	}