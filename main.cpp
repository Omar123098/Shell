/**
 * Windows Shell Implementation in C++
 *
 * A custom shell implementation designed for Windows systems with advanced interactive features.
 * This shell provides a readline-like interface with tab completion, command history, cursor
 * navigation, and built-in commands.
 *
 * Features:
 * - Custom readline implementation using Windows console APIs
 * - Tab completion for commands and files
 * - Command history with persistent storage
 * - Full cursor navigation (left/right/up/down arrows, backspace, delete)
 * - Built-in commands: echo, pwd, cd, ls, cat, type, history, exit
 * - I/O redirection support (>, >>, 1>, 1>>)
 * - Windows-compatible path handling
 *
 * Key Components:
 * - readline(): Custom input handler with advanced editing capabilities
 * - find_completion(): Tab completion system for commands and files
 * - Command handlers: Individual functions for each built-in command
 * - History management: Persistent command history with file storage
 *
 * Author: Omar Elgedawy
 * Date: August 2025
 * Platform: Windows (using MinGW/GCC)
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <unistd.h>
#include <fstream>
#include <dirent.h>
#include <cstring>
#include <conio.h>
#include <windows.h>

using namespace std;

// Global variables
vector<string> commands = {"history", "cat", "ls", "echo", "type", "exit", "pwd", "cd"}; // List of built-in commands
vector<string> history;                                                                  // In-memory command history storage

/**
 * Find completion candidates for tab completion
 *
 * This function implements a two-stage completion system:
 * 1. First, it searches through built-in commands for matches
 * 2. If no command matches, it searches the current directory for files/folders
 *
 * @param partial The partial string to complete (e.g., "ec" for "echo")
 * @return Vector of matching completion candidates
 */
vector<string> find_completion(const string &partial)
{
    vector<string> results;
    for (const string &cmd : commands)
    {
        if (cmd.substr(0, partial.length()) == partial)
        {
            results.push_back(cmd);
        }
    }
    if (!results.empty())
        return results;
    char path[1024];
    getcwd(path, sizeof(path));
    DIR *dir = opendir(path);
    if (dir == nullptr)
        return {};
    vector<string> contents;
    dirent *entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        if (entry->d_name[0] == '.')
            continue; // Skip hidden files
        string tst = entry->d_name;
        if (tst.substr(0, partial.length()) == partial)
        {
            contents.push_back(entry->d_name);
        }
    }
    closedir(dir);
    if (!contents.empty())
        return contents;
    return {};
}

/**
 * Count exact matches for tab completion
 *
 * Counts how many completion candidates exactly match the partial string.
 * Used to determine if there's a unique completion (count=1) or multiple options.
 *
 * @param completion Vector of completion candidates
 * @param word_to_complete Partial string to match against
 * @return Number of exact matches
 */
int test(const vector<string> &completion, const string &word_to_complete)
{
    int cnt = 0;
    for (const auto &cmd : completion)
    {
        if (cmd.substr(0, word_to_complete.length()) == word_to_complete)
            cnt++;
    }
    return cnt;
}

/**
 * Custom readline implementation for Windows
 *
 * This is the core input handling function that provides advanced line editing capabilities.
 * It replaces the GNU readline library with a Windows-compatible implementation using _getch().
 *
 * Key Features:
 * - Character-by-character input processing
 * - Cursor movement with left/right arrows
 * - Command history navigation with up/down arrows
 * - Tab completion with double-tab to show options
 * - Backspace and Delete key support
 * - Insert mode character insertion at cursor position
 *
 * Key Bindings:
 * - Enter: Submit command
 * - Backspace: Delete character before cursor
 * - Delete: Delete character after cursor
 * - Tab: Auto-complete command/filename
 * - Up/Down Arrows: Navigate command history
 * - Left/Right Arrows: Move cursor within line
 * - Ctrl+D: Exit shell
 *
 * @return Dynamically allocated C-string containing the input line
 *         (caller must delete[] the returned pointer)
 */
char *readline()
{
    string line;
    char ch, lst;
    int history_index = -1;     // Changed from static to local - reset each call
    string current_line_backup; // backup of current line when navigating history
    int cursor_pos = 0;         // cursor position in the line

    while (true)
    {
        ch = _getch();

        if (ch == '\r')
        { // Enter key
            cout << endl;
            break;
        }
        else if (ch == '\b' || ch == 127)
        { // Backspace
            if (!line.empty() && cursor_pos > 0)
            {
                line.erase(cursor_pos - 1, 1);
                cursor_pos--;

                // Move cursor back and reprint rest of line with trailing space to clear
                cout << "\b";
                string remaining = line.substr(cursor_pos);
                cout << remaining << " ";

                // Move cursor back to correct position
                for (int i = (int)remaining.length() + 1; i > 0; i--)
                {
                    cout << "\b";
                }
            }
        }
        else if (ch == '\t')
        { // Tab key for completion
            string word_to_complete;
            size_t space_pos = line.rfind(' ');
            if (space_pos == string::npos)
            {
                word_to_complete = line; // No spaces, entire line is the word
            }
            else
            {
                word_to_complete = line.substr(space_pos + 1);
            }
            vector<string> completion = find_completion(word_to_complete);
            if (lst == '\t')
            {
                if (completion.size() > 1)
                {
                    cout << endl;
                    for (const auto &cmd : completion)
                    {
                        cout << cmd << "    ";
                    }
                    cout << endl;
                    cout << "$ ";
                    cout << line << flush;
                    // Reset cursor position after redraw
                    for (int i = line.length(); i > cursor_pos; i--)
                    {
                        cout << "\b";
                    }
                }
            }
            else if (!completion.empty() && test(completion, word_to_complete) == 1)
            {
                // Clear current line
                for (size_t i = 0; i < line.length(); i++)
                {
                    cout << "\b \b";
                }
                if (space_pos == string::npos)
                {
                    line = completion[0]; // Replace entire line
                }
                else
                {
                    line = line.substr(0, space_pos + 1) + completion[0];
                }
                cursor_pos = line.length(); // Move cursor to end after completion
                cout << line;
            }
            else
            {
                // Ring bell when no completion is found
                MessageBeep(MB_OK); // Windows system beep
            }
        }
        else if (ch == -32 || ch == 224) // Arrow keys start with these escape codes
        {
            ch = _getch(); // Get the actual arrow key code
            if (ch == 72)  // Up arrow
            {
                if (!history.empty())
                {
                    // Save current line if we're not already in history navigation
                    if (history_index == -1)
                    {
                        current_line_backup = line;
                        history_index = history.size() - 1;
                    }
                    else if (history_index > 0)
                    {
                        history_index--;
                    }

                    // Move cursor to beginning of line
                    for (int i = 0; i < cursor_pos; i++)
                    {
                        cout << "\b";
                    }

                    // Clear entire line by overwriting with spaces
                    for (size_t i = 0; i < line.length(); i++)
                    {
                        cout << " ";
                    }

                    // Move cursor back to beginning
                    for (size_t i = 0; i < line.length(); i++)
                    {
                        cout << "\b";
                    }

                    // Display history item
                    line = history[history_index];
                    cursor_pos = line.length(); // Move cursor to end
                    cout << line;
                }
            }
            else if (ch == 80) // Down arrow
            {
                if (history_index != -1)
                {
                    // Store current line length for clearing
                    size_t old_line_length = line.length();

                    if (history_index < (int)history.size() - 1)
                    {
                        history_index++;
                        line = history[history_index];
                    }
                    else
                    {
                        // Restore the backup line when moving past the latest history
                        history_index = -1;
                        line = current_line_backup;
                    }

                    // Move cursor to beginning of line
                    for (int i = 0; i < cursor_pos; i++)
                    {
                        cout << "\b";
                    }

                    // Clear entire line by overwriting with spaces
                    for (size_t i = 0; i < old_line_length; i++)
                    {
                        cout << " ";
                    }

                    // Move cursor back to beginning
                    for (size_t i = 0; i < old_line_length; i++)
                    {
                        cout << "\b";
                    }

                    cursor_pos = line.length(); // Move cursor to end
                    cout << line;
                }
            }
            else if (ch == 75) // Left arrow
            {
                if (cursor_pos > 0)
                {
                    cursor_pos--;
                    cout << "\b"; // Move cursor left
                }
            }
            else if (ch == 77) // Right arrow
            {
                if (cursor_pos < (int)line.length())
                {
                    cursor_pos++;
                    cout << line[cursor_pos - 1]; // Move cursor right by printing character
                }
            }
            else if (ch == 83) // Delete key
            {
                if (!line.empty() && cursor_pos < (int)line.length())
                {
                    line.erase(cursor_pos, 1);

                    // Reprint rest of line with trailing space to clear
                    string remaining = line.substr(cursor_pos);
                    cout << remaining << " ";

                    // Move cursor back to correct position
                    for (int i = (int)remaining.length() + 1; i > 0; i--)
                    {
                        cout << "\b";
                    }
                }
            }
        }
        else if (ch >= 32 && ch <= 126)
        { // Printable characters
            // Insert character at cursor position
            line.insert(cursor_pos, 1, ch);
            cursor_pos++;

            // Clear from cursor to end and reprint
            string remaining = line.substr(cursor_pos - 1);
            cout << remaining;

            // Move cursor back to correct position
            for (int i = (int)remaining.length() - 1; i > 0; i--)
            {
                cout << "\b";
            }
        }
        else if (ch == 4)
        {
            // Ctrl+D
            exit(1);
        }
        lst = ch;
    }

    char *result = new char[line.length() + 1];
    strcpy(result, line.c_str());
    return result;
}

// Global variables for I/O redirection
int fnd = -1;   // Position index for redirection operator in command string
string ap = ""; // Redirection operator type ("1>", ">", "1>>", ">>")

/**
 * Parse and process echo command with quote handling and I/O redirection
 *
 * This function handles complex echo command parsing including:
 * - Single and double quote processing
 * - Escape sequence handling (\")
 * - I/O redirection detection (>, >>, 1>, 1>>)
 * - Whitespace normalization
 *
 * The function sets global variables (fnd, ap) when redirection is detected
 * to be used by the main command processor.
 *
 * @param input Full command string starting with "echo"
 * @return Processed message string (without "echo" prefix and redirection)
 */
string echo(string input)
{
    if (input.size() < 5)
    {
        cerr << "Error: Invalid command format. Use 'echo <message>'.";
        return "";
    }
    string message = input.substr(5);
    int token = 0;
    char c;
    char prev = '-';
    string result = "";
    for (int i = 0; i < message.length(); i++)
    {
        if (token == 1)
        {
            if (c == '\"' && prev == '\\')
            {
                result += message[i];
            }
            else if (message[i] == '\'' && c == '\'')
            {
                token = 0;
            }
            else if (message[i] == '\"' && c == '\"')
            {
                token = 0;
            }
            else if (c != '\"' || message[i] != '\\')
            {
                result += message[i];
            }
        }
        else
        {
            if ((message[i] == '1' && i + 1 < message.length() && message[i + 1] == '>') || message[i] == '>')
            {
                if (message[i] == '1')
                {
                    if (i + 1 < message.length() && message[i + 1] == '>')
                    {
                        ap = "1>>";
                        fnd = i + 7;
                    }
                    else
                    {
                        ap = "1>";
                        fnd = i + 6;
                    }
                }
                else
                {
                    if (i + 1 < message.length() && message[i + 1] == '>')
                    {
                        ap = ">>";
                        fnd = i + 6;
                    }
                    else
                    {
                        ap = ">";
                        fnd = i + 5;
                    }
                }
                return result;
            }
            else if (message[i] == ' ')
            {
                if (prev != ' ')
                    result += ' ';
            }
            else if (prev == '\\')
            {
                result += message[i];
            }
            else if (message[i] == '\'' || message[i] == '\"')
            {
                token = 1;
                c = message[i];
            }
            else if (message[i] != '\\')
            {
                if (message[i] != ' ' || prev != ' ')
                    result += message[i];
            }
        }
        prev = message[i];
    }
    return result;
}

/**
 * Print shell prompt
 * Simple function to display the shell prompt "$ " with flush to ensure immediate output.
 */
void print_prompt()
{
    cout << "$ " << flush;
}

/**
 * Check if a command is a built-in command
 *
 * @param input Command name to check
 * @return true if the command is in the built-in commands list, false otherwise
 */
bool found(string input)
{
    return find(commands.begin(), commands.end(), input) != commands.end();
}

/**
 * Search for executable files in PATH environment variable
 *
 * This function searches through all directories in the Windows PATH environment
 * variable to find executable files. It handles Windows-specific path separators
 * and file extensions (.exe).
 *
 * @param input Command name to search for
 * @return Full path to executable if found, "f" if not found
 */
string found2(string input)
{
    vector<string> paths;
    string cur = "";
    string path = getenv("PATH");

    for (char c : path)
    {
        if (c == ';') // Fixed: Windows uses semicolon, not colon
        {
            paths.push_back(cur);
            cur = "";
        }
        else
        {
            cur += c;
        }
    }
    paths.push_back(cur);

    for (const string &p : paths)
    {
        string full_path = p + "\\" + input + ".exe"; // Fixed: Windows path separator and .exe extension
        if (access(full_path.c_str(), 0) == 0)        // Check if file exists
        {
            return full_path;
        }
        // Also try without .exe extension
        full_path = p + "\\" + input;
        if (access(full_path.c_str(), 0) == 0)
        {
            return full_path;
        }
    }
    return "f";
}

/**
 * Implementation of 'type' built-in command
 *
 * Shows information about a command - whether it's a shell built-in or not found.
 * Similar to the 'type' command in bash.
 *
 * @param input Full command string (e.g., "type echo")
 */
void type(string input)
{
    string substr = input.substr(5);
    cout << substr;
    if (found(substr))
        cout << " is a shell builtin" << "\n";
    else
        cout << ": not found\n";
}

/**
 * Implementation of 'cd' built-in command
 *
 * Changes the current working directory with support for:
 * - Windows home directory (~) using USERPROFILE environment variable
 * - Proper whitespace trimming
 * - Error handling for invalid directories
 *
 * @param input Full command string (e.g., "cd /path/to/dir")
 */
void cd(string input)
{
    if (input == "cd")
        input = "cd ";
    string path = input.substr(3);
    // Trim whitespace
    path.erase(0, path.find_first_not_of(" \t"));
    path.erase(path.find_last_not_of(" \t") + 1);

    if (path == "~")
    {
        // Try Windows home directory environment variables
        char *home = getenv("USERPROFILE"); // Windows standard
        if (home == nullptr)
            home = getenv("HOME"); // Fallback
        if (home != nullptr)
        {
            path = home;
        }
        else
        {
            cout << "cd: HOME environment variable not set" << endl;
            return;
        }
    }
    if (chdir(path.c_str()) != 0)
    {
        cout << "cd: " << path << ": No such file or directory" << endl;
    }
}

/**
 * Implementation of 'cat' built-in command
 *
 * Reads and returns the contents of a file. Handles:
 * - Whitespace trimming from filename
 * - File opening error handling
 * - Line-by-line reading with newline preservation
 *
 * @param input Full command string (e.g., "cat filename.txt")
 * @return File contents as string, or "-1" if file cannot be opened
 */
string cat(string input)
{
    vector<string> args;
    input = input.substr(4);
    string cur = "";
    for (auto c : input)
    {
        if (c == ' ')
        {
            args.push_back(cur);
            cur = "";
        }
        else
        {
            cur += c;
        }
    }
    args.push_back(cur);
    string contents;
    string file = args[0];
    file.erase(0, file.find_first_not_of(" \t"));
    file.erase(file.find_last_not_of(" \t") + 1);
    ifstream infile(file);
    if (!infile.is_open())
    {
        cout << "Error: Could not open file " << file << endl;
        return "-1";
    }
    string line;
    while (getline(infile, line))
    {
        contents += line + "\n";
    }
    infile.close();
    return contents;
}

/**
 * Write content to file (overwrite mode)
 *
 * Handles output redirection for commands using > or 1> operators.
 * Creates or overwrites the target file with the provided content.
 *
 * @param contents Content to write to file
 * @param input Command string containing redirection (e.g., "> output.txt")
 */
void put(string contents, string input)
{
    if (contents == "-1")
        return;
    int pos = input.find(">");
    string filename = input.substr(pos + 1);
    filename.erase(0, filename.find_first_not_of(" \t"));
    filename.erase(filename.find_last_not_of(" \t") + 1);
    ofstream file(filename, ios::trunc); // Fixed: use trunc instead of app for overwrite
    if (!file.is_open())
    {
        cerr << "Error: Could not open file " << filename << endl;
        return;
    }
    file << contents; // Removed seekp(0) - not needed with trunc
}

/**
 * Implementation of 'ls' built-in command
 *
 * Lists directory contents with support for:
 * - I/O redirection parsing (>, >>, 1>, 1>>)
 * - Default to current directory if no path specified
 * - Hidden file filtering (files starting with '.')
 * - Error handling for invalid directories
 *
 * @param input Full command string (e.g., "ls /path/to/dir")
 * @return Directory contents as string, or "-1" on error
 */
string ls(string input)
{
    if (input == "ls")
        input = "ls ";
    string path = input.substr(3);
    if (path.find("1>>") != string::npos)
    {
        path = path.substr(0, path.find("1>>"));
    }
    else if (path.find(">>") != string::npos)
    {
        path = path.substr(0, path.find(">>"));
    }
    else if (path.find("1>") != string::npos)
    {
        path = path.substr(0, path.find("1>"));
    }
    else if (path.find(">") != string::npos)
    {
        path = path.substr(0, path.find(">"));
    }
    if (path.empty())
    {
        path = ".";
    }
    DIR *dir = opendir(path.c_str());
    if (dir == nullptr)
    {
        cerr << "ls: " << path << ": No such file or directory" << endl;
        return "-1";
    }
    string contents;
    dirent *entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        if (entry->d_name[0] == '.')
            continue;
        contents += entry->d_name;
        contents += "\n";
    }
    closedir(dir);
    return contents;
}

/**
 * Detect I/O redirection operators in command string
 *
 * Analyzes command string to identify redirection operators in priority order.
 * Returns the first match found.
 *
 * @param s Command string to analyze
 * @return Redirection operator found ("1>>", ">>", ">", "1>", or "" if none)
 */
string output_finder(string s)
{
    if (s.find("1>>") != string::npos)
    {
        return "1>>";
    }
    if (s.find(">>") != string::npos)
    {
        return ">>";
    }
    if (s.find(">") != string::npos)
    {
        return ">";
    }
    if (s.find("1>") != string::npos)
    {
        return "1>";
    }
    return "";
}

/**
 * Append content to file (append mode)
 *
 * Handles output redirection for commands using >> or 1>> operators.
 * Appends content to the target file, creating it if it doesn't exist.
 *
 * @param contents Content to append to file
 * @param input Command string containing redirection (e.g., ">> output.txt")
 */
void append(string contents, string input)
{
    if (contents == "-1")
        return;
    string filename = input;
    if (input.find(">>") != string::npos)
        filename = input.substr(input.find(">>") + 2);
    else if (input.find("1>>") != string::npos)
        filename = input.substr(input.find("1>>") + 3);

    filename.erase(0, filename.find_first_not_of(" \t"));
    filename.erase(filename.find_last_not_of(" \t") + 1);
    ofstream file(filename, ios::app);
    if (!file.is_open())
    {
        cerr << "Error: Could not open file " << filename << endl;
        return;
    }
    file << contents << endl;
}

/**
 * Load command history from persistent storage
 *
 * Reads the history.txt file and loads all non-empty lines into the global
 * history vector. This function is called once at shell startup to restore
 * previous session history.
 *
 * The function clears existing history to avoid duplicates and handles
 * cases where the history file doesn't exist gracefully.
 */
void get_history()
{
    history.clear(); // Clear existing history to avoid duplicates
    string file = "history.txt";
    ifstream infile(file);
    if (!infile.is_open())
    {
        return;
    }
    string line;
    while (getline(infile, line))
    {
        if (!line.empty())
        { // Only add non-empty lines
            history.push_back(line);
        }
    }
    infile.close();
}

/**
 * Add command to persistent history storage
 *
 * Appends a new command to both the in-memory history vector and the
 * persistent history.txt file. This ensures command history is preserved
 * across shell sessions.
 *
 * @param command Command string to add to history
 */
void add_history(string command)
{
    ofstream outfile("history.txt", ios::app);
    if (!outfile.is_open())
    {
        cerr << "Error: Could not open history file for writing" << endl;
        return;
    }
    outfile << command << endl;
    outfile.close();
}

/**
 * Main shell loop and command processor
 *
 * This is the main function that implements the shell's REPL (Read-Eval-Print Loop).
 * It handles:
 *
 * 1. Initialization: Load command history from file
 * 2. Input Processing: Use custom readline() for advanced input handling
 * 3. Command Parsing: Extract command and arguments from input
 * 4. Command Execution: Route to appropriate built-in command handlers
 * 5. History Management: Add valid commands to history
 * 6. I/O Redirection: Handle output redirection for supported commands
 * 7. Error Handling: Display "command not found" for unknown commands
 *
 * Built-in Commands Supported:
 * - history [n]: Show command history (last n commands if specified)
 * - echo <text>: Display text with I/O redirection support
 * - type <cmd>: Show command type information
 * - pwd: Print current working directory
 * - cd <path>: Change directory
 * - ls [path]: List directory contents
 * - cat <file>: Display file contents
 * - exit 0: Exit shell
 *
 * The function runs in an infinite loop until 'exit 0' is entered or Ctrl+D is pressed.
 *
 * @return Program exit status (0 for normal termination)
 */
int main()
{
    string input = "";
    get_history(); // Load history only once at startup
    while (1)
    {
        // Flush after every cout / cerr
        cout << unitbuf;
        cerr << unitbuf;
        // Uncomment this block to pass the first stage
        print_prompt();
        char *input_cstr = readline();
        input = string(input_cstr);
        delete[] input_cstr; // Free the allocated memory
        input.erase(0, input.find_first_not_of(" \t"));
        input.erase(input.find_last_not_of(" \t") + 1);
        if (input.empty())
            continue;

        string command = input.substr(0, input.find(" "));
        if (command.empty()) // Additional safety check
            continue;

        add_history(input);
        history.push_back(input);
        if (input == "exit 0")
        {
            exit(0);
        }
        if (found(command))
        {
            if (command == "history")
            {
                int i = 0;
                if (command != input)
                {
                    string num_str = input.substr(8);
                    // Add bounds checking and error handling
                    try
                    {
                        if (num_str.empty())
                        {
                            cerr << "Error: history command requires a number" << endl;
                            continue;
                        }
                        i = stoll(num_str);
                        if (i < 1 || i > (int)history.size())
                        {
                            cerr << "Error: Invalid history index. Range: 1-" << history.size() << endl;
                            continue;
                        }
                        i = history.size() - i;
                    }
                    catch (const std::invalid_argument &e)
                    {
                        cerr << "Error: Invalid number format" << endl;
                        continue;
                    }
                    catch (const std::out_of_range &e)
                    {
                        cerr << "Error: Number out of range" << endl;
                        continue;
                    }
                }
                for (; i < (int)history.size(); i++)
                {
                    cout << i + 1 << ". " << history[i] << endl;
                }
            }
            if (command == "echo")
            {
                string content = echo(input);
                if (fnd != -1)
                {
                    if (ap == "1>" || ap == ">")
                        put(content, input.substr(fnd));
                    else
                        append(content, input.substr(fnd - 1));
                    ap = "";
                    fnd = -1; // Reset fnd for the next command
                }
                else
                {
                    cout << content << endl;
                }
            }
            if (command == "type")
            {
                type(input);
            }
            if (command == "pwd")
            {
                char cwd[1024];
                getcwd(cwd, sizeof(cwd));
                cout << cwd << endl;
            }
            if (command == "cd")
            {
                cd(input);
            }
            if (command == "ls")
            {
                string contents = ls(input);
                if (output_finder(input) == "")
                {
                    cout << contents;
                }
                else if (output_finder(input) == "1>>" || output_finder(input) == ">>")
                {
                    append(contents, input);
                }
                else
                {
                    put(contents, input);
                }
            }
            if (command == "cat")
            {
                string contents = cat(input);
                if (output_finder(input) == "")
                {
                    cout << contents;
                }
                else if (output_finder(input) == "1>>" || output_finder(input) == ">>")
                {
                    append(contents, input);
                }
                else
                {
                    put(contents, input);
                }
            }
        }
        else
        {
            cout << command << ": command not found" << endl;
        }
    }
    return 0;
}
