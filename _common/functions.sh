#/bin/bash

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
    echo "===Passed time===== "$hour"h "$min"m "$sec"s ============"
}

errorofbuild()
{
    printf "\n\n=========\E[37;41mAN ERROR OCCURED!\E[0m==========\n"
    cd $bak
    exit 1
}

checkState()
{
    if [ $? -eq 0 ]
    then
        echo -e "\E[37;42mOK!\E[0m"
    else
        errorofbuild
    fi
}

osx_realpath() {
  case "${1}" in
    [./]*)
    echo "$(cd ${1%/*}; pwd)/${1##*/}"
    ;;
    *)
    echo "${PWD}/${1}"
    ;;
  esac
}
