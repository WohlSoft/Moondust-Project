#/bin/bash

# Prints a line with text in middle
# Syntax:
#   printLine <string> <color of text in ANSI format> <color of line>
function printLine()
{
    lineLenght=64
    Str=$1
    StrLen=${#Str}
    BeginAt=$(( ($lineLenght/2) - ($StrLen/2) ))

    lineColor=$3
    textColor=$2

    if [[ "$lineColor" != "" ]]; then
    printf $lineColor; fi

    for((i=0; i < $lineLenght; i++))
    do
        if (($i == $BeginAt))
        then
            if [[ "$textColor" != "" ]]; then
            printf $textColor; fi
        fi

        if (($i == $BeginAt + $StrLen))
        then
            if [[ "$lineColor" != "" ]]; then
            printf $lineColor; fi
        fi

        if (( $i >= $BeginAt && $i < $BeginAt + $StrLen ))
        then
            printf "${Str:$(($i-$BeginAt)):1}"
        else
            printf "="
        fi
    done
    printf "\E[0m"
    printf "\n"
}

function getCpusCount()
{
    if [[ "$OSTYPE" == "darwin"* ]]; then
        echo $(sysctl -n hw.ncpu);
    elif [[ "$OSTYPE" == "msys"* ]]; then
        echo 4; # Windows says 'No way!'. Yet... Until we find a way to do this
    else
        echo $(grep -c ^processor /proc/cpuinfo);
    fi
}

function show_time()
{
    num=$1
    min=0
    hour=0
    if((num>59));then
        ((sec=num%60))
        ((num=num/60))
        if((num>59));then
            ((min=num%60))
            ((num=num/60))
            ((hour=num))
        else
            ((min=num))
        fi
    else
        ((sec=num))
    fi

    printLine "Passed time: ${hour}h ${min}m ${sec}s" "\E[0;36m" "\E[0;35m"
}

function pause()
{
    echo "Press any key to continue..."
    read -n 1
}

function errorofbuild()
{
    printLine "AN ERROR OCCURED!" "\E[0;41;37m" "\E[0;31m"
    cd $bak
    exit 1
}

function checkState()
{
    if [ $? -eq 0 ]
    then
        printf "=== \E[37;42mOK!\E[0m ===\n\n"
    else
        errorofbuild
    fi
}

function lackOfDependency()
{
    printLine "DEPENDENCIES ARE NOT BUILT! Run ./build_deps.sh first!" "\E[0;41;37m" "\E[0;31m"
    cd $bak
    exit 1
}

function osx_realpath() 
{
    case "${1}" in
    [./]*)
        echo "$(cd ${1%/*}; pwd)/${1##*/}"
    ;;
    *)
        echo "${PWD}/${1}"
    ;;
    esac
}

