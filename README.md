Moondust Project by Wohlstand
===========
Moondust Project (formerly known as PGE Project) is a free and open-source game engine and development kit for it, giving developers the ability to create platform games.

------

Created by Vitaly Novichkov "Wohlstand" &lt;admin@wohlnet.ru&gt;

## Contacts and help

[![Official project site](https://img.shields.io/badge/Official%20Site-Visit-yellow.svg)](http://wohlsoft.ru/PGE/)
[![Official community](https://img.shields.io/badge/Community-Visit-green.svg)](http://wohlsoft.ru/forum/)
[![Official documentation](https://img.shields.io/badge/Documentation-Read-green.svg)](http://wohlsoft.ru/pgewiki/)
[![Contact us by special form](https://img.shields.io/badge/Contact-us-blue.svg)](http://wohlsoft.ru/forum/memberlist.php?mode=contactadmin)
[![Join to our chat server](https://img.shields.io/badge/Discord-Join%20us-7289DA.svg)](https://discord.gg/qPBsvMy)

How to build (Linux, Windows, Mac OS X):
https://wohlsoft.ru/pgewiki/Building_Moondust_Project_from_sources

## Downloads
[![Download latest stable release](https://img.shields.io/badge/Download-Stable-green.svg)](http://wohlsoft.ru/PGE/#links)
[![Official project site](https://img.shields.io/badge/Download-Experimental-orange.svg)](http://wohlsoft.ru/docs/_laboratory/)
[![Configuration packages are requires to work with Moondust Project](https://img.shields.io/badge/Download-Config%20packs-blue.svg)](http://wohlsoft.ru/config_packs/)


## Donate us

[![Donate to this project using Flattr](https://img.shields.io/badge/flattr-donate-green.svg)](https://flattr.com/submit/auto?fid=lgdjjy&url=https%3A%2F%2Fgithub.com%2FWohlSoft%2FPGE-Project)
[![Donate to this project using Patreon](https://img.shields.io/badge/patreon-donate-orange.svg)](http://patreon.com/Wohlstand)
[![Donate to this project using Paypal](https://img.shields.io/badge/paypal-donate-blue.svg)](http://wohlsoft.ru/paypal)


## CI Build status
Linux | Mac OS X | Windows
------------ | ------------- | -------------
[![Build Status](https://semaphoreci.com/api/v1/wohlstand/pge-project/branches/master/shields_badge.svg)](https://semaphoreci.com/wohlstand/pge-project) | [![Build Status](https://travis-ci.org/WohlSoft/Moondust-Project.svg?branch=master)](https://travis-ci.org/WohlSoft/Moondust-Project) | [![Windows CI](https://github.com/WohlSoft/Moondust-Project/workflows/Windows%20CI/badge.svg?branch=master&event=push)](https://github.com/WohlSoft/Moondust-Project/actions?query=workflow%3A%22Windows+CI%22)

# Components
A list of components are included with Moondust Project

## Moondust Engine

![PGE Engine](https://github.com/WohlSoft/PGE-Project/blob/master/Engine/_resources/icon/cat_256.png)

The Moondust Engine is the main part of the Moondust Project, allowing users to play levels and episodes.

Provides a compatibility layer with the legacy game engine (SMBX Engine 1.3) (when used configuration packages which are following [SMBX64 Standard](http://wohlsoft.ru/pgewiki/SMBX64)).

:point_up: **Note** This component is experimental and has not completed base implementation, but can be used for various testing.

[More information on the PGE Wiki...](http://wohlsoft.ru/pgewiki/Moondust_Engine)


## Development Kit
This is a set of tools used for creating levels and episodes for the Moondust Engine and also used to develop levels and episodes for some other projects too.

### Editor

![Moondust Editor](https://github.com/WohlSoft/PGE-Project/blob/master/Editor/_resources/images/cat_builder/cat_builder_256.png)

The PGE Editor is the main development tool, allowing the creation and editing of levels, world maps, and NPC configurations. Maintenance utilities are also provided.

Provides a compatibility layer with the legacy game engine (SMBX Engine 1.3) (when used configuration packages which are following [SMBX64 Standard](http://wohlsoft.ru/pgewiki/SMBX64)).

[More information on the PGE Wiki...](http://wohlsoft.ru/pgewiki/Moondust_Editor)

### Playable Character Calibrator

![Moondust Calibrator](https://github.com/WohlSoft/PGE-Project/blob/master/PlayableCalibrator/_resourses/icon/calibrator_256.png)

The Playable Character Calibrator allows the developer to define a collision box for each respective frame of a playable character, and calibrate any incorrect positions of the frames in an SMBX64 playable character sprite.

[More information on the PGE Wiki...](http://wohlsoft.ru/pgewiki/Playable_character_Calibrator)


### Music Player

![Moondust Music Player](https://github.com/WohlSoft/PGE-Project/blob/master/MusicPlayer/_resources/cat_musplay/cat_musplay_256x256.png)

Music Player is a lightweight music player based on [SDL Mixer X audio library](http://wohlsoft.ru/pgewiki/SDL_Mixer_X) to test music files (include [OGG files with a loop tags](http://wohlsoft.ru/pgewiki/How_To:_Looping_music_files))


### Maintainer wizard

This is a small and user-friendly graphical utility that allows you to process and convert various stuff like level and episode data, music files, graphic resources, etc.


### GIFs to PNG Converter

This is a small console utility that merges bit-mask-based GIF pairs used in the legacy engine into PNG format with alpha channel (semi-transparency) support.

### PNG to GIFs Converter

This is a small console utility that converts PNG images into a pair of GIF files for bit-mask algorithm rendering, simplifying the creation of graphics for the legacy engine.

### LazyFix tool

This is a small console utility that fixes incorrect mask images into valid black&white mask images.

### Palette filter tool

This is a small console utility that adjusts the colors of a given image with the reference picture taken as a palette.



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
