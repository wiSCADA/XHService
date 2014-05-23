/****************************************************************************
**
**
****************************************************************************/

#include "../xhservice.h"
#include "../xhservice_p.h"
#include <functional>
#include <stdio.h>
#include <windows.h>
#include <iostream>

typedef SERVICE_STATUS_HANDLE(WINAPI*PRegisterServiceCtrlHandler)(LPCTSTR, LPHANDLER_FUNCTION);
static PRegisterServiceCtrlHandler pRegisterServiceCtrlHandler = 0;
typedef BOOL(WINAPI*PSetServiceStatus)(SERVICE_STATUS_HANDLE, LPSERVICE_STATUS);
static PSetServiceStatus pSetServiceStatus = 0;
typedef BOOL(WINAPI*PChangeServiceConfig2)(SC_HANDLE, DWORD, LPVOID);
static PChangeServiceConfig2 pChangeServiceConfig2 = 0;
typedef BOOL(WINAPI*PCloseServiceHandle)(SC_HANDLE);
static PCloseServiceHandle pCloseServiceHandle = 0;
typedef SC_HANDLE(WINAPI*PCreateService)(SC_HANDLE, LPCTSTR, LPCTSTR, DWORD, DWORD, DWORD, DWORD, LPCTSTR, LPCTSTR, LPDWORD, LPCTSTR, LPCTSTR, LPCTSTR);
static PCreateService pCreateService = 0;
typedef SC_HANDLE(WINAPI*POpenSCManager)(LPCTSTR, LPCTSTR, DWORD);
static POpenSCManager pOpenSCManager = 0;
typedef BOOL(WINAPI*PDeleteService)(SC_HANDLE);
static PDeleteService pDeleteService = 0;
typedef SC_HANDLE(WINAPI*POpenService)(SC_HANDLE, LPCTSTR, DWORD);
static POpenService pOpenService = 0;
typedef BOOL(WINAPI*PQueryServiceStatus)(SC_HANDLE, LPSERVICE_STATUS);
static PQueryServiceStatus pQueryServiceStatus = 0;
typedef BOOL(WINAPI*PStartServiceCtrlDispatcher)(CONST SERVICE_TABLE_ENTRY*);
static PStartServiceCtrlDispatcher pStartServiceCtrlDispatcher = 0;
typedef BOOL(WINAPI*PStartService)(SC_HANDLE, DWORD, const char**);
static PStartService pStartService = 0;
typedef BOOL(WINAPI*PControlService)(SC_HANDLE, DWORD, LPSERVICE_STATUS);
static PControlService pControlService = 0;
typedef HANDLE(WINAPI*PDeregisterEventSource)(HANDLE);
static PDeregisterEventSource pDeregisterEventSource = 0;
typedef BOOL(WINAPI*PReportEvent)(HANDLE, WORD, WORD, DWORD, PSID, WORD, DWORD, LPCTSTR*, LPVOID);
static PReportEvent pReportEvent = 0;
typedef HANDLE(WINAPI*PRegisterEventSource)(LPCTSTR, LPCTSTR);
static PRegisterEventSource pRegisterEventSource = 0;
typedef DWORD(WINAPI*PRegisterServiceProcess)(DWORD, DWORD);
static PRegisterServiceProcess pRegisterServiceProcess = 0;
typedef BOOL(WINAPI*PQueryServiceConfig)(SC_HANDLE, LPQUERY_SERVICE_CONFIG, DWORD, LPDWORD);
static PQueryServiceConfig pQueryServiceConfig = 0;
typedef BOOL(WINAPI*PQueryServiceConfig2)(SC_HANDLE, DWORD, LPBYTE, DWORD, LPDWORD);
static PQueryServiceConfig2 pQueryServiceConfig2 = 0;

static bool winServiceInit()
{
	if (!pOpenSCManager) {
		HINSTANCE hdll=NULL;
		// Get a handle to the DLL module.
		hdll = LoadLibrary("advapi32.dll");
		if (!hdll){
			LPTSTR s;
			::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,
				GetLastError(),
				0,
				(LPTSTR)&s,
				0,
				NULL);
			printf("%d\n%s\n", GetLastError(), s);
			return false;
		}
		pRegisterServiceCtrlHandler = (PRegisterServiceCtrlHandler)GetProcAddress(hdll, "RegisterServiceCtrlHandlerA");		
		pSetServiceStatus = (PSetServiceStatus)GetProcAddress(hdll, "SetServiceStatus");		
		pChangeServiceConfig2 = (PChangeServiceConfig2)GetProcAddress(hdll, "ChangeServiceConfig2A");
		pCloseServiceHandle = (PCloseServiceHandle)GetProcAddress(hdll, "CloseServiceHandle");
		pCreateService = (PCreateService)GetProcAddress(hdll, "CreateServiceA");
		pOpenSCManager = (POpenSCManager)GetProcAddress(hdll, "OpenSCManagerA");
		pDeleteService = (PDeleteService)GetProcAddress(hdll, "DeleteService");
		pOpenService = (POpenService)GetProcAddress(hdll, "OpenServiceA");
		pQueryServiceStatus = (PQueryServiceStatus)GetProcAddress(hdll, "QueryServiceStatus");
		pStartServiceCtrlDispatcher = (PStartServiceCtrlDispatcher)GetProcAddress(hdll, "StartServiceCtrlDispatcherA");
		pStartService = (PStartService)GetProcAddress(hdll, "StartServiceA");
		pControlService = (PControlService)GetProcAddress(hdll, "ControlService");
		pDeregisterEventSource = (PDeregisterEventSource)GetProcAddress(hdll, "DeregisterEventSource");
		pReportEvent = (PReportEvent)GetProcAddress(hdll, "ReportEventA");
		pRegisterEventSource = (PRegisterEventSource)GetProcAddress(hdll, "RegisterEventSourceA");
		pQueryServiceConfig = (PQueryServiceConfig)GetProcAddress(hdll, "QueryServiceConfigA");
		pQueryServiceConfig2 = (PQueryServiceConfig2)GetProcAddress(hdll, "QueryServiceConfig2A");
		FreeLibrary(hdll);
	}
	if (!pOpenSCManager){
		LPTSTR s;
		::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			GetLastError(),
			0,
			(LPTSTR)&s,
			0,
			NULL);
		printf("%d\n%s\n", GetLastError(),s);
	}
	return pOpenSCManager != 0;
}

bool XHServiceController::isInstalled() const
{
	bool result = false;
	if (!winServiceInit())
		return result;
	SC_HANDLE hSCM = ::OpenSCManager(0, 0, 0);
	// Open the Service Control Manager
	//hSCM = pOpenSCManager(0, 0, 0);
	if (hSCM) {
		// Try to open the service
		SC_HANDLE hService = pOpenService(hSCM,d_ptr->serviceName.c_str(),
			SERVICE_QUERY_CONFIG);

		if (hService) {
			result = true;
			pCloseServiceHandle(hService);
		}
		pCloseServiceHandle(hSCM);
	}
	return result;
}

bool XHServiceController::isRunning() const
{
	bool result = false;
	if (!winServiceInit())
		return result;

	// Open the Service Control Manager
	SC_HANDLE hSCM = pOpenSCManager(0, 0, 0);
	if (hSCM) {
		// Try to open the service
		SC_HANDLE hService = pOpenService(hSCM,d_ptr->serviceName.c_str(),
			SERVICE_QUERY_STATUS);
		if (hService) {
			SERVICE_STATUS info;
			int res = pQueryServiceStatus(hService, &info);
			if (res)
				result = info.dwCurrentState != SERVICE_STOPPED;
			pCloseServiceHandle(hService);
		}
		pCloseServiceHandle(hSCM);
	}
	return result;
}
std::string XHServiceController::serviceFilePath() const
{
	std::string result;
	if (!winServiceInit())
		return result;

	// Open the Service Control Manager
	SC_HANDLE hSCM = pOpenSCManager(0, 0, 0);
	if (hSCM) {
		// Try to open the service
		SC_HANDLE hService = pOpenService(hSCM,d_ptr->serviceName.c_str(),
			SERVICE_QUERY_CONFIG);
		if (hService) {
			DWORD sizeNeeded = 0;
			char data[8 * 1024];
			if (pQueryServiceConfig(hService, (LPQUERY_SERVICE_CONFIG)data, 8 * 1024, &sizeNeeded)) {
				LPQUERY_SERVICE_CONFIG config = (LPQUERY_SERVICE_CONFIG)data;
				result = config->lpBinaryPathName;
			}
			pCloseServiceHandle(hService);
		}
		pCloseServiceHandle(hSCM);
	}
	return result;
}
std::string XHServiceController::serviceDescription() const
{
	std::string result;
	if (!winServiceInit())
		return result;

	// Open the Service Control Manager
	SC_HANDLE hSCM = pOpenSCManager(0, 0, 0);
	if (hSCM) {
		// Try to open the service
		SC_HANDLE hService = pOpenService(hSCM,d_ptr->serviceName.c_str(),
			SERVICE_QUERY_CONFIG);
		if (hService) {
			DWORD dwBytesNeeded;
			char data[8 * 1024];
			if (pQueryServiceConfig2(
				hService,
				SERVICE_CONFIG_DESCRIPTION,
				(unsigned char *)data,
				8096,
				&dwBytesNeeded)) {
				LPSERVICE_DESCRIPTION desc = (LPSERVICE_DESCRIPTION)data;
				if (desc->lpDescription){
					//result = (const ushort*)desc->lpDescription;
					std::cout << "error" << std::endl;
				}
			}
			pCloseServiceHandle(hService);
		}
		pCloseServiceHandle(hSCM);
	}
	return result;
}

XHServiceController::StartupType XHServiceController::startupType() const
{
	StartupType result = ManualStartup;
	if (!winServiceInit())
		return result;

	// Open the Service Control Manager
	SC_HANDLE hSCM = pOpenSCManager(0, 0, 0);
	if (hSCM) {
		// Try to open the service
		SC_HANDLE hService = pOpenService(hSCM,d_ptr->serviceName.c_str(),
			SERVICE_QUERY_CONFIG);
		if (hService) {
			DWORD sizeNeeded = 0;
			char data[8 * 1024] = {};
			if (pQueryServiceConfig(hService, (QUERY_SERVICE_CONFIG *)data, 8 * 1024, &sizeNeeded)) {
				QUERY_SERVICE_CONFIG *config = (QUERY_SERVICE_CONFIG *)data;
				result = config->dwStartType == SERVICE_DEMAND_START ? ManualStartup : AutoStartup;
			}
			pCloseServiceHandle(hService);
		}
		pCloseServiceHandle(hSCM);
	}
	return result;
}

bool XHServiceController::uninstall()
{
	bool result = false;
	if (!winServiceInit())
		return result;

	// Open the Service Control Manager
	SC_HANDLE hSCM = pOpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);
	if (hSCM) {
		// Try to open the service
		SC_HANDLE hService = pOpenService(hSCM,d_ptr->serviceName.c_str(), DELETE);
		if (hService) {
			if (pDeleteService(hService))
				result = true;
			pCloseServiceHandle(hService);
		}
		pCloseServiceHandle(hSCM);
	}
	return result;
}

bool XHServiceController::start(const std::vector<std::string> &args)
{
	bool result = false;
	if (!winServiceInit())
		return result;

	// Open the Service Control Manager
	SC_HANDLE hSCM = pOpenSCManager(0, 0, SC_MANAGER_CONNECT);
	if (hSCM) {
		// Try to open the service
		SC_HANDLE hService = pOpenService(hSCM,d_ptr->serviceName.c_str(), SERVICE_START);
		if (hService) {
			std::vector<const char*>argv(args.size());
			for (int i = 0; i < args.size(); ++i)
				argv[i] = (const char*)args[i].c_str();
			if (pStartService(hService, args.size(), argv.data()))
				result = true;
			pCloseServiceHandle(hService);
		}
		pCloseServiceHandle(hSCM);
	}
	return result;
}

bool XHServiceController::stop()
{
	bool result = false;
	if (!winServiceInit())
		return result;

	SC_HANDLE hSCM = pOpenSCManager(0, 0, SC_MANAGER_CONNECT);
	if (hSCM) {
		SC_HANDLE hService = pOpenService(hSCM,d_ptr->serviceName.c_str(), SERVICE_STOP | SERVICE_QUERY_STATUS);
		if (hService) {
			SERVICE_STATUS status;
			if (pControlService(hService, SERVICE_CONTROL_STOP, &status)) {
				bool stopped = status.dwCurrentState == SERVICE_STOPPED;
				int i = 0;
				while (!stopped && i < 10) {
					Sleep(200);
					if (!pQueryServiceStatus(hService, &status))
						break;
					stopped = status.dwCurrentState == SERVICE_STOPPED;
					++i;
				}
				result = stopped;
			}
			else {
				std::cout << GetLastError() << "stopping" << std::endl;
			}
			pCloseServiceHandle(hService);
		}
		pCloseServiceHandle(hSCM);
	}
	return result;
}

bool XHServiceController::pause()
{
	bool result = false;
	if (!winServiceInit())
		return result;

	SC_HANDLE hSCM = pOpenSCManager(0, 0, SC_MANAGER_CONNECT);
	if (hSCM) {
		SC_HANDLE hService = pOpenService(hSCM,d_ptr->serviceName.c_str(),
			SERVICE_PAUSE_CONTINUE);
		if (hService) {
			SERVICE_STATUS status;
			if (pControlService(hService, SERVICE_CONTROL_PAUSE, &status))
				result = true;
			pCloseServiceHandle(hService);
		}
		pCloseServiceHandle(hSCM);
	}
	return result;
}

bool XHServiceController::resume()
{
	bool result = false;
	if (!winServiceInit())
		return result;

	SC_HANDLE hSCM = pOpenSCManager(0, 0, SC_MANAGER_CONNECT);
	if (hSCM) {
		SC_HANDLE hService = pOpenService(hSCM,d_ptr->serviceName.c_str(),
			SERVICE_PAUSE_CONTINUE);
		if (hService) {
			SERVICE_STATUS status;
			if (pControlService(hService, SERVICE_CONTROL_CONTINUE, &status))
				result = true;
			pCloseServiceHandle(hService);
		}
		pCloseServiceHandle(hSCM);
	}
	return result;
}

bool XHServiceController::sendCommand(int code)
{
	bool result = false;
	if (!winServiceInit())
		return result;

	if (code < 0 || code > 127 || !isRunning())
		return result;

	SC_HANDLE hSCM = pOpenSCManager(0, 0, SC_MANAGER_CONNECT);
	if (hSCM) {
		SC_HANDLE hService = pOpenService(hSCM,d_ptr->serviceName.c_str(),
			SERVICE_USER_DEFINED_CONTROL);
		if (hService) {
			SERVICE_STATUS status;
			if (pControlService(hService, 128 + code, &status))
				result = true;
			pCloseServiceHandle(hService);
		}
		pCloseServiceHandle(hSCM);
	}
	return result;
}


void XHServiceBase::logMessage(const std::string &message, MessageType type,
	int id, uint16_t category, const std::string &data)
{
	if (!winServiceInit())
		return;
	WORD wType;
	switch (type) {
		case Error: wType = EVENTLOG_ERROR_TYPE; break;
		case Warning: wType = EVENTLOG_WARNING_TYPE; break;
		case Information: wType = EVENTLOG_INFORMATION_TYPE; break;
		default: wType = EVENTLOG_SUCCESS; break;
	}
	HANDLE h = pRegisterEventSource(0,d_ptr->controller.serviceName().c_str());
	if (h) {
		const char *msg = message.c_str();
		const char *bindata = data.size() > 0 ? data.c_str() : 0;
		pReportEvent(h, wType, category, id, 0, 1, data.size(), (const char **)&msg,
			const_cast<char *>(bindata));
		pDeregisterEventSource(h);
	}
}
class XHServiceControllerHandler
{
public:
	XHServiceControllerHandler(XHServiceSysPrivate *sys);

protected:

private:
	XHServiceSysPrivate *d_sys;
};

class XHServiceSysPrivate
{
public:
	enum {
		XHSERVICE_STARTUP = 256
	};
	XHServiceSysPrivate();
	~XHServiceSysPrivate();
	void setStatus(DWORD dwState);
	void setServiceFlags(int flags);
	DWORD serviceFlags(int flags) const;
	inline bool available() const;
	static void WINAPI serviceMain(DWORD dwArgc, char** lpszArgv);
	static void WINAPI handler(DWORD dwOpcode);

	SERVICE_STATUS status;
	SERVICE_STATUS_HANDLE serviceStatus;
	std::vector<std::string> serviceArgs;
	static XHServiceSysPrivate *instance;
	
	XHServiceControllerHandler *controllerHandler;
};

XHServiceControllerHandler::XHServiceControllerHandler(XHServiceSysPrivate *sys)
:d_sys(sys)
{

}

XHServiceSysPrivate *XHServiceSysPrivate::instance = 0;

XHServiceSysPrivate::XHServiceSysPrivate()
{
	instance = this;
}
XHServiceSysPrivate::~XHServiceSysPrivate()
{
	
}
inline bool XHServiceSysPrivate::available() const
{
	return 0 != pOpenSCManager;
}
DWORD dwThreadID = 0;
void WINAPI XHServiceSysPrivate::serviceMain(DWORD dwArgc, char** lpszArgv)
{
	dwThreadID = GetCurrentThreadId();

	if (!instance || !XHServiceBase::instance())
		return;

	// Windows spins off a random thread to call this function on
	// startup, so here we just signal to the QApplication event loop
	// in the main thread to go ahead with start()'ing the service.

	for (DWORD i = 0; i < dwArgc; i++)
		instance->serviceArgs.push_back(lpszArgv[i]);

	// Register the control request handler
	instance->serviceStatus = pRegisterServiceCtrlHandler(XHServiceBase::instance()->serviceName().c_str(), handler);

	if (!instance->serviceStatus) // cannot happen - something is utterly wrong
		return;

	handler(XHSERVICE_STARTUP); // Signal startup to the application -
	// causes XHServiceBase::start() to be called in the main thread

	// The MSDN doc says that this thread should just exit - the service is
	// running in the main thread (here, via callbacks in the handler thread).
}

void WINAPI XHServiceSysPrivate::handler(DWORD code)
{
	if (!instance)
		return;
	switch (code) {
		case XHSERVICE_STARTUP: // QtService startup (called from WinMain when started)			
			instance->setStatus(SERVICE_START_PENDING);		// 准备开始
			XHServiceBase::instance()->start();
			instance->setStatus(SERVICE_RUNNING);			// 启动成功 正在运行
			break;
		case SERVICE_CONTROL_STOP: // 1
			instance->setStatus(SERVICE_STOP_PENDING);		// 正在停止
			XHServiceBase::instance()->stop();
			::PostThreadMessage(dwThreadID, WM_QUIT, 0, 0);
			instance->setStatus(SERVICE_STOPPED);			// 已经停止
			break;
		case SERVICE_CONTROL_PAUSE: // 2
			instance->setStatus(SERVICE_PAUSE_PENDING);		//正在暂停
			XHServiceBase::instance()->pause();
			instance->setStatus(SERVICE_PAUSED);			// 暂停成功
			break;
		case SERVICE_CONTROL_CONTINUE: // 3
			instance->setStatus(SERVICE_CONTINUE_PENDING);	//正在恢复
			XHServiceBase::instance()->resume();
			instance->setStatus(SERVICE_RUNNING);			// 恢复成功
			break;
		case SERVICE_CONTROL_INTERROGATE: // 4
			break;
		case SERVICE_CONTROL_SHUTDOWN: // 5
			// Don't waste time with reporting stop pending, just do it
			XHServiceBase::instance()->stop();
			::PostThreadMessage(dwThreadID, WM_QUIT, 0, 0);
			break;
		default:
			if (code >= 128 && code <= 255) {
				XHServiceBase::instance()->processCommand(code - 128);
			}
			break;
	}

	// Report current status
	if (instance->available() && instance->status.dwCurrentState != SERVICE_STOPPED)
		pSetServiceStatus(instance->serviceStatus, &instance->status);
}

void XHServiceSysPrivate::setStatus(DWORD state)
{
	if (!available())
		return;
	status.dwCurrentState = state;
	pSetServiceStatus(serviceStatus, &status);
}

void XHServiceSysPrivate::setServiceFlags(int flags)
{
	if (!available())
		return;
	status.dwControlsAccepted = serviceFlags(flags);
	pSetServiceStatus(serviceStatus, &status);
}

DWORD XHServiceSysPrivate::serviceFlags(int flags) const
{
	DWORD control = 0;
	if (flags & XHServiceBase::CanBeSuspended)
		control |= SERVICE_ACCEPT_PAUSE_CONTINUE;
	if (!(flags & XHServiceBase::CannotBeStopped))
		control |= SERVICE_ACCEPT_STOP;
	if (flags & XHServiceBase::NeedsStopOnShutdown)
		control |= SERVICE_ACCEPT_SHUTDOWN;

	return control;
}

/* There are three ways we can be started:

   - By a service controller (e.g. the Services control panel), with
   no (service-specific) arguments. ServiceBase::exec() will then call
   start() below, and the service will start.

   - From the console, but with no (service-specific) arguments. This
   means we should ask a controller to start the service (i.e. another
   instance of this executable), and then just terminate. We discover
   this case (as different from the above) by the fact that
   StartServiceCtrlDispatcher will return an error, instead of blocking.

   - From the console, with -e(xec) argument. ServiceBase::exec() will
   then call ServiceBasePrivate::exec(), which calls
   ServiceBasePrivate::run(), which runs the application as a normal
   program.
   */

bool XHServiceBasePrivate::start()
{
	sysInit();
	if (!winServiceInit())
		return false;

	SERVICE_TABLE_ENTRY st[2];
	st[0].lpServiceName = (LPSTR)XHServiceBase::instance()->serviceName().c_str();
	st[0].lpServiceProc = XHServiceSysPrivate::serviceMain;
	st[1].lpServiceName = 0;
	st[1].lpServiceProc = 0;

	bool console = true;
	bool success = (::StartServiceCtrlDispatcher(st) != 0);//  (pStartServiceCtrlDispatcher(st) != 0);// should block

	if (!success) {
		DWORD dwRet = GetLastError();
		if (dwRet == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT) {
			LPTSTR s;
			::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,
				dwRet,
				0,
				(LPTSTR)&s,
				0,
				NULL);
			printf("%d\n%s\n", dwRet, s);
			// Means we're started from console, not from service mgr
			// start() will ask the mgr to start another instance of us as a service instead
			console = true;
		}
		else {
			LPTSTR s;
			::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,
				GetLastError(),
				0,
				(LPTSTR)&s,
				0,
				NULL);
			printf("%d\n%s\n", GetLastError(), s);
			XHServiceBase::instance()->logMessage(
				std::string("The Service failed to start:").append(s), XHServiceBase::Error);
		}
	}
	if (!success) {
		if (console){
			std::vector<std::string> _vector;
			for (int i = 1; i < args.size(); i++){
				_vector.push_back(args[i]);
			}
			return controller.start(_vector);
		}
		else
			return false;
	}
	XHServiceSysPrivate* sys = XHServiceSysPrivate::instance;
	
	sys->controllerHandler = new XHServiceControllerHandler(sys);
	sys->status.dwWin32ExitCode = q_ptr->executeApplication();
	sys->setStatus(SERVICE_STOPPED);

	delete sys->controllerHandler;
	sys->controllerHandler = 0;
	sysCleanup();
	return true;
}

bool XHServiceBasePrivate::install(const std::string &account, const std::string &password)
{
	bool result = false;
	if (!winServiceInit())
		return result;

	// Open the Service Control Manager
	SC_HANDLE hSCM = NULL;
	hSCM=pOpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);
	if (hSCM) {
		std::string acc = account;
		DWORD dwStartType = startupType == XHServiceController::AutoStartup ? SERVICE_AUTO_START : SERVICE_DEMAND_START;
		DWORD dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		char *act = 0;
		char *pwd = 0;
		if (acc.length()!=0) {
		    // The act string must contain a string of the format "Domain\UserName",
		    // so if only a username was specified without a domain, default to the local machine domain.
			std::string::size_type _pos = acc.find_first_of('\\');
			if (_pos == std::string::npos){// 如果没有
				acc = std::string(".\\") + acc;
			}
			if (!acc.find_last_of("\\LocalSystem"))//.endsWith(QLatin1String("\\LocalSystem")))
				act = (char*)acc.c_str();
		}
		if (password.length() != 0 && act){// !password.isEmpty() && act) {
			pwd = (char*)password.c_str();
		}
		// Only set INTERACTIVE if act is LocalSystem. (and act should be 0 if it is LocalSystem).
		if (!act)
			dwServiceType |= SERVICE_INTERACTIVE_PROCESS;

		// Create the service
		SC_HANDLE hService = pCreateService(hSCM,controller.serviceName().c_str(),
			controller.serviceName().c_str(),
			SERVICE_ALL_ACCESS,
			dwServiceType, // QObject::inherits ( const char * className ) for no inter active ????
			dwStartType, SERVICE_ERROR_NORMAL,filePath().c_str(),
			0, 0, 0,
			act, pwd);
		if (hService) {
			result = true;
			if (serviceDescription.length() != 0) {
				SERVICE_DESCRIPTION sdesc;
				sdesc.lpDescription = (LPSTR)serviceDescription.c_str();
				std::cout << sdesc.lpDescription << std::endl;
				pChangeServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, &sdesc);
			}
			pCloseServiceHandle(hService);
		}
		pCloseServiceHandle(hSCM);
	}
	return result;
}

std::string XHServiceBasePrivate::filePath() const
{
	char path[_MAX_PATH];
	::GetModuleFileName(0, path, sizeof(path));
	return  path;
}

bool XHServiceBasePrivate::sysInit()
{
	sysd = new XHServiceSysPrivate();

	sysd->serviceStatus = 0;
	sysd->status.dwServiceType = SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS;
	sysd->status.dwCurrentState = SERVICE_STOPPED;
	sysd->status.dwControlsAccepted = sysd->serviceFlags(serviceFlags);
	sysd->status.dwWin32ExitCode = NO_ERROR;
	sysd->status.dwServiceSpecificExitCode = 0;
	sysd->status.dwCheckPoint = 0;
	sysd->status.dwWaitHint = 0;

	return true;
}

void XHServiceBasePrivate::sysSetPath()
{

}

void XHServiceBasePrivate::sysCleanup()
{
	if (sysd) {
		delete sysd;
		sysd = 0;
	}
}

void XHServiceBase::setServiceFlags(int flags)
{
	if (d_ptr->serviceFlags == flags)
		return;
	d_ptr->serviceFlags = flags;
	if (d_ptr->sysd)
		d_ptr->sysd->setServiceFlags(flags);
}


