#include<iostream>
#include<exception>
#include "hashtable.h"
#include <sstream>
#include <fstream>
#include <cctype>
using namespace std;

//functions of the translation class
Translation::Translation(string meanings, string language)
{
  this->language=language;
  stringstream ss(meanings); //stringstream from the meanings string
  string separatedMeanings; //to store the meanings we extract from the string with ';'

  while (getline(ss, separatedMeanings, ';'))
  {
    //as long as we are able to extract a word from the string of input
    // push it into the vector of meanings
    if (!separatedMeanings.empty())
    {
      this->meanings.push_back(separatedMeanings);
    }

  }
}

void Translation::addMeaning(string newMeanings)
{
  stringstream ss(newMeanings); //stringstream from the meanings string
  string separatedMeanings; //to store the meanings we extract from the string with ';'
 

  while (getline(ss, separatedMeanings, ';'))
  {
    bool exists=false;
    //make all letters lowercase
    for (char &c : separatedMeanings) 
    {
      c = tolower(c);
    }
    //check if the meaning exists already
    for (size_t i=0; i<meanings.size(); i++)
    {
      if (separatedMeanings==meanings[i])
      {
        exists=true;
        break; //no need to check further if already found
      }
    }

    //only add the meaning to the vector if it is npt empty and doesn't exist
    if (!exists && !separatedMeanings.empty())
    {
      meanings.push_back(separatedMeanings);
    }

  }
}


//fucntions of the entry class
Entry::Entry(string word, string meanings,string language)
{
  //check if any of the passed parameters are empty
  if (word.empty() || meanings.empty() || language.empty()) 
  {
    cout<<"Word, meanings, or language cannot be empty."<<endl;
  }

  //make language input case insensitive
  for (char &c:language)
  {
    c=tolower(c);
  }

  this->word=word;
  Translation T(meanings, language);
  translations.push_back(T);
  deleted=false;

}

void Entry::addTranslation(string newMeanings, string language)
{
  //check if any of the passed parameters are empty
  if (newMeanings.empty() || language.empty()) 
  {
    cout<<"Word, meanings, or language cannot be empty."<<endl;
  }

  //make language input case insensitive
  for (char &c:language)
  {
    c=tolower(c);
  }

  bool found=false;

  //check if a tanslation with the same language exists
  for (size_t i=0; i<translations.size(); i++)
  {
    if (translations[i].language==language)
    {
      found=true;

      //make all letters lowercase
      for (char &c : newMeanings) 
      {
        c = tolower(c);
      }

      //add all new meanings to this trnaslation
      translations[i].addMeaning(newMeanings);
      break;

    }
  }

  //if no translation with the same language was found, create a new translation
  if (!found)
  {
    Translation T(newMeanings, language);
    translations.push_back(T);
  }

}

void Entry::addSynonyms(string syns)
{
  // Create a string stream to process the input string
  istringstream ss(syns);
  string synonym;

  // Split the input by semicolons
  while (getline(ss, synonym, ';')) 
  {
    // Check if the synonym is not already in the list to avoid duplicates
    if (find(synonyms.begin(), synonyms.end(), synonym) == synonyms.end()) 
    {
      // Add the synonym to the list
      synonyms.push_back(synonym);
    }
  }

}

void Entry::addAntonyms(string ants)
{
  // Create a string stream to process the input string
  istringstream ss(ants);
  string antonym;

  // Split the input by semicolons
  while (getline(ss, antonym, ';')) 
  {
    // Check if the antonym is not already in the list to avoid duplicates
    if (find(antonyms.begin(), antonyms.end(), antonym) == antonyms.end()) 
    {
      // Add the antonnym to the list
      antonyms.push_back(antonym);
    }
  }

}



void Entry::print()
{
  //loop through translations and print them
  for (size_t i=0; i<translations.size(); i++)
  {
    string displayLanguage=translations[i].language;
    
    //capitalize the first letter for printing
    if (!displayLanguage.empty()) 
    {
      displayLanguage[0] = toupper(displayLanguage[0]);
    }
    cout<<displayLanguage<<" : ";

    for (size_t j=0; j<translations[i].meanings.size(); j++)
    {
      cout<<translations[i].meanings[j];
      //if this meaning is not the last, print ";"
      if (j != translations[i].meanings.size() - 1)
      {
        cout << "; ";
      }
    }
    cout<<endl;
  }
}


//functions of the hash table class
HashTable::HashTable(int capacity)
{
	buckets = new Entry*[capacity];		// Array of Pointers to Entries
	for(int i=0; i<capacity; i++)
		buckets[i] = nullptr;

	this->capacity = capacity;
	this->size = 0;
	this->collisions = 0;
}
unsigned long HashTable::hashCode(string word)
{
  //FNV-1a HASHING!!
  unsigned long hash = 2166136261;  //This is the initial basis used in FNV-1a.
  for (int i = 0; i < word.length(); i++) 
  {
    hash ^= word[i];       //XOR the hash with the ASCII value of the current character to integrate 
    // bits of the current character into the hash
    hash *= 16777619;                // Multiply by a prime to reduce collisions

  }

  return hash % capacity;
  
}

unsigned int HashTable::getSize()
{
  return size;
}
unsigned int HashTable:: getCollisions()
{
  return collisions;
}

void HashTable::import(string path)
{
  // Open the file for reading
  ifstream infile(path);

  //check if the file opened successfully
  if (!infile.is_open())
  {
    cout << "Failed to open file: " << path << endl;
    return;
  }

  string line;
  //first line has the language of the words
  getline(infile, line); 
  string language = line;
  int importedCount = 0; //to keep track of sucessfully imported words

  //read each of the lines that remain
  while (getline(infile, line))
  {
    //skip empty  lines
    if (line.empty()) continue;

    stringstream ss(line);
    string word;
    string meaningPart;

    //extract word before the colon
    if (!getline(ss, word, ':')) continue;
    
    //extract the meanings
    if (!getline(ss, meaningPart)) continue;


    // Extract meanings, synonyms, and antonyms from meaningPart
    size_t firstBar = meaningPart.find('|');
    size_t secondBar = string::npos;
    string meaningsOnly = meaningPart;
    string synonymsPart = "";
    string antonymsPart = "";

    if (firstBar != string::npos)
    {
      secondBar = meaningPart.find('|', firstBar + 1);
      meaningsOnly = meaningPart.substr(0, firstBar);

      // Extract synonyms between first and second bar, or until end if no second bar
      synonymsPart = (secondBar != string::npos)
        ? meaningPart.substr(firstBar + 1, secondBar - firstBar - 1)
        : meaningPart.substr(firstBar + 1);

      // Extract antonyms after second bar, if exists
      antonymsPart = (secondBar != string::npos)
        ? meaningPart.substr(secondBar + 1)
        : "";
    }

    // Now split the meaningsOnly string by semicolons
    stringstream ms(meaningsOnly);
    string singleMeaning;
    bool hasValidMeaning = false;


    // combine all meanings into one string, separated by semicolons
    string combinedMeanings;
    while (getline(ms, singleMeaning, ';'))
    {
      if (!singleMeaning.empty())
      {
        //add to the combined meanings with proper separation
        if (!combinedMeanings.empty()) combinedMeanings += ";";
        combinedMeanings += singleMeaning;
        hasValidMeaning = true;
      }
    }

    //only insert if the word if atleast one valid meaning exists
    if (hasValidMeaning)
    {
      //change all to lowercase letters
      for (char &c : word) c = tolower(c);
      for (char &c : language) c = tolower(c);
      for (char &c : combinedMeanings) c = tolower(c);

      Entry* entry = insert(word, combinedMeanings, language); 
      importedCount++;

      // If the Entry was successfully found and synonyms/antonyms exist
      if (entry && firstBar != string::npos)
      {
        // If the synonyms string is not empty, add synonyms to the Entry
        if (!synonymsPart.empty())
          entry->addSynonyms(synonymsPart);

        // If the antonyms string is not empty, add antonyms to the Entry  
        if (!antonymsPart.empty())
          entry->addAntonyms(antonymsPart);
      }

    }

  }

  //close the file
  infile.close();

  //print the number of imported words
  cout << importedCount << " " << language << " words have been imported successfully." << endl;
}

Entry* HashTable::insert(string word, string meanings,string language)
{
  //change word to lower case
  for (size_t i=0; i<word.length(); i++)
  {
    word[i]=tolower(word[i]);
  }

	//hash the key and map it to fit into the capacity of the table
	unsigned long index = hashCode(word)% capacity;
	//initialize the index for the quadratic probing
	int i=0;

	while(true)
	{

		//quadratic probing
		unsigned long probeIndex = (index+i*i)% capacity; 

		//check if slot is empty or deleted to add here
    if (buckets[probeIndex]==nullptr)
    {
      buckets[probeIndex]= new Entry(word, meanings, language);
      size++;
      return buckets[probeIndex]; // return pointer to the updated Entry
    }

    //check that the slot is not empty but the word matches
    else if (buckets[probeIndex]!=nullptr && buckets[probeIndex]->word==word)
    {
      //if the slot is deleted, unmark it and then add a new translation
      if (buckets[probeIndex]->deleted)
      {
        buckets[probeIndex]->deleted=false;
        buckets[probeIndex]->addTranslation(meanings, language);
        size++;
        return buckets[probeIndex]; // return pointer to the updated Entry
      }
      //if the slot is not marked deleted but occupied, 
      // just add the translation to the existing translations
      else
      {
        buckets[probeIndex]->addTranslation(meanings, language);
        return buckets[probeIndex];  // return pointer to the existing Entry
      }
    }
    //if none of the cases above, then a collsion must have occured
    else
    {
      i++;
		  collisions++;
      //after repeated increases of the index, if capacity is reached, print a message
		  if (i==capacity)
		  {
			  cout<<"HashTable is full!! Could not insert:"<<word<<endl;
			  return nullptr;
		  }
    }
		
	}

}


void HashTable::find(string word)
{
  //convert input word to lowercase to ensure case-insensitive search
  for (int i = 0; i < word.length(); i++)
  {
    word[i] = tolower(word[i]);
  }

  //initialize comparison counter
  int comparisons = 0;

  //calculate hash index using the hash function
  int index = hashCode(word);
  int originalIndex = index;
  int i = 0; // changed from i = 1 to start correctly from 0

  //start quadratic probing to find the word
  while (i < capacity)
  {
    int probeIndex = (originalIndex + i * i) % capacity;

    if (buckets[probeIndex] == nullptr)
      break;

    comparisons++; // increment comparison for each bucket probed

    //copy the stored word and convert to lowercase
    string storedWord = buckets[probeIndex]->word;

    for (int j = 0; j < storedWord.length(); j++)
    {
      storedWord[j] = tolower(storedWord[j]);
    }

    //check if stored word matches input word and the bucket is not marked deleted
    if (storedWord == word && !buckets[probeIndex]->deleted)
    {
      vector<string> languages;
      vector<vector<string>> meaningsGrouped;
      int langCount = 0;

      // loop over all translations for the found word
      for (size_t k = 0; k < buckets[probeIndex]->translations.size(); k++)
      {
        string lang = buckets[probeIndex]->translations[k].language;

        // Check if this language already exists in the languages vector
        int probeLangIndex = -1;
        for (size_t m = 0; m < languages.size(); m++)
        {
          if (languages[m] == lang)
          {
            probeLangIndex = m; // Found existing language
            break;
          }
        }

        // If it's a new language, add it and initialize a new meanings group
        if (probeLangIndex == -1)
        {
          probeLangIndex = langCount;
          languages.push_back(lang);
          meaningsGrouped.push_back(vector<string>());
          langCount++;
        }

        // Add all meanings from this translation to the language group
        for (size_t n = 0; n < buckets[probeIndex]->translations[k].meanings.size(); n++)
        {
          meaningsGrouped[probeLangIndex].push_back(buckets[probeIndex]->translations[k].meanings[n]);
        }
      }

      // Print out grouped meanings by language
      if (storedWord == word && !buckets[probeIndex]->deleted)
      {
        buckets[probeIndex]->search_count++;  // Track how often the word is searched

        cout << word << " found in the Dictionary after " << comparisons << " comparisons." << endl;
        buckets[probeIndex]->print(); // <-- call your Entry print function here
        
        cout << "Synonyms: ";
        for (const auto& s : buckets[probeIndex]->synonyms) cout << s << "; ";

        cout << "\nAntonyms: ";
        for (const auto& a : buckets[probeIndex]->antonyms) cout << a << "; ";

        cout << "\n";

        return;
        
      }
      return;
    }

    i++; // move to next probe
  }

  //if word is not found after probing
  cout << word << " not found in the Dictionary" << endl;

}

void HashTable::printTrending(int limit) 
{
    vector<Entry*> entries; // Vector to hold valid (non-deleted) entries

    // Collect all valid entries from the hash table
    for (int i = 0; i < capacity; i++) 
    {
      if (buckets[i] && !buckets[i]->deleted) 
      {
          entries.push_back(buckets[i]);
      }
    }

    // Sort the entries in descending order by search_count (most searched first)
    sort(entries.begin(), entries.end(), [](Entry* a, Entry* b) 
    {
      return a->search_count > b->search_count;
    });

    // Print the top 'limit' trending words
    for (int i = 0; i < min(limit, (int)entries.size()); i++) 
    {
      cout << entries[i]->word << "\t[";

      int bars = entries[i]->search_count / 5; // Scale search count to visual bar length

      // Print a bar made of '=' symbols proportional to the search count
      for (int j = 0; j < bars; j++) 
          cout << "=";

      // Display the number of searches
      cout << "] " << entries[i]->search_count << " searches\n";
    }
}

void HashTable::delWord(string word)
{
  //make the word lowercase to make function case-insensitive
  for (int i = 0; i < word.length(); i++)
  {
    word[i] = tolower(word[i]);
  }

  //hash the key and map it to fit into the capacity of the table
	unsigned long index = hashCode(word)% capacity;
	//initialize the index for the quadratic probing
	int i=0;

	while(true)
	{

		//quadratic probing
		unsigned long probeIndex = (index+i*i)% capacity; 

    //if the slot is empty, then the word doesnt exist
    if (buckets[probeIndex]==nullptr)
    {
      cout<<"Word not found"<<endl;
      return;
    }

    //otherwise, check if the word is found and not deleted
    else if(buckets[probeIndex]->word==word && !buckets[probeIndex]->deleted)
    {
      buckets[probeIndex]->deleted=true;
      size--;
      cout<<word<<" successfully deleted"<<endl;
      return;
    }
    else
    {
      i++;
    }
    
    //after repeated increases of the index, if capacity is reached, print a message
    if (i==capacity)
    {
      cout<<word<<" not found in Dictionary."<<endl;
      return;
    }
    
  }

}

void HashTable::delTranslation(string word, string language)
{
  // Convert word and language to lowercase for case-insensitive search
  for (int i = 0; i < word.length(); i++)
  {
    word[i] = tolower(word[i]);
  }
  for (int i = 0; i < language.length(); i++)
  {
    language[i] = tolower(language[i]);
  }


  //hash the key and map it to fit into the capacity of the table
	unsigned long index = hashCode(word)% capacity;
	//initialize the index for the quadratic probing
	int i=0;

	while(true)
	{

		//quadratic probing
		unsigned long probeIndex = (index+i*i)% capacity; 

    //if the slot is empty, then the word doesnt exist
    if (buckets[probeIndex]==nullptr)
    {
      cout<<"Word not found"<<endl;
      return;
    }

    //otherwise, check if the word is found and not deleted
    if (buckets[probeIndex]->word == word && !buckets[probeIndex]->deleted)
    {
      bool removed=false;

      //delete the translation for the given word
      for (size_t i=0; i<buckets[probeIndex]->translations.size(); i++)
      {
        if (buckets[probeIndex]->translations[i].language == language)
        {
          buckets[probeIndex]->translations.erase(buckets[probeIndex]->translations.begin() + i);
          removed=true;
          //if the translation is the last translation, delete the word
          if (buckets[probeIndex]->translations.size()==0)
          {
            buckets[probeIndex]->deleted=true;
            size--;
          }

          //stop the loop after deleting the translation
          break;

        }

      }
      if (removed)
        cout << language << " translation for \"" << word << "\" successfully deleted!" << endl;
      else
       cout << "Translation in " << language << " not found for \"" << word << "\"" << endl;

      return;

    }
    else
    {
      i++;
    }
    
    //after repeated increases of the index, if capacity is reached, print a message
    if (i==capacity)
    {
      cout<<"Word not found"<<endl;
      return;
    }
    
  }

}
void HashTable::delMeaning(string word, string meaning, string language)
{
  // Convert word, language, and meaning to lowercase for case-insensitive search
  for (int i = 0; i < word.length(); i++)
  {
    word[i] = tolower(word[i]);
  }
  for (int i = 0; i < language.length(); i++)
  {
    language[i] = tolower(language[i]);
  }
  for (int i = 0; i < meaning.length(); i++)
  {
    meaning[i] = tolower(meaning[i]);
  }


  //hash the key and map it to fit into the capacity of the table
  unsigned long index = hashCode(word) % capacity;
  //initialize the index for the quadratic probing
  int i = 0;

  while (true)
  {
    //quadratic probing
    unsigned long probeIndex = (index + i*i) % capacity;

    //if the slot is empty, then the word doesn't exist
    if (buckets[probeIndex] == nullptr)
    {
      cout << "Word not found" << endl;
      return;
    }

    //otherwise, check if the word is found and not deleted
    if (buckets[probeIndex]->word == word && !buckets[probeIndex]->deleted)
    {
      bool foundTranslation = false;
      bool removedMeaning = false;

      //look for the translation in the specified language
      for (size_t j = 0; j < buckets[probeIndex]->translations.size(); j++)
      {
        if (buckets[probeIndex]->translations[j].language == language)
        {
          foundTranslation = true;

          //find the specific meaning in the meanings vector
          vector<string>& meanings = buckets[probeIndex]->translations[j].meanings;
          for (size_t k = 0; k < meanings.size(); k++)
          {
            if (meanings[k] == meaning)
            {
              meanings.erase(meanings.begin() + k);
              removedMeaning = true;
              break;
            }
          }

          //if that was the last meaning, delete the whole translation
          if (meanings.size() == 0)
          {
            buckets[probeIndex]->translations.erase(buckets[probeIndex]->translations.begin() + j);

            //if that was the last translation, delete the word
            if (buckets[probeIndex]->translations.size() == 0)
            {
              buckets[probeIndex]->deleted = true;
              size--;
            }
          }

          break; // no need to continue searching
        }
      }

      if (!foundTranslation)
        cout << "Translation in " << language << " not found for \"" << word << "\"" << endl;

      else if (removedMeaning)
        cout << "Meaning \"" << meaning << "\" in " << language << " translation for \"" << word << "\" successfully deleted!" << endl;

      else
        cout << "Meaning \"" << meaning << "\" not found in " << language << " translation for \"" << word << "\"" << endl;

      return;
    }


    else
    {
      i++;
    }

    //after repeated increases of the index, if capacity is reached, print a message
    if (i == capacity)
    {
      cout << "Word not found" << endl;
      return;
    }

  }


}

void HashTable::exportData(string language, string filePath)
{
  //open the file for writing
  ofstream outfile(filePath);

  //check if the file was opened successfully
  if (!outfile.is_open())
  {
    cout << "Failed to open file for writing " << filePath << endl;
    return;
  }

  // Convert language to lowercase for case-insensitive match
  for (int i = 0; i < language.length(); i++)
  {
    language[i] = tolower(language[i]);
  }

  int exportedCount = 0;

  //write the language as the first line in the file
  outfile << language << endl;

  //loop through all buckets in the hash table
  for (int i = 0; i < capacity; i++)
  {
    Entry* entry = buckets[i];

    //check if the bucket is used and not marked as deleted
    if (entry != nullptr && !entry->deleted)
    {
      //prepare the line in "word:meaning1;meaning2;..." format for the given language
      vector<string> meanings;

      for (Translation t : entry->translations)
      {
        //make each language lowecase to be case insensitive
        string entryLang = t.language;
        for (int i = 0; i < entryLang.length(); i++) 
        {
          entryLang[i] = tolower(entryLang[i]);
        }

        if (entryLang == language)
        {
          for (string m : t.meanings)
          {
            meanings.push_back(m);
          }
        }
      }

      //only write if we found at least one meaning for the language
      if (!meanings.empty())
      {
        outfile << entry->word << ":";
        for (size_t j = 0; j < meanings.size(); j++)
        {
          outfile << meanings[j];
          if (j != meanings.size() - 1)
            outfile << ";";
        }
        outfile << endl;
        exportedCount++;
      }
    }
  }

  //close the output file
  outfile.close();

  //print success message
  cout << exportedCount << " records have been successfully exported to " << filePath << endl;


}

void HashTable::exportAnalytics(string filename) 
{
    ofstream file(filename);  // Open the output file to write the analytics report
    
    file << "--- WORD ANALYTICS REPORT ---\n\n";  // Header for the report
    file << "Total words: " << size << "\n";    // Print total number of words in the hash table
    
    // Variables to accumulate total synonyms and antonyms counts
    int totalSynonyms = 0, totalAntonyms = 0;
    
    // Loop over all buckets in the hash table
    for (int i = 0; i < capacity; i++) {
        // Check if the bucket is not empty and not marked deleted
        if (buckets[i] && !buckets[i]->deleted) {
            // Add the size of synonyms and antonyms vectors for this word
            totalSynonyms += buckets[i]->synonyms.size();
            totalAntonyms += buckets[i]->antonyms.size();
        }
    }
    
    // Calculate and write average number of synonyms and antonyms per word
    file << "Avg synonyms/word: " << (totalSynonyms / (double)size) << "\n";
    file << "Avg antonyms/word: " << (totalAntonyms / (double)size) << "\n\n";
    
    // Write heading for the list of top searched words
    file << "TOP SEARCHED WORDS:\n";
    
    // Call existing function to print the top 10 trending (most searched) words
    printTrending(10);  // Assumes printTrending writes to the same output stream or console
}

HashTable::~HashTable() 
{
    // Loop through each bucket in the hash table
    for (unsigned int i = 0; i < capacity; ++i) 
    {
        // Check if the bucket is not nullptr 

        if (buckets[i] != nullptr) 
        {
            buckets[i]->translations.clear();  // Clear meanings inside Entry
            // Delete the Entry object in this bucket
            delete buckets[i];
            buckets[i]=nullptr;
        }
    }
    // Delete the array of bucket pointers
    delete[] buckets;
}
