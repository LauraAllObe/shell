# Shell
This project is centered around the development of a custom shell interface. The primary objective 
is to create a sophisticated command-line environment that allows users to interact seamlessly with 
the underlying system.
Key Features and Goals:
-Efficient command execution: Ensuring commands run smoothly and effectively.
-Advanced error handling: Minimizing user errors and enhancing system response.
-Intuitive user interaction: Creating a user-friendly environment with clear feedback and 
instruction.
-Integration of common shell features: Including but not limited to input/output redirection, 
piping, and background processing.

## Demonstration Video:
[![](https://drive.google.com/uc?export=view&id=1AohBsFQG4EXJNaBkqxbo0XRiSQBXFiDV)](https://youtu.be/c9-mLxW2zOw)

###### *Note: some implementations, such as I/O redirection and nested execution, are incomplete.*
## Group Members
- **Jeyma Rodrìguez**: jdr21@fsu.edu
- **Autumn Harp**: aom21a@fsu.edu
- **Laura Obermaier**: lao21@fsu.edu
## Division of Labor

### Part 1: Prompt
- **Responsibilities**: Greet the user with a prompt that indicates the current working directory, 
the user name, and the machine name.
- **Assigned to**: Laura Obermaier

### Part 2: Environment Variables
- **Responsibilities**: Replace tokens prefixed with the dollar sign character ($) with their 
corresponding values (before commands are processed.)
- **Assigned to**: Laura Obermaier

### Part 3: Tilde Expansion
- **Responsibilities**: handle tilde expansion for ~/ and ~ 
(should go to $HOME with or without /)
- **Assigned to**: Jeyma Rodrìguez

### Part 4: $PATH Search
- **Responsibilities**: employ string operations to extract and examine each directory in the $PATH 
variable. In the event that the command is not found in any of the directories listed in $PATH, an 
error message should be displayed. Handling this scenario involves proper error detection and 
reporting, ensuring a clear indication when a command is not available within the directories 
specified in $PATH.
- **Assigned to**: Autumn Harp, Jeyma Rodrìguez, Laura Obermaier

### Part 5: External Command Execution
- **Responsibilities**: execute external commands using execv(). Single command execution is split
from command execution with piping and I/O redirection, which was carried out by each of the
assignees of part 6 and part 7.
- **Assigned to**: Autumn Harp, Laura Obermaier

### Part 6: I/O Redirection (NOT COMPLETED)
- **Responsibilities**: Implement input/output redirection w/ < in, > out. Input is flagged as read
  only, output flagged as read/write truncated w/ output file flagged as having read/write 
  permission enabled only for the owner.
- **Assigned to**: Autumn Harp, Laura Obermaier

### Part 7: Piping
- **Responsibilities**: It handles a maximum of two pipes in any given command. Piping behavior 
follows the required project desription: <cmd1 | cmd2>,  <cmd1 | cmd2 | cmd3>. By adding a piping 
feature, we let data move easily between commands. This makes the computer's command tool more 
flexible and strong.
- **Assigned to**: Jeyma Rodriguez

### Part 8: Background Processing
- **Responsibilities**: Executes processes in the background for cmd &, cmd1 | cmd2 &, cmd > file &,
cmd < file &, and cmd < file_in > file_out &. Increments jobs structure when execution starts. When
execution is finished, decrements job strucutre. (this is more so a part of #9 than #8). There will
be at most 10 background processes.
- **Assigned to**: Laura Obermaier

### Part 9: Internal Command Execution
- **Responsibilities**: Implements cd, exit, and jobs internal commands. cd uses chdir. If no
arguments are present, change to $HOME. If too many arguments are present, the directory does not
exist, or the directory is not a directory, signal an error. jobs is implemented using a jobs
structure that decrements at the beginning of the loop by checking for finished background processes
and increments when a background process is executed. If there are no active background processes,
an error message is displayed. exit displays the last three valid commands using char * to save each
command and overwrite the last. This waits for any background processes to finish running. If there
are less than 3 valid commands, this prints the last one. If there are none, displays an error
message.
- **Assigned to**: Laura Obermaier

### Part 10: External Timeout Executable
- **Responsibilities**: Implement the external executable/command "mytimeout" that can be executed 
in our shell. This command is analogous to the Unix command "timeout" and offers enhanced control 
over command execution duration. 
- **Assigned to**: Jeyma Rodriguez, Autumn Harp, Laura Obermaier

### Extra Credit (#3, NOT COMPLETED)
- **Responsibilities**: Implement ability to execute shell within a shell (nested instances)
- **Assigned to**: Laura Obermaier

## File Listing
```
shell/
│
├── src/
│ ├── lexer.c
| └── mytimeout
│   └── mytimeout.c
|
├── include/
│ └── lexer.h
│
├── .gitignore
├── README.md
└── Makefile
```
## How to Compile & Execute

### Requirements
- **Compiler**: `gcc`

### Compilation
make

### Execution
.bin/shell or bin/shell

## Bugs
N/A

## Extra Credit
- **Extra Credit 3**: Assigned to Laura Obermaier, allows for nested instances of a shell (allows
users to execute the shell within the shell by running ./bin/shell or bin/shell)

## Considerations
N/A
