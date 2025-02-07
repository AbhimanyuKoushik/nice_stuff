#include "stdio.h"
#include "stdlib.h"

float fastinvsqrt(float number){
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y = number;
	i = * ( long * ) &y;
	i = 0x5f3759df - ( i >> 1 );
	y = * ( float * ) &i;
	y = y * ( threehalfs - ( x2 * y * y ) );
	y = y * ( threehalfs - ( x2 * y * y ) );

	return y;
}

int main(){
	printf("1/square root of 4 is %f", fastinvsqrt(4));
	return 0;
}
