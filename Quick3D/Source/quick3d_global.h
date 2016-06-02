
#ifndef QUICK3D_GLOBAL_H
#define QUICK3D_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qvector.h>
#include <QtCore/qmap.h>
#include <QtCore/qsettings.h>
#include <QCoreApplication>

#ifdef QUICK3D_LIB
# define QUICK3D_EXPORT Q_DECL_EXPORT
#else
# define QUICK3D_EXPORT Q_DECL_IMPORT
#endif

#endif // QUICK3D_GLOBAL_H
