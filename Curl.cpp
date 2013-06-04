#include "Global.h"
#include "Curl.h"

#include "curl/curl.h"
#include "Util.h"

void Curl::GlobalInit()
{
	curl_global_init(CURL_GLOBAL_ALL);
}

void Curl::GlobalQuit()
{
	curl_global_cleanup();
}

void* Curl::Init()
{
	void* curl = curl_easy_init();
	if (curl == NULL)
	{
		throw string("libcurl initialization failure");
	}
	return curl;
}

void Curl::Quit(void* curl)
{
	if (curl != NULL)
	{
		curl_easy_cleanup(curl);
	}
}

size_t ResponseCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
	string* getworksentdata = (string*)data;
	try 	
	{ 	
		for(uint i=0; i<size*nmemb; ++i) 	
		{ 		
			if(ptr!=NULL && data!=NULL) 		
			{ 			
				char c = ((char*)ptr)[i]; 			
				getworksentdata->push_back(c); 		
			} 	
		}
	} 	
	catch(std::exception s) 	
	{ 		
		cout << "(1) Error: " << s.what() << endl; 	
	}
	return size*nmemb; 
}

string longpoll_url;
bool longpoll_active=false;
size_t HeaderCallback( void *ptr, size_t size, size_t nmemb, void *userdata)
{
	string hdr;
	for(uint i=0; i<size*nmemb; ++i)
	{
		char c = ((char*)ptr)[i];
		hdr.push_back(c);
	}
	if (!longpoll_active && hdr.length() >= 0x10 && hdr.substr(0,0xF) == "X-Long-Polling:")
	{
		longpoll_url = hdr.substr(0x10);
		longpoll_url = longpoll_url.substr(0, longpoll_url.length()-2);
		cout << "Longpoll url -->" << longpoll_url << "<-- " << endl;
		longpoll_active = true;
	}
	return size*nmemb;
}

string Curl::GetWork_LP(ServerSettings& s, string path, uint timeout)
{
	return Execute(s,GETWORK_LP, "", path, timeout);
}

string Curl::GetWork_NP(ServerSettings& s, string path, uint timeout)
{
	return Execute(s,GETWORK_NP, "", path, timeout);
}

string Curl::GetWork(ServerSettings& s, string path, uint timeout)
{
	return Execute(s,GETWORK, "", path, timeout);
}

string Curl::TestWork(ServerSettings& s, string work)
{
	return Execute(s,TESTWORK, work, "", 30);
}

string Curl::TestWork_NP(ServerSettings& s, string work)
{
	return Execute(s,TESTWORK_NP, work, "", 10);
}

string Curl::Execute(ServerSettings& s, Curl::EXEC_TYPE type, string work, string path, uint timeout)
{
	void* curl = Init();
	string responsedata;
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	///@todo impl. https mining
	curl_easy_setopt(curl, CURLOPT_URL, ("http://" + s.host + path).c_str());
	curl_easy_setopt(curl, CURLOPT_USERPWD, (s.user + ":" + s.pass).c_str());
	curl_easy_setopt(curl, CURLOPT_PORT, s.port);

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ResponseCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responsedata);

	if (s.proxy != "")
		curl_easy_setopt(curl, CURLOPT_PROXY, s.proxy.c_str());

	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	if(globalconfs.coin.protocol == "litecoin")
	{
		if((globalconfs.coin.use_noncerange_staticspeed>0) && (globalconfs.coin.config.GetValue<bool>("use_noncerange")))
			Execute_LTC(curl,type,work,path,timeout,globalconfs.coin.use_noncerange_staticspeed);
		else if((s.hashespersec>1000) && (globalconfs.coin.config.GetValue<bool>("use_noncerange")))
			Execute_LTC(curl,type,work,path,timeout,s.hashespersec);
		else
			Execute_LTC(curl,type,work,path,timeout,0);
	}
	else if (globalconfs.coin.protocol == "bitcoin")
	{
		if((globalconfs.coin.use_noncerange_staticspeed>0) && (globalconfs.coin.config.GetValue<bool>("use_noncerange")))
			Execute_BTC(curl,type,work,path,timeout,s.hashespersec);
		else if((s.hashespersec>1000) && (globalconfs.coin.config.GetValue<bool>("use_noncerange")))
			Execute_BTC(curl,type,work,path,timeout,s.hashespersec);
		else
			Execute_BTC(curl,type,work,path,timeout,0);	
	}
	else
		cout << "Wrong protocol type " << globalconfs.coin.protocol << "." << endl;
	Quit(curl);
	return responsedata;
}

void Curl::Execute_BTC(void* curl, Curl::EXEC_TYPE type, string work, string path, uint timeout,unsigned int hashespersec=0)
{
	curl_slist* headerlist = NULL;
	headerlist = curl_slist_append(headerlist, "Accept: */*");
	headerlist = curl_slist_append(headerlist, "Content-Type: application/json");
	headerlist = curl_slist_append(headerlist, "User-Agent: reaper/" REAPER_VERSION);
	headerlist = curl_slist_append(headerlist, "User-Agent: jansson 1.3");
	if((hashespersec>0) && (globalconfs.coin.config.GetValue<bool>("use_noncerange")))
		headerlist = curl_slist_append(headerlist, "X-Mining-Extensions: midstate rollntime noncerange");
	else
		headerlist = curl_slist_append(headerlist, "X-Mining-Extensions: midstate rollntime");
	///@todo switchto hostlist extension support
//	headerlist = curl_slist_append(headerlist, "X-Mining-Hashrate: /" +string(hashespersec));

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);

	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, timeout);
	
	if (type == GETWORK_LP)
	{
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, NULL);
		curl_easy_setopt(curl, CURLOPT_POST, 0);
	}
	else if (type == GETWORK)
	{
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, "{\"method\":\"getwork\",\"params\":[],\"id\":\"1\"}");
	}
	else if (type == TESTWORK)
	{
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		string str = "{\"method\":\"getwork\",\"id\":\"1\",\"params\":[\"" + work + "\"]}";
		curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, str.c_str());
	}
	else
	{
		cout << "Unknown case " << (int)type << " in Curl::Execute()" << endl;
	}
	CURLcode code = curl_easy_perform(curl);
	if(code != CURLE_OK && type != TESTWORK)
	{
		if (code == CURLE_COULDNT_CONNECT)
		{
			cout << "Could not connect. Server down?" << endl;
		}
		else
		{
			cout << "Error " << code << " getting work. See http://curl.haxx.se/libcurl/c/libcurl-errors.html for error code explanations." << endl;
		}
	}
	curl_slist_free_all(headerlist);
}

void Curl::Execute_LTC(void* curl, Curl::EXEC_TYPE type, string work, string path, uint timeout,unsigned int hashespersec)
{
#ifdef HEAVYDEBUG
	cout << "executing LTC" <<endl;
#endif
	curl_slist* headerlist = NULL;
	headerlist = curl_slist_append(headerlist, "Accept: */*");
	headerlist = curl_slist_append(headerlist, "Content-Type: application/json");
	headerlist = curl_slist_append(headerlist, "User-Agent: reaper/" REAPER_VERSION);
	headerlist = curl_slist_append(headerlist, "User-Agent: jansson 1.3");
	if((hashespersec>0) && (globalconfs.coin.config.GetValue<bool>("use_noncerange")))
		headerlist = curl_slist_append(headerlist, "X-Mining-Extensions: midstate rollntime noncerange");
	else
		headerlist = curl_slist_append(headerlist, "X-Mining-Extensions: midstate rollntime");
	if(hashespersec>0)
	{
		string hashratestr="X-Mining-Hashrate: ";
		hashratestr+=ToString(hashespersec);
		headerlist = curl_slist_append(headerlist,  hashratestr.c_str());
	}
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);

	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, timeout);
	string str ;
#ifdef HEAVYDEBUG
	cout << "executing LTC headers done" <<endl;
#endif
		switch(type)
	{
		case GETWORK_LP:
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, NULL);
			curl_easy_setopt(curl, CURLOPT_POST, 0);
			break;
		case GETWORK:
			curl_easy_setopt(curl, CURLOPT_POST, 1);
			curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, "{\"method\":\"getwork\",\"params\":[],\"id\":1}");
//			curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, "{\"method\":\"sc_getwork\",\"params\":[],\"id\":1}");
		break;
		case TESTWORK:
			curl_easy_setopt(curl, CURLOPT_POST, 1);
			str = "{\"method\":\"getwork\",\"id\":\"1\",\"params\":[\"" + work + "\"]}";
			curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, str.c_str());
		break;
		case GETWORK_NP:
			curl_easy_setopt(curl, CURLOPT_POST, 1);
			curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, "{\"method\":\"getwork_np\",\"params\":[],\"id\":1}");
			break;
		case TESTWORK_NP:
			curl_easy_setopt(curl, CURLOPT_POST, 1);
			str = "{\"method\":\"getwork_np\",\"id\":\"1\",\"params\":[\"" + work + "\"]}";
			curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, str.c_str());
			break;
		default:
			cout << "Unknown case " << (int)type << " in Curl::Execute()" << endl;
		break;
	};

	/**
	if (type == GETWORK_LP)
	{
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, NULL);
		curl_easy_setopt(curl, CURLOPT_POST, 0);
	}
	else if (type == GETWORK)
	{
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, "{\"method\":\"getwork\",\"params\":[],\"id\":\"1\"}");
	}
	else if (type == TESTWORK)
	{
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		string str = "{\"method\":\"getwork\",\"id\":\"1\",\"params\":[\"" + work + "\"]}";
		curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, str.c_str());
	}
	else
	{
		cout << "Unknown case " << (int)type << " in Curl::Execute()" << endl;
	}
	**/
#ifdef HEAVYDEBUG
	cout << "curl perform start" <<endl;
#endif
	CURLcode code = curl_easy_perform(curl);
#ifdef HEAVYDEBUG
	cout << "curl perform done" <<endl;
#endif
	if(code != CURLE_OK && type != TESTWORK)
	{
		if (code == CURLE_COULDNT_CONNECT)
		{
			cout << "Could not connect. Server down?" << endl;
		}
		else
		{
			cout << "Error " << code << " getting work. See http://curl.haxx.se/libcurl/c/libcurl-errors.html for error code explanations." << endl;
		}
	}
	curl_slist_free_all(headerlist);
}

