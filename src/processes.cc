#include "processes.hh"

processes::processes()
{
    
}

int processes::getNProcs() const
{
    return nProcs;
}

string processes::getProcName(int i) const
{
    auto it = procsInfo.begin();
    for(int j = 0; j < i; ++j) ++it;
    return it->first;
}

string processes::getProcState(int i) const
{
    auto it = procsInfo.begin();
    for(int j = 0; j < i; ++j) ++it;
    return it->second.state;
}

int processes::getProcThreads(int i) const
{
    auto it = procsInfo.begin();
    for(int j = 0; j < i; ++j) ++it;
    return it->second.threads;
}

int processes::getProcIndex(string procName) const
{
    auto it = procsInfo.find(procName);
    if(it != procsInfo.end()) return distance(procsInfo.begin(), it);
    else return -1;
}

int processes::getProcPid(int i) const
{
    auto it = procsInfo.begin();
    for(int j = 0; j < i; ++j) ++it;
    return it->second.pid;
}

double processes::getReadKB(int i) const
{
    auto it = procsInfo.begin();
    for(int j = 0; j < i; ++j) ++it;
    return it->second.readKB - it->second.lastReadKB;
}

double processes::getWriteKB(int i) const
{
    auto it = procsInfo.begin();
    for(int j = 0; j < i; ++j) ++it;
    return it->second.writeKB - it->second.lastWriteKB;
}


void processes::update()
{
    map<string, process> auxMap = procsInfo;
    procsInfo.clear();
    for(const auto file : filesystem::directory_iterator("/proc")) {
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
                
                if(procsInfo.find(procName) != procsInfo.end()) {
                    string aux = procName + "(1)";
                    int i = 2;
                    while(procsInfo.find(aux) != procsInfo.end()) {
                        aux = procName + "(" + to_string(i) + ")";
                        ++i;
                    }
                    procName = aux;
                }
                
                double lastReadKB = 0, lastWriteKB = 0;
                auto it = auxMap.find(procName);
                if(it != auxMap.end()) {
                    lastReadKB = it->second.readKB;
                    lastWriteKB = it->second.writeKB;
                }
                
                
                procsInfo.insert({procName, {state, threads, pid, writeKB, readKB, lastWriteKB, lastReadKB}});
            }
        }
    }
    nProcs = procsInfo.size();
}
