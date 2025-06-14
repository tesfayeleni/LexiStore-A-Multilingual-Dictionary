//============================================================================
// Description  : HashTable Implementation using C++
//============================================================================

#include<iostream>
#include<fstream>
#include<sstream>
#include<math.h>
#include<iomanip>
#include<list>
#include "hashtable.h"
using namespace std;
#include <set> // for storing favorites uniquely
//======================================================

void help()
{
	cout<<"find <word>                         : Search a word and its meanings in the dictionary."<<endl;
	cout<<"import <path>                       : Import a dictionary file."<<endl;
	cout<<"add <word:meaning(s):language>      : Add a word and/or its meanings (separated by ;) to the dictionary."<<endl;
	cout<<"delTranslation <word:language>      : Delete a specific translation of a word from the dictionary."<<endl;
	cout<<"delMeaning <word:meaning:language>  : Delete only a specific meaning of a word from the dictionary."<<endl;
	cout<<"delWord <word>                      : Delete a word and its all translations from the dictionary."<<endl;
	cout<<"similar <word>                      : Show synonyms of a word"<<endl;
	cout<<"opposite <word>                     : Show antonyms of a word"<<endl;
	cout<<"trending [limit]                    : Show most searched words"<<endl;
	cout<<"favorite add:<word>                 : Add word to favorites"<<endl;
	cout<<"favorite remove:<word>              : Remove word from favorites"<<endl;
	cout<<"favorite list                       : List all favorite words"<<endl;
	cout<<"export <language:filename>          : Export a a given language dictionary to a file."<<endl;
	cout<<"exit                                : Exit the program"<<endl;
}
//======================================================
set<string> favorites;

void handleFavorites(string subcmd) 
{
    string command, word;

	// Use stringstream to split the subcommand using ':' as delimiter
    stringstream ss(subcmd);
    getline(ss, command, ':');  // Get the main subcommand
    getline(ss, word);  // Get the word (if provided)

    // Convert command and word to lowercase
    for (char& c : command) c = tolower(c);
    for (char& c : word) c = tolower(c);

	// Case 1: Add a word to the favorites set
    if (command == "add") 
	{
        if (!word.empty()) 
		{
            favorites.insert(word);  // Insert into the set (no duplicates)
            cout << "Added '" << word << "' to favorites." << endl;
        } 
		else 
		{
            cout << "Please specify a word to add." << endl;
        }
    }
	// Case 2: Remove a word from the favorites set
    else if (command == "remove") 
	{
		// erase() returns 1 if the word existed and was removed
        if (favorites.erase(word)) 
		{
            cout << "Removed '" << word << "' from favorites." << endl;
        } 
		else 
		{
            cout << "Word '" << word << "' is not in favorites." << endl;
        }
    }
	// Case 3: List all favorite words
    else if (command == "list") 
	{
        if (favorites.empty()) 
		{
            cout << "No favorite words yet." << endl;
        } 
		else 
		{
            cout << "Favorite words:" << endl;
            for (const string& fav : favorites)
                cout << "- " << fav << endl;  // Print each word in the set
        }
    }
	// Case 4: Unknown command
    else 
	{
        cout << "Unknown favorite command. Use add:<word>, remove:<word>, or list." << endl;
    }

}

// Save the current favorite words to a file
void saveFavorites() 
{
    ofstream file("favorites.txt");  // Open file for overwriting existing content
    for (const string& word : favorites) 
	{
        file << word << endl; // Write each favorite word on a new line
    }
}

// Load favorite words from the file into the set
void loadFavorites() {
    ifstream file("favorites.txt");  // Open file for reading
    string word;
    while (file >> word) 
	{
        favorites.insert(word); // Add each word to the favorites set
    }
}

string toLower(string str) 
{
	//to convert the string into lower case letters
    for (char& c : str) c = tolower(c);
    return str;
}


int main(int argc, char** args)
{
	HashTable myHashTable(1171891);
	myHashTable.import("en-de.txt");

	loadFavorites(); // Load saved favorite words from file

	cout<<"==================================================="<<endl;
	cout<<"Size of HashTable                = "<<myHashTable.getSize()<<endl;
	cout<<"Total Number of Collisions       = "<<myHashTable.getCollisions()<<endl;
	cout<<"Avg. Number of Collisions/Entry  = "<<setprecision(2)<<float(myHashTable.getCollisions())/myHashTable.getSize()<<endl;	
	cout<<"==================================================="<<endl;
	help();
	string user_input, command, argument1, argument2, argument3;

	while(true)
	{
		user_input=command=argument1=argument2=argument3=""; //clear all old values
		cout<<">";
		getline(cin,user_input);

		stringstream sstr(user_input);
		getline(sstr,command,' ');
		getline(sstr,argument1,':');
		getline(sstr,argument2,':');
		getline(sstr,argument3);

		// Convert command to lowercase to allow case-insensitive commands
        for (char &c : command) 
		{
			c = tolower(c);
		}

		if(command == "find")   		  myHashTable.find(argument1);
		else if(command == "import")		  myHashTable.import(argument1);
		else if(command == "add")		      myHashTable.insert(argument1,argument2,argument3); //word,meaning,language
		else if(command == "delword")         myHashTable.delWord(argument1);
		else if(command == "deltranslation")  myHashTable.delTranslation(argument1,argument2);
		else if(command == "delmeaning")      myHashTable.delMeaning(argument1,argument2,argument3);
		else if(command == "export")          myHashTable.exportData(argument1,argument2);
		else if (command == "similar")        myHashTable.printSynonyms(toLower(argument1));
		else if (command == "opposite")       myHashTable.printAntonyms(toLower(argument1));
		else if (command == "trending")     
		{
			int limit = 10;

			// Check if user_input had an argument after "trending"
			if (!argument1.empty()) 
			{
				stringstream ss(argument1);
				ss >> limit;
			}
			myHashTable.printTrending(limit);
		}
		else if (command == "favorite")       handleFavorites(argument1); // assuming this function is implemented
		else if(command == "help")	  	      help();
		else if(command == "exit")
		{
			saveFavorites();  // Save favorites before exiting
			break;
		}
		else cout<<"Invalid command !!!"<<endl;

	}
	return 0;
}