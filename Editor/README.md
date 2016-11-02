# PGE Editor

![PGE Editor](https://github.com/WohlSoft/PGE-Project/blob/master/Editor/_resources/images/cat_builder/cat_builder_128.png)

A main development tool which allow creating and editing levles, world map, NPC configurations and providing some maintenance utilities.

Provides a compatibility layer with legacy game (when used a configuration packages which are following [SMBX64 Standard](http://wohlsoft.ru/pgewiki/SMBX64)).

# Important files

* **pge_editor.pro** - project file for QMake
* **Editor.qbs** - project file for Qbs, using for some experiments around and doesn't using in regular workflow
* **version.h** - Version information
* **languages.pri** - list of available languages

# Folders

* **languages** - Translations of editor interface into multiple languages
* **_resources** - Hardcoded editor resources
