#include "processinfo.hh"
#include <unistd.h>

ProcessInfo::ProcessInfo()
{
}

int ProcessInfo::getNProcs() const
{
    return procsNameMap.size();
}

string ProcessInfo::getProcName(int i) const
{
    return procsList[i]->name;
}

string ProcessInfo::getProcState(int i) const
{
    return procsList[i]->state;
}

int ProcessInfo::getProcThreads(int i) const
{
    return procsList[i]->threads;
}

int ProcessInfo::getProcIndex(string procName) const
{
    for (uint i = 0; i < procsList.size(); ++i)
    {
        for (uint j = 0; j < procsList[i]->name.size() and j < procName.size(); ++j)
        {
            if (procsList[i]->name[j] != procName[j])
                break;
            if (j == procName.size() - 1)
                return i;
        }
    }
    return -1;
}

int ProcessInfo::getProcPid(int i) const
{
    return procsList[i]->pid;
}

double ProcessInfo::getReadKB(int i) const
{
    return procsList[i]->readKB - procsList[i]->lastReadKB;
}

double ProcessInfo::getWriteKB(int i) const
{
    return procsList[i]->writeKB - procsList[i]->lastWriteKB;
}

double ProcessInfo::getCpuUsage(int i) const
{
    return procsList[i]->cpuUsage;
}

double ProcessInfo::getMemUsage(int i) const
{
    return procsList[i]->memUsage;
}

void ProcessInfo::sortProcesses(sortBy criteria)
{
    switch (criteria)
    {
    case sortBy::CPU:
        sort(procsList.begin(), procsList.end(), [](process *a, process *b)
             { return a->cpuUsage > b->cpuUsage; });
        break;
    case sortBy::MEM:
        sort(procsList.begin(), procsList.end(), [](process *a, process *b)
             { return a->memUsage > b->memUsage; });
        break;
    case sortBy::NAME:
        sort(procsList.begin(), procsList.end(), [](process *a, process *b)
             { return a->name < b->name; });
        break;
    }
}

void ProcessInfo::update()
{
    map<string, process *> auxNameMap = procsNameMap;

    procsNameMap.clear();

    for (const auto &file : filesystem::directory_iterator("/proc"))
    {
        string fileDir{file.path().u8string()};
        string fileName = fileDir.substr(6); // length of "/proc/" is 6
        bool itsProcFolder = true;
        for (auto c : fileName)
        {
            if (!isdigit(c))
                itsProcFolder = false;
        }

        if (itsProcFolder)
        {
            ifstream procStatusFile;
            procStatusFile.open(fileDir + "/status");
            if (procStatusFile.is_open())
            {
                string procName, state, aux;
                int threads, pid;
                double memUsage;
                procStatusFile >> aux >> procName;
                while (procStatusFile >> aux and aux != "State:")
                    ;
                procStatusFile >> state;
                while (procStatusFile >> aux and aux != "Pid:")
                    ;
                procStatusFile >> pid;
                while (procStatusFile >> aux and aux != "RssAnon:")
                    ;
                procStatusFile >> memUsage;
                memUsage /= 1024;
                while (procStatusFile >> aux and aux != "Threads:")
                    ;
                procStatusFile >> threads;

                ifstream procIOFile;
                procIOFile.open(fileDir + "/io");

                double readKB = 0, writeKB = 0;
                if (procIOFile.is_open())
                {
                    string aux;
                    procIOFile >> aux;
                    procIOFile >> readKB;
                    readKB /= 1024;
                    procIOFile >> aux;
                    procIOFile >> writeKB;
                    writeKB /= 1024;
                }

                if (procsNameMap.find(procName) != procsNameMap.end())
                {
                    string aux = procName + "(1)";
                    int i = 2;
                    while (procsNameMap.find(aux) != procsNameMap.end())
                    {
                        aux = procName + "(" + to_string(i) + ")";
                        ++i;
                    }
                    procName = aux;
                }

                double lastReadKB = 0, lastWriteKB = 0;
                double lastUTime = 0, lastSTime = 0;
                double lastSysUptime = 0;
                auto it = auxNameMap.find(procName);
                if (it != auxNameMap.end())
                {
                    lastReadKB = it->second->readKB;
                    lastWriteKB = it->second->writeKB;
                    lastUTime = it->second->lastUTime;
                    lastSTime = it->second->lastSTime;
                    lastSysUptime = it->second->lastSysUptime;
                    delete it->second;
                }

                ifstream procStatFile(fileDir + "/stat");
                double cpuUsage = 0;
                if (procStatFile.is_open())
                {
                    double sysUptime;
                    ifstream uptimeFile("/proc/uptime");
                    uptimeFile >> sysUptime;
                    uptimeFile.close();
                    string aux;
                    for (int i = 0; i < 13; ++i)
                        procStatFile >> aux;
                    long uTime, sTime, startTime;
                    procStatFile >> uTime >> sTime;
                    for (int i = 0; i < 6; ++i)
                        procStatFile >> aux;
                    procStatFile >> startTime;
                    double clkTck = sysconf(_SC_CLK_TCK);
                    double uTimeSec = uTime / clkTck;
                    double sTimeSec = sTime / clkTck;
                    cpuUsage = abs(uTimeSec + sTimeSec - lastUTime - lastSTime) / (sysUptime - lastSysUptime) * 100;
                    lastUTime = uTimeSec;
                    lastSTime = sTimeSec;
                    lastSysUptime = sysUptime;
                    procStatFile.close();
                }

                process *proc = new process();
                *proc = {procName, state, threads, pid, writeKB, readKB, lastWriteKB, lastReadKB, cpuUsage, lastUTime, lastSTime, lastSysUptime, memUsage};
                procsNameMap[procName] = proc;
                procStatusFile.close();
            }
        }
    }

    uint nProcs = procsNameMap.size();
    procsList = vector<process *>(nProcs);
    uint processIndex = 0;
    for (const auto &pair : procsNameMap)
    {
        procsList[processIndex] = pair.second;
        ++processIndex;
    }
}
