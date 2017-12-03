#ifndef QTWAIN_GLOBAL_H
#define QTWAIN_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QTwain_LIBRARY)
#  define QTwain_EXPORT Q_DECL_EXPORT
#else
#  define QTwain_EXPORT Q_DECL_IMPORT
#endif

#endif // QTWAIN_GLOBAL_H

