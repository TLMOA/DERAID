#include <iostream>
#include <chrono>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

vector<vector<int>> a;
vector<vector<int>> a1;
vector<vector<int>> a2;
unsigned long long m=252;

// LFSR������Կ��
vector<int> generateKeyStream(int seed, int length, int tap1, int tap2) {
    vector<int> keyStream;
    int lfsr = seed;
    for (int i = 0; i < length; ++i) {
        int bit = (lfsr >> tap1) ^ (lfsr >> tap2);  // ���ݳ�ͷλ�ý����������
        keyStream.push_back(bit);
//        cout << bit << " ";

        lfsr = (lfsr << 1) | bit;  // ��λ������LFSRֵ
    }
//    cout<<endl;
    return keyStream;
}

// ���ܺͽ��ܺ���
vector<int> encryptDecrypt(const vector<int>& input, const vector<int>& keyStream) {
    vector<int> output;
    for (size_t i = 0; i < input.size(); ++i) {
        int encrypted = input[i] ^ keyStream[i];  // ��������Կ�����
        output.push_back(encrypted);
    }
    return output;
}




void RAID5M(unsigned long long n,const vector<int>& keyStream){
    unsigned long long i,j;
    for(j=0;j<128;j++){
        a[j][n-1]=(a[j][0]^a[j][1]);
        for(i=2;i<n-1;i++){
            a[j][n-1]=(a[j][n-1]^a[j][i]);
        }

        for(i=0;i<n;i++){
            a[j][i]=(keyStream[i]^a[j][i]);
        }
    }


}

void MRAID5(unsigned long long n,const vector<int>& keyStream){
    unsigned long long i,j;
    for(j=0;j<128;j++){
        for(i=0;i<n-1;i++){
            a1[j][i]=(keyStream[i]^a1[j][i]);
        }
        a1[j][n-1]=(a1[j][0]^a1[j][1]);
        for(i=2;i<n-1;i++){
            a1[j][n-1]=(a1[j][n-1]^a1[j][i]);
        }
    }



}



void DERAID5(unsigned long long n){
    unsigned long long i,j;
    for(j=0;j<128;j++){
        a2[j][n-1]=(m^a2[j][n-2]);
        for(i=3;i>=1;i--){
            a2[j][i]=(a2[j][i]^a2[j][i-1]);
        }
        a2[j][0]=(a2[j][0]^m);
    }
}





void RAID5M1(unsigned long long n,const vector<int>& keyStream){
    unsigned long long i,j;
    for(j=0;j<128;j++){
        for(i=0;i<n-1;i++){
            cout<<(a[j][i]^keyStream[i])<<" ";
//            if((a[j][i]^m)!=t[j][i]&&(a[j][i]^m)!=222)cout<<"!"<<endl;
        }
        cout<<endl;
    }

}
void MRAID51(unsigned long long n,const vector<int>& keyStream){
    unsigned long long i,j;
    for(j=0;j<128;j++){
        for(i=0;i<n-1;i++){
            cout<<(a1[j][i]^keyStream[i])<<" ";
//            if((a1[j][i]^m)!=t[j][i]&&(a1[j][i]^m)!=222)cout<<"!"<<endl;
        }
        cout<<endl;
    }

}


void DERAID51(unsigned long long n){
    unsigned long long i,j,k;
    for(j=0;j<128;j++){
        for(i=0;i<n-1;i++) {
            if(i==0){
                k=(m^a2[j][0]);
                cout<<k<<" ";
            }else{
                k=k^a2[j][i];
                cout<<k<<" ";
            }
        }
        cout<<endl;
    }



}



void RAID5M2(unsigned long long n,unsigned long long index,unsigned long long data,const vector<int>& keyStream){
    unsigned long long i,q,p,j;
    for(j=0;j<128;j++){
        a[j][index]=(data^keyStream[index]);
        q=a[j][0]^keyStream[0];
        p=a[j][1]^keyStream[1];
        a[j][n-1]=q^p;
        for(i=2;i<n-1;i++){
            a[j][n-1]=(a[j][n-1]^(a[j][i]^keyStream[i]));
        }
        a[j][n-1]=(a[j][n-1]^keyStream[n-1]);
    }

}
void MRAID52(unsigned long long n,unsigned long long index,unsigned long long data,const vector<int>& keyStream){
    for(unsigned long long j=0;j<128;j++){
        a1[j][index]=(data^keyStream[index]);
        a1[j][n-1]=(a1[j][0]^a1[j][1]);
        for(unsigned long long i=2;i<n-1;i++){
            a1[j][n-1]=(a1[j][n-1]^a1[j][i]);
        }
    }


}



void DERAID52(unsigned long long n,unsigned long long index,unsigned long long data){
    unsigned long long j,k;
    for(j=0;j<128;j++){
        unsigned long long i=1;
        k=(m^a2[j][0]);
        unsigned long long x=k;
        if(index==1) {
            a2[j][0]=(m^data);
            k=(k^a2[j][1]);
            a2[j][1]=(data^k);
        }else if(index>1&&index<4

                ){

            while(i!=index+1) {

                k=k^a2[j][i];
                if(i==index-2)x=k;
                i++;
            }
            a2[j][index]=(k^data);
            a2[j][index-1]=(x^data);
        }else{
            while(i!=index-1) {

                k=k^a2[j][i];
                i++;
            }
            a2[j][index]=(m^data);
            a2[j][index-1]=(k^data);
        }
    }


}






void RAID5M3(unsigned long long n,unsigned long long index,const vector<int>& keyStream) {
    unsigned long long i,j;
    for(j=0;j<128;j++){
        for (i = 0; i < n; i++) {
            if (i == index)a[j][i] = 0;
            else a[j][i] = a[j][i] ^ keyStream[i];
        }
        for (i = 0; i < n; i++) {
            if (i == index)continue;
            else a[j][index] = a[j][i] ^ a[j][index];
        }
        for (i = 0; i < n; i++) {
            a[j][i] = a[j][i] ^ keyStream[i];
        }
    }

}
void MRAID53(unsigned long long n,unsigned long long index,const vector<int>& keyStream) {
    unsigned long long i,j;
    for(j=0;j<128;j++){
        a1[j][index]=0;
        for(i=0;i<n;i++){
            if(i==index)continue;
            else a1[j][index]=(a1[j][i]^a1[j][index]);
        }
    }

}

void DERAID53(unsigned long long n,unsigned long long index){
    unsigned long long i,j;
    unsigned long long q,p;
    for(j=0;j<128;j++){
        q=(a2[j][0]^m);
        for(i=1;i<=index-1;i++){
            q=(q^a2[j][i]);
        }
        p=(a2[j][n-1]^m);
        for(i=n-2;i>=index+1;i--){
            p=(p^a2[j][i]);
        }
        a2[j][index]=(q^p);
    }

}





int main() {
    string filename = R"(D:\clion\DERAID5&TRA\output128+5.txt)";
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "�޷����ļ�: " << filename << std::endl;
        return 1;
    }


    string line;

    while (getline(file, line)) {
        vector<int> row;
        istringstream iss(line);
        int value;
        while (iss >> value) {
            row.push_back(value);
        }
        a.push_back(row);
        a1.push_back(row);
        a2.push_back(row);
    }

    file.close();

    vector<int> keyStream;
    int seed = 0b1011;  // LFSR��ʼֵ
//    int length = input.size();  // ��Կ�����ȣ����������鳤����ͬ
    int tap1 = 2;  // ��ͷλ��1
    int tap2 = 3;  // ��ͷλ��2

    // ������Կ��
    keyStream = generateKeyStream(seed, 8, tap1, tap2);



    cout<<"�������******************************************"<<endl;
    auto start = chrono::high_resolution_clock::now(); // ??????

    RAID5M(5, keyStream);

    auto end = chrono::high_resolution_clock::now(); // ???????

    chrono::duration<double, milli> elapsed = end - start; // ???????????

    cout << "RAID5M " << elapsed.count() << " milliseconds." << endl;


    auto start1 = chrono::high_resolution_clock::now(); // ??????

    MRAID5(5,keyStream);

    auto end1 = chrono::high_resolution_clock::now(); // ???????

    chrono::duration<double, milli> elapsed1 = end1 - start1; // ???????????

    cout << "MRAID5 " << elapsed1.count() << " milliseconds." << endl;


    auto start2 = chrono::high_resolution_clock::now(); // ??????

    DERAID5(5);


    auto end2 = chrono::high_resolution_clock::now(); // ???????

    chrono::duration<double, milli> elapsed2 = end2 - start2; // ???????????

    cout << "DERAID5 " << elapsed2.count() << " milliseconds." << endl;

//    cout<<"*****************************************"<<endl;
//    for(int i=0;i<128;i++){
//        for(int j=0;j<5;j++){
//            cout<<a[i][j]<<" ";
//        }
//        cout<<endl;
//    }
//    cout<<"*****************************************"<<endl;
//    for(int i=0;i<128;i++){
//        for(int j=0;j<5;j++){
//            cout<<a1[i][j]<<" ";
//        }
//        cout<<endl;
//    }
//    cout<<"*****************************************"<<endl;
//    for(int i=0;i<128;i++){
//        for(int j=0;j<5;j++){
//            cout<<a2[i][j]<<" ";
//        }
//        cout<<endl;
//    }
//    cout<<"*****************************************"<<endl;

    auto start00 = chrono::high_resolution_clock::now(); // ��ʼʱ��

    RAID5M1(5,keyStream);

    auto end00 = chrono::high_resolution_clock::now(); // ����ʱ��

    chrono::duration<double, milli> elapsed00 = end00 - start00; // ���㾭����ʱ��




    auto start11 = chrono::high_resolution_clock::now(); // ��ʼʱ��

    MRAID51(5,keyStream);

    auto end11 = chrono::high_resolution_clock::now(); // ����ʱ��

    chrono::duration<double, milli> elapsed11 = end11 - start11; // ���㾭����ʱ��




    auto start22 = chrono::high_resolution_clock::now(); // ��ʼʱ��

    DERAID51(5);

    auto end22 = chrono::high_resolution_clock::now(); // ����ʱ��

    chrono::duration<double, milli> elapsed22 = end22 - start22; // ���㾭����ʱ��


    cout<<"�������******************************************"<<endl;
    cout << "RAID5M1 " << elapsed00.count() << " milliseconds." << endl;
    cout << "MRAID51 " << elapsed11.count() << " milliseconds." << endl;
    cout << "DERAID51 " << elapsed22.count() << " milliseconds." << endl;

    cout<<"���¹���******************************************"<<endl;
    auto start000 = chrono::high_resolution_clock::now(); // ��ʼʱ��

    RAID5M2(5,2,222,keyStream);

    auto end000 = chrono::high_resolution_clock::now(); // ����ʱ��

    chrono::duration<double, milli> elapsed000 = end000 - start000; // ���㾭����ʱ��

    cout << "RAID5M2 " << elapsed000.count() << " milliseconds." << endl;



    auto start111 = chrono::high_resolution_clock::now(); // ��ʼʱ��

    MRAID52(5,2,222,keyStream);

    auto end111 = chrono::high_resolution_clock::now(); // ����ʱ��

    chrono::duration<double, milli> elapsed111 = end111 - start111; // ���㾭����ʱ��

    cout << "MRAID52 " << elapsed111.count() << " milliseconds." << endl;


    auto start222 = chrono::high_resolution_clock::now(); // ��ʼʱ��

    DERAID52(5,3,222);

    auto end222 = chrono::high_resolution_clock::now(); // ����ʱ��

    chrono::duration<double, milli> elapsed222 = end222 - start222; // ���㾭����ʱ��

    cout << "DERAID52 " << elapsed222.count() << " milliseconds." << endl;






//RAID5M1(8);
//MRAID51(8);
//DERAID51(8);


//    cout<<"*************************"<<endl;
//    MRAID51(8);
//    cout<<"*************************"<<endl;
//    DERAID51(8);
//    cout<<"*************************"<<endl;
//    RAID6M1(8);
//    cout<<"*************************"<<endl;
//    MRAID61(8);
//    cout<<"*************************"<<endl;
//    DERAID61(8);










    cout<<"�ؽ�����******************************************"<<endl;
    auto start0000 = chrono::high_resolution_clock::now(); // ��ʼʱ��

    RAID5M3(5,1,keyStream);

    auto end0000 = chrono::high_resolution_clock::now(); // ����ʱ��

    chrono::duration<double, milli> elapsed0000 = end0000 - start0000; // ���㾭����ʱ��

    cout << "RAID5M3 " << elapsed0000.count() << " milliseconds." << endl;



    auto start1111 = chrono::high_resolution_clock::now(); // ��ʼʱ��

    MRAID53(5,1,keyStream);

    auto end1111 = chrono::high_resolution_clock::now(); // ����ʱ��

    chrono::duration<double, milli> elapsed1111 = end1111 - start1111; // ���㾭����ʱ��

    cout << "MRAID53 " << elapsed1111.count() << " milliseconds." << endl;


    auto start2222 = chrono::high_resolution_clock::now(); // ��ʼʱ��

    DERAID53(5,1);

    auto end2222 = chrono::high_resolution_clock::now(); // ����ʱ��

    chrono::duration<double, milli> elapsed2222 = end2222 - start2222; // ���㾭����ʱ��

    cout << "DERAID53 " << elapsed2222.count() << " milliseconds." << endl;

//RAID5M1(5,keyStream);
//MRAID51(5,keyStream);
//DERAID51(5);

    cout << "RAID5M���롢���롢���¡��ؽ���" << endl  ;
    cout << elapsed.count() <<endl  ;
    cout << elapsed00.count() <<endl  ;
    cout <<  elapsed000.count() <<  endl ;
    cout << elapsed0000.count() <<endl ;

    cout << "MRAID5���롢���롢���¡��ؽ���" << endl  ;
    cout <<  elapsed1.count() << endl  ;
    cout <<  elapsed11.count() << endl  ;
    cout << elapsed111.count() << endl ;
    cout << elapsed1111.count() <<  endl ;

    cout << "DERAID5���롢���롢���¡��ؽ���" << endl  ;
    cout << elapsed2.count() << endl  ;
    cout << elapsed22.count() << endl  ;
    cout << elapsed222.count() << endl ;
    cout << elapsed2222.count() <<endl ;


    cout<<"*******************************"<<endl;


    cout << elapsed.count() <<endl  ;
    cout << elapsed00.count() <<endl  ;
    cout <<  elapsed000.count() <<  endl ;
    cout << elapsed0000.count() <<endl ;


    cout <<  elapsed1.count() << endl  ;
    cout <<  elapsed11.count() << endl  ;
    cout << elapsed111.count() << endl ;
    cout << elapsed1111.count() <<  endl ;


    cout << elapsed2.count() << endl  ;
    cout << elapsed22.count() << endl  ;
    cout << elapsed222.count() << endl ;
    cout << elapsed2222.count() <<endl ;

    return 0;
}
