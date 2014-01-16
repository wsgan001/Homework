//使用17.14格式，17位整数，14位小数，整型左移14位即为浮点型
/*
Convert n to fixed point:	n * f
Convert x to integer (rounding toward zero):	x / f
Convert x to integer (rounding to nearest):	(x + f / 2) / f if x >= 0, 
(x - f / 2) / f if x <= 0.
Add x and y:	x + y
Subtract y from x:	x - y
Add x and n:	x + n * f
Subtract n from x:	x - n * f
Multiply x by y:	((int64_t) x) * y / f
Multiply x by n:	x * n
Divide x by y:	((int64_t) x) * f / y
Divide x by n:	x / n
*/ 

#define FLOATM 16384

/*整型转浮点*/
#define INT2FLOAT(n) (n*FLOATM)

/*浮点数转整型，向下取整*/
#define F2INTTRIM(x) (x/FLOATM)

/*浮点数转整型，四舍五入 */
#define F2INT45(x) (x>=0?((x+FLOATM/2)/FLOATM):((x-FLOATM/2)/FLOATM))

/*浮点数加整数*/
#define FADDI(x,n) (x+n*FLOATM)

/*浮点数减整数 */
#define FSUBI(x,n) (x-n*FLOATM) 

/*浮点数相乘*/
#define FMULF(x,y) (((int64_t)x)*y/FLOATM)

/*浮点数乘整数*/
#define FMULI(x,n) (x*n)

/*浮点数相除*/
#define FDIVF(x,y) (((int64_t)x)*FLOATM/y)

/*浮点数除以整数*/
#define FDIVI(x,n) (x/n)






 





