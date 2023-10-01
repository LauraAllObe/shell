# Shell

[Description]

## Group Members
- **Jeyma Rodrìguez**: jdr21@fsu.edu
- **Autumn Harp**: aom21a@fsu.edu
- **Laura Obermaier**: lao21@fsu.edu
## Division of Labor

### Part 1: Prompt
- **Responsibilities**: Greet the user with a prompt that indicates 
the absolute working directory, the user name, and the machine name.
- **Assigned to**: Laura Obermaier

### Part 2: Environment Variables
- **Responsibilities**: Replace tokens prefixed with the dollar sign 
character ($) with their corresponding values (before commands
are processed.)
- **Assigned to**: Laura Obermaier

### Part 3: Tilde Expansion
- **Responsibilities**: handle tilde expansion for ~/ and ~ 
(should go to $HOME with or without /)
- **Assigned to**: Jeyma Rodrìguez

### Part 4: $PATH Search
- **Responsibilities**: employ string operations to extract and examine
 each directory in the $PATH variable. In the event that the command is 
 not found in any of the directories listed in $PATH, an error message 
 should be displayed. Handling this scenario involves proper error 
 detection and reporting, ensuring a clear indication when a command 
 is not available within the directories specified in $PATH.
- **Assigned to**: Autumn Harp, Jeyma Rodrìguez, Laura Obermaier

### Part 5: External Command Execution
- **Responsibilities**: execute external commands using execv(). This
part only covers one command execution for now, as we are still working
on piping, I/O redirection, and background processing.
- **Assigned to**: Autumn Harp, Laura Obermaier

### Part 6: I/O Redirection
- **Responsibilities**: [Description]
- **Assigned to**: 

### Part 7: Piping
- **Responsibilities**: [Description]
- **Assigned to**: 

### Part 8: Background Processing
- **Responsibilities**: [Description]
- **Assigned to**: 

### Part 9: Internal Command Execution
- **Responsibilities**: [Description]
- **Assigned to**: 

### Part 10: External Timeout Executable
- **Responsibilities**: [Description]
- **Assigned to**: 

### Extra Credit
- **Responsibilities**: [Description]
- **Assigned to**: 

## File Listing
```
shell/
│
├── src/
│ ├── main.c
│ └── shell.c
│
├── include/
│ └── shell.h
│
├── README.md
└── Makefile
```
## How to Compile & Execute

### Requirements
- **Compiler**: e.g., `gcc` for C/C++, `rustc` for Rust.
- **Dependencies**: List any libraries or frameworks necessary (rust only).

### Compilation
For a C/C++ example:
```bash
make
```
This will build the executable in ...
### Execution
```bash
make run
```
This will run the program ...

## Bugs
- **Bug 1**: This is bug 1.
- **Bug 2**: This is bug 2.
- **Bug 3**: This is bug 3.

## Extra Credit
- **Extra Credit 1**: [Extra Credit Option]
- **Extra Credit 2**: [Extra Credit Option]
- **Extra Credit 3**: [Extra Credit Option]

## Considerations
[Description]
