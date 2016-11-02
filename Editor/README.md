# PGE Editor

![PGE Editor](https://github.com/WohlSoft/PGE-Project/blob/master/Editor/_resources/images/cat_builder/cat_builder_128.png)

The main development tool which allows a creating and editing levels, world map, NPC configurations and providing some maintenance utilities.

Provides a compatibility layer with the legacy game (when used configuration packages which are following [SMBX64 Standard](http://wohlsoft.ru/pgewiki/SMBX64)).

[More information on the PGE Wiki...](http://wohlsoft.ru/pgewiki/PGE_Editor)

# Important files

* **pge_editor.pro** - project file for QMake
* **Editor.qbs** - project file for Qbs, using for some experiments around and doesn't using in regular workflow
* **version.h** - Version information
* **languages.pri** - list of available languages

# Folders

* **languages** - Translations of editor interface into multiple languages
* **_resources** - Hardcoded editor resources
