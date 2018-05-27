#include <stdio.h>
#include <stdint.h>

int main() {
	short x = -1;
	uint32_t xx = (uint32_t)x;

	printf("xx = %u\n", xx);
	return 0;
}