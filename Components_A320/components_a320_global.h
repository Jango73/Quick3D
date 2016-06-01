
#ifndef COMPONENTS_A320_GLOBAL_H
#define COMPONENTS_A320_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qvector.h>
#include <QtCore/qmap.h>
#include <QtCore/qsettings.h>
#include <QCoreApplication>

#ifdef COMPONENTS_A320_LIB
# define COMPONENTS_A320_EXPORT Q_DECL_EXPORT
#else
# define COMPONENTS_A320_EXPORT Q_DECL_IMPORT
#endif

#endif // COMPONENTS_A320_GLOBAL_H
