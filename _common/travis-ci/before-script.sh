#!/bin/bash

if [ $TRAVIS_OS_NAME == linux ];
then
    echo 'Europe/Moscow' | sudo tee /etc/timezone
    sudo dpkg-reconfigure --frontend noninteractive tzdata
    sudo ntpdate ntp.ubuntu.com
    echo $(TZ=Europe/Moscow date)

elif [ $TRAVIS_OS_NAME == osx ];
then
    sudo systemsetup -settimezone Europe/Moscow;
    echo $(date)

fi

