#ifndef XHSERVCIE_GLOBAL_H__
#define XHSERVCIE_GLOBAL_H__

#if defined(XHSERVICE_LIBRARY)
#  define XHSERVICE_EXPORT __declspec(dllexport)
#elif  defined(XHSERVICE_STATIC_LIB) // Abuse single files for manual tests
#  define XHSERVICE_EXPORT
#else
#  define XHSERVICE_EXPORT __declspec(dllimport)
#endif

#endif//XHSERVCIE_GLOBAL_H__
