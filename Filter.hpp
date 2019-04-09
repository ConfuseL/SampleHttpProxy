#ifndef FILTER_HPP
#define FILTER_HPP
#include<fstream>
#include<set>
#include<string>
using namespace std;

class Filter
{
set<string> hosts;
set<string> ips;
set<string>::iterator it;
public:
	Filter()
	{
		hosts.clear();
		ips.clear();
		fstream f;
		string temp;
		f.open("ipFilter.f");
		while(getline(f,temp))
		{
			ips.insert(temp);
		}
		f.close();
		f.open("hostFilter.f");
		while(getline(f,temp))
		{
			hosts.insert(temp);
		}
	}
	bool JudgeIp(char* target)
	{
		if(ips.empty())
			return false;
		it=ips.find(target);
		if(it!=ips.end())
			return true;
		else
			return false;
	}
	bool JudgeHost(char* target)
	{
		if(hosts.empty())
			return false;
		it=hosts.find(target);
		if(it!=hosts.end())
			return true;
		else
			return false;
	}
};

#endif