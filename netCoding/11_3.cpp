#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
using namespace std;
void from10To16(const int num,string & tempStr){
    int num1=num/16;
    int num2=num%16;
    //cout<<"num:"<<num<<endl;
    //cout<<num1<<"-"<<num2<<endl;
    tempStr="";
    if(num1>=10){
        tempStr+=('a'+(num1-10));
    }else{
        tempStr+=(num1+'0');
    }
    if(num2>=10){
        tempStr+=('a'+(num2-10));
    }else{
        tempStr+=(num2+'0');
    }
}
int main(){
    string str;
    cin>>str;
    int temp=0;
    size_t pos;
    int i=0;
    cout<<"0x";
    while((pos=str.find('.',temp))){
        string tempStr=str.substr(temp,(pos-temp));
        //cout<<"tempStr:"<<tempStr<<endl;
        temp=pos+1;
        int num=stoi(tempStr);
        //cout<<"num:"<<num<<endl;
        from10To16(num,tempStr);
        //itoa(num,tempStr,16);
        cout<<tempStr;
        i++;
        if(i==4) break;
    }
    cout<<endl;
    return 0;
}