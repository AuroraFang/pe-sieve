#pragma once

#include <Windows.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "pe_sieve_types.h"

typedef enum module_scan_status {
	SCAN_ERROR = -1,
	SCAN_NOT_MODIFIED = 0,
	SCAN_MODIFIED = 1
} t_scan_status;

class ModuleScanReport
{
public:
	ModuleScanReport(HANDLE processHandle, HMODULE _module, t_scan_status _status)
	{
		this->pid = GetProcessId(processHandle);
		this->module = _module;
		this->status = _status;
	}

	ModuleScanReport(HANDLE processHandle, HMODULE _module)
	{
		this->pid = GetProcessId(processHandle);
		this->module = _module;
		this->status = SCAN_NOT_MODIFIED;
	}

	virtual ~ModuleScanReport() {}

	const virtual bool toJSON(std::stringstream &outs)
	{
		outs << "\"pid\" : ";
		outs << std::hex << pid << ",\n";
		outs << "\"module\" : ";
		outs << std::hex << (ULONGLONG) module << ",\n";
		outs << "\"status\" : " ;
		outs << status;
		return true;
	}

	HMODULE module;
	DWORD pid;
	t_scan_status status;
};

class ProcessScanReport
{
public:
	ProcessScanReport(DWORD pid)
	{
		memset(&summary,0,sizeof(summary));
		summary.pid = pid;
	}
	~ProcessScanReport()
	{
		deleteModuleReports();
	}

	void appendReport(ModuleScanReport *report)
	{
		if (report == nullptr) return;
		module_reports.push_back(report);
	}

	t_report summary;
	std::vector<ModuleScanReport*> module_reports; //TODO: make it protected

protected:
	void deleteModuleReports()
	{
		std::vector<ModuleScanReport*>::iterator itr = module_reports.begin();
		for (; itr != module_reports.end(); itr++) {
			ModuleScanReport* module = *itr;
			delete module;
		}
		module_reports.clear();
	}
};
