#include <iostream>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <chrono>
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
        a= res; // ������õ�����Ԫ�洢����
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
            // �ҵ����е����Ԫ����Ϊ��Ԫ//�ҵ����г���0�������СԪ��
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

            // ��Ԫ����
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
        // �ش�����
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

    vector<vector<int>> a={{231,217,200,236},
                           {65,35,58,26},
                           {6,175,167,247},
                           {205,34,52,18},
                           {255,145,91,98},
                           {15,19,119,112},
                           {199,84,120,33},
                           {94,45,109,92},
                           {251,87,96,150},
                           {174,136,201,254},
                           {31,192,219,183},
                           {101,102,202,44},
                           {207,6,39,184},
                           {22,100,83,13},
                           {133,44,130,209},
                           {169,183,93,46},
                           {89,52,244,30},
                           {8,219,102,201},
                           {139,37,183,42},
                           {95,240,67,244},
                           {66,152,142,173},
                           {104,87,247,80},
                           {192,180,88,127},
                           {164,78,14,48},
                           {198,125,96,131},
                           {57,144,23,160},
                           {96,157,188,99},
                           {162,167,133,141},
                           {26,251,60,241},
                           {208,35,123,2},
                           {89,31,134,116},
                           {45,136,72,80},
                           {211,172,216,238},
                           {11,38,203,92},
                           {96,191,16,164},
                           {21,149,183,182},
                           {201,217,141,155},
                           {159,109,27,158},
                           {172,183,56,109},
                           {149,108,46,47},
                           {32,182,173,228},
                           {111,62,147,96},
                           {9,221,2,236},
                           {54,104,60,136},
                           {48,193,107,94},
                           {64,98,255,224},
                           {175,24,237,110},
                           {182,53,132,242},
                           {174,51,235,213},
                           {174,106,23,250},
                           {182,48,10,50},
                           {222,69,59,206},
                           {159,5,179,136},
                           {209,185,87,238},
                           {39,199,17,229},
                           {213,75,16,42},
                           {176,189,152,10},
                           {110,1,244,43},
                           {16,171,70,121},
                           {230,95,211,152},
                           {80,13,217,241},
                           {47,136,191,195},
                           {183,152,116,254},
                           {248,54,157,230},
                           {50,150,124,105},
                           {109,85,29,88},
                           {137,117,137,64},
                           {17,177,172,210},
                           {217,205,14,128},
                           {236,119,170,224},
                           {27,170,134,118},
                           {220,215,145,218},
                           {90,211,67,125},
                           {16,89,166,204},
                           {170,103,71,89},
                           {125,226,196,12},
                           {71,33,229,112},
                           {29,18,99,120},
                           {218,5,90,178},
                           {87,218,189,240},
                           {140,11,34,166},
                           {72,50,229,38},
                           {113,63,99,63},
                           {51,192,215,7},
                           {155,126,234,208},
                           {242,161,233,183},
                           {41,41,189,78},
                           {191,225,224,139},
                           {1,95,33,166},
                           {49,90,113,176},
                           {230,44,81,138},
                           {112,172,194,240},
                           {235,222,239,142},
                           {8,111,106,95},
                           {142,194,219,87},
                           {250,21,210,66},
                           {48,56,249,100},
                           {194,123,126,34},
                           {146,208,83,247},
                           {217,22,38,211},
                           {234,152,115,194},
                           {39,244,124,237},
                           {253,45,1,129},
                           {81,123,124,207},
                           {1,154,143,103},
                           {104,36,217,223},
                           {244,165,152,178},
                           {206,82,72,219},
                           {169,15,176,130},
                           {98,190,133,214},
                           {253,41,168,174},
                           {171,161,253,211},
                           {134,66,201,15},
                           {56,182,29,130},
                           {144,2,96,228},
                           {120,248,65,73},
                           {27,29,248,85},
                           {176,108,103,110},
                           {220,136,51,41},
                           {208,208,220,9},
                           {216,19,6,250},
                           {244,59,251,221},
                           {35,133,227,83},
                           {29,242,75,251},
                           {171,223,19,155},
                           {97,188,191,54},
                           {16,71,159,30},
                           {57,23,242,184},
                           {154,147,41,141},
                           {55,189,55,93},
                           {161,207,126,249},
                           {28,192,181,107},
                           {85,116,214,110},
                           {81,176,26,126},
                           {19,157,197,37},
                           {126,155,159,151},
                           {109,11,206,137},
                           {42,59,166,41},
                           {124,195,179,122},
                           {223,198,116,170},
                           {201,50,191,136},
                           {109,49,85,55},
                           {224,140,176,61},
                           {42,148,117,155},
                           {35,215,170,215},
                           {40,35,4,153},
                           {162,118,41,23},
                           {56,73,172,248},
                           {2,158,61,139},
                           {63,126,45,248},
                           {116,17,155,123},
                           {131,185,146,123},
                           {6,160,195,136},
                           {202,103,134,43},
                           {140,158,80,225},
                           {220,74,28,203},
                           {217,139,87,250},
                           {240,5,84,124},
                           {110,100,141,1},
                           {109,50,109,106},
                           {114,128,88,247},
                           {188,43,224,151},
                           {151,105,227,237},
                           {53,27,51,140},
                           {103,37,201,240},
                           {195,125,88,21},
                           {234,158,56,85},
                           {11,211,113,38},
                           {96,189,55,127},
                           {45,184,10,31},
                           {145,173,231,202},
                           {41,248,124,72},
                           {250,208,68,245},
                           {157,131,140,144},
                           {199,59,81,221},
                           {65,124,231,185},
                           {204,52,50,64},
                           {81,218,249,114},
                           {196,86,120,236},
                           {20,86,162,183},
                           {4,252,142,92},
                           {106,47,252,69},
                           {8,97,119,69},
                           {246,124,255,228},
                           {75,144,151,80},
                           {107,200,228,228},
                           {182,232,233,254},
                           {36,246,25,22},
                           {1,96,124,30},
                           {62,166,198,122},
                           {31,191,255,51},
                           {109,229,188,2},
                           {24,255,146,105},
                           {125,135,54,125},
                           {57,11,43,152},
                           {27,99,168,84},
                           {57,238,247,49},
                           {241,150,5,193},
                           {196,132,195,237},
                           {66,47,99,204},
                           {5,8,194,58},
                           {105,7,144,200},
                           {111,94,52,222},
                           {222,154,149,209},
                           {193,76,184,208},
                           {111,77,150,188},
                           {148,178,86,185},
                           {56,210,109,82},
                           {30,29,186,96},
                           {213,232,5,98},
                           {111,254,118,67},
                           {130,197,77,115},
                           {112,78,22,61},
                           {29,75,240,42},
                           {133,174,148,50},
                           {246,149,175,119},
                           {213,104,96,108},
                           {101,82,80,232},
                           {36,137,3,145},
                           {32,32,78,7},
                           {123,24,54,96},
                           {170,77,61,103},
                           {59,213,46,210},
                           {151,103,97,83},
                           {241,48,215,159},
                           {99,138,36,74},
                           {59,148,116,10},
                           {175,220,125,13},
                           {102,73,177,184},
                           {212,222,129,127},
                           {128,109,249,229},
                           {69,126,213,168},
                           {81,148,223,200},
                           {217,38,245,74},
                           {245,230,109,176},
                           {86,46,116,11},
                           {24,190,91,224},
                           {84,230,160,106},
                           {102,24,49,225},
                           {255,19,33,81},
                           {23,76,178,214},
                           {90,90,13,192},
                           {154,229,198,57},
                           {190,253,245,17},
                           {211,6,154,31},
                           {238,249,104,99},
                           {83,85,115,166},
                           {12,19,9,183},
                           {236,5,220,18},
                           {243,179,152,234},
                           {15,248,167,201},
                           {207,127,74,54},
                           {102,45,85,31},
                           {97,5,18,173},
                           {75,184,107,186},
                           {67,112,211,110},
                           {167,253,146,104},
                           {68,82,45,140},
                           {127,213,168,92},
                           {65,70,184,32},
                           {141,209,178,245},
                           {183,235,21,211},
                           {186,51,37,42},
                           {145,170,168,92},
                           {3,230,67,116},
                           {229,255,47,213},
                           {232,106,228,115},
                           {8,165,100,236},
                           {102,44,119,83},
                           {129,154,97,195},
                           {102,167,161,59},
                           {121,171,174,30},
                           {119,45,188,43},
                           {132,47,14,18},
                           {163,65,49,6},
                           {194,222,142,144},
                           {149,244,14,230},
                           {161,250,26,213},
                           {199,217,52,212},
                           {203,238,66,17},
                           {208,75,142,185},
                           {130,144,30,163},
                           {213,156,176,142},
                           {211,16,58,232},
                           {102,233,162,24},
                           {109,189,123,133},
                           {215,186,117,186},
                           {2,128,165,105},
                           {77,77,38,48},
                           {209,176,224,221},
                           {146,75,192,130},
                           {159,254,86,209},
                           {62,247,16,150},
                           {79,68,118,32},
                           {70,244,14,89},
                           {139,191,60,8},
                           {7,29,40,173},
                           {133,97,133,235},
                           {229,11,155,40},
                           {202,211,30,119},
                           {244,192,171,69},
                           {172,159,113,23},
                           {184,59,241,187},
                           {170,170,226,193},
                           {113,114,168,45},
                           {128,67,244,245},
                           {117,7,131,221},
                           {81,4,100,57},
                           {2,82,249,3},
                           {217,16,33,7},
                           {48,216,174,215},
                           {156,59,54,5},
                           {222,153,15,110},
                           {207,11,168,173},
                           {181,116,23,51},
                           {192,125,237,242},
                           {234,184,105,84},
                           {7,102,238,173},
                           {11,32,172,238},
                           {66,49,154,117},
                           {114,38,227,66},
                           {249,79,215,35},
                           {51,176,217,129},
                           {179,44,114,125},
                           {105,2,181,85},
                           {93,116,88,111},
                           {49,186,178,131},
                           {201,218,191,216},
                           {155,44,236,38},
                           {209,97,134,160},
                           {220,232,31,187},
                           {96,44,59,211},
                           {159,134,62,78},
                           {58,177,239,85},
                           {50,107,21,73},
                           {95,2,106,255},
                           {87,182,4,172},
                           {64,64,89,188},
                           {148,92,191,244},
                           {70,226,111,124},
                           {188,180,132,75},
                           {174,134,83,65},
                           {81,3,120,198},
                           {38,73,20,106},
                           {249,68,134,102},
                           {179,88,218,76},
                           {204,227,65,223},
                           {49,212,121,74},
                           {80,55,94,56},
                           {84,154,87,194},
                           {218,186,171,213},
                           {24,112,57,50},
                           {1,158,207,128},
                           {80,34,201,215},
                           {244,56,63,45},
                           {215,144,233,241},
                           {234,150,217,235},
                           {134,82,118,170},
                           {15,130,19,147},
                           {234,217,62,221},
                           {166,112,36,35},
                           {177,93,188,38},
                           {123,48,239,227},
                           {31,77,124,61},
                           {88,95,240,180},
                           {171,200,136,29},
                           {191,90,152,130},
                           {216,73,126,141},
                           {157,128,127,99},
                           {240,13,246,92},
                           {227,142,185,164},
                           {37,230,86,82},
                           {85,69,76,170},
                           {191,214,249,20},
                           {142,174,80,215},
                           {178,109,26,30},
                           {26,59,217,139},
                           {37,182,66,49},
                           {2,187,15,235},
                           {124,97,152,118},
                           {82,41,115,142},
                           {26,100,137,79},
                           {175,239,15,21},
                           {57,30,213,136},
                           {129,46,126,70},
                           {198,108,7,188},
                           {78,89,57,253},
                           {218,23,195,1},
                           {113,28,247,90},
                           {8,95,113,243},
                           {192,143,183,23},
                           {213,114,21,228},
                           {32,59,65,171},
                           {79,79,49,90},
                           {129,250,178,60},
                           {59,94,102,155},
                           {201,118,31,237},
                           {247,241,3,240},
                           {236,62,18,134},
                           {147,112,237,99},
                           {80,101,59,76},
                           {15,11,24,248},
                           {131,187,190,243},
                           {242,76,220,90},
                           {156,23,139,117},
                           {44,81,151,247},
                           {199,87,198,66},
                           {118,3,57,168},
                           {113,132,97,51},
                           {163,36,61,83},
                           {26,70,190,56},
                           {7,48,110,112},
                           {224,19,138,41},
                           {10,107,48,203},
                           {112,83,214,73},
                           {77,178,247,188},
                           {106,177,244,224},
                           {123,71,162,173},
                           {218,186,211,112},
                           {25,196,254,194},
                           {55,55,201,15},
                           {150,66,35,137},
                           {110,46,8,253},
                           {246,138,99,131},
                           {167,32,100,184},
                           {25,127,86,158},
                           {95,249,59,49},
                           {90,198,27,197},
                           {111,64,196,40},
                           {134,193,169,253},
                           {64,59,111,165},
                           {57,79,225,146},
                           {225,184,60,77},
                           {122,71,158,243},
                           {171,33,63,191},
                           {195,178,8,223},
                           {224,142,113,183},
                           {53,128,89,149},
                           {191,7,231,237},
                           {185,84,210,202},
                           {7,37,62,229},
                           {88,162,87,147},
                           {236,66,155,228},
                           {160,44,81,165},
                           {168,166,180,58},
                           {190,98,135,162},
                           {224,179,10,20},
                           {110,18,190,101},
                           {203,91,199,225},
                           {243,242,97,228},
                           {134,247,89,87},
                           {103,51,104,108},
                           {247,226,138,173},
                           {78,177,14,73},
                           {145,3,117,65},
                           {19,114,46,235},
                           {19,236,241,228},
                           {55,158,145,107},
                           {15,58,155,111},
                           {215,147,251,183},
                           {133,51,61,77},
                           {5,47,175,59},
                           {56,245,121,81},
                           {101,199,240,194},
                           {156,176,242,61},
                           {245,54,253,111},
                           {139,187,227,62},
                           {59,199,240,170},
                           {40,75,53,66},
                           {50,29,64,127},
                           {109,206,6,196},
                           {190,223,69,147},
                           {206,245,154,123},
                           {120,125,16,96},
                           {23,174,96,116},
                           {41,216,181,161},
                           {45,104,99,64},
                           {216,170,64,94},
                           {16,105,248,9},
                           {58,158,125,120},
                           {252,147,22,114},
                           {216,37,198,147},
                           {104,137,181,12},
                           {148,120,185,147},
                           {42,126,201,122},
                           {137,39,2,81},
                           {60,123,244,84},
                           {17,129,104,192},
                           {21,20,4,181},
                           {227,230,164,36},
                           {136,221,134,132},
                           {137,26,66,189},
                           {98,127,48,190},
                           {100,164,185,27},
                           {243,166,121,82},
                           {87,231,183,113},
                           {109,120,139,39},
                           {240,76,6,149},
                           {165,132,39,68},
                           {253,208,184,219},
                           {64,190,102,26},
                           {78,241,81,195},
                           {164,137,12,246},
                           {142,99,100,127},
                           {129,60,95,170},
                           {215,158,103,217},
                           {111,240,42,247},
                           {212,89,123,18},
                           {107,102,133,79},
                           {89,177,66,156},
                           {233,24,57,227},
                           {80,120,57,108},



    };

    vector<vector<int>> encoded;
    auto start = chrono::high_resolution_clock::now(); // ��ʼʱ��

    for(int i=0;i<512;i++){
         encoded.push_back(qsc.encode(a[i]));
    }



    auto end = chrono::high_resolution_clock::now(); // ����ʱ��

    chrono::duration<double, milli> elapsed = end - start; // ���㾭����ʱ��

    cout << "������� " << elapsed.count() << " milliseconds." << endl;



    auto start1 = chrono::high_resolution_clock::now(); // ��ʼʱ��

    for(int i=0;i<512;i++){
        vector<vector<int>> received = encoded;
        vector<int> erasures ;
        vector<int> decoded = qsc.decode(received[i], erasures);
        for (int d : decoded) cout << " " << d;
        cout << endl;
    }


    auto end1 = chrono::high_resolution_clock::now(); // ����ʱ��

    chrono::duration<double, milli> elapsed1 = end1 - start1; // ���㾭����ʱ��

    cout << "������� " << elapsed1.count() << " milliseconds." << endl;




    auto start2 = chrono::high_resolution_clock::now(); // ��ʼʱ��
    for(int i = 0; i < 512; i++){
        vector<vector<int>> received1 = encoded;

        vector<int> erasures1 ;
        vector<int> decoded1 = qsc.decode(received1[i], erasures1);

        decoded1[2]=3;

      encoded[i] = qsc.encode(decoded1);

    }

    auto end2 = chrono::high_resolution_clock::now(); // ����ʱ��

    chrono::duration<double, milli> elapsed2 = end2 - start2; // ���㾭����ʱ��

    cout << "���¹��� " << elapsed2.count() << " milliseconds." << endl;



    auto start3 = chrono::high_resolution_clock::now(); // ��ʼʱ��
    for(int i=0;i<512;i++){
        vector<vector<int>> received2 = encoded;
        received2[i][1] = 0;
        vector<int> erasures2 = {1};
        vector<int> decoded2 = qsc.decode(received2[i], erasures2);

    }

    auto end3 = chrono::high_resolution_clock::now(); // ����ʱ��

    chrono::duration<double, milli> elapsed3 = end3 - start3; // ���㾭����ʱ��

    cout << "�ؽ����� " << elapsed3.count() << " milliseconds." << endl;


    cout << "���롢���롢���¡��ؽ���" <<  endl;
    cout << elapsed.count() <<  endl;
    cout << elapsed1.count() <<  endl;
    cout << elapsed2.count() <<  endl;
    cout << elapsed3.count() <<  endl;
    return 0;
}