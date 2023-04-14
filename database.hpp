#ifndef DATABASE
#define DATABASE

#include <iostream>
#include <cstring>
#include <string>
#include <cstdio>
#include <cmath>
#include <fstream>
#include "utility.hpp"
#include "vector.hpp"
template<class T>
class database
{
public:
    class data{
    public:
        char key[65];
        T value;
        data(){}
        data(char *obj,const T &value_){
            strcpy(key,obj);
            value=value_;
        }
        friend bool operator < (const data &obj1,const data &obj2){
            if(strcmp(obj1.key,obj2.key)<0)return true;
            if(strcmp(obj1.key,obj2.key)==0 && obj1.value<obj2.value)return true;
            return false;
        }
        friend bool operator > (const data &obj1,const data &obj2){
            if(strcmp(obj1.key,obj2.key)>0)return true;
            if(strcmp(obj1.key,obj2.key)==0 && obj1.value>obj2.value)return true;
            return false;
        }
        friend bool operator == (const data &obj1,const data &obj2){
            if(strcmp(obj1.key,obj2.key)==0 && obj1.value==obj2.value)return true;
            else return false;
        }
        friend bool operator >= (const data &obj1,const data &obj2){
            if(strcmp(obj1.key,obj2.key)>0)return true;
            if(strcmp(obj1.key,obj2.key)==0 && obj1.value>=obj2.value)return true;
            return false;
        }
        friend bool operator <= (const data &obj1,const data &obj2){
            if(strcmp(obj1.key,obj2.key)<0)return true;
            if(strcmp(obj1.key,obj2.key)==0 && obj1.value<=obj2.value)return true;
            return false;
        }
        data operator = (const data &obj){
            strcpy(key,obj.key);
            value=obj.value;
            return *this;
        }
    };
    static const int size_of_block=4;
    class start
    {
    public:
        int pos_of_root;
        int num_of_block;
        start(){
            pos_of_root=1;
            num_of_block=1;
        }
        start(int pos_of_root_,int num_of_block_)
            :pos_of_root(pos_of_root_),num_of_block(num_of_block_){}
    };
    enum nodetype{index,leaf};
    class node
    {
    public:
        nodetype type;
        int pos_of_fa;
        int now_num;
        int edge[size_of_block+1];
        data value[size_of_block];
        int front_pos,back_pos;
        node(){
            type=leaf;
            now_num=0;
            pos_of_fa=0;
            front_pos=0;
            back_pos=0;
        }
    };
    std::fstream opfile;
    start head;
    void getstart(start &st){
        opfile.seekg(0);
        opfile.read(reinterpret_cast<char*>(&st),sizeof(st));
    }
    void writestart(start &st){
        opfile.seekp(0);
        opfile.write(reinterpret_cast<char*>(&st),sizeof(st));
    }
    void getnode(node &obj,int num){
        opfile.seekg(sizeof(start)+(num-1)*sizeof(node));
        opfile.read(reinterpret_cast<char*>(&obj),sizeof(obj));
    }
    void writenode(node &obj,int num){
        opfile.seekp(sizeof(start)+(num-1)*sizeof(node));
        opfile.write(reinterpret_cast<char*>(&obj),sizeof(obj));
    }
    database(){}
    database(std::string name){
        std::ifstream in;
        in.open(name);
        if(!in){
            std::ofstream outfile(name);
            outfile.seekp(0);
            start t1;
            outfile.write(reinterpret_cast<char*>(&t1),sizeof(start));
            outfile.seekp(sizeof(start));
            node t2;
            outfile.write(reinterpret_cast<char*>(&t2),sizeof(node));
        }
        opfile.open(name);
        getstart(head);
    }
    ~database(){
        writestart(head);
        opfile.close();
    }
    int finds(char* key,int num){
        node temp;
        getnode(temp,num);
        if(temp.type==leaf)return num;
        int t=sjtu::upper_bound(temp.value,temp.value+temp.now_num,data(key,T()))-temp.value;
        return finds(key,temp.edge[t]);
    }
    sjtu::vector<T> find(char* key){
        int num=finds(key,head.pos_of_root);
        node temp;
        getnode(temp,num);
        int t=sjtu::lower_bound(temp.value,temp.value+temp.now_num,data(key,T()))-temp.value;
        //std::cout<<t<<" 114514";
        int x=t;
        while(true){
            if(x==-1){
                if(temp.back_pos==0){
                    x++;
                    break;
                }
                getnode(temp,temp.back_pos);
                x=temp.now_num-1;
                if(strcmp(temp.value[x].key,key)!=0){
                    getnode(temp,temp.front_pos);
                    x=0;
                    break;
                }
            }
            if(strcmp(temp.value[x].key,key)!=0){
                x++;
                break;
            }
            x--;
        }
        sjtu::vector<T> ans;
        while(true){
            if(x==temp.now_num){
                if(temp.front_pos==0)break;
                getnode(temp,temp.front_pos);
                x=0;
            }
            if(strcmp(temp.value[x].key,key)==0)ans.push_back(temp.value[x].value);
            else break;
            x++;
        }
        return ans;
    }
    int find_pos(const data &obj,int num){
        node temp;
        getnode(temp,num);
        if(temp.type==leaf)return num;
        else{
            int t=sjtu::upper_bound(temp.value,temp.value+temp.now_num,obj)-temp.value;
            return find_pos(obj,temp.edge[t]);
        }
    }
    void updatefather(int pos,int fa_pos){
        node temp;
        getnode(temp,pos);
        temp.pos_of_fa=fa_pos;
        writenode(temp,pos);
    }
    void flashindex(int fa_pos,int son_pos,const data &obj){
        node temp;
        getnode(temp,fa_pos);
        int t=sjtu::upper_bound(temp.value,temp.value+temp.now_num,obj)-temp.value;
        for(int i=temp.now_num;i>t;i--){
            temp.value[i]=temp.value[i-1];
            temp.edge[i+1]=temp.edge[i];
        }//proofed
        temp.value[t]=obj;
        temp.edge[t+1]=son_pos;
        temp.now_num++;
        writenode(temp,fa_pos);
        if(temp.now_num==size_of_block){
            node temp2;
            int mid=size_of_block>>1;
            temp2.now_num=mid-1;
            for(int i=mid+1;i<size_of_block;i++){
                temp2.value[i-mid-1]=temp.value[i];
                temp2.edge[i-mid-1]=temp.edge[i];
                updatefather(temp.edge[i],head.num_of_block+1);
            }
            temp2.edge[mid-1]=temp.edge[size_of_block];
            updatefather(temp.edge[size_of_block],head.num_of_block+1);
            temp.now_num=mid;
            temp2.type=index;
            if(temp.pos_of_fa==0){
                temp.pos_of_fa=head.num_of_block+2;
                temp2.pos_of_fa=head.num_of_block+2;
                writenode(temp,fa_pos);
                writenode(temp2,head.num_of_block+1);
                head.num_of_block++;
                node temp3;
                temp3.edge[0]=fa_pos;
                temp3.edge[1]=head.num_of_block;
                temp3.value[0]=temp.value[mid];
                temp3.now_num=1;
                temp3.type=index;
                writenode(temp3,head.num_of_block+1);
                head.num_of_block++;
                head.pos_of_root=head.num_of_block;
            }
            else{
                temp2.pos_of_fa=temp.pos_of_fa;
                writenode(temp,fa_pos);
                writenode(temp2,head.num_of_block+1);
                head.num_of_block++;
                flashindex(temp.pos_of_fa,head.num_of_block,temp.value[mid]);
            }
        }
    }
    void devideleaf(int pos){
        node temp1;
        getnode(temp1,pos);
        node temp2;
        temp2.back_pos=pos;
        temp2.front_pos=temp1.front_pos;
        temp1.front_pos=head.num_of_block+1;
        for(int i=0;i<size_of_block/2;i++)temp2.value[i]=temp1.value[size_of_block/2+i];
        temp2.now_num=size_of_block/2;
        temp1.now_num=size_of_block/2;
        if(temp1.pos_of_fa==0){
            temp1.pos_of_fa=head.num_of_block+2;
            temp2.pos_of_fa=head.num_of_block+2;
            writenode(temp1,pos);
            writenode(temp2,head.num_of_block+1);
            head.num_of_block++;
            node temp3;
            temp3.now_num=1;
            temp3.edge[0]=pos;
            temp3.edge[1]=head.num_of_block;
            temp3.value[0]=temp2.value[0];
            temp3.type=index;
            writenode(temp3,head.num_of_block+1);
            head.num_of_block++;
            head.pos_of_root=head.num_of_block;
        }
        else{
            temp2.pos_of_fa=temp1.pos_of_fa;
            writenode(temp1,pos);
            writenode(temp2,head.num_of_block+1);
            head.num_of_block++;
            flashindex(temp1.pos_of_fa,head.num_of_block,temp2.value[0]);
        }
    }
    void test(bool typ){
        if(typ==0)std::cout<<"****************insert"<<'\n';
        else std::cout<<"****************delete"<<'\n';
        std::cout<<"root:"<<head.pos_of_root<<'\n';
        for(int i=1;i<=head.num_of_block;i++){
            node tempx;
            getnode(tempx,i);
            std::cout<<i<<':'<<'\n';
            std::cout<<' '<<"now_num="<<tempx.now_num<<'\n';
            std::cout<<' '<<"back_pos="<<tempx.back_pos<<'\n';
            std::cout<<' '<<"front_pos="<<tempx.front_pos<<'\n';
            std::cout<<' '<<"pos_of_fa="<<tempx.pos_of_fa<<'\n';
            std::cout<<' '<<"type="<<tempx.type<<'\n';
            for(int j=0;j<tempx.now_num;j++){
                std::cout<<tempx.value[j].key<<' '<<tempx.value[j].value<<'\n';
            }
            std::cout<<'\n'<<'\n';
        }
    }
    void insert(char* index,T val){
        data obj(index,val);
        int pos=find_pos(obj,head.pos_of_root);
        node temp;
        getnode(temp,pos);
        int t=sjtu::upper_bound(temp.value,temp.value+temp.now_num,obj)-temp.value;
        for(int i=temp.now_num;i>t;i--)temp.value[i]=temp.value[i-1];
        temp.value[t]=obj;
        temp.now_num++;
        writenode(temp,pos);
        node tempx;
        getnode(tempx,pos);
        if(temp.now_num==size_of_block)devideleaf(pos);
        //以下为调试信息
        //test(0);
    }
    bool judge(int pos,int fa,node& temp){
        if(pos==0)return false;
        getnode(temp,pos);
        if(temp.pos_of_fa!=fa)return false;
        return true;
    }
    data getleft(int pos){
        node temp;
        getnode(temp,pos);
        if(temp.type==leaf)return temp.value[0];
        else return getleft(temp.edge[0]);
    }
    void merge(int pos,node &obj1,node &obj2,node &fa,int tx){
        obj1.value[obj1.now_num]=fa.value[tx];
        obj1.now_num++;
        for(int i=0;i<obj2.now_num;i++){
            obj1.value[obj1.now_num+i]=obj2.value[i];
            obj1.edge[obj1.now_num+i]=obj2.edge[i];
            node temp;
            updatefather(obj2.edge[i],pos);
        }
        obj1.now_num+=obj2.now_num;
        obj1.edge[obj1.now_num]=obj2.edge[obj2.now_num];
        updatefather(obj2.edge[obj2.now_num],pos);
    }
    void balanceindex(int pos,data delindex){
        //如果根节点且儿子删完只剩一个则用儿子替代根
        node temp;
        getnode(temp,pos);
        if(pos==head.pos_of_root && temp.now_num==1){
            node son;
            getnode(son,temp.edge[0]);
            son.pos_of_fa=0;
            head.pos_of_root=temp.edge[0];
            writenode(son,temp.edge[0]);
            return;
        }
        int t=sjtu::upper_bound(temp.value,temp.value+temp.now_num,delindex)-temp.value;
        for(int i=t-1;i<temp.now_num-1;i++)temp.value[i]=temp.value[i+1];
        for(int i=t;i<temp.now_num;i++)temp.edge[i]=temp.edge[i+1];
        temp.now_num--;
        if(pos==head.pos_of_root){
            writenode(temp,pos);
            return;
        }
        if(temp.now_num<size_of_block/2-1){
            node fa;
            getnode(fa,temp.pos_of_fa);
            int tx=sjtu::upper_bound(fa.value,fa.value+fa.now_num,temp.value[0])-fa.value;
            if(tx!=fa.now_num){
                node rightbro;
                getnode(rightbro,fa.edge[tx+1]);
                if(rightbro.now_num>=size_of_block/2){
                    temp.edge[size_of_block/2-1]=rightbro.edge[0];
                    updatefather(rightbro.edge[0],pos);
                    temp.value[size_of_block/2-2]=fa.value[tx];
                    for(int i=0;i<rightbro.now_num-1;i++){
                        rightbro.value[i]=rightbro.value[i+1];
                        rightbro.edge[i]=rightbro.edge[i+1];
                    }
                    rightbro.now_num--;
                    temp.now_num++;
                    fa.value[tx]=getleft(rightbro.edge[0]);
                    writenode(temp,pos);
                    writenode(rightbro,fa.edge[tx+1]);
                    writenode(fa,temp.pos_of_fa);
                }
                else{
                    merge(pos,temp,rightbro,fa,tx);
                    writenode(temp,pos);
                    balanceindex(temp.pos_of_fa,fa.value[tx]);//可优化，直接传tx
                }
            }
            else{
                node leftbro;
                getnode(leftbro,fa.edge[tx-1]);
                if(leftbro.now_num>=size_of_block/2){
                    for(int i=temp.now_num;i>0;i--){
                        temp.value[i]=temp.value[i-1];
                        temp.edge[i+1]=temp.edge[i];
                    }
                    temp.edge[1]=temp.edge[0];
                    temp.edge[0]=leftbro.edge[leftbro.now_num];
                    temp.value[0]=fa.value[fa.now_num-1];
                    node xx;
                    getnode(xx,temp.edge[0]);
                    xx.pos_of_fa=pos;
                    writenode(xx,temp.edge[0]);
                    temp.now_num++;
                    leftbro.now_num--;
                    if(xx.type==leaf)fa.value[tx-1]=xx.value[0];
                    else fa.value[tx-1]=getleft(xx.edge[0]);
                    writenode(temp,pos);
                    writenode(leftbro,fa.edge[tx-1]);
                    writenode(fa,temp.pos_of_fa);
                }
                else{
                    merge(fa.edge[tx-1],leftbro,temp,fa,tx);
                    writenode(leftbro,fa.edge[tx-1]);
                    balanceindex(temp.pos_of_fa,fa.value[tx-1]);
                }
            }
        }
    }
    void balanceleaf(int pos){
        node temp,temp_back,temp_front;
        getnode(temp,pos);
        if(!temp.back_pos && !temp.front_pos)return;
        if(!judge(temp.back_pos,temp.pos_of_fa,temp_back)){
            getnode(temp_front,temp.front_pos);
            if(temp_front.now_num>size_of_block/2){
                temp.value[size_of_block/2-1]=temp_front.value[0];
                for(int i=0;i<temp_front.now_num-1;i++)temp_front.value[i]=temp_front.value[i+1];
                temp_front.now_num--;
                temp.now_num++;
                node temp_fa;
                getnode(temp_fa,temp.pos_of_fa);
                int t=sjtu::upper_bound(temp_fa.value,temp_fa.value+temp_fa.now_num,
                                temp.value[size_of_block/2-1])-temp_fa.value;
                temp_fa.value[t]=temp_front.value[0];
                writenode(temp,pos);
                writenode(temp_front,temp.front_pos);
                writenode(temp_fa,temp.pos_of_fa);
            }
            else{
                for(int i=size_of_block/2-1;i<size_of_block-1;i++)
                    temp.value[i]=temp_front.value[i-(size_of_block/2-1)];
                temp.front_pos=temp_front.front_pos;
                temp.now_num=size_of_block-1;
                writenode(temp,pos);
                balanceindex(temp.pos_of_fa,temp_front.value[0]);
            }
            return;
        }
        if(!judge(temp.front_pos,temp.pos_of_fa,temp_front)){
            getnode(temp_back,temp.back_pos);
            if(temp_back.now_num>size_of_block/2){
                temp.value[size_of_block/2-1]=temp_back.value[0];
                for(int i=0;i<temp_back.now_num-1;i++)temp_back.value[i]=temp_back.value[i+1];
                temp_back.now_num--;
                temp.now_num++;
                node temp_fa;
                getnode(temp_fa,temp.pos_of_fa);
                int t=sjtu::upper_bound(temp_fa.value,temp_fa.value+temp_fa.now_num,
                                temp.value[size_of_block/2-1])-temp_fa.value;
                temp_fa.value[t]=temp_back.value[0];
                writenode(temp,pos);
                writenode(temp_back,temp.back_pos);
                writenode(temp_fa,temp.pos_of_fa);
            }
            else{
                for(int i=size_of_block/2;i<size_of_block-1;i++)
                    temp_back.value[i]=temp.value[i-size_of_block/2];
                temp_back.front_pos=temp.front_pos;
                temp_back.now_num=size_of_block-1;
                writenode(temp_back,temp.back_pos);
                balanceindex(temp_back.pos_of_fa,temp.value[0]);
            }
            return;
        }
        //左右都是亲兄弟
        if(temp_front.now_num>size_of_block/2){
            temp.value[size_of_block/2-1]=temp_front.value[0];
            for(int i=0;i<temp_front.now_num-1;i++)temp_front.value[i]=temp_front.value[i+1];
            temp_front.now_num--;
            temp.now_num++;
            node temp_fa;
            getnode(temp_fa,temp.pos_of_fa);
            int t=sjtu::upper_bound(temp_fa.value,temp_fa.value+temp_fa.now_num,
                            temp.value[size_of_block/2-1])-temp_fa.value;
            temp_fa.value[t]=temp_front.value[0];
            writenode(temp,pos);
            writenode(temp_front,temp.front_pos);
            writenode(temp_fa,temp.pos_of_fa);
            return;
        }
        if(temp_back.now_num>size_of_block/2){
            temp.value[size_of_block/2-1]=temp_back.value[0];
            for(int i=0;i<temp_back.now_num-1;i++)temp_back.value[i]=temp_back.value[i+1];
            temp_back.now_num--;
            temp.now_num++;
            node temp_fa;
            getnode(temp_fa,temp.pos_of_fa);
            int t=sjtu::upper_bound(temp_fa.value,temp_fa.value+temp_fa.now_num,
                            temp.value[size_of_block/2-1])-temp_fa.value;
            temp_fa.value[t]=temp_back.value[0];
            writenode(temp,pos);
            writenode(temp_back,temp.back_pos);
            writenode(temp_fa,temp.pos_of_fa);
            return;
        }
        for(int i=size_of_block/2-1;i<size_of_block-1;i++)
            temp.value[i]=temp_front.value[i-(size_of_block/2-1)];
        temp.front_pos=temp_front.front_pos;
        temp.now_num=size_of_block-1;
        writenode(temp,pos);
        balanceindex(temp.pos_of_fa,temp_front.value[0]);
    }
    void erase(char* key,T val){
        data obj(key,val);
        int pos=find_pos(obj,head.pos_of_root);
        node temp;
        getnode(temp,pos);
        int t=sjtu::lower_bound(temp.value,temp.value+temp.now_num,obj)-temp.value;
        if(!(temp.value[t]==obj))return;
        for(int i=t;i<temp.now_num;i++)temp.value[i]=temp.value[i+1];
        temp.now_num--;
        writenode(temp,pos);
        if(temp.now_num<size_of_block/2)balanceleaf(pos);
        //test(1);
    }
};

#endif