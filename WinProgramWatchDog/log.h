#ifndef __WATCH_DOG_LOGH__
#define __WATCH_DOG_LOGH__
#endif

#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <time.h>
#include <fstream>
#include <Windows.h>

#ifndef _LOG_LEVEL_
#define _LOG_LEVEL_ 1 //1全部输出  2输出警告和错误 3仅仅输出错误
#endif

namespace watch_dog_log
{
	inline void GetToday(char * recvstr) {
		//获取当前日期的字符串
		time_t timep;
		time(&timep);
		struct tm *p = (tm *)malloc(sizeof(tm));
		localtime_s(p, &timep);
		_snprintf(recvstr, 64, "%d-%d-%d", 1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday);
		delete p;
	}

	inline void GetCurrentTimeSelf(char * recvstr) {
		//获取当前的时间字符串
		time_t timep;
		time(&timep);
		struct tm *p = (tm *)malloc(sizeof(tm));
		localtime_s(p, &timep);
		_snprintf(recvstr, 64, "%d-%d-%d %2d:%2d:%2d ", 1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
		delete p;
	}

	inline void GetFileName(char * filename, const char * in_type, std::string location) {
		//获取日志文件名字
		//yyyy-mm-dd-type.log
		char * today = (char *)malloc(64);
		GetToday(today);
		_snprintf(filename, 64, "./log/%s-%s-%s.log", location.c_str(), today, in_type);
		delete today;
	}

	inline void LogInfo(std::string data, std::string location, bool check = true) {
		//信息日志
		if (_LOG_LEVEL_ == 1 || !check) {
			std::ofstream fs;
			char * filename = (char *)malloc(64);
			char * currenttime = (char *)malloc(64);
			GetFileName(filename, "Info", location);
			GetCurrentTimeSelf(currenttime);
			fs.open(filename, std::ios::app);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
			printf("[%s] %s\n", currenttime, data.c_str());
			fs << currenttime << data << std::endl;
			fs.close();
			delete filename;
			delete currenttime;
		}
	}
	inline void LogError(std::string data, std::string location) {
		//错误日志
		std::ofstream fs;
		char * filename = (char *)malloc(64);
		char * currenttime = (char *)malloc(64);
		GetFileName(filename, "Error", location);
		GetCurrentTimeSelf(currenttime);
		fs.open(filename, std::ios::app);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
		printf("[%s] %s\n",currenttime, data.c_str());
		fs << currenttime << data << std::endl;
		fs.close();
		delete filename;
		delete currenttime;
	}
	inline void LogWarnings(std::string data, std::string location) {
		//警告日志
		if (_LOG_LEVEL_ != 3) {
			std::ofstream fs;
			char * filename = (char *)malloc(64);
			char * currenttime = (char *)malloc(64);
			GetFileName(filename, "Warnings", location);
			GetCurrentTimeSelf(currenttime);
			fs.open(filename, std::ios::app);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE);
			printf("[%s] %s\n", currenttime, data.c_str());
			fs << currenttime << data << std::endl;
			fs.close();
			delete filename;
			delete currenttime;
		}
	}

	void inline traceNormal(char * format, ...) {
#if _LOG_LEVEL_ ==1
		char s[128];
		va_list arg_ptr;
		va_start(arg_ptr, format);
		_vsnprintf(s, sizeof(s) - 1, format, arg_ptr);
		std::string info = s;
		LogInfo(info,"watch_dog");
#endif
	}
	//info日志 -- 不计入文件
	void inline traceInfo(char * format, ...) {
#if _LOG_LEVEL_ ==1
		char s[128];
		va_list arg_ptr;
		va_start(arg_ptr, format);
		_vsnprintf(s, sizeof(s) - 1, format, arg_ptr);
		std::string info = s;
		LogInfo(info,"watch_dog");
#endif
	}
	//warnings -- 写入文件
	void inline traceWarnings(char * format, ...) {
#if _LOG_LEVEL ==1 || _LOG_LEVEL ==2
		char s[128];
		va_list arg_ptr;
		va_start(arg_ptr, format);
		_vsnprintf(s, sizeof(s) - 1, format, arg_ptr);
		std::string info = s;
		LogWarnings(info, "GAME");
#endif
	}
	//error ---写入文件
	void inline traceError(char * format, ...) {
		char s[128];
		va_list arg_ptr;
		va_start(arg_ptr, format);
		_vsnprintf(s, sizeof(s) - 1, format, arg_ptr);
		std::string info = s;
		LogError(info, "watch_dog");
	}
	//直接写入文件
	void inline writeLog(char *logtype, char * format, ...) {
		char s[128];
		va_list arg_ptr;
		va_start(arg_ptr, format);
		_vsnprintf(s, sizeof(s) - 1, format, arg_ptr);
		std::string info = s;
		LogInfo(info, logtype, false);
	}

};