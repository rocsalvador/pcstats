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



void processes::update()
{
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
                int threads;
                procStatusFile >> aux >> procName;
                while(procStatusFile >> aux and aux != "State:");
                procStatusFile >> state;
                while(procStatusFile >> aux and aux != "Threads:");
                procStatusFile >> aux;
                threads = stoi(aux);
                procsInfo.insert({procName, {state, threads}});
            }
        }
    }
    nProcs = procsInfo.size();
}
