#include <iostream>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;


class GF {
public:
    GF(int prim = 0x11b, int field_size = 256) : field_size(field_size), prim(prim) {
        exp.resize(2 * field_size);
        log.resize(field_size);
        init_tables();
    }

    int add(int x, int y) { return x ^ y; }
    int sub(int x, int y) { return x ^ y; }



    int mul(int a, int b) {
        int result = 0;
        int temp = a;


        for (int i = 0; i < 8; i++) {
            if (b & 1) {
                result ^= temp;
            }
            temp <<= 1;
            if (temp & 0x100) {
                temp ^= prim;
            }
            b >>= 1;
        }

        return result;
    }

    int gf_inverse(int a) {
        int res = 0;
        for (int start = 1; start < 256; start++) {
            int temp = mul(a, start);
            if (temp == 1) {
                res = start;
                break;
            }
        }
        if (res == 0) {
            throw "inverse_does_not_exist";
        }
        a= res; // 将计算得到的逆元存储起来
        return a;
    }

    int div(int a, int b) {
        if (b == 0) throw invalid_argument("Division by zero.");
        int b_inv = gf_inverse(b);
        return mul(a, b_inv);
    }


    const std::vector<int>& get_exp() const { return exp; }
    int get_field_size() const { return field_size; }

private:
    int field_size;
    int prim;
    std::vector<int> exp;
    std::vector<int> log;

    void init_tables() {
        int x = 1;
        for (int i = 0; i < field_size - 1; ++i) {
            exp[i] = x;
            log[x] = i;
//            cout<<"exp"<<i<<"   "<<exp[i]<<endl;
//            cout<<"log"<<x<<"   "<<log[x]<<endl;
            x <<= 1;
            if (x & field_size) x ^= prim;

        }
        for (int i = field_size - 1; i < 2 * field_size - 2; ++i) {
            exp[i] = exp[i - (field_size - 1)];
//            cout<<"exp"<<i<<"   "<<exp[i]<<endl;
        }
    }
};


class QSCode {
public:
    QSCode(int k, int m, GF &gf) : k(k), m(m), n(k + m), gf(gf) {
        generator_matrix = create_generator_matrix();
//        for(int i=0;i<5;i++){
//            for(int j=0;j<4;j++){
//                cout<<generator_matrix[i][j]<<" ";
//            }
//            cout<<endl;
//        }
    }

    vector<int> encode(const vector<int> &data) {
        if (data.size() != k)
            throw invalid_argument("Input data must have " + to_string(k) + " blocks.");

        vector<int> encoded(n-1, 0);
        for (int i = 0; i < n-1; ++i) {
            for (int j = 0; j < k; ++j) {
                encoded[i] = gf.add(encoded[i], gf.mul(generator_matrix[i][j], data[j]));
            }
        }
        return encoded;
    }

    vector<int> decode(const vector<int> &received, const vector<int> &erasures) {
        if (received.size() != n-1)
            throw invalid_argument("Received data must have " + to_string(n) + " blocks.");


        vector<vector<int>> known_matrix;
        vector<int> known_data;


        for (int i = 0; i < n-1; ++i) {
            if (find(erasures.begin(), erasures.end(), i) == erasures.end()) {
                known_matrix.push_back(generator_matrix[i]);
//                cout<<"know_matrix"<<endl;
//                for(int p=0; p<4;p++){
//                    cout<<known_matrix[i][p]<<" ";
//                }
//                cout<<endl;
                known_data.push_back(received[i]);
//                cout<<"know_data"<<endl;
//                cout<<received[i]<<endl;
            }
        }


        vector<int> decoded(k, 0);
        if (!gaussian_elimination(known_matrix, known_data, decoded)) {
            throw runtime_error("Cannot decode due to singular matrix.");
        }
        return decoded;
    }

private:
    int k, m, n;
    GF &gf;
    vector<vector<int>> generator_matrix;

    vector<vector<int>> create_generator_matrix() {
        vector<vector<int>> matrix(n, vector<int>(k, 0));
        for (int i = 0; i < k-1; ++i) {
            matrix[i][i] = 1;
        }
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < k; ++j) {
                matrix[k-1 + i][j] = gf.get_exp()[(i * j) % (gf.get_field_size() - 1)];
            }
        }
        return matrix;
    }

    bool gaussian_elimination(vector<vector<int>> &matrix, vector<int> &data, vector<int> &decoded) {


//        for(int i=0;i<4;i++){
//            for(int j=0;j<4;j++){
//                cout<<matrix[i][j]<<" ";
//            }
//            cout<<endl;
//        }
//        for(int i=0;i<4;i++){
//            cout<<data[i]<< " "<<endl;
//        }
        int rows = matrix.size(), cols = matrix[0].size();

        for (int i = 0; i < cols; ++i) {
            // 找到列中的最大元素作为主元//找到列中除了0以外的最小元素
            int pivot = i;
            for (int j = i + 1; j < rows; ++j) {
                if(matrix[pivot][i]!=0){
                    if (matrix[j][i] < matrix[pivot][i]&&matrix[j][i]!=0) pivot = j;
                }else{

                    pivot++;
                    if (matrix[j][i] < matrix[pivot][i]&&matrix[j][i]!=0) pivot = j;
                }

            }
            swap(matrix[i], matrix[pivot]);
            swap(data[i], data[pivot]);

            if (matrix[i][i] == 0) return false;

            // 消元过程
            for (int j = i + 1; j < rows; ++j) {
                int factor = gf.div(matrix[j][i], matrix[i][i]);
                for (int k = i; k < cols; ++k) {
                    matrix[j][k] = gf.sub(matrix[j][k], gf.mul(factor, matrix[i][k]));
                }
                data[j] = gf.sub(data[j], gf.mul(factor, data[i]));
            }
        }
//        for(int i=0;i<4;i++){
//            for(int j=0;j<4;j++){
//                cout<<matrix[i][j]<<" ";
//            }
//            cout<<endl;
//        }
//        for(int i=0;i<4;i++){
//            cout<<data[i]<< " "<<endl;
//        }
        // 回代过程
        for (int i = cols - 1; i >= 0; --i) {
//            cout<<i<<" "<<data[i]<<" "<<matrix[i][i]<<endl;
            decoded[i] = gf.div(data[i], matrix[i][i]);
//            cout<<decoded[i]<<endl;
            for (int j = 0; j < i; ++j) {
                data[j] = gf.sub(data[j], gf.mul(matrix[j][i], decoded[i]));
            }
        }


        return true;
    }

};


int main() {
    GF gf;
    int k = 4, m = 2;
    QSCode qsc(k, m, gf);



    string filename = "D:\\clion\\qscodeupdate0\\output128+5.txt";
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "无法打开文件: " << filename << std::endl;
        return 1;
    }

    vector<std::vector<int>> a;
    string line;

    while (getline(file, line)) {
        vector<int> row;
        istringstream iss(line);
        int value;
        while (iss >> value) {
            row.push_back(value);
        }
        a.push_back(row);
    }

    file.close();


    vector<vector<int>> encoded;
    auto start = chrono::high_resolution_clock::now(); // 开始时间

    for(int i=0;i<128;i++){
        encoded.push_back(qsc.encode(a[i]));
    }


    auto end = chrono::high_resolution_clock::now(); // 结束时间

    chrono::duration<double, milli> elapsed = end - start; // 计算经过的时间

    cout << "编码过程 " << elapsed.count() << " milliseconds." << endl;
//    cout<<"*****************************************"<<endl;
//    for(int i=0;i<128;i++){
//        for(int j=0;j<5;j++){
//            cout<<encoded[i][j]<<" ";
//        }
//        cout<<endl;
//    }
//    cout<<"*****************************************"<<endl;

    auto start1 = chrono::high_resolution_clock::now(); // 开始时间
    for(int i=0;i<128;i++){
        vector<vector<int>> received = encoded;
        vector<int> erasures ;
        vector<int> decoded = qsc.decode(received[i], erasures);
        for (int d : decoded) cout << " " << d;
        cout << endl;
    }



    auto end1 = chrono::high_resolution_clock::now(); // 结束时间

    chrono::duration<double, milli> elapsed1 = end1 - start1; // 计算经过的时间

    cout << "解码过程 " << elapsed1.count() << " milliseconds." << endl;




    auto start2 = chrono::high_resolution_clock::now(); // 开始时间
    for(int i = 0; i < 128; i++){
        vector<vector<int>> received1 = encoded;

        vector<int> erasures1 ;
        vector<int> decoded1 = qsc.decode(received1[i], erasures1);

        decoded1[2]=222;

        encoded[i] = qsc.encode(decoded1);

    }

    auto end2 = chrono::high_resolution_clock::now(); // 结束时间

    chrono::duration<double, milli> elapsed2 = end2 - start2; // 计算经过的时间

    cout << "更新过程 " << elapsed2.count() << " milliseconds." << endl;



    auto start3 = chrono::high_resolution_clock::now(); // 开始时间
    for(int i=0;i<128;i++){
        vector<vector<int>> received2 = encoded;
        received2[i][1] = 0;
        vector<int> erasures2 = {1};
        vector<int> decoded2 = qsc.decode(received2[i], erasures2);

    }

    auto end3 = chrono::high_resolution_clock::now(); // 结束时间

    chrono::duration<double, milli> elapsed3 = end3 - start3; // 计算经过的时间

    cout << "重建过程 " << elapsed3.count() << " milliseconds." << endl;


    cout << "编码、解码、更新、重建：" <<  endl;
    cout << elapsed.count() <<  endl;
    cout << elapsed1.count() <<  endl;
    cout << elapsed2.count() <<  endl;
    cout << elapsed3.count() <<  endl;
    return 0;
}