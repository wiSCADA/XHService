/****************************************************************************
**
**
****************************************************************************/

#include "xhservice.h"
#include "xhservice_p.h"
#include <stdio.h>
#include <iostream>
/*!
    \class XHServiceController

    \brief The XHServiceController class allows you to control
    services from separate applications.

    XHServiceController provides a collection of functions that lets
    you install and run a service controlling its execution, as well
    as query its status.

    In order to run a service, the service must be installed in the
    system's service database using the install() function. The system
    will start the service depending on the specified StartupType; it
    can either be started during system startup, or when a process
    starts it manually.

    Once a service is installed, the service can be run and controlled
    manually using the start(), stop(), pause(), resume() or
    sendCommand() functions.  You can at any time query for the
    service's status using the isInstalled() and isRunning()
    functions, or you can query its properties using the
    serviceDescription(), serviceFilePath(), serviceName() and
    startupType() functions. For example:

    \code
    MyService service;       \\ which inherits XHService
    QString serviceFilePath;

    XHServiceController controller(service.serviceName());

    if (controller.install(serviceFilePath))
        controller.start()

    if (controller.isRunning())
        QMessageBox::information(this, tr("Service Status"),
                                 tr("The %1 service is started").arg(controller.serviceName()));

    ...

    controller.stop();
    controller.uninstall();
    }
    \endcode

    An instance of the service controller can only control one single
    service. To control several services within one application, you
    must create en equal number of service controllers.

    The XHServiceController destructor neither stops nor uninstalls
    the associated service. To stop a service the stop() function must
    be called explicitly. To uninstall a service, you can use the
    uninstall() function.

    \sa XHServiceBase, XHService
*/

/*!
    \enum XHServiceController::StartupType
    This enum describes when a service should be started.

    \value AutoStartup The service is started during system startup.
    \value ManualStartup The service must be started manually by a process.

    \warning The \a StartupType enum is ignored under UNIX-like
    systems. A service, or daemon, can only be started manually on such
    systems with current implementation.

    \sa startupType()
*/


/*!
    Creates a controller object for the service with the given
    \a name.
*/
XHServiceController::XHServiceController(const std::string &name)
 : d_ptr(new XHServiceControllerPrivate())
{
	d_ptr->q_ptr = this;
	d_ptr->serviceName = name;
}
/*!
    Destroys the service controller. This neither stops nor uninstalls
    the controlled service.

    To stop a service the stop() function must be called
    explicitly. To uninstall a service, you can use the uninstall()
    function.

    \sa stop(), XHServiceController::uninstall()
*/
XHServiceController::~XHServiceController()
{
    delete d_ptr;
}
/*!
    \fn bool XHServiceController::isInstalled() const

    Returns true if the service is installed; otherwise returns false.

    On Windows it uses the system's service control manager.

    On Unix it checks configuration written to QSettings::SystemScope
    using "QtSoftware" as organization name.

    \sa install()
*/

/*!
    \fn bool XHServiceController::isRunning() const

    Returns true if the service is running; otherwise returns false. A
    service must be installed before it can be run using a controller.

    \sa start(), isInstalled()
*/

/*!
    Returns the name of the controlled service.

    \sa XHServiceController(), serviceDescription()
*/
std::string XHServiceController::serviceName() const
{
	return d_ptr->serviceName;
}
/*!
    \fn QString XHServiceController::serviceDescription() const

    Returns the description of the controlled service.

    \sa install(), serviceName()
*/

/*!
    \fn XHServiceController::StartupType XHServiceController::startupType() const

    Returns the startup type of the controlled service.

    \sa install(), serviceName()
*/

/*!
    \fn QString XHServiceController::serviceFilePath() const

    Returns the file path to the controlled service.

    \sa install(), serviceName()
*/

/*!
    Installs the service with the given \a serviceFilePath
    and returns true if the service is installed
    successfully; otherwise returns false.

    On Windows service is installed in the system's service control manager with the given
    \a account and \a password.

    On Unix service configuration is written to QSettings::SystemScope
    using "QtSoftware" as organization name. \a account and \a password
    arguments are ignored.

    \warning Due to the different implementations of how services (daemons)
    are installed on various UNIX-like systems, this method doesn't
    integrate the service into the system's startup scripts.

    \sa uninstall(), start()
*/
bool XHServiceController::install(const std::string &serviceFilePath,
	const std::string &account,
	const std::string &password)
{
	std::vector<std::string>  arguments;
	arguments.push_back(std::string("-i"));
	arguments.push_back(account);
	arguments.push_back(password);
	return true;
    //return (QProcess::execute(serviceFilePath, arguments) == 0);
}


/*!
    \fn bool XHServiceController::uninstall()

    Uninstalls the service and returns true if successful; otherwise returns false.

    On Windows service is uninstalled using the system's service control manager.

    On Unix service configuration is cleared using QSettings::SystemScope
    with "QtSoftware" as organization name.


    \sa install()
*/

/*!
    \fn bool XHServiceController::start(const QStringList &arguments)

    Starts the installed service passing the given \a arguments to the
    service. A service must be installed before a controller can run it.

    Returns true if the service could be started; otherwise returns
    false.

    \sa install(), stop()
*/

/*!
    \overload

    Starts the installed service without passing any arguments to the service.
*/
bool XHServiceController::start()
{
    return start(std::vector<std::string>());
}

/*!
    \fn bool XHServiceController::stop()

    Requests the running service to stop. The service will call the
    XHServiceBase::stop() implementation unless the service's state
    is XHServiceBase::CannotBeStopped.  This function does nothing if
    the service is not running.

    Returns true if a running service was successfully stopped;
    otherwise false.

    \sa start(), XHServiceBase::stop(), XHServiceBase::ServiceFlags
*/

/*!
    \fn bool XHServiceController::pause()

    Requests the running service to pause. If the service's state is
    XHServiceBase::CanBeSuspended, the service will call the
    XHServiceBase::pause() implementation. The function does nothing
    if the service is not running.

    Returns true if a running service was successfully paused;
    otherwise returns false.

    \sa resume(), XHServiceBase::pause(), XHServiceBase::ServiceFlags
*/

/*!
    \fn bool XHServiceController::resume()

    Requests the running service to continue. If the service's state
    is XHServiceBase::CanBeSuspended, the service will call the
    XHServiceBase::resume() implementation. This function does nothing
    if the service is not running.

    Returns true if a running service was successfully resumed;
    otherwise returns false.

    \sa pause(), XHServiceBase::resume(), XHServiceBase::ServiceFlags
*/

/*!
    \fn bool XHServiceController::sendCommand(int code)

    Sends the user command \a code to the service. The service will
    call the XHServiceBase::processCommand() implementation.  This
    function does nothing if the service is not running.

    Returns true if the request was sent to a running service;
    otherwise returns false.

    \sa XHServiceBase::processCommand()
*/

class XHServiceStarter 
{
public:
    XHServiceStarter(XHServiceBasePrivate *service)
        :d_ptr(service) {}
public:
    void slotStart()
    {
        d_ptr->startService();
    }
private:
    XHServiceBasePrivate *d_ptr;
};
XHServiceBase * XHServiceBasePrivate::instance = 0;
XHServiceBasePrivate::XHServiceBasePrivate(const std::string &name)
    : startupType(XHServiceController::ManualStartup), serviceFlags(0), controller(name)
{

}

XHServiceBasePrivate::~XHServiceBasePrivate()
{

}

void XHServiceBasePrivate::startService()
{
    q_ptr->start();
}

int XHServiceBasePrivate::run(bool asService, const std::vector<std::string> &argList)
{
	int argc = argList.size();
    std::vector<char *> argv(argc);	
	std::vector<std::string>  argvData;
	for (int i = 0; i < argc; ++i)
		argvData.push_back(argList[i]);
	for (int i = 0; i < argc; ++i){
		char* c;
		const int len = argvData[i].length();
		c = new char[len + 1];
		strcpy(c, argvData[i].c_str());
		argv[i] = c;
	}

    if (asService && !sysInit())
        return -1;

	q_ptr->createApplication(argc,argv.data());   

    if (asService)
        sysSetPath();

    XHServiceStarter starter(this);
	starter.slotStart();
	// TODO 
    int res = q_ptr->executeApplication();
    if (asService)
        sysCleanup();
    return res;
}


/*!
    \class XHServiceBase

    \brief The XHServiceBase class provides an API for implementing
    Windows services and Unix daemons.

    A Windows service or Unix daemon (a "service"), is a program that
    runs "in the background" independently of whether a user is logged
    in or not. A service is often set up to start when the machine
    boots up, and will typically run continuously as long as the
    machine is on.

    Services are usually non-interactive console applications. User
    interaction, if required, is usually implemented in a separate,
    normal GUI application that communicates with the service through
    an IPC channel. For simple communication,
    XHServiceController::sendCommand() and XHService::processCommand()
    may be used, possibly in combination with a shared settings
    file. For more complex, interactive communication, a custom IPC
    channel should be used, e.g. based on Qt's networking classes. (In
    certain circumstances, a service may provide a GUI itself,
    ref. the "interactive" example documentation).

    Typically, you will create a service by subclassing the XHService
    template class which inherits XHServiceBase and allows you to
    create a service for a particular application type.

    The Windows implementation uses the NT Service Control Manager,
    and the application can be controlled through the system
    administration tools. Services are usually launched using the
    system account, which requires that all DLLs that the service
    executable depends on (i.e. Qt), are located in the same directory
    as the service, or in a system path.

    On Unix a service is implemented as a daemon.

    You can retrieve the service's description, state, and startup
    type using the serviceDescription(), serviceFlags() and
    startupType() functions respectively. The service's state is
    decribed by the ServiceFlag enum. The mentioned properites can
    also be set using the corresponding set functions. In addition you
    can retrieve the service's name using the serviceName() function.

    Several of XHServiceBase's protected functions are called on
    requests from the XHServiceController class:

    \list
        \o start()
        \o pause()
        \o processCommand()
        \o resume()
        \o stop()
    \endlist

    You can control any given service using an instance of the
    XHServiceController class which also allows you to control
    services from separate applications. The mentioned functions are
    all virtual and won't do anything unless they are
    reimplemented. You can reimplement these functions to pause and
    resume the service's execution, as well as process user commands
    and perform additional clean-ups before shutting down.

    XHServiceBase also provides the static instance() function which
    returns a pointer to an application's XHServiceBase instance. In
    addition, a service can report events to the system's event log
    using the logMessage() function. The MessageType enum describes
    the different types of messages a service reports.

    The implementation of a service application's main function
    typically creates an service object derived by subclassing the
    XHService template class. Then the main function will call this
    service's exec() function, and return the result of that call. For
    example:

    \code
        int main(int argc, char **argv)
        {
            MyService service(argc, argv);
            return service.exec();
        }
    \endcode

    When the exec() function is called, it will parse the service
    specific arguments passed in \c argv, perform the required
    actions, and return.

    \target serviceSpecificArguments

    The following arguments are recognized as service specific:

    \table
    \header \i Short \i Long \i Explanation
    \row \i -i \i -install \i Install the service.
    \row \i -u \i -uninstall \i Uninstall the service.
    \row \i -e \i -exec
         \i Execute the service as a standalone application (useful for debug purposes).
            This is a blocking call, the service will be executed like a normal application.
            In this mode you will not be able to communicate with the service from the contoller.
    \row \i -t \i -terminate \i Stop the service.
    \row \i -p \i -pause \i Pause the service.
    \row \i -r \i -resume \i Resume a paused service.
    \row \i -c \e{cmd} \i -command \e{cmd}
	 \i Send the user defined command code \e{cmd} to the service application.
    \row \i -v \i -version \i Display version and status information.
    \endtable

    If \e none of the arguments is recognized as service specific,
    exec() will first call the createApplication() function, then
    executeApplication() and finally the start() function. In the end,
    exec() returns while the service continues in its own process
    waiting for commands from the service controller.

    \sa XHService, XHServiceController
*/

/*!
    \enum XHServiceBase::MessageType

    This enum describes the different types of messages a service
    reports to the system log.

    \value Success An operation has succeeded, e.g. the service
           is started.
    \value Error An operation failed, e.g. the service failed to start.
    \value Warning An operation caused a warning that might require user
           interaction.
    \value Information Any type of usually non-critical information.
*/

/*!
    \enum XHServiceBase::ServiceFlag

    This enum describes the different capabilities of a service.

    \value Default The service can be stopped, but not suspended.
    \value CanBeSuspended The service can be suspended.
    \value CannotBeStopped The service cannot be stopped.
    \value NeedsStopOnShutdown (Windows only) The service will be stopped before the system shuts down. Note that Microsoft recommends this only for services that must absolutely clean up during shutdown, because there is a limited time available for shutdown of services.
*/

/*!
    Creates a service instance called \a name. The \a argc and \a argv
    parameters are parsed after the exec() function has been
    called. Then they are passed to the application's constructor.
    The application type is determined by the XHService subclass.

    The service is neither installed nor started. The name must not
    contain any backslashes or be longer than 255 characters. In
    addition, the name must be unique in the system's service
    database.

    \sa exec(), start(), XHServiceController::install()
*/
void string_replace(std::string& strBig, const std::string & strsrc, const std::string &strdst)

{
	std::string::size_type pos = 0;
	while ((pos = strBig.find(strsrc, pos)) != std::string::npos)
	{
		strBig.replace(pos, strsrc.length(), strdst);
		pos += strdst.length();
	}

}
void string_replace(std::wstring& strBig, const std::wstring & strsrc, const std::wstring &strdst)

{
	std::wstring::size_type pos = 0;
	while ((pos = strBig.find(strsrc, pos)) != std::wstring::npos)
	{
		strBig.replace(pos, strsrc.length(), strdst);
		pos += strdst.length();
	}

}
XHServiceBase::XHServiceBase(int argc, char **argv, const std::string &name)
{
    XHServiceBasePrivate::instance = this;
	std::string nm(name);
    if (nm.length() > 255) {
		std::cout << "XHService: 'name' is longer than 255 characters." << std::endl;;
		nm.substr(0, 255);
    }
	std::string::size_type _pos = nm.find_first_of('\\');
	if (_pos != std::string::npos) {
		std::cout << "XHService: 'name' contains backslashes '\\'.";
		string_replace(nm, "'\\'", "'\0'");
	}
    d_ptr = new XHServiceBasePrivate(nm);
    d_ptr->q_ptr = this;

    d_ptr->serviceFlags = 0;
    d_ptr->sysd = 0;
    for (int i = 0; i < argc; ++i)
        d_ptr->args.push_back(argv[i]);
}

/*!
    Destroys the service object. This neither stops nor uninstalls the
    service.

    To stop a service the stop() function must be called
    explicitly. To uninstall a service, you can use the
    XHServiceController::uninstall() function.

    \sa stop(), XHServiceController::uninstall()
*/
XHServiceBase::~XHServiceBase()
{
    delete d_ptr;
    XHServiceBasePrivate::instance = 0;
}

/*!
    Returns the name of the service.

    \sa XHServiceBase(), serviceDescription()
*/
std::string XHServiceBase::serviceName() const
{
	return d_ptr->controller.serviceName();
}
/*!
    Returns the description of the service.

    \sa setServiceDescription(), serviceName()
*/
std::string XHServiceBase::serviceDescription() const
{
    return d_ptr->serviceDescription;
}

/*!
    Sets the description of the service to the given \a description.

    \sa serviceDescription()
*/
void XHServiceBase::setServiceDescription(const std::string &description)
{
    d_ptr->serviceDescription = description;
}

/*!
    Returns the service's startup type.

    \sa XHServiceController::StartupType, setStartupType()
*/
XHServiceController::StartupType XHServiceBase::startupType() const
{
    return d_ptr->startupType;
}

/*!
    Sets the service's startup type to the given \a type.

    \sa XHServiceController::StartupType, startupType()
*/
void XHServiceBase::setStartupType(XHServiceController::StartupType type)
{
    d_ptr->startupType = type;
}

/*!
    Returns the service's state which is decribed using the
    ServiceFlag enum.

    \sa ServiceFlags, setServiceFlags()
*/
int XHServiceBase::serviceFlags() const
{
    return d_ptr->serviceFlags;
}

/*!
    \fn void XHServiceBase::setServiceFlags(ServiceFlags flags)

    Sets the service's state to the state described by the given \a
    flags.

    \sa ServiceFlags, serviceFlags()
*/

/*!
    Executes the service.

    When the exec() function is called, it will parse the \l
    {serviceSpecificArguments} {service specific arguments} passed in
    \c argv, perform the required actions, and exit.

    If none of the arguments is recognized as service specific, exec()
    will first call the createApplication() function, then executeApplication() and
    finally the start() function. In the end, exec()
    returns while the service continues in its own process waiting for
    commands from the service controller.

    \sa XHServiceController
*/

int XHServiceBase::exec()
{
    if (d_ptr->args.size() > 1) {
        std::string a =  d_ptr->args.at(1);
        if (a == std::string("-i") || a == std::string("-install")) {
            if (!d_ptr->controller.isInstalled()) {
                std::string account;
                std::string password;
                if (d_ptr->args.size() > 2)
                    account = d_ptr->args.at(2);
                if (d_ptr->args.size() > 3)
                    password = d_ptr->args.at(3);
                if (!d_ptr->install(account, password)) {
                    fprintf(stderr, "The service [%s] could not be installed\n%s\n", 
						serviceName().c_str());
                    return -1;
                } else {
                    printf("The service [%s] has been installed under: %s\n",
                        serviceName().c_str(), d_ptr->filePath().c_str());
                }
            } else {
				fprintf(stderr, "The service [%s] is already installed\n", serviceName().c_str());
            }
            return 0;
        } else if (a == std::string("-u") || a == std::string("-uninstall")) {
            if (d_ptr->controller.isInstalled()) {
                if (!d_ptr->controller.uninstall()) {
                    fprintf(stderr, "The service [%s] could not be uninstalled\n", serviceName().c_str());
                    return -1;
                } else {
                    printf("The service [%s] has been uninstalled.\n",serviceName().c_str());
                }
            } else {
                fprintf(stderr, "The service [%s] is not installed\n", serviceName().c_str());
            }
            return 0;
        } else if (a == std::string("-v") || a == std::string("-version")) {
            printf("The service\n"
                "\t[%s]\n\t%s\n\n", serviceName().c_str(), d_ptr->args[0].c_str());
            printf("is %s", (d_ptr->controller.isInstalled() ? "installed" : "not installed"));
            printf(" and %s\n\n", (d_ptr->controller.isRunning() ? "running" : "not running"));
            return 0;
		}
		else if (a == std::string("-e") || a == std::string("-exec")) {
			std::vector<std::string>::iterator it = d_ptr->args.begin() + 1;
			d_ptr->args.erase(it);
			int ec = d_ptr->run(false, d_ptr->args);
			if (ec == -1)
				fprintf(stderr, "The service could not be executed.");
			return ec;
        } else if (a == std::string("-t") || a == std::string("-terminate")) {
            if (!d_ptr->controller.stop())
				fprintf(stderr, "The service could not be stopped.");
            return 0;
        } else if (a == std::string("-p") || a == std::string("-pause")) {
            d_ptr->controller.pause();
            return 0;
        } else if (a == std::string("-r") || a == std::string("-resume")) {
            d_ptr->controller.resume();
            return 0;
        } else if (a == std::string("-c") || a == std::string("-command")) {
            int code = 0;
			if (d_ptr->args.size() > 2){
				std::string _v = d_ptr->args[2];
				code = atoi(_v.c_str());
			}
            d_ptr->controller.sendCommand(code);
            return 0;
        } else if(a == std::string("-h") || a == std::string("-help")) {
			printHelp();
            return 0;
		}
	}
#if defined(Q_OS_UNIX)
	if (::getenv("XHSERVICE_RUN")) {
		// Means we're the detached, real service process.
		int ec = d_ptr->run(true, d_ptr->args);
		if (ec == -1)
			fprintf(stderr, "The service [%s] could not start\n", serviceName().c_str());
		return ec;
	}
#endif
	if (!d_ptr->start()) {
		fprintf(stderr, "The service [%s] could not start\n", serviceName().c_str());
		return -4;
	}
	return 0;
}
void XHServiceBase::printHelp()
{
	printf("\n%s -[i|u|e|s|t|c|v|h]\n"
		"\t-i(nstall) [account] [password]\t: Install the service, optionally using given account and password\n"
		"\t-u(ninstall)\t: Uninstall the service.\n"
		"\t-e(xec)\t\t: Run as a regular application. Useful for debugging.\n"
		"\t-s(tart)\t: Start the service.\n"
		"\t-t(erminate)\t: Stop the service.\n"
		"\t-c(ommand) num\t: Send command code num to the service.\n"
		"\t-v(ersion)\t: Print version and status information.\n"
		"\t-h(elp)   \t: Show this help\n",
		"\tNo arguments\t: Start the service.\n",
		d_ptr->args[0].c_str());
}
/*!
    \fn void XHServiceBase::logMessage(const QString &message, MessageType type,
            int id, uint category, const QByteArray &data)

    Reports a message of the given \a type with the given \a message
    to the local system event log.  The message identifier \a id and
    the message \a category are user defined values. The \a data
    parameter can contain arbitrary binary data.

    Message strings for \a id and \a category must be provided by a
    message file, which must be registered in the system registry.
    Refer to the MSDN for more information about how to do this on
    Windows.

    \sa MessageType
*/

/*!
    Returns a pointer to the current application's XHServiceBase
    instance.
*/
XHServiceBase *XHServiceBase::instance()
{
    return XHServiceBasePrivate::instance;
}

/*!
    \fn void XHServiceBase::start()

    This function must be implemented in XHServiceBase subclasses in
    order to perform the service's work. Usually you create some main
    object on the heap which is the heart of your service.

    The function is only called when no service specific arguments
    were passed to the service constructor, and is called by exec()
    after it has called the executeApplication() function.

    Note that you \e don't need to create an application object or
    call its exec() function explicitly.

    \sa exec(), stop(), XHServiceController::start()
*/

/*!
    Reimplement this function to perform additional cleanups before
    shutting down (for example deleting a main object if it was
    created in the start() function).

    This function is called in reply to controller requests. The
    default implementation does nothing.

    \sa start(), XHServiceController::stop()
*/
void XHServiceBase::stop()
{
}

/*!
    Reimplement this function to pause the service's execution (for
    example to stop a polling timer, or to ignore socket notifiers).

    This function is called in reply to controller requests.  The
    default implementation does nothing.

    \sa resume(), XHServiceController::pause()
*/
void XHServiceBase::pause()
{
}

/*!
    Reimplement this function to continue the service after a call to
    pause().

    This function is called in reply to controller requests. The
    default implementation does nothing.

    \sa pause(), XHServiceController::resume()
*/
void XHServiceBase::resume()
{
}

/*!
    Reimplement this function to process the user command \a code.


    This function is called in reply to controller requests.  The
    default implementation does nothing.

    \sa XHServiceController::sendCommand()
*/
void XHServiceBase::processCommand(int /*code*/)
{
}

/*!
    \fn void XHServiceBase::createApplication(int &argc, char **argv)

    Creates the application object using the \a argc and \a argv
    parameters.

    This function is only called when no \l
    {serviceSpecificArguments}{service specific arguments} were
    passed to the service constructor, and is called by exec() before
    it calls the executeApplication() and start() functions.

    The createApplication() function is implemented in XHService, but
    you might want to reimplement it, for example, if the chosen
    application type's constructor needs additional arguments.

    \sa exec(), XHService
*/

/*!
    \fn int XHServiceBase::executeApplication()

    Executes the application previously created with the
    createApplication() function.

    This function is only called when no \l
    {serviceSpecificArguments}{service specific arguments} were
    passed to the service constructor, and is called by exec() after
    it has called the createApplication() function and before start() function.

    This function is implemented in XHService.

    \sa exec(), createApplication()
*/

/*!
    \class XHService

    \brief The XHService is a convenient template class that allows
    you to create a service for a particular application type.

    A Windows service or Unix daemon (a "service"), is a program that
    runs "in the background" independently of whether a user is logged
    in or not. A service is often set up to start when the machine
    boots up, and will typically run continuously as long as the
    machine is on.

    Services are usually non-interactive console applications. User
    interaction, if required, is usually implemented in a separate,
    normal GUI application that communicates with the service through
    an IPC channel. For simple communication,
    XHServiceController::sendCommand() and XHService::processCommand()
    may be used, possibly in combination with a shared settings file. For
    more complex, interactive communication, a custom IPC channel
    should be used, e.g. based on Qt's networking classes. (In certain
    circumstances, a service may provide a GUI itself, ref. the
    "interactive" example documentation).

    \bold{Note:} On Unix systems, this class relies on facilities
    provided by the QtNetwork module, provided as part of the
    \l{Qt Open Source Edition} and certain \l{Qt Commercial Editions}.

    The XHService class functionality is inherited from XHServiceBase,
    but in addition the XHService class binds an instance of
    XHServiceBase with an application type.

    Typically, you will create a service by subclassing the XHService
    template class. For example:

    \code
    class MyService : public XHService<QApplication>
    {
    public:
        MyService(int argc, char **argv);
        ~MyService();

    protected:
        void start();
        void stop();
        void pause();
        void resume();
        void processCommand(int code);
    };
    \endcode

    The application type can be QCoreApplication for services without
    GUI, QApplication for services with GUI or you can use your own
    custom application type.

    You must reimplement the XHServiceBase::start() function to
    perform the service's work. Usually you create some main object on
    the heap which is the heart of your service.

    In addition, you might want to reimplement the
    XHServiceBase::pause(), XHServiceBase::processCommand(),
    XHServiceBase::resume() and XHServiceBase::stop() to intervene the
    service's process on controller requests. You can control any
    given service using an instance of the XHServiceController class
    which also allows you to control services from separate
    applications. The mentioned functions are all virtual and won't do
    anything unless they are reimplemented.

    Your custom service is typically instantiated in the application's
    main function. Then the main function will call your service's
    exec() function, and return the result of that call. For example:

    \code
        int main(int argc, char **argv)
        {
            MyService service(argc, argv);
            return service.exec();
        }
    \endcode

    When the exec() function is called, it will parse the \l
    {serviceSpecificArguments} {service specific arguments} passed in
    \c argv, perform the required actions, and exit.

    If none of the arguments is recognized as service specific, exec()
    will first call the createApplication() function, then executeApplication() and
    finally the start() function. In the end, exec()
    returns while the service continues in its own process waiting for
    commands from the service controller.

    \sa XHServiceBase, XHServiceController
*/

/*!
    \fn XHService::XHService(int argc, char **argv, const QString &name)

    Constructs a XHService object called \a name. The \a argc and \a
    argv parameters are parsed after the exec() function has been
    called. Then they are passed to the application's constructor.

    There can only be one XHService object in a process.

    \sa XHServiceBase()
*/

/*!
    \fn XHService::~XHService()

    Destroys the service object.
*/

/*!
    \fn Application *XHService::application() const

    Returns a pointer to the application object.
*/

/*!
    \fn void XHService::createApplication(int &argc, char **argv)

    Creates application object of type Application passing \a argc and
    \a argv to its constructor.

    \reimp

*/

/*!
    \fn int XHService::executeApplication()

    \reimp
*/



