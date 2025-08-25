# Advanced Windows Shell Implementation

**A fully-featured, Windows-compatible shell with advanced interactive capabilities, tab completion, command history, and comprehensive I/O redirection support.**

Built from scratch in C++ using native Windows APIs for optimal performance and user experience.

## 🚀 Key Features

### Advanced Interactive Shell

- **Custom Readline Implementation**: Built from scratch using Windows APIs (`_getch()`) for immediate key response
- **Full Cursor Navigation**: Move cursor freely within command line using arrow keys
- **Advanced Line Editing**: Insert, delete, and modify text at any cursor position
- **Persistent Command History**: Commands saved to `history.txt` and restored between sessions

### Intelligent Tab Completion System

- **Command Completion**: Auto-complete built-in commands and executables in PATH
- **File/Directory Completion**: Complete filenames and directory names in current directory
- **Smart Completion Logic**:
  - Single match: Automatically completes
  - Multiple matches: Double-tab shows all options
  - No matches: System beep notification
- **Context-Aware**: Completes commands at start of line, files/directories elsewhere

### Enhanced Navigation & History

- **Arrow Key Navigation**:
  - **↑/↓**: Navigate through command history
  - **←/→**: Move cursor within current line
- **Advanced History Features**:
  - `history`: Show all commands with line numbers
  - `history n`: Show last n commands
  - History persistence across sessions
  - Smart history backup during navigation

### Comprehensive Built-in Commands

| Command       | Description              | Advanced Features                                |
| ------------- | ------------------------ | ------------------------------------------------ |
| `echo <text>` | Print text to output     | Quote parsing, escape sequences, I/O redirection |
| `pwd`         | Print working directory  | Windows path format support                      |
| `cd <path>`   | Change directory         | Home directory (`~`) support via `USERPROFILE`   |
| `ls [path]`   | List directory contents  | Hidden file filtering, I/O redirection           |
| `cat <file>`  | Display file contents    | Error handling, I/O redirection                  |
| `type <cmd>`  | Show command information | Built-in vs external command detection           |
| `history [n]` | Show command history     | Optional line count, persistent storage          |
| `exit [0]`    | Exit shell               | Clean shutdown                                   |

### Advanced I/O Redirection

- **Output Redirection**: `>`, `>>`, `1>`, `1>>`
- **Intelligent Parsing**: Handles redirection within echo commands
- **File Operations**: Create, overwrite, and append to files
- **Error Handling**: Graceful handling of file access errors

### Windows-Specific Optimizations

- **Path Handling**: Windows backslash separators and drive letters
- **Environment Variables**: `USERPROFILE` for home directory, `PATH` for executables
- **Console APIs**: Native Windows console functions for optimal performance
- **File Extensions**: Automatic `.exe` extension handling for executables

## 🎮 Key Bindings

| Key         | Action                         |
| ----------- | ------------------------------ |
| `Enter`     | Execute command                |
| `Tab`       | Auto-complete command/filename |
| `Tab` `Tab` | Show all completion options    |
| `↑`         | Previous command in history    |
| `↓`         | Next command in history        |
| `←`         | Move cursor left               |
| `→`         | Move cursor right              |
| `Backspace` | Delete character before cursor |
| `Delete`    | Delete character after cursor  |
| `Ctrl+D`    | Exit shell                     |

## 🛠️ Local Installation Guide

### Prerequisites

Before installing the project locally, ensure you have the following:

- **Windows 10/11** (64-bit recommended)
- **MinGW-w64** or **Microsoft Visual Studio** (with C++ compiler)
- **Git** (for cloning the repository)
- **PowerShell** or **Command Prompt**

### Installation Methods

#### Method 1: Quick Installation (Recommended)

```powershell
# 1. Clone the repository to your local machine
git clone <your-repo-url>
cd "Shell project"

# 2. Compile the project
g++ main.cpp -o shell.exe

# 3. Run the shell immediately
.\shell.exe
```

#### Method 2: Development Setup

```powershell
# 1. Clone and navigate to project
git clone <your-repo-url>
cd "Shell project"

# 2. Create build directory for organized compilation
mkdir build -Force
cd build

# 3. Compile with optimizations
g++ -O2 -std=c++17 ..\main.cpp -o shell.exe

# 4. Test the installation
.\shell.exe
```

#### Method 3: System-Wide Installation

```powershell
# 1. Follow Method 1 or 2 to compile
# 2. Copy to a directory in your PATH (requires admin privileges)
copy shell.exe "C:\Program Files\CustomShell\shell.exe"

# 3. Add to PATH environment variable
# Go to System Properties > Environment Variables
# Add "C:\Program Files\CustomShell" to PATH

# 4. Now you can run from anywhere
shell.exe
```

### Verifying Installation

After installation, verify everything works correctly:

```powershell
# Test basic functionality
.\shell.exe

# In the shell, try these commands:
$ pwd                    # Should show current directory
$ echo "Hello World"     # Should print "Hello World"
$ history               # Should show command history
$ ls                    # Should list directory contents
$ exit                  # Should close the shell
```

### Troubleshooting

**Common Issues:**

1. **Compiler not found**: Install MinGW-w64 or Visual Studio Build Tools
2. **Permission denied**: Run PowerShell as Administrator for system-wide installation
3. **File not found**: Ensure you're in the correct directory (`Shell project`)
4. **Compilation errors**: Check that you have C++17 support

**Quick Fixes:**

```powershell
# Check if g++ is available
g++ --version

# Install MinGW-w64 via chocolatey (if you have it)
choco install mingw

# Or download from: https://www.mingw-w64.org/downloads/
```

## 📖 Usage Examples

### Basic Commands

```bash
$ pwd
C:\Users\YourName\shell-cpp\src

$ echo "Hello, World!"
Hello, World!

$ ls
main.cpp  shell.exe  history.txt
```

### Tab Completion

```bash
$ ec<TAB>        # Completes to "echo"
$ ls ma<TAB>     # Completes to "main.cpp"
$ his<TAB><TAB>  # Shows "history" if available
```

### Command History

```bash
$ history        # Show all commands
1. pwd
2. echo "Hello"
3. ls

$ history 2      # Show last 2 commands
2. echo "Hello"
3. ls
```

### I/O Redirection

```bash
$ echo "Hello" > output.txt          # Write to file
$ echo "World" >> output.txt         # Append to file
$ ls > filelist.txt                  # Save directory listing
$ cat output.txt
Hello
World
```

### Advanced Line Editing

```bash
$ echo "This is a test"
# Use ← → arrows to move cursor
# Use Backspace/Delete to edit anywhere in line
# Use ↑ ↓ to navigate history while editing
```

## 🏗️ Architecture

### Core Components

1. **`readline()`** - Custom input handler with advanced editing
2. **`find_completion()`** - Tab completion system
3. **Command Processors** - Individual handlers for each built-in command
4. **History Manager** - Persistent command history with file I/O
5. **I/O Redirector** - Output redirection parsing and handling

### Key Technical Features

- **Memory Management**: Proper allocation/deallocation of dynamic strings
- **Error Handling**: Comprehensive error checking for file operations
- **Windows Integration**: Native Windows API usage for optimal performance
- **Cross-Platform Considerations**: Easy adaptation to Unix systems if needed

## 🧪 Testing

The shell has been thoroughly tested with:

- ✅ All basic shell operations
- ✅ Complex command line editing scenarios
- ✅ Tab completion edge cases
- ✅ I/O redirection combinations
- ✅ History navigation with cursor movement
- ✅ Error conditions and recovery

## 🚀 Deployment

### Building Release Version

```bash
g++ -O2 -DNDEBUG main.cpp -o shell.exe
```

### Installation

```bash
# Copy to a directory in your PATH
copy shell.exe C:\Windows\System32\myshell.exe
```

## 🔧 Customization

### Adding New Commands

1. Add command name to `commands` vector
2. Implement command handler function
3. Add handler call in main command processing loop
4. Update help documentation

### Modifying Key Bindings

Edit the key code handling in `readline()` function:

```cpp
else if (ch == YOUR_KEY_CODE) {
    // Your custom key handling
}
```

## 📝 License

This project is part of the CodeCrafters Shell Challenge. Feel free to use and modify for educational purposes.

## 🤝 Contributing

Contributions are welcome! Key areas for enhancement:

- Additional built-in commands
- More sophisticated I/O redirection
- Cross-platform compatibility improvements
- Performance optimizations

## 🏆 Features Showcase

This implementation demonstrates advanced C++ programming techniques:

- Custom readline implementation
- Windows API integration
- Sophisticated command parsing
- Memory management best practices
- Professional code documentation

---

**Built with ❤️ using modern C++ and Windows APIs**
#   S h e l l  
 