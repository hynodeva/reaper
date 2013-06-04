#ifndef SERVERSETTINGS_H
#define SERVERSETTINGS_H

class ServerSettings
{
public:
	string host,user,pass,proxy;
	unsigned short port;
	unsigned int hashespersec;
	bool noncerange_prediction_primary;
	string ToString()
	{
		return string("http://") + user + ":" + pass  + "@" + host + ":" + ::ToString(port) + "/";
	}
};

#endif
