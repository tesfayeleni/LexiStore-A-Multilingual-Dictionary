# LexiStore: A Multilingual Dictionary Using Hash Tables

**LexiStore** is a fast and memory-efficient multilingual dictionary application built in C++. It uses a custom **hash table** to enable rapid storage, retrieval, and manipulation of dictionary entries across different languages.

Designed for the command line, it supports file import/export, lazy deletion, synonym/antonym management, trending words, and a persistent favorites system — all with a focus on performance and clean structure.

---

## Features

-  `find <word>` – Lookup a word and its translations
-  `import <path>` – Import dictionary files (e.g. `en-de.txt`)
-  `add <word:meaning(s):language>` – Add new words (multiple meanings via `;`)
-  `delTranslation <word:language>` – Remove a word’s entire translation in a language
-  `delMeaning <word:meaning:language>` – Delete a single meaning
-  `delWord <word>` – Delete a word and all its translations
-  `similar <word>` – Show synonyms
-  `opposite <word>` – Show antonyms
-  `trending [limit]` – Show the most searched words
-  `favorite add:<word>` / `remove:<word>` / `list` – Manage a favorites list with file persistence
-  `export <language:filename>` – Export a language-specific dictionary to a file
-  `help` – Display available commands
-  `exit` – Save state and quit the program

---

##  Sample Output
```
hello
English: hi;greetings|hey|bye;adieu
German: hallo;gruß
```
Upon starting the program:
```
Size of HashTable = 1500
Total Number of Collisions = 29
Avg. Number of Collisions/Entry = 0.019
```
## Build Instructions

Make sure you have g++ and make installed on your system.

To compile the project, run:
```bash
make
```

To run the program, run:
```bash
./translator
```

To clean up build files, run:
```bash
make clean
```

## File Overview

**main.cpp** – Implements the user interface, command parsing, and main program loop  
**hashtable.cpp** – Implements the hash table (insertions, deletions, lookups)  
**hashtable.h** – Declares the `HashTable` class and supporting data structures  
**Makefile** – Builds the project and handles compilation flags and linking

## Dictionary File Format
When importing dictionary files, the program expects a text file in the following format:
```
language
word: meaning1;meaning2|synonym1;synonym2|antonym1;antonym2
another_word: meaning1|synonym1|antonym1
```
  
- The file should start with the language name (e.g., English, German) on its own line.
- Each word line contains the word, followed by a colon `:` , then meanings separated by semicolons `;` .
- Synonyms and antonyms follow after pipe | separators.
- Multiple synonyms and antonyms are separated by semicolons `;`.

**Example:**
```
English
hello: hi;greetings|hey|bye;adieu
world: earth;globe|planet|void
```
Make sure your dictionary files follow this format for proper import and functionality.


## Why Hash Tables?

This dictionary project leverages hash tables to:

- Provide fast, nearly constant-time lookups for words and translations
- Efficiently handle collisions via linear or quadratic probing
- Implement lazy deletion to optimize removal without compromising performance
- Maintain a flexible structure to support multilingual word storage and retrieval

Using hash tables rather than simpler arrays or trees allows LexiStore to scale gracefully as the dictionary grows, making it ideal for large multilingual datasets and interactive applications.


