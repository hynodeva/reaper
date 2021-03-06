
Reaper
======


Reaper is a [OpenCL](http://en.wikipedia.org/wiki/Opencl) GPU  miner for
Scrypt & SHA256D CryptoCoins, coded by mtrlt & hynodeva.com , currently in an early stage of development. It is
open source and licensed under the GPL 3.0.

Reaper is commonly used for scrypt mining.

Reaper works on all GPUs which support OpenCL. Currently you can obtain OpenCL libs for Intel, AMD, and Nvidia devices.
Although ATI GPUs appear to be way more efficient, many types of GPU remain untested.

How to use
----------

Use from the command line, syntax:

    reaper <host> <port> <user> <pass> [config_filename]

Configuration file
------------------

The default configuration file name is **reaper.conf** which should be located
in the same directory as the executable.
The available configuration options are:


    device [number]

Used to specify which GPU devices reaper should use. For example, if a user had
4 GPUs, inserting the following lines into the conf:

    device 0
    device 2

would make Reaper use devices 0 and 2, and leave devices 1 and 3 free. If there
are no device lines in the config, Reaper will attempt to use all available
GPUs.

    threads_per_gpu [number]

How many threads serve each GPU. Different types of GPU may benefit from a
higher or lower number, but 2 is optimal in most cases.

Recommended values: 1, 2 or 4

    aggression [number]/max

How much work is pushed onto the GPU at a time. Higher values for Aggression
typically produce higher hash rates. Experiment with different values to find
the best setting for your system.

From v10 onwards, there is a "maximum aggression" setting. It automatically sets
the aggression to an optimal value. It's useful for dedicated mining machines.
It is enabled like this: `aggression max` instead of a number.

Recommended values: max for dedicated miners, otherwise over 10

    worksize [number]

The size of the work sent to the GPU thread. Experiment with different values to
find the fastest hash rate for your setup. 128 seems to be optimal for most
setups.

Recommended values: 16, 32, 64, 128, 256
Hynodeva: Use lower values for CPU-OpenCL
			Use higher values for new GPUs
reaperv14: worksize can now exceed 256

    kernel [filename]

What file to use as the kernel. The default is reaper.cl.

Recommended value: reaper.cl

    save_binaries [yes/no]

Whether to save binaries after compiling. With this option enabled, subsequent
start-ups are faster. If this option is enabled, remember to delete the
binaries when updating drivers.

Recommended value: yes

    platform [number]

Select which OpenCL platform to use. For example the AMD one is called "AMD Accelerated Parallel Processing". The NVIDIA one is "CUDA something". Usually platform number 0 is the one you want.

Recommended value: 0

    enable_graceful_shutdown [yes/no]

Whether to enable the "Graceful Shutdown" option. When this is enabled, users
can press "Q" then "Enter" to shut down Reaper gracefully.

Recommended value: yes


	long_polling [yes/no]
	
Whether to enable the long polling support.

Recommended value: yes

	host [address]
	port [number]
	user [text]
	pass [text]
	
You can now configure the server's info in the config file instead of having
to use command line arguments.

	proxy [address]

Proxy information such as:
socks4://user:pass@proxyaddr:port

	include [filename]

Loads a config file and its settings.

#can put the host/port/user/pass info to the config instead
#proxy settings
#include directive in config

	getwork_rate [ms]

sets the getwork refresh in miliseconds (should be at least 1000, 5000 is recommend, 7500 default)

	use_noncerange [true/false]	

enabled noncerange support (experimental)

	use_noncerange_staticspeed [number]

sets the speed which is reported to server when using noncerange
do not set this much lower then your hashspeed is, otherwise you will spam only getworks without hashing.


	thread_concurrency [number]

should be a multiple of worksize, this sets the buffer size when hashing scrypt coins

lookup_gap [number]

this sets the shader/memory tradeoff which is used when hashing scrypt on GPUs
can be 1 to N(1024 in case of litecoin, current nfactor in case of yacoin)
usually 2 is best.
known expections:
	using CPU-OpenCL with reaper (mostly 1 best, thread_concurrency should be L2 cache size / 128kb  ,worksize CPUCores)
	ATI 4890 Vapor-X (1 best, with 4096 thread concurrency & workersize 64)

	mine [coinname]

sets the coin protocol which is used for mining.
currently supported:

bitcoin (all sha256d coins)

litecoin (all scrypt 1024_1_1_8 coins)

UPCOMING:

yacoin (scrypt with variable N & Keccak/Cacha)

netcoin (scrypt multiple setups)

terracoin& ppcoin same as bitcoin, just for having the opportunity of a seperate coin config file
feathercoin - same as litecoin, just for having the opportunity of a seperate coin config file

Compiling
---------

Requirements:

CMake
opencl header files (AMD SDK e.g., or Intel OpenCL SDK)

Reaper is compiled using [CMake](http://www.cmake.org/). If you're on Windows,
you can use the supplied >>cmake-win.cmd<< batch file. Under Linux, there is a >>cmake-linux.sh<< Otherwise, issue the
following commands in the reaper directory:

    mkdir build
    cd build
    cmake -D CMAKE_BUILD_TYPE=Release ..
    make


After compiling you can move the resulting `reaper` binary where you want, but
make sure to take along the `reaper.cl` kernel file as well as the `reaper.conf`
configuration file found in the root directory.

Known bugs
----------

    Kernel build not successful: -46

This bug seems to occur on systems with multiple GPUs. The best available
workaround is to try repeatedly until the error goes away.

    Windows reports MSVCP100.DLL missing

Install the [Microsoft Visual C++ 2010 Redistributable Package](http://www.microsoft.com/download/en/details.aspx?id=5555)

See Also
--------

[Mining Hardware Performance](http://wiki.solidcoin.info/wiki/Mining_Hardware_Performance)

