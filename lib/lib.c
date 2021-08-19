#include<stdio.h>
#include<stdarg.h>
#include<sys/time.h>
/* Input & output functions */
int getint(){int t; scanf("%d",&t); return t; }
int getch(){char c; scanf("%c",&c); return (int)c; }
int getarray(int a[]){
  int n;
  scanf("%d",&n);
  for(int i=0;i<n;i++)scanf("%d",&a[i]);
  return n;
}
void putint(int a){ printf("%d",a);}
void putch(int a){ printf("%c",a); }
void putarray(int n,int a[]){
  printf("%d:",n);
  for(int i=0;i<n;i++)printf(" %d",a[i]);
  printf("\n");
}

/* Timing function implementation */
struct timeval _sysy_start,_sysy_end;
#define _SYSY_N 1024
int _sysy_l1[_SYSY_N],_sysy_l2[_SYSY_N];
int _sysy_h[_SYSY_N], _sysy_m[_SYSY_N],_sysy_s[_SYSY_N],_sysy_ms[_SYSY_N],_sysy_us[_SYSY_N];
int _sysy_idx;
__attribute((constructor)) void before_main(){
  for(int i=0;i<_SYSY_N;i++)
    _sysy_h[i] = _sysy_m[i]= _sysy_s[i] = _sysy_ms[i] = _sysy_us[i] =0;
  _sysy_idx=1;
}  
__attribute((destructor)) void after_main(){
  for(int i=1;i<_sysy_idx;i++){
//    fprintf(stderr,"Timer@%04d-%04d: %dh %dm %ds %dms %dus\n",\
//      _sysy_l1[i],_sysy_l2[i],_sysy_h[i],_sysy_m[i],_sysy_s[i],_sysy_ms[i],_sysy_us[i]);
    _sysy_us[0]+= _sysy_us[i]; 
    _sysy_ms[0] += _sysy_ms[i]; _sysy_us[0] %= 1000;
    _sysy_s[0] += _sysy_s[i]; _sysy_ms[0] %= 1000;
    _sysy_m[0] += _sysy_m[i]; _sysy_s[0] %= 60;
    _sysy_h[0] += _sysy_h[i]; _sysy_m[0] %= 60;
  }
  fprintf(stderr,"TOTAL: %dh %dmin %ds %dms %dus\n",_sysy_h[0],_sysy_m[0],_sysy_s[0],_sysy_ms[0],_sysy_us[0]);
}  
void starttime(){
  _sysy_l1[_sysy_idx] = __LINE__;
  gettimeofday(&_sysy_start,NULL);
}
void stoptime(){
  gettimeofday(&_sysy_end,NULL);
  _sysy_l2[_sysy_idx] = __LINE__;
  _sysy_us[_sysy_idx] += 1000000 * ( _sysy_end.tv_sec - _sysy_start.tv_sec ) + _sysy_end.tv_usec - _sysy_start.tv_usec;
  _sysy_ms[_sysy_idx] += _sysy_us[_sysy_idx] / 1000 ; _sysy_us[_sysy_idx] %= 1000;
  _sysy_s[_sysy_idx] += _sysy_ms[_sysy_idx] / 1000 ; _sysy_ms[_sysy_idx] %= 1000;
  _sysy_m[_sysy_idx] += _sysy_s[_sysy_idx] / 60 ; _sysy_s[_sysy_idx] %= 60;
  _sysy_h[_sysy_idx] += _sysy_m[_sysy_idx] / 60 ; _sysy_m[_sysy_idx] %= 60;
  _sysy_idx ++;
}
