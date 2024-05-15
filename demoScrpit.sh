make
bin/shell
echo 'Note there are pauses of 2-5 seconds between most commands for increased readability'
sleep 2

# Change to home directory using tilde expansion
cd ~
sleep 2
pwd
sleep 2

# Navigate back to the project directory (modify path as necessary)
cd Desktop/Uni Misc/shell/shell
sleep 2
pwd
sleep 2

# Testing environment variable replacement
echo $HOME
sleep 2

# Testing external command execution
ls
sleep 2
ls src
sleep 2

# Testing error handling with $PATH search
wrongcommand
sleep 2

# Testing I/O redirection
cat demoOutput.txt
sleep 2
ls > demoOutput.txt
cat demoOutput.txt
sleep 2
grep i < demoOutput.txt
sleep 2
grep o < demoOutput.txt > demoOutput.txt
cat demoOutput.txt
sleep 2

# Testing piping
cat demoOutput.txt | grep o
sleep 2
cat demoOutput.txt | grep o | sleep 3
sleep 2

# Testing background processing
sleep 5 &
ls | sleep 10 &
ls > sleep 113 &
sleep 50 < ls &
grep a < demoOutput.txt > sleep 8 &

#Testing jobs command
jobs
sleep 5

# Testing external timeout command
./mytimeout 5 sleep 30
sleep 2

#testing double shell
bin/shell
sleep 2
exit
sleep 2

# Testing 'exit' command using a subshell to avoid terminating the main shell
exit
sleep 2

