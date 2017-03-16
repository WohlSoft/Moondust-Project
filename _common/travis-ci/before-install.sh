#!/bin/bash
git submodule init;
git submodule update;

if [ $TRAVIS_OS_NAME == linux ];
then
    if [ ! -d /home/runner ];
    then
        echo -n | openssl s_client -connect scan.coverity.com:443 | sed -ne '/-BEGIN CERTIFICATE-/,/-END CERTIFICATE-/p' | sudo tee -a /etc/ssl/certs/ca-
        # Make sure we have same home directory as on Semaphore-CI
        sudo ln -s /home/travis /home/runner
    fi

    QtCacheFolder=qtcache580
    QtTarballName=qt-5-8-0-static-ubuntu-14-04-x64.tar.bz2
    QtStaticVersion=5.8.0_static

    bash _Misc/dev_scripts/generate_version_files.sh
    sudo add-apt-repository --yes ppa:ubuntu-sdk-team/ppa
    sudo apt-get update -qq
    sudo apt-get upgrade -qq
    sudo apt-get install -qq "^libxcb.*" libx11-dev libx11-xcb-dev libxcursor-dev libxrender-dev libxrandr-dev libxext-dev libxi-dev libxss-dev libxt-dev libxv-dev libxxf86vm-dev libxinerama-dev libxkbcommon-dev libfontconfig1-dev libasound2-dev libpulse-dev libdbus-1-dev udev mtdev-tools webp libudev-dev libglm-dev libwayland-dev libegl1-mesa-dev mesa-common-dev libgl1-mesa-dev libglu1-mesa-dev libgles2-mesa libgles2-mesa-dev libmirclient-dev libproxy-dev ccache

    if [ ! -d /home/runner/Qt/$QtCacheFolder ]; then
        mkdir -p /home/runner/Qt/$QtCacheFolder
    fi
    wget http://wohlsoft.ru/docs/Software/QtBuilts/$QtTarballName -O /home/runner/Qt/$QtCacheFolder/$QtTarballName
    tar -xf /home/runner/Qt/$QtCacheFolder/$QtTarballName -C /home/runner/Qt
    export PATH=/home/runner/Qt/$QtStaticVersion/bin:$PATH
    /home/runner/Qt/$QtStaticVersion/bin/qmake --version
    chmod u+x generate_paths.sh
    bash generate_paths.sh silent static

elif [ $TRAVIS_OS_NAME == osx ];
then

    QT_VER=5.8.0
    export PATH=/Users/StaticQt/$QT_VER/bin:/Users/StaticQt/$QT_VER/lib:/usr/local/opt/coreutils/libexec/gnubin:$PATH
    #source _common/travis-ci/_osx_env.sh
    QtCacheFolder=qtcache580
    QtTarballName=qt-5.8.0-static-osx-10.12.3.tar.gz

# Try out the caching thing (if caching is works, downloading must not be happen)
    if [ ! -d /Users/StaticQt/$QtCacheFolder ]
    then
        sudo mkdir -p /Users/StaticQt/$QtCacheFolder;
        sudo chown -R travis /Users/StaticQt/;
# ==============================================================================
# Downloading and unpacking of pre-built static Qt 5.8.0 on OS X 10.12.3
# ------------------------------------------------------------------------------
# Static Qt is dependent to absolute build path, so,
# we are re-making same tree which was on previous machine where this build of Qt was built
# ==============================================================================
        wget http://wohlsoft.ru/docs/Software/QtBuilts/$QtTarballName -O /Users/StaticQt/$QtCacheFolder/$QtTarballName;
    fi
    printf "Unpacking $QtTarballName..."
    tar -xf /Users/StaticQt/$QtCacheFolder/$QtTarballName -C /Users/StaticQt;
    if [ $? -eq 0 ]
    then
        printf " \E[37;42mOK!\E[0m\n"
    else
        printf "\n\n=========\E[37;41mAN ERROR OCCURED!\E[0m==========\n"
        echo "Can't extract ${QtCacheFolder}/${QtTarballName}!"
        exit 1
    fi

# ==============================================================================
# Installing of required for building process tools via homebrew toolset
# ==============================================================================
    brew install coreutils binutils gnu-sed lftp

    # # Thanks to St. StackOverflow if this will work http://stackoverflow.com/questions/39633159/homebrew-cant-find-lftp-formula-on-macos-sierra
    # [[recently FIXED, lftp successfully installs by old way!]]
    # brew install homebrew/boneyard/lftp

    # Workaround for ElCapitan (Fixed on Qt 5.8 on macOS Sierra!)
    #if [ ! -d /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.10.sdk ];
    #then
    #    ln -s /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.10.sdk /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk
    #fi

# ==============================================================================
# Making "_paths.sh" config file
# ==============================================================================
    echo "== QT Path is /Users/StaticQt/$QT_VER/bin/ =="

    echo "# ==============Qt paths================" > _paths.sh;
    echo "QT_PATH=\"/Users/StaticQt/${QT_VER}/bin/\"" >> _paths.sh;
    echo "QT_LIB_PATH=\"/Users/StaticQt/${QT_VER}/lib/\"" >> _paths.sh;
    echo "QMake=\"qmake\"" >> _paths.sh;
    echo "LRelease=\"lrelease\"" >> _paths.sh;
    echo "" >> _paths.sh;
    chmod u+x _paths.sh;
    echo "============== _paths.sh ================="
    cat _paths.sh
    echo "=========================================="

fi
