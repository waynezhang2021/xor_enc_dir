#include<iostream>
#include<cstring>
#include<windows.h>
#include<fstream>
using namespace std;
void xor_file(string from,string key,string to,bool delete_original=false)
{
	string newFilePath=to;
	string passwd=key;
	string filePath=from;
	char* buffer=new char[passwd.length()];
	for(int i=0; i<passwd.length(); i++)
		buffer[i]='\0';
	ifstream infile(filePath,ios::in|ios::binary);
	ofstream outfile(newFilePath,ios::out|ios::binary);
	if(!infile.is_open())
	{
		cout<<"\""<<from<<"\"";
		cout<<" does not exist"<<endl;
		return;
	}
	while(!infile.eof())
	{
		infile.read(buffer,sizeof(buffer));
		for(int i=0; i<passwd.length(); i++)
			buffer[i]=buffer[i]^passwd[i];
		outfile.write(buffer,sizeof(buffer));
	}
	infile.close();
	outfile.close();
	if(delete_original)
		DeleteFile(from.c_str());
}
void xor_dir(string dir,string key)
{
	string full_name,trunc_name;
	HANDLE hFind;
	WIN32_FIND_DATA findData;
	char dirNew[256];
	strcpy(dirNew, dir.c_str());
	strcat(dirNew, "\\*.*");
	hFind=FindFirstFile(dirNew, &findData);
	do
	{
		if ((findData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0&&strcmp(findData.cFileName,".")!=0&&strcmp(findData.cFileName,"..")!=0)
		{
			strcpy(dirNew, dir.c_str());
			strcat(dirNew, "\\");
			strcat(dirNew, findData.cFileName);
			xor_dir(dirNew,key);
		}
		else
		{
			full_name=(dir+"\\"+findData.cFileName);
			if(full_name.length()>12)
			{
				trunc_name=full_name.substr(full_name.length()-12);
				if(trunc_name==".xor_encrypt")
					xor_file(full_name,key,full_name.substr(0,full_name.length()-12),true);
				else
					xor_file(full_name,key,full_name+".xor_encrypt",true);
			}
			else
				xor_file(full_name,key,full_name+".xor_encrypt",true);
		}
	}
	while(FindNextFile(hFind,&findData));
	FindClose(hFind);
}
int main(int argc,char** argv)
{
	if(argc==1)
	{
		cout<<"usage:\n\t"<<argv[0]<<" <directory> <key>";
		exit(0);
	}
	string s=argv[1];
	s+="\\.enc_info";
	CreateDirectory(argv[1],NULL);
	fputs("key:\n",fopen(s.c_str(),"w"));
	fputs(argv[2],fopen(s.c_str(),"a"));
	string dir=argv[1],key=argv[2];
	xor_dir(dir,key);
	return 0;
}
