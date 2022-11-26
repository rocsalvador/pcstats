#include "processes.hh"

processes::processes()
{
    
}

int processes::getNProcs() const
{
    return procsNameMap.size();
}

string processes::getProcName(int i) const
{
    auto it = procsNameMap.begin();
    for(int j = 0; j < i; ++j) ++it;
    return it->first;
}

string processes::getProcState(int i) const
{
    auto it = procsNameMap.begin();
    for(int j = 0; j < i; ++j) ++it;
    return it->second->state;
}

int processes::getProcThreads(int i) const
{
    auto it = procsNameMap.begin();
    for(int j = 0; j < i; ++j) ++it;
    return it->second->threads;
}

int processes::getProcIndex(string procName) const
{
    auto it = procsNameMap.find(procName);
    if(it != procsNameMap.end()) return distance(procsNameMap.begin(), it);
    else return -1;
}

int processes::getProcPid(int i) const
{
    auto it = procsNameMap.begin();
    for(int j = 0; j < i; ++j) ++it;
    return it->second->writeKB - it->second->lastWriteKB;
}

double processes::getReadKB(int i) const
{
    auto it = procsNameMap.begin();
    for(int j = 0; j < i; ++j) ++it;
    return it->second->readKB - it->second->lastReadKB;
}

double processes::getWriteKB(int i) const
{
    auto it = procsNameMap.begin();
    for(int j = 0; j < i; ++j) ++it;
    return it->second->writeKB - it->second->lastWriteKB;
}


processes::process processes::getProcByName(string name) const
{
    return *procsNameMap.find(name)->second;
}

processes::process processes::getProcByPid(int pid) const
{
    return *procsPidMap.find(pid)->second;
}

void processes::update()
{
    map<string, process*> auxNameMap = procsNameMap;
    map<int, process*> auxPidMap = procsPidMap;

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
                procStatusFile >> aux >> procName;
                while(procStatusFile >> aux and aux != "State:");
                procStatusFile >> state;
                while(procStatusFile >> aux and aux != "Pid:");
                procStatusFile >> pid;
                while(procStatusFile >> aux and aux != "Threads:");
                procStatusFile >> aux;
                threads = stoi(aux);
                
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
                auto it = auxNameMap.find(procName);
                if(it != auxNameMap.end()) {
                    lastReadKB = it->second->readKB;
                    lastWriteKB = it->second->writeKB;
                }
                
                process auxProc = {procName, state, threads, pid, writeKB, readKB, lastWriteKB, lastReadKB};
                process *proc = &auxProc;
                procsNameMap.insert({procName, proc});
                procsPidMap.insert({pid, proc});
            }
        }
    }
}
