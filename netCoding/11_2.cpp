#include<iostream>
using namespace std;
int main(){
    string inputNum;
    cin>>inputNum;
    if(inputNum.length()<=2){
        cout<<"input error"<<endl;
    }else{
        int temp=0;
        for(int i=2;i<inputNum.length();i++){
            int num=inputNum[i];
            if(num<'a'){
                num-='0';
            }else{
                num-='a';
                num+=10;
            }
            if(i%2==0)
                temp+=(num*16);
            else{
                temp+=(num);
                cout<<temp;
                if(i!=inputNum.length()-1){
                    cout<<".";
                }
                temp=0;
            }
        }
    }
    return 0;
}