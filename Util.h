#ifndef UTIL_H
#define UTIL_H

#include <sstream>
using std::stringstream;

template<typename T>
T FromString(string key)
{	
	stringstream sstr(key);
	T ret;
	sstr >> ret;
	return ret;
}

template<> bool FromString<bool>(string key);
template<> int FromString<int>(string key);

template<typename T>
string ToString(T key)
{	
	stringstream sstr;
	sstr << key;
	return sstr.str();
}

string ToString(bool key, string truestring="yes", string falsestring="no");

template<typename T>
void SetValue(uchar* pos, T value)
{
	*(T*)pos = value;
}

template<typename T>
T GetValue(uchar* binary, uint pos)
{
	return *(T*)&(binary[pos]);
}

uint EndianSwap(uint n);

#include <ctime>

clock_t ticker();
void Wait_ms(uint n);

string humantime();

#include "ServerSettings.h"
extern vector<ServerSettings> servers;

struct Work
{
	uint server_id;
	vector<uchar> data;
	vector<uchar> target_share;
	vector<uchar> midstate;
	vector<uint> precalc;
	vector<uchar> noncerange;
	unsigned int lastnonce;
	unsigned int firstnonce;
	bool old;
	bool rangeused;
	bool prediction_mode;
	unsigned int prediction_res;
	unsigned int prediction_depth;
	clock_t time;
	ullint ntime_at_getwork;
};

struct Share
{
	uint server_id;
	vector<uchar> data;
	vector<uchar> target;
public:
	Share() {}
	//Share(vector<uchar> data_, uint server_id_, uint user_id_) : server_id(server_id_),data(data_),user_id(user_id_) {}
	Share(vector<uchar> data_,vector<uchar> target_, uint server_id_) : server_id(server_id_),data(data_),target(target_) {}
};

struct RangeOrderShare
{
	unsigned int server_id;
	vector<uchar> data;
	vector<uchar> rangeorder;
public:
	RangeOrderShare() {}
	RangeOrderShare(vector<uchar> data_, unsigned int server_id_, vector<uchar> rangeorder_) : server_id(server_id_), data(data_), rangeorder(rangeorder_) {}	
};

vector<string> Explode(string str, char delim);

#endif
