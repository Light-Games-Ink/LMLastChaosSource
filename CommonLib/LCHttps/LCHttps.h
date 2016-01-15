#pragma once

#ifndef __LC_HTTPS_CONNECT__
#define __LC_HTTPS_CONNECT__

#include <iostream>
#include <curl/curl.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>
using namespace std;

#define MAX_CERTS 20
#define FINGERPRINT_FILENAME ".fp"

class CLCHttps
{
private:
	CURL*		m_curl;

public:
	enum { E_INIT_CURL=-1, E_OK=0 , E_CONNECT_URL = 1, E_MATCH_FINGERPRINT=2 };

	CLCHttps()
	{
		m_curl = curl_easy_init();
	}
	virtual ~CLCHttps()
	{
		curl_easy_cleanup(m_curl);
	}

public:
	/*
	param:
	*	url : �������� �ּ�
	*	fingerprint : CA�������� X509�� ��ȯ�Ͽ� base64�� ���ڵ����� sha1 ���� ��ȣȭ�� �� ( ���Ͽ��� �Ѱ��� �� )
	return:
	*	E_CONNECT_URL - ��������
	*	E_MATCH_FINGERPRINT - ��ȣ���� ��ġ��
	*/
	int Connect( const char* url, const char* fingerprint );

private:
	static int	verify_callback(int preverify_ok, X509_STORE_CTX *x509_ctx);
	static CURLcode	sslctxfun(CURL *curl, SSL_CTX *sslctx, void *parm);
	CURLcode	go ( CURL *curl, const char* url );
};
#endif
