#!/bin/bash

reponame=$(basename `git rev-parse --show-toplevel`)
dstbranch=$(git branch | sed -n 's/^\* //p')
echo "-----------------------------------"
echo "Updating repo $reponame, branch $dstbranch"

git checkout $dstbranch
git pull origin $dstbranch
