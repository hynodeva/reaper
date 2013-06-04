#ifndef GLOBAL_H
#define GLOBAL_H

#include "CMakeConf.h"

#define _CRT_SECURE_NO_WARNINGS

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>

#include <vector>
#include <iostream>
#include <string>
#include <deque>
using namespace std;

typedef unsigned long long int ullint;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;

#include "Config.h"

struct Coin
{
	string name;
	string protocol;
	Config config;
	uint global_worksize;//2^(aggression-1)
	uint local_worksize;//2nd dim worksize for kernel, value out of configfile
	uint kernellocal_worksize;//internel local kernel worksize
	uint threads_per_gpu;
	bool max_aggression;
	double sharekhs;//this is due lookup accuracy in kernels. 16bit easy target for ltc
	uint use_noncerange_staticspeed;
	uint cputhreads;
	string cpu_algorithm;
	string host,port,user,pass,proxy;
};

struct GlobalConfs
{
	bool save_binaries;
	vector<uint> devices;
	uint platform;
	bool use_noncerange;
	Coin coin;
};

enum SHARETEST_VALUE
{
	ST_HNOTZERO,
	ST_MORETHANTARGET,
	ST_GOOD,
};

extern GlobalConfs globalconfs;
extern bool shutdown_now;

const uint KERNEL_INPUT_SIZE = 128;
//@todo consider raise that up  to 2048 for large scale computing
const uint KERNEL_OUTPUT_SIZE = 256;
//attention this should be set to res*depth

const uint WORK_EXPIRE_TIME_SEC = 120;
const uint SHARE_THREAD_RESTART_THRESHOLD_SEC = 20;

const uint TARGET_RUNTIME_MS = 320;
const uint TARGET_RUNTIME_ALLOWANCE_MS = 25;
const uint RUNTIMES_SIZE = 16;

const uint CPU_BATCH_SIZE = 1024;

#define foreachgpu() for(vector<_clState>::iterator it = GPUstates.begin(); it != GPUstates.end(); ++it)

#if defined(_M_X64) || defined(__x86_64__)
#define REAPER_PLATFORM "64-bit"
#else
#define REAPER_PLATFORM "32-bit"
#endif

#endif
