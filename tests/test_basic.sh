#!/bin/bash
DIRECTORY=$( dirname $0)
SHELL_CMD="$DIRECTORY/../myshell"
PASS=0
FAIL=0

check(){
local description=$1
local expected=$2
local actual=$3

if [ "$expected" = "$actual" ]; then
echo "PASS: $description"
((PASS++))
else
echo "FAIL: $description"
echo "expected: $expected"
echo "actual: $actual"
((FAIL++))
fi
}
# HAPPY PATH - basic - simple echo hello
result=$(echo 'echo hello' | $SHELL_CMD | grep -v "myshell>>")
check "testing echo hello"  "hello" "$result"

# HAPPY PATH - basic - ls test (FLAKY TEST - FIX = CONTROL ENVIRONMENT IT DEPENDS ON)
# use /tmp because it is designed for temporary files
touch /tmp/myshell_test_ls.txt
result=$(echo 'ls /tmp/myshell_test_ls.txt' | $SHELL_CMD | grep -v "myshell>>")
check "testing ls" "/tmp/myshell_test_ls.txt" "$result"
rm /tmp/myshell_test_ls.txt

# HAPPY PATH - basic - single pipe between commands
result=$(echo 'echo  hello world | grep hello' | $SHELL_CMD | grep -v "myshell>>")
check "testing pipes" "hello world" "$result"

# HAPPY PATH - basic - program writes to a file (output)
echo 'echo test > /tmp/myshell_test_redirecting_output.txt' | $SHELL_CMD | grep -v "myshell>>"
result=$(cat '/tmp/myshell_test_redirecting_output.txt')
check "testing redirecting to another file" "test" "$result"
rm /tmp/myshell_test_redirecting_output.txt

# HAPPY PATH - basic - program reads from a file (input)
touch /tmp/myshell_test_input.txt
echo 'test' > /tmp/myshell_test_input.txt
result=$(echo 'cat < /tmp/myshell_test_input.txt' | $SHELL_CMD | grep -v "myshell>>")
check "testing redirecting from another file" "test" "$result"
rm /tmp/myshell_test_input.txt

# HAPPY PATH - basic - cd -builtin- used printf to treat \n as literals
result=$(printf 'cd /tmp\npwd\n' | $SHELL_CMD | grep -v "myshell>>")
check "testing cd" "/tmp" "$result"

# HAPPY PATH - basic - exit -builtin- use to exit terminal
# use /dev/null to preserve exit code of exit command
echo 'exit' | $SHELL_CMD > /dev/null 2>&1 # redirect stdout and stderr to /dev/null to silence prompt
result=$?
check "testing exiting shell" "0" "$result"

# EDGE CASE - empty - user presses enter with no input
result=$(printf '\n' | $SHELL_CMD | grep -v "myshell>>")
check "User presses enter with no input" "" "$result"

# EDGE CASE - BOUNDARY VALUE - user enters 127 arguments
arguments=$(printf ' a%.0s' {1..126})
# echo + 126 = 127 arguments
result=$(echo "echo$arguments" | $SHELL_CMD | grep -v "myshell>>" | wc -w)
check "testing user enters max arguments" "126" "$result"

# EDGE CASE - BOUNDARY VALUE - user enters 128 arguments
arguments=$(printf ' a%.0s' {1..127})
result=$(echo "echo$arguments" | $SHELL_CMD 2>&1 | grep "myshell: too many arguments")
check "testing user entering one past max arguments" "myshell: too many arguments" "$result"

# EDGE CASE = COMBINATION - user multiple commands
result=$(echo 'echo test | grep test | wc -w' | $SHELL_CMD | grep -v "myshell>>")
check "testing multiple pipes" "1" "$result"

# EDGE CASE - COMBINATION - user enters pipes, input, output together
echo "test" > /tmp/combination_out_in_pipe.txt
echo 'cat < /tmp/combination_out_in_pipe.txt | grep test > /tmp/result_combination_out_in_pipe.txt' | $SHELL_CMD | grep -v "myshell>>"
result=$(cat /tmp/result_combination_out_in_pipe.txt)
check "testing combination of pipe, input and output redirection" "test" "$result"
rm /tmp/combination_out_in_pipe.txt /tmp/result_combination_out_in_pipe.txt

#EDGE CASE - REPETITION - user appends to a file: should not overwrite
echo 'echo test >> /tmp/repetition_appending.txt' | $SHELL_CMD | grep -v "myshell>>"
echo 'echo test2 >> /tmp/repetition_appending.txt' | $SHELL_CMD | grep -v "myshell>>"
result=$(cat /tmp/repetition_appending.txt)
check "testing appending to file repetition" $'test\ntest2' "$result"
rm /tmp/repetition_appending.txt

#ERROR HANDLING - BAD PIPES - user only enters |
result=$(echo '|' | $SHELL_CMD 2>&1| grep -v "myshell>>")
check "testing bad pipe: |" "bash: syntax error near unexpected token '|'" "$result"

#ERROR HANDLING - MISSING RESOURCE - user enters wrong directory
result=$(echo ' cd notacommand123' | $SHELL_CMD 2>&1| grep -v "myshell>>")
check "testing cd notacommand123" "cd: No such file or directory" "$result"

#ERROR HANDLING - WRONG USAGE - user enters only grep
result=$(echo 'grep' | $SHELL_CMD 2>&1| grep -v "myshell>>")
check "testing wrong usage : grep" "Usage: grep [OPTION]... PATTERNS [FILE]...
Try 'grep --help' for more information." "$result"


#ERRROR HANDLING - RECOVERY - user enters invalid command, program continues succesfully
result=$(printf 'cleeeaar\necho hello\n' | $SHELL_CMD 2>/dev/null| grep -v "myshell>>")
check "testing program recovery: cleearr -> echo hello" "hello" "$result"


TOTAL=$(($PASS + $FAIL))
echo "Total Passed: $PASS/$TOTAL"
echo "Total Failed: $FAIL"

