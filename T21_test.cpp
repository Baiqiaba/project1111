
//    int a,cnt=0;
//    cin>>a;
//    for(int i=1;i*i<=a;i++){//i作为宽,当i*i=a,即变为正方形是，我们的长方形就找完了
//        if(a%i==0){ //面积%宽==0的时候， 长*宽=a
//            cnt++;
//        }
//    }
//    cout<<cnt<<endl;

#include <bits/stdc++.h>
using namespace std;
int test() {
    int y,m;
    cin>>y>>m;
    if(m==1||m==3||m==5||m==7||m==8||m==10||m==12) cout<<31;
    else if(m==2){
        if(y%4==0&&y%100!=0||y%400==0) cout<<29;
        else cout<<28;
    }
    else cout<<30;
    return 0;
}


//    cout<< typeid(7.0).name();
//    int a,b=0;
////    float x;
////    cin>>x;
////    cout<<typeid(x).name();
////    cin>>m;