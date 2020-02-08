#include <cstdio>
#include <vector>
#include <string>
#include <stack>
#include <iostream>
using namespace std;

typedef unsigned char uchar;    //1字节
typedef unsigned short ushort;  //2字节
typedef unsigned int uint;      //4字节

int  BytsPerSec;	//每扇区字节数
int  SecPerClus;	//每簇扇区数
int  RsvdSecCnt;	//Boot记录占用的扇区数
int  NumFATs;	    //FAT表个数
int  RootEntCnt;	//根目录最大文件数
int  FATSz;	        //FAT扇区数

#pragma pack (1) /*指定按1字节对齐*/

struct BPB {
    ushort  BPB_BytsPerSec;	//每扇区字节数
    uchar   BPB_SecPerClus;	//每簇扇区数
    ushort  BPB_RsvdSecCnt;	//Boot记录占用的扇区数
    uchar   BPB_NumFATs;	//FAT表个数
    ushort  BPB_RootEntCnt;	//根目录最大文件数
    ushort  BPB_TotSec16;
    uchar   BPB_Media;
    ushort  BPB_FATSz16;	//FAT扇区数
    ushort  BPB_SecPerTrk;
    ushort  BPB_NumHeads;
    uint    BPB_HiddSec;
    uint    BPB_TotSec32;	//如果BPB_FATSz16为0，该值为FAT扇区数
};
//BPB至此结束，长度25字节

//根目录条目
struct RootEntry{
    uchar DIR_Name[11];
    uchar  DIR_Attr;
    uchar reserve[10];
    ushort DIR_WrtTime;
    ushort DIR_WrtDate;
    ushort DIR_FstClus;
    uint DIR_FileSize;
};
//根目录条目结束，32字节

#pragma pack () /*取消指定对齐，恢复缺省对齐*/

FILE* fat12;
struct BPB bpb;

vector<int> G[10000];
int dirCount[10000];
int fileCount[10000];
string pathName[10000];
string name[10000];
vector<RootEntry>Dirs;


void readBPB(struct BPB* bpb_ptr);
void readRoot(int ID);
void readChildren(int ID,int startClus);
void printLS(int id);
void printLS_L(int id);
void printCAT(int id);
int  getFATValue(int num);
vector<string> split(const string& str, const string& c);
bool checkL(string s);
bool checkFile(const string& s);

extern "C"{
void printW(const char*);
void printR(const char*);
}

int main(){
    fat12 = fopen("a.img","rb");
    struct BPB* bpb_ptr = &bpb;
    //载入BPB
    readBPB(bpb_ptr);
    //初始化各个全局变量
    BytsPerSec = bpb_ptr->BPB_BytsPerSec;
    SecPerClus = bpb_ptr->BPB_SecPerClus;
    RsvdSecCnt = bpb_ptr->BPB_RsvdSecCnt;
    NumFATs = bpb_ptr->BPB_NumFATs;
    RootEntCnt = bpb_ptr->BPB_RootEntCnt;
    if (bpb_ptr->BPB_FATSz16 != 0) {
        FATSz = bpb_ptr->BPB_FATSz16;
    } else {
        FATSz = bpb_ptr->BPB_TotSec32;
    }
    struct RootEntry rootEntry;
    Dirs.push_back(rootEntry);
    pathName[0]="";
    readRoot(0);
//    for(int i=0;i<Dirs.size();i++){
//        printf("%s\n",pathName[i].c_str());
//    }
//    printf("\n");
//    for(int i=0;i<Dirs.size();i++){
//        printf("%s\n",name[i].c_str());
//    }
//    printf("%d\n",Dirs.size());
    string input;
    while(true){
        printW(">");
        getline(cin,input);
        vector<string> cmd = split(input," ");
        string hint;
        if(cmd.empty())
            continue;
        if(cmd[0]=="exit")
            break;
        if(cmd[0]=="ls"){
            int ID =0;
            if(cmd.size()==1){
                //"ls"
                printLS(ID);
            }
//            else if(cmd[1]=="-l"){
//                //"ls -l"
//                path = "/";
//                printLS_L(path,ID);
//            }
            else{
                bool hasL=false;
                bool hasDir=false;
                bool mulDir=false;
                bool rightL=true;
                bool hasPath = false;
                bool wrongCommand=false;
                string getpath;
                for(int i=1;i<cmd.size();i++){
                    if(cmd[i][0]=='-'){
                        hasL = true;
                        rightL = checkL(cmd[i]);
                        if(!rightL)
                            break;
                    }
                    else if(cmd[i][0]=='/'){
                        if(hasDir){
                            mulDir=true;
                            break;
                        }
                        hasDir= true;
                        getpath=cmd[i];
                    }
                    else{
                        wrongCommand= true;
                        break;
                    }
                }
                if(wrongCommand){
                    hint = "please enter a valid command!\n";
                    printW(hint.c_str());
//                    printf("please enter a valid command!\n");
                    continue;
                }
                if(!rightL){
                    hint = "please enter -l in right way!\n";
                    printW(hint.c_str());
//                    printf("please enter -l in right way!\n");
                    continue;
                }
                if(mulDir){
                    hint = "please only enter one directory!\n";
                    printW(hint.c_str());
//                    printf("please only enter one directory!\n");
                    continue;
                }
                int p;
                for(p=0;p<Dirs.size();p++){
                    string pa = pathName[p];
                    if(pa==getpath){
                        hasPath= true;
                        break;
                    }
                }
                if(!hasPath){
                    hint = "please enter the right directory!\n";
                    printW(hint.c_str());
//                    printf("please enter the right directory!\n");
                    continue;
                }
                else{
                    bool isFile = checkFile(pathName[p]);
                    if(isFile){
                        hint = "this path is a file!\n";
                        printW(hint.c_str());
//                        printf("this path is a file!\n");
                        continue;
                    }
                    if(hasL){
                        printLS_L(p);
                    }
                    else{
                        printLS(p);
                    }
                }
            }
        } else if(cmd[0]=="cat"){
            if(cmd.size()==1){
                hint = "please enter a path!\n";
                printW(hint.c_str());
//                printf("please enter a path!\n");
                continue;
            }
            string filePath=cmd[1];
            if(filePath[0]!='/')
                filePath = "/"+filePath;
            bool hasPath = false;
            int p;
            for(p=0;p<Dirs.size();p++){
                string pa = pathName[p];
                if(pa==filePath){
                    hasPath= true;
                    break;
                }
            }
            if(!hasPath){
                hint = "please enter a right path!\n";
                printW(hint.c_str());
//                printf("please enter a right path!\n");
                continue;
            }
            bool isFile = checkFile(filePath);
            if(!isFile){
                hint = "this path is not a file!\n";
                printW(hint.c_str());
//                printf("this path is not a file!\n");
                continue;
            }
//            printf("%d\n",p);
            printCAT(p);
        }
        else{
            hint = "please enter a valid command!\n";
            printW(hint.c_str());
//            printf("please enter a valid command!\n");
            continue;
        }
    }
}

bool checkFile(const string& s){
    int len = s.length();
    if(len<6)
        return false;
    string txt = ".TXT";
    string end = s.substr(len-4,4);
    return txt==end;
}

bool checkL(string s){
    int len = s.length();
    if(len<=1)
        return false;
    bool flag=true;
    for(int i=1;i<len;i++){
        if(s[i]!='l'){
            flag = false;
            break;
        }
    }
    return flag;
}

void readBPB(struct BPB* bpb_ptr){
    int check;
    //BPB从偏移11个字节处开始
    check = fseek(fat12,11,SEEK_SET);
    if (check == -1)
        printf("fseek in readBPB failed!");
    //BPB长度为25字节
    check = fread(bpb_ptr,1,25,fat12);
    if (check != 25)
        printf("fread in readBPB failed!");
}

void readRoot(int ID){
    int base = (RsvdSecCnt + NumFATs * FATSz) * BytsPerSec;	//根目录首字节的偏移数
    int check;
    for(int i=0;i<RootEntCnt;i++){
        struct RootEntry rootEntry;
        struct RootEntry* rootEntry_ptr = &rootEntry;
        check = fseek(fat12,base,SEEK_SET);
        if (check == -1)
            printf("fseek in readRoot failed!");
        check = fread(rootEntry_ptr,1,32,fat12);
        if (check != 32)
            printf("fread in readRoot failed!");
        base += 32;
        if(rootEntry_ptr->DIR_Name[0]==0)//空条目
            continue;
        if(rootEntry_ptr->DIR_Name[0]==0xE5)//没使用
            continue;
        if(rootEntry_ptr->DIR_Attr==0x0F)//长文件名
            continue;
        Dirs.push_back(rootEntry);
        G[ID].push_back(Dirs.size()-1);
        //
        string rootPath = pathName[ID];
        rootPath = rootPath+"/";
        char realName[12];    //暂存将空格替换成点后的文件名
        if ((rootEntry_ptr->DIR_Attr & 0x10) == 0) {
            //文件
            int tempLong = -1;
            for (int k = 0; k < 11; k++) {
                if (rootEntry_ptr->DIR_Name[k] != ' ') {
                    tempLong++;
                    realName[tempLong] = rootEntry_ptr->DIR_Name[k];
                } else {
                    tempLong++;
                    realName[tempLong] = '.';
                    while (rootEntry_ptr->DIR_Name[k] == ' ') k++;
                    k--;
                }
            }
            tempLong++;
            realName[tempLong] = '\0';    //到此为止，把文件名提取出来放到了realName里
        } else {
            //目录
            int tempLong = -1;
            for (int k = 0; k < 11; k++) {
                if (rootEntry_ptr->DIR_Name[k] != ' ') {
                    tempLong++;
                    realName[tempLong] = rootEntry_ptr->DIR_Name[k];
                } else {
                    tempLong++;
                    realName[tempLong] = '\0';
                    break;
                }
            }    //到此为止，把目录名提取出来放到了realName
        }
        pathName[Dirs.size()-1]=rootPath+realName;
        name[Dirs.size()-1]=realName;
        //
        if(rootEntry_ptr->DIR_Attr==0x10){
            dirCount[ID]++;
            readChildren(Dirs.size()-1,rootEntry_ptr->DIR_FstClus);
        }
        else
            fileCount[ID]++;
    }
}

void readChildren(int ID,int startClus){
    //数据区的第一个簇（即2号簇）的偏移字节
    int dataBase = BytsPerSec * ( RsvdSecCnt + FATSz*NumFATs + (RootEntCnt*32 + BytsPerSec - 1)/BytsPerSec );
    int currentClus = startClus;
    int value = 0;
    while(value<0xFF8){
        value=getFATValue(currentClus);
        if (value == 0xFF7) {
            printf("坏簇，读取失败!\n");
            break;
        }
        int startByte = dataBase + (currentClus - 2)*SecPerClus*BytsPerSec;
        int count = SecPerClus*BytsPerSec;	//每簇的字节数
        int loop=0;
        while(loop<count){
            struct RootEntry rootEntry;
            struct RootEntry* rootEntry_ptr = &rootEntry;
            fseek(fat12, startByte+loop, SEEK_SET);
            fread(rootEntry_ptr, 1, 32, fat12);
            loop += 32;
            if (rootEntry_ptr->DIR_Name[0] == 0)
                continue;
            if (rootEntry_ptr->DIR_Name[0] == 0xE5)
                continue;
            if (rootEntry_ptr->DIR_Attr == 0x0F)
                continue;
            Dirs.push_back(rootEntry);
            G[ID].push_back(Dirs.size() - 1);
            //
            string rootPath = pathName[ID];
            rootPath = rootPath+"/";
            char realName[12];    //暂存将空格替换成点后的文件名
            bool isDir;
            if ((rootEntry_ptr->DIR_Attr & 0x10) == 0) {
                //文件
                int tempLong = -1;
                for (int k = 0; k < 11; k++) {
                    if (rootEntry_ptr->DIR_Name[k] != ' ') {
                        tempLong++;
                        realName[tempLong] = rootEntry_ptr->DIR_Name[k];
                    } else {
                        tempLong++;
                        realName[tempLong] = '.';
                        while (rootEntry_ptr->DIR_Name[k] == ' ') k++;
                        k--;
                    }
                }
                tempLong++;
                realName[tempLong] = '\0';    //到此为止，把文件名提取出来放到了realName里
                isDir=false;
            } else {
                //目录
                int tempLong = -1;
                for (int k = 0; k < 11; k++) {
                    if (rootEntry_ptr->DIR_Name[k] != ' ') {
                        tempLong++;
                        realName[tempLong] = rootEntry_ptr->DIR_Name[k];
                    } else {
                        tempLong++;
                        realName[tempLong] = '\0';
                        break;
                    }
                }    //到此为止，把目录名提取出来放到了realName
                isDir=true;
            }
            pathName[Dirs.size()-1]=rootPath+realName;
            name[Dirs.size()-1]=realName;

            if (rootEntry_ptr->DIR_Attr == 0x10 && rootEntry_ptr->DIR_Name[0] != '.'){
                dirCount[ID]++;
                readChildren(Dirs.size() - 1,  rootEntry_ptr->DIR_FstClus);
            }
            else if(rootEntry_ptr->DIR_Name[0] != '.')
                fileCount[ID]++;
        }
        currentClus=value;
    }
}

void printLS(int id){
    stack<int> stk;
    stk.push(id);
    while (!stk.empty()){
        int i=stk.top();
        printW((pathName[i]+"/:\n").c_str());
//        printf("%s/:\n",pathName[i].c_str());
        bool first = true;
        for(int j:G[i]){
            RootEntry entry = Dirs[j];
            RootEntry *entry_ptr = &entry;
            bool isDir=(entry_ptr->DIR_Attr==0x10);
            string realName=name[j];
            if (!first)
                printW(" ");
//                printf(" ");
            if(!isDir){
                printW(realName.c_str());
//                printf("%s",realName.c_str());
            }
            else{
                printR(realName.c_str());
//                printf("\033[31m%s",realName.c_str());
//                printf("\033[0m");
            }
            first = false;
        }
        string ent = "\n";
        printW(ent.c_str());
//        printf("\n");
        stk.pop();
        for(int j=G[i].size()-1;j>=0;j--){
            int s = G[i][j];
            RootEntry entry = Dirs[s];
            RootEntry *entry_ptr = &entry;
            bool isDir=(entry_ptr->DIR_Attr==0x10);
            string realName=name[s];
            if(isDir&&string(realName)!="."&&string(realName)!="..")
                stk.push(s);
        }
    }
}

void printLS_L(int id){
    stack<int> stk;
    stk.push(id);
    while (!stk.empty()){
        int i=stk.top();
        string sp = " ";
        printW((pathName[i]+"/"+sp+to_string(dirCount[i])+sp+to_string(fileCount[i])+":\n").c_str());
//        printf("%s/ %d %d:\n",pathName[i].c_str(),dirCount[i],fileCount[i]);
        for(int j:G[i]){
            RootEntry entry = Dirs[j];
            RootEntry *entry_ptr = &entry;
            bool isDir=(entry_ptr->DIR_Attr==0x10);
            string realName=name[j];
            if(!isDir){
                printW((realName+" "+to_string(entry_ptr->DIR_FileSize)+"\n").c_str());
//                printf("%s %d\n",realName.c_str(),entry_ptr->DIR_FileSize);
            }
            else{
                printR(realName.c_str());
//                printf("\033[31m%s",realName.c_str());
//                printf("\033[0m");
                if(string(realName)!="."&&string(realName)!="..")
                    printW((" "+to_string(dirCount[i])+" "+to_string(fileCount[i])).c_str());
//                    printf(" %d %d",dirCount[i],fileCount[i]);
//                printf("\n");
                string ent = "\n";
                printW(ent.c_str());
            }
        }
        string ent = "\n";
        printW(ent.c_str());
//        printf("\n");
        stk.pop();
        for(int j=G[i].size()-1;j>=0;j--){
            int s = G[i][j];
            RootEntry entry = Dirs[s];
            RootEntry *entry_ptr = &entry;
            bool isDir=(entry_ptr->DIR_Attr==0x10);
            string realName=name[s];
            if(isDir&&string(realName)!="."&&string(realName)!="..")
                stk.push(s);
        }
    }
}

void printCAT(int id){
//    printf("%d\n",id);
    RootEntry entry = Dirs[id];
    RootEntry* entry_ptr = &entry;
    int fileSZ = entry_ptr->DIR_FileSize;
    int currentClus = entry_ptr->DIR_FstClus;
    int dataBase=BytsPerSec*(RsvdSecCnt+FATSz*NumFATs+(RootEntCnt*32+BytsPerSec-1)/BytsPerSec);
    if(fileSZ<=512){
        int startByte=dataBase+(currentClus-2)*SecPerClus*BytsPerSec;
        char* content = (char* )malloc(SecPerClus*BytsPerSec);
        int check;
        check=fseek(fat12,startByte,SEEK_SET);
        if(check==-1){
            printf("fseek in printCAT fails.\n");
        }
        check=fread(content,1,fileSZ,fat12);
        if(check!=fileSZ){
            printf("fread in printCAT fails.\n");
        }
        printf("%s\n",content);
    }
    else{
        int value=0;
        while(value<0xFF8){
            value = getFATValue(currentClus);
            if (value == 0xFF7) {
                printf("坏簇，读取失败!\n");
                break;
            }
            int startByte=dataBase+(currentClus-2)*SecPerClus*BytsPerSec;
            char* content = (char* )malloc(SecPerClus*BytsPerSec);
            int check;
            check = fseek(fat12,startByte,SEEK_SET);
            if (check == -1)
                printf("fseek in printCAT failed!");
            check=fread(content,1,SecPerClus*BytsPerSec,fat12);
            if(check!=SecPerClus*BytsPerSec){
                printf("fread in printCAT fails.\n");
            }
            printf("%s",content);
//            printf("\n%d %d\n",currentClus,value);
            currentClus=value;
        }
        printf("\n");
    }
}

int getFATValue(int currentClus){
    //FAT1的偏移字节
    int fatBase = RsvdSecCnt * BytsPerSec;
    //FAT项的偏移字节
    int fatPos = fatBase + currentClus*3/2;
    //奇偶FAT项处理方式不同，分类进行处理，从0号FAT项开始
    int type = currentClus % 2;
    uchar fat[2];
    fseek(fat12, fatPos, SEEK_SET);
    fread(fat, 1, 2, fat12);
    if (type == 0)
        return ushort(((fat[1]&0x0F) << 8)|fat[0]);
    else
        return ushort((fat[1] << 4)|((fat[0] >> 4)&0x0F));
}

vector<string> split(const string& str, const string& c){
    vector<string> res;
    string::size_type pos1,pos2;
    pos2 = str.find(c);
    pos1=0;
    while(string::npos!=pos2){
        res.push_back(str.substr(pos1,pos2-pos1));
        pos1=pos2+c.size();
        pos2=str.find(c,pos1);
    }
    if(pos1!=str.length())
        res.push_back(str.substr(pos1));
    return res;
}
