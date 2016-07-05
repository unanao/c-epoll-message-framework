#ifndef _DEBUG_H_	
#define _DEBUG_H_	

enum {
	ERROR, 
	WARN,
	INFO,
	DEBUG,
	LOG_BUTT,
};

extern void debug_print(int level, const char *file, int line, const char *fmt, ...);
#define DEBUG(level, fmt, ...) debug_print(level, __FUNCTION__, __LINE__, fmt, ## __VA_ARGS__)


#define DEBUG_ERROR(fmt, ...) 	DEBUG(ERROR, fmt, ## __VA_ARGS__) 
#define DEBUG_INFO(fmt, ...) 	DEBUG(INFO, fmt, ## __VA_ARGS__) 
#define DEBUG_WARN(fmt, ...) 	DEBUG(WARN, fmt, ## __VA_ARGS__) 
#define DEBUG_DBG(fmt, ...) 	DEBUG(DEBUG, fmt, ## __VA_ARGS__) 

extern void debug_set_log_level(int level);

#endif

