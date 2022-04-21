#ifndef XML_PARSING_GLOBAL_H
#define XML_PARSING_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XML_PARSING_LIBRARY)
#  define XML_PARSING_EXPORT Q_DECL_EXPORT
#else
#  define XML_PARSING_EXPORT Q_DECL_IMPORT
#endif

#endif // XML_PARSING_GLOBAL_H
