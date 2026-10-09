#ifndef LIB_LOGOFF_H
#define LIB_LOGOFF_H


#undef  LOGD
#define LOGD(TAG, FMT, ...)				do{}while(0)

#undef  LOGD_HEX
#define LOGD_HEX(TAG, DESC, DATA, SIZE)	do{}while(0)

#undef  LOGI
#define LOGI(TAG, FMT, ...)				do{}while(0)

#undef  LOGI_HEX
#define LOGI_HEX(TAG, DESC, DATA, SIZE)	do{}while(0)

#undef  LOGW
#define LOGW(TAG, FMT, ...)				do{}while(0)

#undef  LOGW_HEX
#define LOGW_HEX(TAG, DESC, DATA, SIZE)	do{}while(0)

#undef  LOGE
#define LOGE(TAG, FMT, ...)				do{}while(0)

#undef  LOGE_HEX
#define LOGE_HEX(TAG, DESC, DATA, SIZE)	do{}while(0)


#endif
