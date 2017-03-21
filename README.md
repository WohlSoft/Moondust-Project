Platform Game Engine by Wohlstand
===========
a free and open source game engine and toolkit which gives the ability to create games of the platform genre.

------

Created by Vitaly Novichkov "Wohlstand" &lt;admin@wohlnet.ru&gt;

## Contacts and help

[![Official project site](https://img.shields.io/badge/Official%20Site-Visit-yellow.svg)](http://wohlsoft.ru/PGE/)
[![Official community](https://img.shields.io/badge/Community-Visit-green.svg)](http://wohlsoft.ru/forum/)
[![Official documentation](https://img.shields.io/badge/Documentation-Read-green.svg)](http://wohlsoft.ru/pgewiki/)
[![Contact us by special form](https://img.shields.io/badge/Contact-us-blue.svg)](http://wohlsoft.ru/forum/memberlist.php?mode=contactadmin)
[![Join to our chat server](https://img.shields.io/badge/Discord-Join%20us-7289DA.svg)](http://discord.gg/zC2PY8q)

How to build (Linux, Windows, Mac OS X):
http://wohlsoft.ru/pgewiki/Building_of_PGE_From_sources

## Downloads
[![Download latest stable release](https://img.shields.io/badge/Download-Stable-green.svg)](http://wohlsoft.ru/PGE/#links)
[![Official project site](https://img.shields.io/badge/Download-Experimental-orange.svg)](http://wohlsoft.ru/docs/_laboratory/)
[![Configuration packages are requires to work with PGE Project](https://img.shields.io/badge/Download-Config%20packs-blue.svg)](http://wohlsoft.ru/config_packs/)


## Donate us

[![Donate to this project using Flattr](https://img.shields.io/badge/flattr-donate-green.svg)](https://flattr.com/submit/auto?fid=lgdjjy&url=https%3A%2F%2Fgithub.com%2FWohlSoft%2FPGE-Project)
[![Donate to this project using Patreon](https://img.shields.io/badge/patreon-donate-orange.svg)](http://patreon.com/Wohlstand)
[![Donate to this project using Paypal](https://img.shields.io/badge/paypal-donate-blue.svg)](http://wohlsoft.ru/paypal)


## CI Build status
Linux | Windows | Mac OS X
------------ | ------------- | -------------
[![Build Status](https://semaphoreci.com/api/v1/projects/604cbe8f-44de-4754-99a5-28bc11764b39/471601/badge.svg)](https://semaphoreci.com/wohlstand/pge-project) | [![Build status](https://ci.appveyor.com/api/projects/status/5vpdgj57m3oheqc3?svg=true)](https://ci.appveyor.com/project/Wohlstand/pge-project) | [![Build Status](https://travis-ci.org/WohlSoft/PGE-Project.svg?branch=master)](https://travis-ci.org/WohlSoft/PGE-Project)

Get a most fresh assemblies you can get here: [![Official project site](https://img.shields.io/badge/Download-Experimental-orange.svg)](http://wohlsoft.ru/docs/_laboratory/)!

# Components
A list of components are included with PGE Project

## PGE Engine

![PGE Engine](https://github.com/WohlSoft/PGE-Project/blob/master/Engine/_resources/icon/cat_256.png)

Is the main part of PGE Project which allows users to play levels and episodes.

Provides a compatibility layer with the legacy game engine (SMBX Engine 1.3) (when used configuration packages which are following [SMBX64 Standard](http://wohlsoft.ru/pgewiki/SMBX64)).

:point_up: **Note** This component is experimental and has not completed base implementation, but can be used for various tests around.

[More information on the PGE Wiki...](http://wohlsoft.ru/pgewiki/PGE_Engine)


## PGE Editor

![PGE Editor](https://github.com/WohlSoft/PGE-Project/blob/master/Editor/_resources/images/cat_builder/cat_builder_256.png)

The main development tool which allows a creating and editing levels, world map, NPC configurations and providing some maintenance utilities.

Provides a compatibility layer with the legacy game engine (SMBX Engine 1.3) (when used configuration packages which are following [SMBX64 Standard](http://wohlsoft.ru/pgewiki/SMBX64)).

[More information on the PGE Wiki...](http://wohlsoft.ru/pgewiki/PGE_Editor)

## Playable Character Calibrator

![PGE Calibrator](https://github.com/WohlSoft/PGE-Project/blob/master/PlayableCalibrator/_resourses/icon/calibrator_256.png)

It's a PGE development tool intended for game creators. It allows the user to define a hit (collision) box for each respective frame of a playable character, and also allows developers to calibrate any incorrect positions of the frames in an SMBX64 playable character sprite.

[More information on the PGE Wiki...](http://wohlsoft.ru/pgewiki/Playable_character_Calibrator)


## PGE MusPlay

![PGE MusPlay](https://github.com/WohlSoft/PGE-Project/blob/master/MusicPlayer/_resources/cat_musplay/cat_musplay_256x256.png)

Light-weight music player based on [SDL Mixer X audio library](http://wohlsoft.ru/pgewiki/SDL_Mixer_X) which allows to test music files (include [OGG files with a loop tags](http://wohlsoft.ru/pgewiki/How_To:_Looping_music_files))

## GIFs to PNG Converter

A small console utility which merges bit-mask based GIF pairs used in the legacy engine into PNG format with alpha channel (semi-transparency) support.

## PNG to GIFs Converter

A small console utility which converts PNG image into pair of GIF files ready for bit-mask algorithm rendering. Allows creating graphics for legacy engine easier.

## LazyFix tool

A small console utility which fixes wrong lazily-made mask images into valid black&white mask images.

## PGE Maintainer

A small utility which provides a toolset to convert and fix various data: musics and levels.


# License

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

