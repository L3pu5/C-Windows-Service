#include <windows.h>

#define SVCNAME "SomeService"

void ServiceMain(DWORD argc, LPWSTR* argv);
void ServiceInit();

//Globals
// SERVICE_STATUS - https://learn.microsoft.com/en-us/windows/win32/api/winsvc/ns-winsvc-service_status
SERVICE_STATUS ServiceStatus;
// SERVICE_STATUS_HANDLE - 
SERVICE_STATUS_HANDLE hStatus;

//This functions responds to changes in the control state for cleanups etc. The ServiceScheduler calls this, I think?
void ServiceControlHandler(DWORD control){
	switch (control) {
		case SERVICE_CONTROL_PAUSE:
			ServiceStatus.dwCurrentState = SERVICE_PAUSED;
			break;
		case SERVICE_CONTROL_CONTINUE:
			ServiceStatus.dwCurrentState = SERVICE_RUNNING;
			break;
		case SERVICE_CONTROL_STOP:
			ServiceStatus.dwCurrentState = SERVICE_STOPPED;
            break;
		case SERVICE_CONTROL_SHUTDOWN:
			ServiceStatus.dwCurrentState = SERVICE_STOPPED;
			break;
	}
    // SetServiceStatus - https://learn.microsoft.com/en-us/windows/win32/api/winsvc/nf-winsvc-setservicestatus
	SetServiceStatus(hStatus, &ServiceStatus);
}


//This functions as the EntryPoint for the service?
void ServiceMain(DWORD argc, LPWSTR* argv){
    ServiceStatus.dwServiceType                 = SERVICE_WIN32_OWN_PROCESS;
    ServiceStatus.dwCurrentState                = SERVICE_RUNNING;
    ServiceStatus.dwControlsAccepted            = SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
    ServiceStatus.dwWin32ExitCode               = NO_ERROR;
    ServiceStatus.dwServiceSpecificExitCode     = 0;
    ServiceStatus.dwCheckPoint                  = 0;
    ServiceStatus.dwWaitHint                    = 0;

    // RegisterServiceCtrlHandler -  https://learn.microsoft.com/en-us/windows/win32/api/winsvc/nf-winsvc-registerservicectrlhandlerexa
    hStatus = RegisterServiceCtrlHandler(SVCNAME, (LPHANDLER_FUNCTION) ServiceControlHandler);
    if (hStatus == 0)
        return;

    // SetServiceStatus - https://learn.microsoft.com/en-us/windows/win32/api/winsvc/nf-winsvc-setservicestatus
    if(SetServiceStatus(hStatus, &ServiceStatus) == 0)
        return;

    //Perform the initialisation and enter the main loop;
    ServiceInit();
}

void ServiceInit(){

    //Main Loop of the Service
    while(ServiceStatus.dwCurrentState != SERVICE_STOPPED){
        //Do some stuff

        //Sleep (Remember WinAPI uses capital S).
        Sleep(1000);
    }
}

//The actual executable for a service should just create a table of SERVICE_TABLE_ENTRY and ask for the  windows service to dispatch them.
int main(){
    // SERVICE_TALBE_ENTRY - https://learn.microsoft.com/en-us/windows/win32/api/winsvc/ns-winsvc-service_table_entrya
	SERVICE_TABLE_ENTRY StartTable[] = {{SVCNAME, (LPSERVICE_MAIN_FUNCTIONA) ServiceMain}, {NULL, NULL}};
	// StartServiceCtrlDispatcher - https://learn.microsoft.com/en-us/windows/win32/api/winsvc/nf-winsvc-startservicectrldispatchera
    StartServiceCtrlDispatcher(StartTable);
	return 0;
}