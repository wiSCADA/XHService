/****************************************************************************
**
**
****************************************************************************/

#ifndef XHSERVICE_H
#define XHSERVICE_H

#include "xhservice_global.h"
#include <string>
#include <vector>
#include <stdint.h>

class XHServiceControllerPrivate;

class XHSERVICE_EXPORT XHServiceController
{
public:
	enum StartupType
	{
		AutoStartup = 0, ManualStartup
	};
	XHServiceController(const std::string &name);
	virtual ~XHServiceController();

	bool isInstalled() const;
	bool isRunning() const;

	std::string serviceName() const;
	std::string serviceDescription() const;
	std::string serviceFilePath() const;	
	StartupType startupType() const;
	static bool install(const std::string &serviceFilePath,
		const std::string &account = std::string(),
		const std::string &password = std::string());
	bool uninstall();

	bool start(const std::vector<std::string> &arguments);
	bool start();
	bool stop();
	bool pause();
	bool resume();
	bool sendCommand(int code);

private:
	XHServiceControllerPrivate *d_ptr;
};

class XHServiceBasePrivate;

class XHSERVICE_EXPORT XHServiceBase
{
public:
	enum MessageType
	{
		Success = 0, Error, Warning, Information
	};

	enum ServiceFlag
	{
		Default = 0x00,
		CanBeSuspended = 0x01,
		CannotBeStopped = 0x02,
		NeedsStopOnShutdown = 0x04
	};
	XHServiceBase(int argc, char **argv, const std::string &name);
	virtual ~XHServiceBase();
	std::string serviceName() const;
	std::string serviceDescription() const;
	void setServiceDescription(const std::string &description);

	XHServiceController::StartupType startupType() const;
	void setStartupType(XHServiceController::StartupType startupType);

	int serviceFlags() const;
	void setServiceFlags(int flags);

	int exec();

	void logMessage(const std::string &message, MessageType type = Success,
		int id = 0, uint16_t category = 0, const std::string &data = std::string());

	static XHServiceBase *instance();

public:
	virtual void createApplication(int &argc, char **argv) = 0;
	virtual void start() = 0;
	virtual int executeApplication() = 0;	
	virtual void stop();
protected:	
	virtual void pause();
	virtual void resume();
	virtual void processCommand(int code);
	void printHelp();
private:

	friend class XHServiceSysPrivate;
	XHServiceBasePrivate *d_ptr;
};

#endif // XHSERVICE_H
