#include<bits/stdc++.h>
#include <fstream>
#include <io.h>
#define memoryspace 1000000
using namespace std;
struct objcode{
    vector<string> H;
    vector<string> D;
    vector<string> R;
    vector<vector<string>> T;
    vector<vector<string>> M;
};
map<string,int> ESTAB;
const int PROGADDR=0x4000;
char memory[memoryspace];
int get_num_by_hex_string(string s){
    stringstream ss;
    int num;
    ss<<hex<<s;
    ss>>num;
    return num;
}
string get_string_by_hex_num(int s){
    stringstream ss;
    ss<<hex<<s;
    return ss.str();
}
string fill_space(string s,int n){
    while(s.length()<n){
        s=s+' ';
    }
    return s;
}
string fill_zero(string s,int n){
    while(s.length()<n){
        s='0'+s;
    }
    return s;
}
void reduce(string &s,int n){
    if(s.length()>n) s=s.substr(s.length()-n);
}
void clear_memory(){
    for(int i=0;i<memoryspace;i++){
        memory[i]='.';
    }
}
void getFiles(string path, string exd, vector<string>& files)
{
    //cout << "getFiles()" << path<< endl;
    long   hFile = 0;
    struct _finddata_t fileinfo;
    string pathName, exdName;

    if (0 != strcmp(exd.c_str(), ""))
    {
        exdName = "\\*." + exd;
    }
    else
    {
        exdName = "\\*";
    }

    if ((hFile = _findfirst(pathName.assign(path).append(exdName).c_str(), &fileinfo)) != -1)
    {
        do
        {
            //cout << fileinfo.name << endl;
            if ((fileinfo.attrib &  _A_SUBDIR))
            {
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
                    getFiles(pathName.assign(path).append("\\").append(fileinfo.name), exd, files);
            }
            else
            {
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
                    files.push_back(pathName.assign(path).append("\\").append(fileinfo.name));
            }
        } while (_findnext(hFile, &fileinfo) == 0);
        _findclose(hFile);
    }
}
void read_file(objcode &objcode_input){
    ifstream in;
    vector<string> t,m,file;
    getFiles("C:\\my_linkerloder", "obj", file);
    for(auto i:file){
        in.open(i);
        string s;
        while(getline(in,s)){
            if(s[0]=='T') t.push_back(s);
            if(s[0]=='D') objcode_input.D.push_back(s);
            if(s[0]=='H') objcode_input.H.push_back(s);
            if(s[0]=='R') objcode_input.R.push_back(s);
            if(s[0]=='M') m.push_back(s);
        }
        in.close();
        objcode_input.T.push_back(t);
        objcode_input.M.push_back(m);
        t.clear();
        m.clear();
    }
}
void handle_T(objcode &objcode_input){
    int CSADDR=PROGADDR;
    for(int i=0;i<objcode_input.T.size();i++){
        for(int j=0;j<objcode_input.T[i].size();j++){
            int n=get_num_by_hex_string(objcode_input.T[i][j].substr(7,2))*2;
            int start_addr=CSADDR+get_num_by_hex_string(objcode_input.T[i][j].substr(1,6))*2;
            for(int k=0;k<n;k++){
                memory[start_addr+k]=objcode_input.T[i][j][k+9];
            }
        }
        CSADDR+=get_num_by_hex_string(objcode_input.H[i].substr(13,6))*2;
    }
}
void handle_M(objcode &objcode_input){
    int CSADDR=PROGADDR;
    int plunum=0;
    int pre=-1,preml=0;
    string s="";
    for(int i=0;i<objcode_input.M.size();i++){
        for(int j=0;j<objcode_input.M[i].size();j++){
            int ml=get_num_by_hex_string(objcode_input.M[i][j].substr(7,2));
            ml=(ml-5)^1;
            int addr=CSADDR+get_num_by_hex_string(objcode_input.M[i][j].substr(1,6))*2+ml;
            ml=(ml^1)+5;
            if(pre!=addr){
                s=get_string_by_hex_num(plunum+get_num_by_hex_string(s));
                plunum=0;
                s=fill_zero(s,preml);
                reduce(s,preml);
                for(int i=0;i<preml;i++){
                    memory[i+pre]=s[i];
                }
                pre=addr;
                preml=ml;
                s="";
                for(int i=0;i<ml;i++){
                    s=s+memory[i+addr];
                }
            }
            if(objcode_input.M[i][j][9]=='+') plunum=plunum+ESTAB[fill_space(objcode_input.M[i][j].substr(10),6)];
            else plunum=plunum-ESTAB[fill_space(objcode_input.M[i][j].substr(10),6)];
        }
        CSADDR+=get_num_by_hex_string(objcode_input.H[i].substr(13,6))*2;
        if(i==objcode_input.M.size()-1){
            s=get_string_by_hex_num(plunum+get_num_by_hex_string(s));
            plunum=0;
            s=fill_zero(s,preml);
            reduce(s,preml);
            for(int i=0;i<preml;i++){
                memory[i+pre]=s[i];
            }
        }
    }
}
int pass_1(objcode &objcode_input){
    int CSADDR=PROGADDR;
    for(int i=0;i<objcode_input.H.size();i++){
        ESTAB[objcode_input.H[i].substr(1,6)]=CSADDR;
        for(int j=1;j<objcode_input.D[i].length();j+=12){
            string name=objcode_input.D[i].substr(j,6);
            int addr=get_num_by_hex_string(objcode_input.D[i].substr(j+6,6));
            ESTAB[name]=CSADDR+addr;
        }
        CSADDR+=get_num_by_hex_string(objcode_input.H[i].substr(13,6));
    }
    return CSADDR-PROGADDR;
}
void pass_2(objcode &objcode_input){
    handle_T(objcode_input);
    handle_M(objcode_input);
}
void print_memory(int memory_space){
    for(int i=0;i<memory_space*2;i++){
        if(i%32 == 0) cout<<hex<<PROGADDR+i/2<<" ";
        cout<<memory[PROGADDR+i];
        if(i%32 == 31) cout<<'\n';
    }
}
int main(){
    int memory_space;
    objcode objcode_input;
    clear_memory();
    read_file(objcode_input);
    memory_space=pass_1(objcode_input);
    pass_2(objcode_input);
    print_memory(memory_space);
    return 0;
}
