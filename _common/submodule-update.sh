#!/bin/bash

reponame=$(basename `git rev-parse --show-toplevel`)
dstbranch=$(git symbolic-ref refs/remotes/origin/HEAD --short | sed -n 's/origin\///p')
echo "-----------------------------------"
echo "Updating repo $reponame, branch $dstbranch"

git checkout $dstbranch
git pull origin $dstbranch
