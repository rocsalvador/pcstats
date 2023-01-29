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
    auto it = procsNameMap.begin();
    advance(it, i);
    return it->first;
}

string ProcessInfo::getProcState(int i) const
{
    auto it = procsNameMap.begin();
    advance(it, i);
    return it->second->state;
}

int ProcessInfo::getProcThreads(int i) const
{
    auto it = procsNameMap.begin();
    advance(it, i);
    return it->second->threads;
}

int ProcessInfo::getProcIndex(string procName) const
{
    auto it = procsNameMap.find(procName);
    if(it != procsNameMap.end()) return distance(procsNameMap.begin(), it);
    else {
        auto it = procsNameMap.begin();
        for (int i = 0; i < procsNameMap.size(); ++i, ++it) {
            for (int j = 0; j < it->first.size() and j < procName.size(); ++j) {
                if (it->first[j] != procName[j]) break;
                if (j == procName.size() - 1) return i;
            }
        }
        return -1;
    }
}

int ProcessInfo::getProcPid(int i) const
{
    auto it = procsNameMap.begin();
    advance(it, i);
    return it->second->pid;
}

double ProcessInfo::getReadKB(int i) const
{
    auto it = procsNameMap.begin();
    advance(it, i);
    return it->second->readKB - it->second->lastReadKB;
}

double ProcessInfo::getWriteKB(int i) const
{
    auto it = procsNameMap.begin();
    advance(it, i);
    return it->second->writeKB - it->second->lastWriteKB;
}

double ProcessInfo::getCpuUsage(int i) const 
{
    auto it = procsNameMap.begin();
    advance(it, i);
    return it->second->cpuUsage;
}

double ProcessInfo::getMemUsage(int i) const 
{
    auto it = procsNameMap.begin();
    advance(it, i);
    return it->second->memUsage;
}

ProcessInfo::process ProcessInfo::getProcByName(string name) const
{
    return *procsNameMap.find(name)->second;
}

ProcessInfo::process ProcessInfo::getProcByPid(int pid) const
{
    return *procsPidMap.find(pid)->second;
}

void ProcessInfo::update()
{
    map<string, process*> auxNameMap = procsNameMap;

    procsNameMap.clear();
    procsPidMap.clear();

    for(const auto& file : filesystem::directory_iterator("/proc")) {
        string fileDir{file.path().u8string()};
        string fileName = fileDir;
        fileName = fileName.erase(0, 6);
        bool itsProcFolder = true;
        for(auto c : fileName) {
            if(!isdigit(c)) itsProcFolder = false;
        }
        if(itsProcFolder) {
            ifstream procStatusFile;
            procStatusFile.open(fileDir + "/status");
            if(procStatusFile.is_open()) {
                string procName, state, aux;
                int threads, pid;
                double memUsage;
                procStatusFile >> aux >> procName;
                while(procStatusFile >> aux and aux != "State:");
                procStatusFile >> state;
                while(procStatusFile >> aux and aux != "Pid:");
                procStatusFile >> pid;
                while(procStatusFile >> aux and aux != "RssAnon:");
                procStatusFile >> memUsage;
                memUsage /= 1024;
                while(procStatusFile >> aux and aux != "Threads:");
                procStatusFile >> threads;

                ifstream procIOFile;
                procIOFile.open(fileDir + "/io");
                
                double readKB = 0, writeKB = 0;
                if(procIOFile.is_open()) {
                    string aux;
                    procIOFile >> aux;
                    procIOFile >> readKB;
                    readKB /= 1024;
                    procIOFile >> aux;
                    procIOFile >> writeKB;
                    writeKB /= 1024;
                }
                
                if(procsNameMap.find(procName) != procsNameMap.end()) {
                    string aux = procName + "(1)";
                    int i = 2;
                    while(procsNameMap.find(aux) != procsNameMap.end()) {
                        aux = procName + "(" + to_string(i) + ")";
                        ++i;
                    }
                    procName = aux;
                }

                double lastReadKB = 0, lastWriteKB = 0;
                double lastUTime = 0, lastSTime = 0;
                double lastSysUptime = 0;
                auto it = auxNameMap.find(procName);
                if(it != auxNameMap.end()) {
                    lastReadKB = it->second->readKB;
                    lastWriteKB = it->second->writeKB;
                    lastUTime = it->second->lastUTime;
                    lastSTime = it->second->lastSTime;
                    lastSysUptime = it->second->lastSysUptime;
                    delete it->second;
                }

                ifstream procStatFile(fileDir + "/stat");
                double cpuUsage = 0;
                if (procStatFile.is_open()) {
                    double sysUptime;
                    ifstream uptimeFile("/proc/uptime");
                    uptimeFile >> sysUptime;
                    uptimeFile.close();
                    string aux;
                    for (int i = 0; i < 13; ++i) procStatFile >> aux;
                    long uTime, sTime, startTime;
                    procStatFile >> uTime >> sTime;
                    for (int i = 0; i < 6; ++i) procStatFile >> aux;
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
                procsNameMap.insert({procName, proc});
                procsPidMap.insert({pid, proc});
            }
        }
    }
}
