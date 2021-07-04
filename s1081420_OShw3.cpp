#include <iostream>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <vector>
using namespace std;

int pork_size, slot_size;
int cut_waitting_num, pack_waiting_num,cut_done,pack_done;
bool cut_entry, pack_entry;//可不可進
pthread_t* pork;
struct state
{
    int Thread_num;
    bool cut_ed = false;//有沒有切割過
    bool pack_ed = false;//有沒有變成粽子
    bool in_slot = false;//有沒有在slot裡
};
vector<state>pork_v;
vector<int>slot_v;
pthread_t cut, pack;
pthread_mutex_t mutex_pork = PTHREAD_MUTEX_INITIALIZER, mutex_cut = PTHREAD_MUTEX_INITIALIZER, mutex_pack = PTHREAD_MUTEX_INITIALIZER;/*初始化互斥鎖*/
clock_t start,finish;
void wait(double millisecond) {
    usleep(millisecond * 1000); //<unistd.h> convert to millisecond//原本是微秒
}
void* pack_create(void* arg) {
    pack_waiting_num++;
    pthread_mutex_lock(&mutex_pack);
    double diff;
    if (!pack_entry) {
        double diff;
        if (cut_entry) {
            finish = clock();
            diff = finish - start; // ms
            cout << diff << "ms -- PACKER: under maintenance." << endl;
        }
        wait(diff);
        pack_waiting_num--;
        if (pack_waiting_num > 0)pack_entry = true;

        pthread_mutex_unlock(&mutex_pack);
        pack_create(&arg);
    }
    else {
        int index = -1;
        for (int i = 0; i < slot_v.size(); i++) {
            if (!pork_v[slot_v[i] - 1].pack_ed)index = i;//在slot中且沒有打包過
        }
        pork_v[index].in_slot = false;//從slot中拿出來
        vector<int>tmp;
        for (int i = 0; i < slot_v.size(); i++) {
            if (i != index)tmp.push_back(slot_v[i]);
        }
        slot_v = tmp;//從slot中拿出來//FCFS

        finish = clock();
        diff = finish - start; // ms
        cout << diff << "ms -- Pork#" << index+1 << ": enters to the factory (PACKER)" << endl;//進入
     
        int waiting_time = ((rand() % 6) + 5) * 100;//500~1000ms
        wait(waiting_time);
        cout << diff << "ms -- PACKER: processing & Packing the Pork#" << index+1 << " -- " << waiting_time << "ms" << endl;//工作

        finish = clock();
        diff = finish - start; // ms
        cout << diff << "ms -- Pork#" << index+1 << ": leaves PACKER (Complete)" << endl;//做好
        pork_v[index].pack_ed = true;
        pack_done++;

        if (pack_waiting_num == 1)pack_entry = false;
        pthread_mutex_unlock(&mutex_pack);
    }
    pthread_exit(0);
}
void* cut_create(void* arg) {
    cut_waitting_num++;
    pthread_mutex_lock(&mutex_cut);
    double diff;
    if (!cut_entry) {
        cut_waitting_num--;
        if (!pack_entry) {
            int waiting_time = ((rand() % 10) + 1) * 10;//10~100ms
            wait(waiting_time);
            finish = clock();
           diff = finish - start; // ms
            cout << diff << "ms – CUTTER: under reviewing together..." << endl << diff << "ms – PACKER: under reviewing together..." << endl;
        }
        else {
            if (cut_done == pork_size)pthread_exit(0);
            finish = clock();
            diff = finish - start; // ms
            cout << diff << "ms -- CUTTER: under maintenance." << endl;
        }
        wait(diff);

        if (cut_waitting_num > 0)cut_entry = true;
        
    }
    else {
        int index=-1;
        for (int i = 0; i < slot_v.size(); i++) {
            if(!pork_v[slot_v[i]-1].cut_ed)index = i;//在slot中且沒有切過
        }
        pork_v[index].in_slot = false;//從slot中拿出來
        vector<int>tmp;
        for (int i =0 ; i < slot_v.size(); i++) {
            if(i!=index)tmp.push_back(slot_v[i]);
        }
        slot_v = tmp;//從slot中拿出來//FCFS
        //cout<<"index : "<<index<<endl;
        
        finish = clock();
        diff = finish - start; // ms
        cout << diff << "ms -- Pork#" << index+1 << ": enters the CUTTER"<<endl;//進入

        int waiting_time = ((rand() % 3) + 1) * 100;//100~300ms
        wait(waiting_time);
        cout << diff  << "ms -- CUTTER: cutting... cutting... Pork#" << index+1<< " -- " << waiting_time << "ms"<<endl;//工作

        cut_done++;
        cut_waitting_num--;
        finish = clock();
        diff = finish - start; // ms
        cout << diff  << "ms -- Pork#" << index+1 << ": leaves CUTTER (complete 1st stage)"<<endl;//做好
        cout << diff  << "ms -- Pork#" << index+1 << ": waiting in the slot (cutted)"<<endl;//去slot等

        pork_v[index].in_slot = true;
        slot_v.push_back(index+1);
        pork_v[index].cut_ed = true;

        if (cut_waitting_num == 1)cut_entry = false;
    }
    pthread_mutex_unlock(&mutex_cut);
    cut_create(&arg);
    pthread_exit(0);
}
int slot_count() {
    int count = 0;
    for (int i = 0; i < pork_v.size(); i++) {
        if (pork_v[i].in_slot)count++;
    }
    return count;
}//有多少pork在備料台
void* pork_create(void* arg) {
    //cout<<"pork create hi"<<endl;
    pthread_mutex_lock(&mutex_pork);//上鎖
    state* now_pork = (state*)arg;
    int pork_time = ((rand() % 6) + 5) * 10;//50~100ms
    wait(pork_time);
    while (slot_count() >= slot_size) {
        int Freezer_time= ((rand() % 3) + 3) * 100;//300~500ms
        finish = clock();
        double diff = finish - start; // ms
        cout << diff  << "ms -- Pork#" << now_pork->Thread_num << " has been sent to the Freezer - " << Freezer_time << "ms" << endl;
        pthread_mutex_unlock(&mutex_pork);//解鎖
        wait(Freezer_time);
        pthread_mutex_lock(&mutex_pork);//上鎖
    }
   
    slot_v.push_back(now_pork->Thread_num);
    now_pork->in_slot = true;
    finish = clock();
    double diff = finish - start; // ms
    cout << diff  << "ms -- Pork#" << now_pork->Thread_num << ": waiting in the slot " << endl;
    now_pork->in_slot = true;
    pthread_mutex_unlock(&mutex_pork);//解鎖
    

    cut_entry = true;
    cut_create((void*)now_pork);
    pthread_exit(0);
}
int main(int argc, char** argv) {
    srand(time(NULL));
    pork_size=atoi(argv[1]);
    slot_size=atoi(argv[2]);
    
    pork = new pthread_t[pork_size];
    pork_v.resize(pork_size);
    for (int i = 0; i < pork_v.size(); i++) {
        pork_v[i].Thread_num = i + 1;//1~size
    }
    start = clock();
    for (int i = 0; i < pork_size; i++) {
        pthread_create(&pork[i], NULL, pork_create, (void*)&pork_v[i]);
        sleep(0.005);
    }
    pthread_create(&cut, NULL, cut_create, NULL);
    pthread_create(&pack, NULL, pack_create, NULL);
    for (int i = 0; i < pork_size; i++) {
        pthread_join(pork[i], NULL);
    }
    delete [] pork;
    return 0;
}