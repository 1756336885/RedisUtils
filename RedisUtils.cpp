// RedisUtils.cpp : 定义 DLL 的导出函数。
//

#include "pch.h"
#include "framework.h"
#include "RedisUtils.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock.h>
#include "include\hiredis\hiredis.h"
#include <iostream>
#include<Windows.h>

#define NO_QFORKIMPL
using namespace std;





REDISUTILS_API int testRedis(char* host, int input_port) {
	
	
	LPCTSTR lpFileName = TEXT("D:\\work\\C++\\RedisUtils\\config.ini");

	//读配置
	WCHAR content[100];
	DWORD v = ::GetPrivateProfileString(TEXT("config"), TEXT("host"), TEXT(""), content, 100, lpFileName);
	char* m_char;
	int len = (int)WideCharToMultiByte(CP_ACP, 0, content, (int)wcslen(content), NULL, 0, NULL, NULL);
	m_char = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, content, (int)wcslen(content), m_char, len, NULL, NULL);
	m_char[len] = '\0';
	printf("host : %s\n", m_char);


	DWORD resv = ::GetPrivateProfileInt(TEXT("config"), TEXT("port"), 0, lpFileName);
	cout << "resv:" << resv << endl;
	int p2 = resv;
	printf("port :%i\n", p2);



	unsigned int j;
	redisContext* c;
	redisReply* reply;
	const char* hostname = host;
	int port = input_port;
	struct timeval timeout = { 1, 500000 }; // 1.5 seconds
	c = redisConnectWithTimeout(hostname, port, timeout);
	if (c == NULL || c->err) {
		if (c) {
			printf("Connection error: %s\n", c->errstr);
			redisFree(c);
		}
		else {
			printf("Connection error: can't allocate redis context\n");
		}
		exit(1);
	}

	/* PING server */
	reply = (redisReply*)redisCommand(c, "PING");
	printf("PING: %s\n", reply->str);
	freeReplyObject(reply);

	/* Set a key */
	reply = (redisReply*)redisCommand(c, "SET %s %s", "foo", "hello world");
	printf("SET: %s\n", reply->str);
	freeReplyObject(reply);

	/* Set a key using binary safe API */
	reply = (redisReply*)redisCommand(c, "SET %b %b", "bar", (size_t)3, "hello", (size_t)5);
	printf("SET (binary API): %s\n", reply->str);
	freeReplyObject(reply);

	/* Try a GET and two INCR */
	reply = (redisReply*)redisCommand(c, "GET foo");
	printf("GET foo: %s\n", reply->str);
	freeReplyObject(reply);

	reply = (redisReply*)redisCommand(c, "INCR counter");
	printf("INCR counter: %lld\n", reply->integer);
	freeReplyObject(reply);
	/* again ... */
	reply = (redisReply*)redisCommand(c, "INCR counter");
	printf("INCR counter: %lld\n", reply->integer);
	freeReplyObject(reply);

	/* Create a list of numbers, from 0 to 9 */
	reply = (redisReply*)redisCommand(c, "DEL mylist");
	freeReplyObject(reply);
	for (j = 0; j < 10; j++) {
		char buf[64];

		snprintf(buf, 64, "%d", j);
		reply = (redisReply*)redisCommand(c, "LPUSH mylist element-%s", buf);
		freeReplyObject(reply);
	}

	/* Let's check what we have inside the list */
	reply = (redisReply*)redisCommand(c, "LRANGE mylist 0 -1");
	if (reply->type == REDIS_REPLY_ARRAY) {
		for (j = 0; j < reply->elements; j++) {
			printf("%u) %s\n", j, reply->element[j]->str);
		}
	}
	freeReplyObject(reply);

	/* Disconnects and frees the context */
	redisFree(c);

	return 0;
}



