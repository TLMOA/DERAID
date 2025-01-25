#include <iostream>
#include <chrono>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
using namespace std;

unsigned long long m=252;



vector<vector<int>> b3;
vector<vector<vector<int>>> b, b1;  // 修改为三维数组

// LFSR生成密钥流
vector<int> generateKeyStream(int seed, int length, int tap1, int tap2) {
    vector<int> keyStream;
    int lfsr = seed;
    for (int i = 0; i < length; ++i) {
        int bit = (lfsr >> tap1) ^ (lfsr >> tap2);  // 根据抽头位置进行异或运算
        keyStream.push_back(bit);
//        cout << bit << " ";

        lfsr = (lfsr << 1) | bit;  // 移位并更新LFSR值
    }
//    cout<<endl;
    return keyStream;
}

// 加密和解密函数
vector<int> encryptDecrypt(const vector<int>& input, const vector<int>& keyStream) {
    vector<int> output;
    for (size_t i = 0; i < input.size(); ++i) {
        int encrypted = input[i] ^ keyStream[i];  // 明文与密钥流异或
        output.push_back(encrypted);
    }
    return output;
}


void RAID6M(unsigned long long row,unsigned long long col,const vector<int>& keyStream){
    unsigned long long i,j;
    for(int y=0;y<25;y++){
        for(i=0;i<row-1;i++){
            b[y][i][col-2]=(b[y][i][0]^b[y][i][1]);
            for(j=2;j<col-2;j++){
                b[y][i][col-2]=(b[y][i][col-2]^b[y][i][j]);
            }
        }
        unsigned long long k=0;
        for(i=0;i<row-1;i++){

            for(j=0;j<col-2;j++){
                b[y][k][col-1]=(b[y][k][col-1]^b[y][i][j]);
                k=(k+1)%5;
            }
            k++;
        }
        for(i=0;i<row-1;i++){
            for(j=0;j<col;j++){
                b[y][i][j]=(b[y][i][j]^keyStream[j]);
            }
        }
        b[y][row-1][col-1]=(b[y][row-1][col-1]^keyStream[col-1]);
    }
    

}

void MRAID6(unsigned long long row,unsigned long long col,const vector<int>& keyStream){
    unsigned long long i,j;
    for(int y=0;y<25;y++){
        for(i=0;i<row-1;i++){
            for(j=0;j<col-2;j++){
                b1[y][i][j]=(b1[y][i][j]^keyStream[j]);
            }
        }

        for(i=0;i<row-1;i++){
            b1[y][i][col-2]=(b1[y][i][0]^b1[y][i][1]);
            for(j=2;j<col-2;j++){
                b1[y][i][col-2]=(b1[y][i][col-2]^b1[y][i][j]);
            }
        }
        unsigned long long k=0;
        for(i=0;i<row-1;i++){

            for(j=0;j<col-2;j++){
                b1[y][k][col-1]=(b1[y][k][col-1]^b1[y][i][j]);
                k=(k+1)%5;
            }
            k++;
        }
    }
    
}
void DERAID5TWO(unsigned long long row,unsigned long long col,unsigned long long l){
    unsigned long long i,j;
    for(j=0;j<80;j++) {
        b3[j][col - 1] = (m ^ b3[j][col - 2]);
        for (i = col-2; i >= l+1; i--) {
            b3[j][i] = (b3[j][i - 1] ^ b3[j][i]);
        }
        b3[j][l] = (b3[j][l] ^ m);
    }
    for(j=0;j<80;j++){
        b3[j][l-1]=(m^b3[j][l-2]);
        for(i=l-2;i>=1;i--){
            b3[j][i]=(b3[j][i-1]^b3[j][i]);
        }
        b3[j][0]=(b3[j][0]^m);
    }



}






void RAID6M1(unsigned long long n,const vector<int>& keyStream){
    unsigned long long i;
    for(int y=0;y<25;y++){
        for(int j=0;j<4;j++){
            for(i=0;i<n-2;i++){
                cout<<(b[y][j][i]^keyStream[i])<<" ";
            }
            cout<<endl;
        }
    }


}

void MRAID61(unsigned long long n,const vector<int>& keyStream){
    unsigned long long i;
    for(int y=0;y<25;y++){
        for(int j=0;j<4;j++){
            for(i=0;i<n-2;i++){
                cout<<(b1[y][j][i]^keyStream[i])<<" ";
            }
            cout<<endl;
        }
    }


}

void DERAID5TWO1(unsigned long long n){
    unsigned long long i,j;

    for(int k=0;k<80;k++){
        j=m^b3[k][0];
        cout<<j<<" ";
        for(i=1;i<3;i++) {

            j=j^b3[k][i];
            cout<<j<<" ";
        }

        j=m^b3[k][4];
        cout<<j<<" ";
        for(i=5;i<7;i++) {

            j=j^b3[k][i];
            cout<<j<<" ";
        }
        cout<<endl;
    }



}






void RAID6M2(unsigned long long row,unsigned long long col,unsigned long long rowinx,unsigned long long colinx,unsigned long long data,const vector<int>& keyStream){
    for(int y=0;y<25;y++){


        for(int j=0;j<row-1;j++){

            for(int i=0;i<col;i++){
                if(i==col-1||i==col-2){
                    b[y][j][i]=0;
                }else if(i==colinx){
                    b[y][j][i]=data;
                }else{
                    b[y][j][i]=b[y][j][i]^keyStream[i];
                }

            }

        }
        b[y][row-1][col-1]=0;

    }
    RAID6M(row,col,keyStream);

}

void MRAID62(unsigned long long row,unsigned long long col,unsigned long long rowinx,unsigned long long colinx,unsigned long long data,const vector<int>& keyStream){
    for(int y=0;y<25;y++){

        for(int j=0;j<row-1;j++){
            for(int i=0;i<col;i++){
                if(i==col-1||i==col-2){
                    b1[y][j][i]=0;
                }else if(i==colinx){
                    b1[y][j][i]=data^keyStream[i];
                }

            }

        }
        b1[y][row-1][col-1]=0;

        for(int i=0;i<row-1;i++){
            b1[y][i][col-2]=(b1[y][i][0]^b1[y][i][1]);
            for(int j=2;j<col-2;j++){
                b1[y][i][col-2]=(b1[y][i][col-2]^b1[y][i][j]);
            }
        }
        unsigned long long k=0;
        for(int i=0;i<row-1;i++){

            for(int j=0;j<col-2;j++){
                b1[y][k][col-1]=(b1[y][k][col-1]^b1[y][i][j]);
                k=(k+1)%5;
            }
            k++;
        }
    }


}

void DERAID5TWO2(unsigned long long row,unsigned long long col,unsigned long long rowinx,unsigned long long colinx,unsigned long long data){

for(int p=0;p<80;p++){

        if(colinx>=1&&colinx<=3){
            unsigned long long i=1,j;
            j=(m^b3[p][0]);
            unsigned long long x=j;
            if(colinx==1) {
                b3[p][0]=(m^data);
                j=(j^b3[p][1]);
                b3[p][1]=(data^j);
            }else if(colinx>1&&colinx<3){

                while(i!=colinx+1) {

                    j=j^b3[p][i];
                    if(i==colinx-2)x=j;
                    i++;
                }
                b3[p][colinx]=(j^data);
                b3[p][colinx-1]=(x^data);
            }else if(colinx==3){
                while(i!=colinx-1) {

                    j=j^b3[p][i];
                    i++;
                }
                b3[p][colinx]=(m^data);
                b3[p][colinx-1]=(j^data);
            }
        }else{
            unsigned long long i=5,j;
            j=(m^b3[p][4]);
            unsigned long long x=j;
            if(colinx==5) {
                b3[p][4]=(m^data);
                j=(j^b3[p][5]);
                b3[p][5]=(data^j);
            }else if(colinx>5&&colinx<7){

                while(i!=colinx+1) {

                    j=j^b3[p][i];
                    if(i==colinx-2)x=j;
                    i++;
                }
                b3[p][colinx]=(j^data);
                b3[p][colinx-1]=(x^data);
            }else if(colinx==7){
                while(i!=colinx-1) {

                    j=j^b3[p][i];
                    i++;
                }
                b3[p][colinx]=(m^data);
                b3[p][colinx-1]=(j^data);
            }
        }

    }



}








void RAID6M3(unsigned long long row,unsigned long long col,unsigned long long col1,unsigned long long col2,const vector<int>& keyStream){
    unsigned long long i,j;
    vector<int> a(row-1),c(row-1);
    int x=0;//属于哪个斜的序列
    int x1=0;//要重建块所属的行

    for(int i1 = 0; i1 < row-1; i1++){

        a[i1]=(i1+col1)%5;
        c[i1]=(i1+col2)%5;
    }
    for (int i2 = 0; i2 < row-1; i2++) {
        if (find(a.begin(), a.end(), c[i2]) == a.end()) {
            x = (i2 + col2) % 5;
            x1=i2%5;
            break;
        }
    }


    for(int y=0;y<25;y++){
        for(i=0;i<row-1;i++){
            for(j=0;j<col;j++){

                b[y][i][j]=(b[y][i][j]^keyStream[j]);
            }
        }
        b[y][row-1][col-1]=(b[y][row-1][col-1]^keyStream[col-1]);


        for(int p=0;p<4;p++){
            b[y][x1][col2]=0;
            for(int i3=0;i3<row-1;i3++){
                if(i3!=x1){
                    b[y][x1][col2]=b[y][x1][col2]^b[y][i3][(x-i3+5)%5];
                }
            }
            b[y][x1][col2]=b[y][x1][col2]^b[y][x][col-1];

            b[y][x1][col1]=0;
            for(int i3=0;i3<col-1;i3++){
                if(i3!=col1){
                    b[y][x1][col1]=b[y][x1][col1]^b[y][x1][i3];
                }
            }
            x=(x1+col1)%5;
            x1=(5-col2+col1+x1)%5;
        }





        for(i=0;i<row-1;i++){
            for(j=0;j<col;j++){
                b[y][i][j]=b[y][i][j]^keyStream[j];
            }
        }

        b[y][row-1][col-1]=(b[y][row-1][col-1]^keyStream[col-1]);

    }


}

void MRAID63(unsigned long long row,unsigned long long col,unsigned long long col1,unsigned long long col2,const vector<int>& keyStream){
    unsigned long long i,j;
    vector<int> a(row-1),c(row-1);
    int x=0;//属于哪个斜的序列
    int x1=0;//要重建块所属的行

    for(int i1 = 0; i1 < row-1; i1++){

        a[i1]=(i1+col1)%5;
        c[i1]=(i1+col2)%5;
    }
    for (int i2 = 0; i2 < row-1; i2++) {
        if (find(a.begin(), a.end(), c[i2]) == a.end()) {
            x = (i2 + col2) % 5;
            x1=i2%5;
            break;
        }
    }
    for(int y=0;y<25;y++){
        for(int p=0;p<4;p++){
            b1[y][x1][col2]=0;
            for(int i3=0;i3<row-1;i3++){
                if(i3!=x1){
                    b1[y][x1][col2]=b1[y][x1][col2]^b1[y][i3][(x-i3+5)%5];
                }
            }
            b1[y][x1][col2]=b1[y][x1][col2]^b1[y][x][col-1];

            b1[y][x1][col1]=0;
            for(int i3=0;i3<col-1;i3++){
                if(i3!=col1){
                    b1[y][x1][col1]=b1[y][x1][col1]^b1[y][x1][i3];
                }
            }
            x=(x1+col1)%5;
            x1=(5-col2+col1+x1)%5;
        }
    }


}

void DERAID5TWO3(unsigned long long row,unsigned long long col,unsigned long long col1,unsigned long long col2){
    unsigned long long i;
    unsigned long long q,p;
    for(i=0;i<80;i++){
        q=(b3[i][0]^m);
        p=(b3[i][3]^m);
        p=(b3[i][2]^p);
        b3[i][1]=(q^p);

        q=(b3[i][4]^m);
        p=(b3[i][7]^m);
        p=(b3[i][6]^p);
        b3[i][5]=(q^p);
    }



}



int main() {
    string filename = "D:\\clion1\\DERAID5(2)&TRA(1)\\output25+20.txt";
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "文件未打开 " << filename << endl;
        return 1;
    }


    vector<vector<int>> tempMatrix;     // 临时存储一个6行8列的二维数组

    string line;
    while (getline(file, line)) {
        if (line.empty()) {  // 假设每个6行8列的二维数组之间用空行分隔
            if (tempMatrix.size() > 0) {
                b.push_back(tempMatrix);
                b1.push_back(tempMatrix);
                tempMatrix.clear();  // 清空临时矩阵，准备存储下一个二维数组
            }
            continue;
        }

        vector<int> row;
        istringstream iss(line);
        int value;
        while (iss >> value) {
            row.push_back(value);
        }
        if (row.size() == 7) {  // 确保每行有8个元素
            tempMatrix.push_back(row);
            if (tempMatrix.size() == 5) {  // 当存储了6行后，将其添加到三维数组中
                b.push_back(tempMatrix);
                b1.push_back(tempMatrix);
                tempMatrix.clear();  // 清空临时矩阵，准备存储下一个二维数组
            }
        } else {
            cerr << "错误：行元素数量不正确" << endl;
            return 1;
        }
    }

    // 检查文件末尾是否有未处理的二维数组
    if (!tempMatrix.empty()) {
        b.push_back(tempMatrix);
        b1.push_back(tempMatrix);
    }


    file.close();
// ?????

    // ?????
    string filename1 = "D:\\clion1\\DERAID5(2)&TRA(1)\\output80+6.txt";

    // ?????
    ifstream file1(filename1);

    // ?????????????
    if (!file1.is_open()) {
        cerr << "文件未打开 " << filename1 << endl;
        return 1;
    }


    string line1;

    while (getline(file1, line1)) {
        vector<int> row;
        istringstream iss(line1);
        int value;
        while (iss >> value) {
            row.push_back(value);
        }

        b3.push_back(row);
    }

    // ??????
    file1.close();


    vector<int> keyStream;
    int seed = 0b1011;  // LFSR初始值
//    int length = input.size();  // 密钥流长度，与输入数组长度相同
    int tap1 = 2;  // 抽头位置1
    int tap2 = 3;  // 抽头位置2

    // 生成密钥流
    keyStream = generateKeyStream(seed, 8, tap1, tap2);

    cout<<"编码过程******************************************"<<endl;
    auto start3 = chrono::high_resolution_clock::now(); // ??????

    RAID6M(5,7,keyStream);

    auto end3 = chrono::high_resolution_clock::now(); // ???????

    chrono::duration<double, milli> elapsed3 = end3 - start3; // ???????????

    cout << "RAID6M " << elapsed3.count() << " milliseconds." << endl;


    auto start4 = chrono::high_resolution_clock::now(); // ??????

    MRAID6(5,7,keyStream);

    auto end4 = chrono::high_resolution_clock::now(); // ???????

    chrono::duration<double, milli> elapsed4 = end4 - start4; // ???????????

    cout << "MRAID6 " << elapsed4.count() << " milliseconds." << endl;


    auto start5 = chrono::high_resolution_clock::now(); // ??????

    DERAID5TWO(6,8,4);

    auto end5 = chrono::high_resolution_clock::now(); // ???????

    chrono::duration<double, milli> elapsed5 = end5 - start5; // ???????????

    cout << "DERAID5TWO " << elapsed5.count() << " milliseconds." << endl;




//    for(int i=0;i<80;i++){
//        for(int j=0;j<8;j++){
//            cout<<b3[i][j]<<" ";
//        }
//        cout<<endl;
//    }
//    cout<<"*****************************************"<<endl;
    cout<<"解码过程******************************************"<<endl;
    auto start33 = chrono::high_resolution_clock::now(); // ??????

    RAID6M1(7,keyStream);

    auto end33 = chrono::high_resolution_clock::now(); // ???????

    chrono::duration<double, milli> elapsed33 = end33 - start33; // ???????????

    cout << "RAID6M1 " << elapsed33.count() << " milliseconds." << endl;


    auto start44 = chrono::high_resolution_clock::now(); // ??????

    MRAID61(7,keyStream);

    auto end44 = chrono::high_resolution_clock::now(); // ???????

    chrono::duration<double, milli> elapsed44 = end44 - start44; // ???????????

    cout << "MRAID61 " << elapsed44.count() << " milliseconds." << endl;


    auto start55 = chrono::high_resolution_clock::now(); // ??????

    DERAID5TWO1(8);

    auto end55 = chrono::high_resolution_clock::now(); // ???????

    chrono::duration<double, milli> elapsed55 = end55 - start55; // ???????????

    cout << "DERAID5TWO1 " << elapsed55.count() << " milliseconds." << endl;



    cout<<"更新过程******************************************"<<endl;



    auto start333 = chrono::high_resolution_clock::now(); // ??????

    RAID6M2(5,7,0,4,222,keyStream);

    auto end333 = chrono::high_resolution_clock::now(); // ???????

    chrono::duration<double, milli> elapsed333 = end333 - start333; // ???????????

    cout << "RAID6M2 " << elapsed333.count() << " milliseconds." << endl;


    auto start444 = chrono::high_resolution_clock::now(); // ??????

    MRAID62(5,7,0,4,222,keyStream);

    auto end444 = chrono::high_resolution_clock::now(); // ???????

    chrono::duration<double, milli> elapsed444 = end444 - start444; // ???????????

    cout << "MRAID62 " << elapsed444.count() << " milliseconds." << endl;



    auto start555 = chrono::high_resolution_clock::now(); // ??????

    DERAID5TWO2(6,8,0,6,222);

    auto end555 = chrono::high_resolution_clock::now(); // ???????

    chrono::duration<double, milli> elapsed555 = end555 - start555; // ???????????

    cout << "DERAID5TWO2 " << elapsed555.count() << " milliseconds." << endl;





//    cout<<"*************************"<<endl;
//    MRAID51(8);
//    cout<<"*************************"<<endl;
//    DERAID51(8);
//    cout<<"*************************"<<endl;
//    RAID6M1(8);
//    cout<<"*************************"<<endl;
//    MRAID61(8);
//    cout<<"*************************"<<endl;
//    DERAID5TWO1(8);


//    cout<<"*************************"<<endl;
//    RAID6M1(7,keyStream);
//    cout<<"*************************"<<endl;
//    MRAID61(7,keyStream);
//    cout<<"*************************"<<endl;
//    DERAID5TWO1(8);







    cout<<"重建过程******************************************"<<endl;



    auto start3333 = chrono::high_resolution_clock::now(); // ??????

    RAID6M3(5,7,1,4,keyStream);

    auto end3333 = chrono::high_resolution_clock::now(); // ???????

    chrono::duration<double, milli> elapsed3333 = end3333 - start3333; // ???????????

    cout << "RAID6M3 " << elapsed3333.count() << " milliseconds." << endl;



    auto start4444 = chrono::high_resolution_clock::now(); // ??????

    MRAID63(5,7,1,4,keyStream);

    auto end4444 = chrono::high_resolution_clock::now(); // ???????

    chrono::duration<double, milli> elapsed4444 = end4444 - start4444; // ???????????

    cout << "MRAID63 " << elapsed4444.count() << " milliseconds." << endl;



    auto start5555 = chrono::high_resolution_clock::now(); // ??????

    DERAID5TWO3(6,8,1,4);

    auto end5555 = chrono::high_resolution_clock::now(); // ???????

    chrono::duration<double, milli> elapsed5555 = end5555 - start5555; // ???????????

    cout << "DERAID5TWO3 " << elapsed5555.count() << " milliseconds." << endl;

//    cout<<"*************************"<<endl;
//    RAID6M1(7,keyStream);
//    cout<<"*************************"<<endl;
//    MRAID61(7,keyStream);
//    cout<<"*************************"<<endl;
//    DERAID5TWO1(6);

    cout << "RAID6M编码、解码、更新、重建：" << endl  ;
    cout << elapsed3.count() <<endl  ;
    cout << elapsed33.count() <<endl  ;
    cout <<  elapsed333.count() <<  endl ;
    cout << elapsed3333.count() <<endl ;

    cout << "MRAID6编码、解码、更新、重建：" << endl  ;
    cout <<  elapsed4.count() << endl  ;
    cout <<  elapsed44.count() << endl  ;
    cout << elapsed444.count() << endl ;
    cout << elapsed4444.count() <<  endl ;

    cout << "DERAID5TWO编码、解码、更新、重建：" << endl  ;
    cout << elapsed5.count() << endl  ;
    cout << elapsed55.count() << endl  ;
    cout << elapsed555.count() << endl ;
    cout << elapsed5555.count() <<endl ;

    cout<<"*******************************"<<endl;


    cout << elapsed3.count() <<endl  ;
    cout << elapsed33.count() <<endl  ;
    cout <<  elapsed333.count() <<  endl ;
    cout << elapsed3333.count() <<endl ;


    cout <<  elapsed4.count() << endl  ;
    cout <<  elapsed44.count() << endl  ;
    cout << elapsed444.count() << endl ;
    cout << elapsed4444.count() <<  endl ;


    cout << elapsed5.count() << endl  ;
    cout << elapsed55.count() << endl  ;
    cout << elapsed555.count() << endl ;
    cout << elapsed5555.count() <<endl ;

    return 0;
}
