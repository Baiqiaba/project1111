#include <bits/stdc++.h>

using namespace std;

void f(){
    int x,y;
    // x+y=35;
    // x*4+y*2=94;
    for(int y=0;y<=35;y++){
        x=35-y;
        if(4*x+2*y==94){
            cout<<x<<" "<<y;
            break;
        }
    }
}
