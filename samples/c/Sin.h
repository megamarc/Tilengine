#ifndef _SIN_H
#define _SIN_H

#ifdef __cplusplus
extern "C"{
#endif

void BuildSinTable (void);
int CalcSin (int angle, int factor);
int CalcCos (int angle, int factor);

#ifdef __cplusplus
}
#endif

#endif
