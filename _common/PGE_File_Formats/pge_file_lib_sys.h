#ifndef _PGE_FILE_LIB_SYS_H
#define _PGE_FILE_LIB_SYS_H

/*!
 * \file pge_file_lib_sys.h
 * \brief Contains internally used global haders lists
 *
 */

#ifdef PGE_FILES_QT
#include <QTranslator>
#include <QRegExp>
#include <QFileInfo>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QtDebug>
#else
#include <regex>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <limits.h> /* PATH_MAX */
#endif


#endif // _PGE_FILE_LIB_SYS_H

