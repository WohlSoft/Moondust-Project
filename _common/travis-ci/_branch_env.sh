#!/bin/bash

export IS_PULL_REQUEST=false

if [[ "${TRAVIS_PULL_REQUEST}" == "" ]]; then
    TRAVIS_PULL_REQUEST=false
fi

# FIXME: Figure out for Semaphore Pull-Request identifying at Semaphore 2.0
if [[ "${TRAVIS_PULL_REQUEST}" != "false" || "${PULL_REQUEST_NUMBER}" != "" ]]; then
    export IS_PULL_REQUEST=true
fi

if [[ "${TRAVIS_BRANCH}" != "" ]]; then
    export GIT_BRANCH=${TRAVIS_BRANCH}
else
    export GIT_BRANCH=$(git branch | sed -n -e 's/^\* \(.*\)/\1/p')
fi
