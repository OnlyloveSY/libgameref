#include <vector>
#include <unordered_set>
#include <set>
#ifdef __linux__
#include <sys/time.h>
#include <unistd.h>
#endif
int64_t getTime() {
#ifdef __linux__
	struct timeval tv1;
	gettimeofday(&tv1, NULL);
	auto t1 = tv1.tv_sec * 1000 + tv1.tv_usec / 1000;
	return t1;
#else
	return 0;
#endif

}