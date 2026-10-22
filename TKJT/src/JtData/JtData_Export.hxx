#ifndef _JtData_Export_HeaderFile
#define _JtData_Export_HeaderFile

#if defined(_WIN32)
  #if defined(TKJT_EXPORTS) || defined(TKJT_BUILD)
    #define TKJT_EXPORT __declspec(dllexport)
  #else
    #define TKJT_EXPORT __declspec(dllimport)
  #endif
#elif defined(__GNUC__) || defined(__clang__)
  #define TKJT_EXPORT __attribute__((visibility("default")))
#else
  #define TKJT_EXPORT
#endif

#endif // _JtData_Export_HeaderFile
