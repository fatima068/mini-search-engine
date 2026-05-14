# 🔍 Mini Search Engine

A graphical mini search engine built in **C++** with an **SFML** GUI. Search through a collection of text files using exact, phrase, or substring matching — with autocomplete suggestions powered by a Trie, and results ranked by frequency.

> **Data Structures project — Semester 3, FAST-NUCES**

---

## ✨ Features

- **Exact word search** — finds files containing a precise word match
- **Phrase search** — detects multi-word phrases across documents
- **Substring search** — fallback when no exact match is found
- **Autocomplete suggestions** — powered by a Trie as you type
- **Top N ranked words** — view the most frequent words in any loaded file
- **Search history** — browse, and selectively or fully clear past searches
- **Pre-processing pipeline** — tokenization, punctuation removal, and stop word filtering
- **Inverted index** — fast keyword-to-file lookups via a hash table

---

## 📁 Project Structure

```
mini-search-engine/
│
├── main.cpp              # Entry point — SFML window, UI, and event loop
├── dataStructures.hpp    # Trie, InvertedIndexHashTable, safeArray, SearchHistory
├── functions.hpp         # Search logic, indexing, tokenization, ranking
├── customUI.hpp          # SFML UI components (Button, InputBox, ScrollablePanel, etc.)
├── ARIAL.TTF             # Font used by the GUI
├── textFiles/            # Folder containing .txt documents to search
└── run.txt               # Build/run reference
```

---

## 🛠️ Prerequisites

- **g++** with C++17 support (MinGW recommended on Windows)
- **SFML 3.0.2** — [Download here](https://www.sfml-dev.org/download.php)

Extract SFML to `D:/SFML-3.0.2/`, or adjust the paths in the build command to match your install location.

---

## 🔧 Build

Open a terminal in the project directory and run:

```bash
g++ -ID:/SFML-3.0.2/include main.cpp -LD:/SFML-3.0.2/lib -lsfml-graphics -lsfml-window -lsfml-system -o main.exe
```

---

## ▶️ Run

```bash
./main.exe
```

Make sure `ARIAL.TTF` and the `textFiles/` folder are in the **same directory** as `main.exe` when running.

---

## 🖥️ How to Use

The app opens to the home screen with three options:

| Button | What it does |
|---|---|
| **Search Word** | Search across all loaded text files |
| **View Search History** | Browse or clear past searches |
| **Top N Ranked Words** | Enter a file number and N to see the most frequent words |

### Search Screen
- Type a word or phrase into the search box — suggestions appear automatically from the Trie.
- Press **Enter** or click **SEARCH**.
- Results are listed with filenames and match counts. The engine tries exact → phrase → substring matching in that order.

### History Screen
- Lists all previous searches.
- **Clear Most Recent** removes the latest entry; **Clear All History** wipes everything.

### Top N Ranked Words Screen
- Enter a file number (shown during startup) and how many top words to display, then press **Get Words** or **Enter**.

---

## 🧱 Data Structures Used

- **Trie** — autocomplete / prefix suggestions
- **Inverted Index (Hash Table)** — maps words to the files they appear in
- **Custom `safeArray<T>`** — dynamic array implementation
- **Frequency Table (Hash Map)** — per-file word frequency tracking
- **Stack (`SearchHistory`)** — search history with push/pop operations

## Screenshots: 

**Home Page**
<img width="900" height="415" alt="image" src="https://github.com/user-attachments/assets/a23b4018-238d-42af-953a-b24362536c49" />

**Search Word**
<img width="896" height="434" alt="image" src="https://github.com/user-attachments/assets/ed2bb5d4-02f0-45bb-ab59-e70c424f8f99" />

**View & Clear Search History**
<img width="896" height="415" alt="image" src="https://github.com/user-attachments/assets/63a1bf77-a821-4ef1-a716-e25133eb4eaa" />

<img width="897" height="418" alt="image" src="https://github.com/user-attachments/assets/06a6b48c-381e-4a19-8d6c-ec2c0b402cbf" />

**Top N Ranked Words**
<img width="896" height="417" alt="image" src="https://github.com/user-attachments/assets/d8bcd32b-50bd-4176-a964-1e833500a277" />

<img width="894" height="410" alt="image" src="https://github.com/user-attachments/assets/d9a57451-a2d6-4576-ad22-56e88e3c5702" />
