set(SRC pthread.c)
add_library(pthread ${SRC})
target_include_directories(pthread PRIVATE .)
target_compile_definitions(pthread PUBLIC
	__CLEANUP_C
	PTW32_BUILD
	PTW32_STATIC_LIB
	HAVE_STRUCT_TIMESPEC
)
