#!/bin/bash

pass() {
    printf "\e[32m[PASS]\e[0m %s\n" "$1"
}

# Function to output a fail message
fail() {
    printf "\e[31m[FAIL]\e[0m %s\n" "$@"
}

check_structure() {
    if [ -f "mount/hello" ] && [ -f "mount/sub-directory/hi" ]; then  # Check if directories exist
      pass "mount directory structure is correct" 
    else
      fail "mount directory structure is incorrect"
      echo "Actual: " > /tmp/tree_output.txt
      tree mount | sed 's/├\|─\|│\|└/ /g' >> /tmp/tree_output.txt
      pr -m -t grading/expected.txt /tmp/tree_output.txt
      # paste <(expand grading/expected.txt) <(paste /tmp/tree_output.txt) | expand --tabs=50
      rm /tmp/tree_output.txt
    fi
}

mkdir mount &> /dev/null

./program mount &
PROGRAM_PID=$!

cleanup() {
  kill $PROGRAM_PID
}

trap cleanup EXIT

while [ ! -f "mount/hello" ]
do
if ps -p $PROGRAM_PID &> /dev/null
then
  echo "Waiting for FS to mount..."
  sleep 1
else
  fail "FS process died unexpectedly"
  break
fi
done

pass "FS mounted successfully"


# Check if the 'mount' directory exists
if [ -d "mount" ]; then
  pass "mount directory exists"
    check_structure  # If it exists, check its structure
else
  fail "Mount directory doesn\'t exist"
fi

cat mount/hello &> /dev/null
if [ $? -eq 0 ]; then
  pass "mount/hello content was readable"
else
  fail "mount/hello content was not readable" 
fi
cat mount/sub-directory/hi &> /dev/null
if [ $? -eq 0 ]; then
  pass "mount/sub-directory/hi content was readable" 
else
  fail "mount/sub-directory/hi content was not readable" 
fi
