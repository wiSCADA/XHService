/****************************************************************************
**
**
**
****************************************************************************/

#ifndef XHSERVICE_P_H
#define XHSERVICE_P_H

#include <string>
#include <vector>
#include "xhservice.h"

class XHServiceControllerPrivate
{
public:
	std::string serviceName;
    XHServiceController *q_ptr;
};

class XHServiceBasePrivate
{
public:
    XHServiceBasePrivate(const std::string &name);
    ~XHServiceBasePrivate();

    XHServiceBase *q_ptr;

	std::string serviceDescription;
    XHServiceController::StartupType startupType;
	int serviceFlags;
	std::vector<std::string> args;

    static class XHServiceBase *instance;

    XHServiceController controller;

    void startService();
    int run(bool asService, const std::vector<std::string> &argList);
	bool install(const std::string &account, const std::string &password);

    bool start();

	std::string filePath() const;
    bool sysInit();
    void sysSetPath();
    void sysCleanup();
    class XHServiceSysPrivate *sysd;
};

#endif
