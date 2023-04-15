#include "database.hpp"
#include <iostream>
#include "vector.hpp"
int main ()
{
    freopen("test.in","r",stdin);
    freopen("my.out","w",stdout);
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    int n;
    std::cin>>n;
    std::string s;
    char index[65];
    int value;
    database<int> db("data");
    for(int i=1;i<=n;i++){
        std::cin>>s>>index;
        if(s=="insert"){
            std::cin>>value;
            db.insert(index,value);
        }
        if(s=="delete"){
            std::cin>>value;
            db.erase(index,value);
        }
        if(s=="find"){
            sjtu::vector<int> ans=db.find(index);
            if(ans.empty()){
                std::cout<<"null\n";
                continue;
            }
            for(int i=0;i<ans.size();i++)std::cout<<ans[i]<<' ';
            std::cout<<'\n';
        }
    }
    return 0;
}