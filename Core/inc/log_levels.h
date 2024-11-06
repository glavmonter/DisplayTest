#ifndef LOG_LEVELS_H
#define LOG_LEVELS_H

/**
 * Допустимые уровни логгирования
 *   -RTT_LOG_VERBOSE
 *   -RTT_LOG_DEBUG
 *   -RTT_LOG_INFO
 *   -RTT_LOG_WARN
 *   -RTT_LOG_ERROR
 *   -RTT_LOG_NONE
 */

#if CONFIG_LOG_MAXIMUM_LEVEL > 0

#ifndef LOG_TAG_USB_LOCAL_LEVEL
#define LOG_TAG_USB_LOCAL_LEVEL             RTT_LOG_WARN   ///< Log level for TAG "USB" (Usb CDC high level)
#endif


#ifndef LOG_TAG_MAIN_LEVEL
#define LOG_TAG_MAIN_LEVEL                  RTT_LOG_DEBUG ///< Log level for TAG "MAIN"
#endif

#ifndef LOG_TAG_IIC_LOCAL_LEVEL
#define LOG_TAG_IIC_LOCAL_LEVEL             RTT_LOG_DEBUG
#endif

#else

// Если CONFIG_LOG_MAXIMUM_LEVEL == RTT_LOG_NONE, то все локальные уровни выставляем в RTT_LOG_NONE
#define LOG_TAG_USB_LOCAL_LEVEL             RTT_LOG_NONE
#define LOG_TAG_MAIN_LEVEL                  RTT_LOG_NONE
#define LOG_TAG_IIC_LOCAL_LEVEL             RTT_LOG_NONE
#endif


#endif //LOG_LEVELS_H
