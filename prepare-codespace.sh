#!/bin/bash
echo "Preparing..."

if [ -n "$(uname -a | grep Ubuntu)" ]; then
    echo "Ubuntu is a supported distro"
else
    echo "This assignment has not been tested against your distribution. "
fi

if [ ! command -v go &> /dev/null ]; then
    echo "Setup cannot continue: the 'go' binary is not present in \$PATH"
    exit -1
fi
echo "Installing setup prerequisite..."
go install github.com/charmbracelet/gum@latest &> /dev/null
if [ $? -ne 0 ]; then
    echo "Critical error: gum could not be installed. Setup cannot continue"
    exit -1
fi
echo "Setup prerequisites satisfied. "
sleep 1
clear




gum format -- "# Packages queued for install" "- fuse3" "- libfuse3-dev"

gum confirm --affirmative="Allow" --negative="Deny" "This will install additional packages onto your system. "

if [ $? -ne 0 ]; then
    echo "Setup cancelled."
    exit -1
fi

gum spin --spinner points --title "Updating package lists..." -- sudo apt update -y
if [ $? -ne 0 ]; then
    echo "An error occurred while updating package lists. "
    exit -1
fi
gum spin --spinner points --title "Installing fuse3 library..." -- sudo apt -y install fuse3
if [ $? -ne 0 ]; then 
    echo "An error occured while installing the fuse3 library"
    exit -1
fi
gum spin --spinner points --title "Installing fuse3 development headers..." -- sudo apt -y install libfuse3-dev
if [ $? -ne 0 ]; then
    echo "An error occured while installing the fuse3 development headers."
    exit -1
fi

gum format -- "# Packages installed" "Assignment prerequisites have been installed. "