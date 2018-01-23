#pragma once


#ifdef CMDOPERATOR_EXPORTS
#define AFX_EX_CLASS _declspec(dllexport)
#else
#define AFX_EX_CLASS _declspec(dllimport)
#endif

class AFX_EX_CLASS ICmdOperator
{
public:
	static ICmdOperator* createCmdOperator();
	static void destroy(ICmdOperator* cmdop);

	virtual int  init() = 0;
	virtual void close() = 0;
	// ·µ»Ø0³É¹¦
	virtual int  execCmd(const char* command, char* result, const int length) = 0;
	virtual void execCmd(const char* command) = 0;
};
