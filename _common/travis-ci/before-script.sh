#!/bin/bash
git submodule init;
git submodule update;

if [[ "${TRAVIS_OS_NAME}" == "linux" ]];
then
    IS_SEMAPHORECI=true
    if [[ ! -d /home/runner ]];
    then
        IS_SEMAPHORECI=false
        echo -n | openssl s_client -connect scan.coverity.com:443 | sed -ne '/-BEGIN CERTIFICATE-/,/-END CERTIFICATE-/p' | sudo tee -a /etc/ssl/certs/ca-
        # Make sure we have same home directory as on Semaphore-CI
        sudo ln -s /home/travis /home/runner
    fi

    QtStaticVersion=${QT_VER}
    QtTarballName=${QT_TARBALL}
    QtCacheFolder=qtcache

    if [[ "${QT_VER}" == "" ]]; then
        QtTarballName=qt-5.10.1-static-ubuntu-14-04-x64-gcc6.tar.bz2
        QtStaticVersion=5.10.1_static
    fi

    if ${IS_SEMAPHORECI}; then
        sudo add-apt-repository --yes ppa:ubuntu-sdk-team/ppa
        sudo add-apt-repository --yes ppa:ubuntu-toolchain-r/test
        sudo add-apt-repository --yes ppa:george-edison55/cmake-3.x
        sudo apt-get update -qq
        # sudo DEBIAN_FRONTEND=noninteractive apt-get -yq upgrade
        sudo apt-get install -qq "^libxcb.*" libx11-dev libx11-xcb-dev \
            libxcursor-dev libxrender-dev libxrandr-dev libxext-dev libxi-dev \
            libxss-dev libxt-dev libxv-dev libxxf86vm-dev libxinerama-dev \
            libxkbcommon-dev libfontconfig1-dev libasound2-dev libpulse-dev \
            libdbus-1-dev udev mtdev-tools webp libudev-dev libglm-dev \
            libwayland-dev libegl1-mesa-dev mesa-common-dev libgl1-mesa-dev \
            libglu1-mesa-dev libgles2-mesa libgles2-mesa-dev libmirclient-dev \
            libproxy-dev ccache gcc-6 g++-6 libc6 libstdc++6 cmake
    fi

    sudo update-alternatives --remove-all gcc
    sudo update-alternatives --remove-all g++
    sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-6 60
    sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-6 60

    echo "================================================"
    g++ --version
    cmake --version
    echo "================================================"

#    if [[ ! -d /home/runner ]];
#    then
#        bash _common/travis-ci/_generate_version_files.sh
#    else
#        bash _common/travis-ci/_generate_version_files.sh "bin-cmake-release/versions"
#    fi
    bash _common/travis-ci/_generate_version_files.sh "bin-cmake-release/versions"

    if [[ ! -d /home/runner/Qt/$QtCacheFolder ]]; then
        mkdir -p /home/runner/Qt/$QtCacheFolder
    fi

    printf "Downloading $QtTarballName..."
    wget --quiet http://wohlsoft.ru/docs/Software/QtBuilds/$QtTarballName -O /home/runner/Qt/$QtCacheFolder/$QtTarballName
    if [[ $? -eq 0 ]]
    then
        printf " \E[37;42mOK!\E[0m\n"
    else
        printf "\n\n=========\E[37;41mAN ERROR OCCURRED!\E[0m==========\n"
        echo "Can't download ${QtTarballName}!"
        exit 1
    fi

    printf "Unpacking $QtTarballName..."
    tar -xf /home/runner/Qt/$QtCacheFolder/$QtTarballName -C /home/runner/Qt
    if [[ $? -eq 0 ]]
    then
        printf " \E[37;42mOK!\E[0m\n"
    else
        printf "\n\n=========\E[37;41mAN ERROR OCCURRED!\E[0m==========\n"
        echo "Can't extract ${QtCacheFolder}/${QtTarballName}!"
        exit 1
    fi

    export PATH=/home/runner/Qt/$QtStaticVersion/bin:$PATH
    /home/runner/Qt/$QtStaticVersion/bin/qmake --version
    chmod u+x generate_paths.sh
    bash generate_paths.sh silent static

elif [[ "${TRAVIS_OS_NAME}" == "osx" ]];
then

    #QT_VER=5.12.0
    export PATH=/Users/StaticQt/${QT_VER}/bin:/Users/StaticQt/${QT_VER}/lib:/usr/local/opt/coreutils/libexec/gnubin:$PATH
    #source _common/travis-ci/_osx_env.sh
    QtCacheFolder=qtcache
    #QtTarballName=qt-5-12-0-static-macosx-10-14-1.tar.bz2
    QtTarballName=${QT_TARBALL}

# Try out the caching thing (if caching is works, downloading must not be happen)
    if [[ ! -d /Users/StaticQt/$QtCacheFolder ]];
    then
        sudo mkdir -p /Users/StaticQt/$QtCacheFolder;
        sudo chown -R travis /Users/StaticQt/;
# ==============================================================================
# Downloading and unpacking of pre-built static Qt 5.10.0 on OS X 10.12.6
# ------------------------------------------------------------------------------
# Static Qt is dependent to absolute build path, so,
# we are re-making same tree which was on previous machine where this build of Qt was built
# ==============================================================================
        printf "Downloading $QtTarballName..."
        wget --quiet http://wohlsoft.ru/docs/Software/QtBuilds/$QtTarballName -O /Users/StaticQt/$QtCacheFolder/$QtTarballName;
        if [[ $? -eq 0 ]];
        then
            printf " \E[37;42mOK!\E[0m\n"
        else
            printf "\n\n=========\E[37;41mAN ERROR OCCURRED!\E[0m==========\n"
            echo "Can't download ${QtTarballName}!"
            exit 1
        fi
    fi
    printf "Unpacking $QtTarballName..."
    tar -xf /Users/StaticQt/$QtCacheFolder/$QtTarballName -C /Users/StaticQt;
    if [[ $? -eq 0 ]];
    then
        printf " \E[37;42mOK!\E[0m\n"
    else
        printf "\n\n=========\E[37;41mAN ERROR OCCURRED!\E[0m==========\n"
        echo "Can't extract ${QtCacheFolder}/${QtTarballName}!"
        exit 1
    fi

# ==============================================================================
# Installing of required for building process tools via homebrew toolset
# ==============================================================================
    ## =======use travis.yml to boostrap all necessary packages=====
    #printf "Running brew update...\n"
    #brew update > /dev/null 2>&1

    #printf "Installing of necessary utilities...\n"
    #brew install coreutils binutils gnu-sed lftp cmake ninja
    ## ============


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
    echo "== QT Path is /Users/StaticQt/${QT_VER}/bin/ =="

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
