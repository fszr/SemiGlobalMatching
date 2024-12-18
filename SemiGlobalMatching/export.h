#ifdef _WIN32
	#ifdef LIBTSGM_EXPORTS
		#define LIBTSGM_API __declspec(dllexport)
	#else
		#define LIBTSGM_API __declspec(dllimport)
	#endif
#else
	#define LIBTSGM_API
#endif
