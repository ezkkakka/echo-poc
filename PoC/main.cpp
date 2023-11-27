#include "driver.hpp"

int main()
{
	DriverHelper driver;

	int val = 100;
	const auto val_val = driver.ReadMemory<int>((uintptr_t)&val);

	printf("[*] val: %d\n", val_val);

	return 0;
}