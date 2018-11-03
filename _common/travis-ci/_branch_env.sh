if [[ "$TRAVIS_BRANCH" !=  "" ]]; then
    GIT_BRANCH=$TRAVIS_BRANCH
else
    GIT_BRANCH=$(git branch | sed -n -e 's/^\* \(.*\)/\1/p')
fi

