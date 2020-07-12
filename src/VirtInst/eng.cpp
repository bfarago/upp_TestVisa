/*
Small engineering double to string format function. Header file.
Author: Barna Farago MYND-ideal. (C)2020
*/
#include "VirtInst.h"

#define MICRO "µ"
#define PREFIX_START (-24)

#include <stdio.h>
#include <math.h>

static const char *eng_prefix[] = {
  "y", "z", "a", "f", "p", "n", MICRO, "m", "",
  "k", "M", "G", "T", "P", "E", "Z", "Y"
};
static const double eng_minuspower10[]={
 1,	0.1 , 0.01, 0.001, 0.0001, 0.00001, 0.000001, 0.0000001, 0.00000001, 0.000000001, 0.0000000001, 0.00000000001, 0.000000000001
};
//"yzafpn" MICRO "mkMGTPEZY";
#define ENG_MAX_UEXPS (17)
static const unsigned char eng_uexpletters[ENG_MAX_UEXPS+2]="EGMPTYZafkmnpuyzµ";
static const signed char eng_uexps[ENG_MAX_UEXPS]={18, 9, 6, 15, 12, 24, 21, -18, -15, 3, -3, -9, -12, -6, -24, -21, -6};

signed char eng_getexp(unsigned char c)
{
	if (c<'0') return 0;
	int m=16;
	int i=0;
	while (m>0){
		unsigned char cm=0xff;
		i+=m;
		if (i<ENG_MAX_UEXPS) cm= eng_uexpletters[i];
		if (c<cm){
			i-=m;
		}else if (c==cm){
			return eng_uexps[i];
		}
		m>>=1;
	}
	return 0;
}

char *eng(double value, int digits, int numeric)
{
#define PREFIX_END (PREFIX_START+\
(int)((sizeof(eng_prefix)/sizeof(char *)-1)*3))

      int expof10;
      static char result[100];
      char *res = result;

      if (value < 0.)
        {
            *res++ = '-';
            value = -value;
        }
      if (value == 0.)
        {
	    return "0.0 ";
        }

      expof10 = (int) log10(value);
      if(expof10 > 0)
        expof10 = (expof10/3)*3;
      else
        expof10 = (-expof10+3)/3*(-3);
 
      value *= pow(10,-expof10);

      if (value >= 1000.)
         { value /= 1000.0; expof10 += 3; }
      else if(value >= 100.0)
         digits -= 2;
      else if(value >= 10.0)
         digits -= 1;

      if(numeric || (expof10 < PREFIX_START) ||
                    (expof10 > PREFIX_END))
        sprintf(res, "%.*fe%d", digits-1, value, expof10);
      else
        sprintf(res, "%.*f %s", digits-1, value,
          eng_prefix[(expof10-PREFIX_START)/3]);
      return result;
}
double eng_2d(const char *s)
{
	double d=0;
	size_t size=strlen(s);
	int dot=0;
	for (int i=0; i<size; i++){
		char c= s[i];
		if (c>='0' && c<='9'){
			int v=c-'0';
			if (dot){
				if (0!=v) d=d+ v*eng_minuspower10[dot];
				dot++;
			}else{
				d=d*10+v;
			}
		}else if (','==c || '.'==c){
			dot++;
		}else if (' '==c || 'V'==c || 'A'==c){
			continue;
		}else if (0==c){
			break;
		}else{
			signed char ex= eng_getexp((unsigned char)c);
			if (0!=ex){
				d*=pow(10, ex);
			}
		}
	}

	return d;
}