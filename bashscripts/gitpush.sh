# Just the command gitpush <commit message> pushes all the changes with the given commit message, if the commit message is empty, it is set to the default message 'commit'

#Make sure to run the below commands in the repo before using it
#-> git remote add origin https://github.com/<username>/<reponame>.git
#-> git remote set-url origin https://[token]@github.com/<username>/<reponame>
# They change git push such that you dont need to give username or password each time you give the command git push

#!/bin/bash

# only # is commenting, # with exclamation specifies the interpreter to the OS, in this case /bin/bash, to run the script. If we dont specify the interpreter the OS usually runs in /bin/bash if the command is bash filename.sh, if we run via ./filename.sh then it will run in the default shell which might be different from /bin/bash

# we can run a bash script using filename only when the directory it is in is present is $PATH so that it is accessible from anywhere, if it is not in $PATH the you need to run it using ./filename.sh (./ means specifying current directory)

# Default commit message if none is provided
commit_message=${1:-"commit"}

# Add all changes
git add .

# Commit with the provided or default message
git commit -m "$commit_message"

# Push changes
git push

# Gives the below message once the changes are successfully pushed
echo "Changes pushed successfully!"

#To run this bash script you have two options --
#1. Either copy this bash script in every repo, make it executable using the command chmod +x gitpush.sh and then use it or
#2. Keep it in a place where it is accessible to every directory. Usually such a place can be found by running the command echo $PATH. For the current device, it turns out to be /use/local/bin. Run the following commands to move it and make it an executable file
# -->sudo mv gitpush.sh /usr/local/bin/gitpush
# -->sudo chmod +x /usr/local/bin/gitpush
# Now the file should be executable from anywhere
# To delete the following bash script run sudo rm /usr/local/bin/gitpush

