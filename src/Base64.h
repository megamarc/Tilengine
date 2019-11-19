#ifndef _BASE64_H
#define _BASE64_H

#ifdef __cplusplus
extern "C" {
#endif

	int base64decode(const unsigned char* in, int inLen, unsigned char *out, int *outLen);

#ifdef __cplusplus
}
#endif


#endif
