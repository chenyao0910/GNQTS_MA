#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <math.h>
#include <iomanip>
#include <algorithm>
#include <cstring>
#include <fstream>
#include <filesystem>
#define _GLIBCXX_FULLY_DYNAMIC_STRING 1
#undef _GLIBCXX_DEBUG
#undef _GLIBCXX_DEBUG_PEDANTIC
using namespace std;
using namespace std::filesystem;
#define bits 8
#define nums 4
#define gen 10000
#define particle 10
double delta = 0.00016;
int movingAverage[10][4][8];
int globalMa[4][8];
double betaMatrix[4][8];
int strategy[10][4];
vector<string> date;
vector<double> price;
vector<vector<double> > MA(256);
double localRecord[10];
double globalBest, historyBest = 0;
int historyRecord[4];
int globalRecord[4];
int bestGen, bestExp, trainNum, testNum;
double temp[10];
vector<int> mm;
vector<string> _csv(string s);
string slidingType[31] = {"M2M", "Q2Q", "H2H", "Y2Y", "Y2H", "Y2Q","Q2M", "H2Q", "H2M", "M*", "H*", "Q*","Y2M","YY2YY","YYY2H","YH2H","YH2YH","YY2H","YYY2YH","YYY2YYY","YY2YH","YYY2Y","YH2Y","YYY2Q","YH2M","YH2Q","YYY2M","YY2Y","YY2M","YY2Q","YYY2YY"};
string beginDate;
void M2M(int com)
{
    int beginning = 1;
    for (int i = 1; i < date.size(); i++)
    {
        if (date[i] == beginDate)
        {
            beginning = i;
            break;
        }
    }
    mm.push_back(beginning);
    string temp = date[beginning].substr(5, 2);
    int start = beginning;
    for (start; start < date.size(); start++)
    {
        if (temp != date[start].substr(5, 2))
        {
            temp = date[start].substr(5, 2);
            mm.push_back(start);
        }
    }
}
void initial()
{
    for (int i = 0; i < particle; i++)
    {
        for (int j = 0; j < nums; j++)
        {
            for (int k = 0; k < bits; k++)
            {
                movingAverage[i][j][k] = 0;
                betaMatrix[j][k] = 0.5;
            }
        }
    }
}
void randomize()
{
    double r;
    for (int i = 0; i < particle; i++)
    {
        for (int j = 0; j < nums; j++)
        {
            for (int k = 0; k < bits; k++)
            {
                r = rand();
                r = r / (double)(RAND_MAX);
                if (r > betaMatrix[j][k])
                {
                    movingAverage[i][j][k] = 0;
                }
                else
                {
                    movingAverage[i][j][k] = 1;
                }
            }
        }
    }
}
void calculate()
{
    for (int i = 0; i < particle; i++)
    {
        for (int j = 0; j < nums; j++)
        {
            int count = 0;
            for (int k = 0; k < bits; k++)
            {
                if (movingAverage[i][j][k] == 1)
                {
                    count += pow(2, 7 - k);
                }
                else
                {
                    count += 0;
                }
            }
            count += 1;
            strategy[i][j] = count;
            count = 0;
        }
    }
}
void readFile(int com)
{
    for (int i = 0; i < 256; i++)
    {
        int item = 0;
        string filename = "AAPLSMA.csv";
        // cout << filename << endl;
        ifstream inFile(filename,ios::in);
        if (!inFile)
        {
            cout << "Open failed!" << endl;
            exit(1);
        }
        string line;
        while (getline(inFile, line))
        {
            vector<string> a = _csv(line);
            MA[i].push_back(atof(a[i + 2].c_str()));
            if (i == 0)
            {
                price.push_back(atof(a[1].c_str()));
                date.push_back(a[0]);
            }
        }
    }
}
void compareAndUpdate(int best, int worst, int m, int exp)
{
    if (globalBest == 0)
    {
        for (int i = 0; i < nums; i++)
        {
            for (int j = 0; j < bits; j++)
            {
                betaMatrix[i][j] += 0;
            }
        }
    }
    else
    {
        for (int i = 0; i < nums; i++)
        {
            for (int j = 0; j < bits; j++)
            {
                if (globalMa[i][j] == 1 && movingAverage[worst][i][j] == 0)
                {
                    if (betaMatrix[i][j] < 0.5)
                    {
                        betaMatrix[i][j] = 1 - betaMatrix[i][j];
                    }
                    betaMatrix[i][j] += delta;
                }
                else if (globalMa[i][j] == 0 && movingAverage[worst][i][j] == 1)
                {
                    if (betaMatrix[i][j] > 0.5)
                    {
                        betaMatrix[i][j] = 1 - betaMatrix[i][j];
                    }
                    betaMatrix[i][j] -= delta;
                }
            }
        }
    }
    /*
    if(m == 0 && exp ==0){
        for(int i = 0 ; i < particle;i++){
        for(int j = 0 ; j < nums;j++){
            for(int k = 0 ; k < bits;k++){
                tmpfile << movingAverage[i][j][k] << ",";
            }
            tmpfile <<",";
        }
        tmpfile << temp[i] <<","<<strategy[i][0]<<","<<strategy[i][1]<<","<<strategy[i][2]<<","<<strategy[i][3]<<endl;
    }
        tmpfile << "local Best"<<endl;
        for(int a=0;a<nums;a++){
            for(int b = 0 ; b < bits ;b++){
                tmpfile << globalMa[a][b]<<",";
            }
            tmpfile <<",";
        }
        tmpfile << globalBest<<endl;
        for(int a=0;a<nums;a++){
            for(int b = 0 ; b < bits ;b++){
                tmpfile << movingAverage[worst][a][b]<<",";
            }
            tmpfile <<",";
        }
        tmpfile << localRecord[0]<<endl;
        for(int a=0;a<nums;a++){
            for(int b = 0 ; b < bits ;b++){
                tmpfile << betaMatrix[a][b]<<",";
            }
            tmpfile <<",";
        }
        tmpfile <<endl;
    }
    */
}

void trade(int generation, int exp, string startDate, string endDate, int m)
{
    int begin, end;
    for (int i = 0; i < date.size(); i++)
    {
        if (date[i] == startDate)
        {
            begin = i;
        }
        if (date[i] == endDate)
        {
            end = i;
        }
    }
    for (int i = 0; i < particle; i++)
    {
        double fund = 10000000;
        int flag = 0;
        int share = 0;
        int remain = 0;

        int buy1 = strategy[i][0];
        int buy2 = strategy[i][1];
        int sell1 = strategy[i][2];
        int sell2 = strategy[i][3];
        for (int j = begin; j <= end; j++)
        {
            if (MA[buy1 - 1][j] > MA[buy2 - 1][j] && MA[buy1 - 1][j - 1] <= MA[buy2 - 1][j - 1] && flag == 0)
            {
                share = floor(fund / price[j]);
                remain = (fund - floor(price[j] * share));
                flag = 1;
                // cout << "Buy " <<date[j] << "," << price[j]<<","<< MA[buy1-1][j-1] <<","<<MA[buy2-1][j-1]<<","<<MA[buy1-1][j] <<"," << MA[buy2-1][j] <<"," << share <<"," << remain <<endl;
            }
            else if (MA[sell1 - 1][j] < MA[sell2 - 1][j] && MA[sell1 - 1][j - 1] >= MA[sell2 - 1][j - 1] && flag == 1)
            {
                fund = (share * price[j]) + remain;
                flag = 0;
                share = 0;
                // cout << "Sell " <<date[j] << "," << price[j]<<","<<MA[buy1-1][j-1] <<","<<MA[buy2-1][j-1]<<MA[buy1-1][j] <<"," << MA[buy2-1][j] <<"," <<setprecision(10)<< fund <<endl;
            }
        }
        if (share != 0)
        {
            fund = (share * price[end]) + remain;
            // cout << "Sell " <<date[end] << "," << price[end]<<","<<MA[buy1-1][end-1] <<","<<MA[buy2-1][end-1]<<MA[buy1-1][end] <<"," << MA[buy2-1][end] <<"," <<setprecision(10)<< fund <<endl;
            share = 0;
            flag = 0;
        }
        fund = int(fund);
        localRecord[i] = ((double(fund) - 10000000) / 10000000) * 100;
        // cout << localRecord[i]<<endl;
        // cout <<setprecision(10) <<fund <<endl;
    }
    for (int i = 0; i < particle; i++)
    {
        temp[i] = localRecord[i];
    }
    sort(localRecord, localRecord + 10);
    int best, worst;
    for (int i = 0; i < particle; i++)
    {
        if (temp[i] == localRecord[0])
        {
            worst = i;
            break;
        }
    }
    for (int i = 0; i < particle; i++)
    {
        if (temp[i] == localRecord[9])
        {
            best = i;
            break;
        }
    }
    if (generation == 0)
    {
        globalBest = localRecord[9];
        if (globalBest > historyBest)
        {
            bestGen = 0;
            bestExp = exp + 1;
            for (int i = 0; i < 4; i++)
            {
                globalRecord[i] = strategy[best][i];
            }
        }
        for (int i = 0; i < nums; i++)
        {
            for (int j = 0; j < bits; j++)
            {
                globalMa[i][j] = movingAverage[best][i][j];
            }
        }
    }
    else
    {
        if (localRecord[9] > globalBest)
        {
            globalBest = localRecord[9];
            bestGen = generation;
            bestExp = exp;
            for (int i = 0; i < 4; i++)
            {
                globalRecord[i] = strategy[best][i];
            }
            for (int i = 0; i < nums; i++)
            {
                for (int j = 0; j < bits; j++)
                {
                    globalMa[i][j] = movingAverage[best][i][j];
                }
            }
        }
    }
    compareAndUpdate(best, worst, m, exp);
}
void slidingCase(int slidingNum)
{
    switch (slidingNum)
    {
    case 0: // M2M
        trainNum = 1;
        testNum = 1;
        beginDate = "2011-12-01";
        break;
    case 1: // Q2Q
        trainNum = 3;
        testNum = 3;
        beginDate = "2011-10-03";
        break;
    case 2: // H2H
        trainNum = 6;
        testNum = 6;
        beginDate = "2011-07-01";
        break;
    case 3: // Y2Y
        trainNum = 12;
        testNum = 12;
        beginDate = "2011-01-03";
        break;
    case 4: // Y2H
        trainNum = 12;
        testNum = 6;
        beginDate = "2011-01-03";
        break;
    case 5: // Y2Q
        trainNum = 12;
        testNum = 3;
        beginDate = "2011-01-03";
        break;
    case 6: // Q2M
        trainNum = 3;
        testNum = 1;
        beginDate = "2011-10-03";
        break;
    case 7: // H2Q
        trainNum = 6;
        testNum = 3;
        beginDate = "2011-07-01";
        break;
    case 8: // H2M
        trainNum = 6;
        testNum = 3;
        beginDate = "2011-07-01";
        break;
    case 9: // M*
        trainNum = 1;
        testNum = 1;
        beginDate = "2011-01-03";
        break;
    case 10: // H*
        trainNum = 6;
        testNum = 6;
        beginDate = "2011-01-03";
        break;
    case 11: // Q*
        trainNum = 3;
        testNum = 3;
        beginDate = "2011-01-03";
        break;
    case 12: // Y2M
        trainNum = 12;
        testNum = 1;
        beginDate = "2011-01-03";
        break;
    case 13: // YY2YY
        trainNum = 24;
        testNum = 24;
        beginDate = "2010-01-04";
        break;
    case 14: // YYY2H
        trainNum = 36;
        testNum = 6;
        beginDate = "2009-07-01";
    case 15: //YH2H
        trainNum = 18;
        testNum = 6;
        beginDate = "2013-07-01";
    case 16: //YH2YH
        trainNum = 18;
        testNum = 18;
        beginDate = "2013-07-01";
    case 17: //YY2H
        trainNum = 24;
        testNum = 6;
        beginDate ="2010-01-04";
    case 18: //YYY2YH
        trainNum = 36;
        testNum = 18;
        beginDate = "2009-01-02";
    case 19: //YYY2YYY
        trainNum = 36;
        testNum = 36;
        beginDate = "2009-01-02";
    case 20: //YY2YH
        trainNum = 24;
        testNum = 18;
        beginDate = "2010-01-04";
    case 21: //YYY2Y
        trainNum = 36;
        testNum = 12;
        beginDate = "2009-01-02";
    case 22: //YH2Y
        trainNum = 18;
        testNum = 12;
        beginDate ="2010-07-01";
    case 23: //YYY2Q
        trainNum = 36;
        testNum = 3;
        beginDate = "2009-07-01";
    case 24: //YH2M
        trainNum = 18;
        testNum = 1;
        beginDate = "2010-07-01";
    case 25: //YH2Q
        trainNum = 18;
        testNum = 3;
        beginDate = "2010-07-01";
    case 26: //YYY2M
        trainNum = 36;
        testNum = 1;
        beginDate = "2009-01-02";
    case 27: //YY2Y
        trainNum = 24;
        testNum = 12;
        beginDate = "2010-01-04";
    case 28: //YY2M
        trainNum = 24;
        testNum = 1;
        beginDate = "2010-01-04";
    case 29: //YY2Q
        trainNum = 24;
        testNum = 3;
        beginDate = "2010-01-04";
    case 30: // YYY2YY
        trainNum = 36;
        testNum = 24;
        beginDate = "2009-01-02";
    default:
        break;
    }
}
int main()
{
    
    int historyBestGen, historyBestExp;
    srand(343);
    readFile(0);
    for (int m = 0; m < 8; m++)
    {
        slidingCase(m);
        M2M(0);
        filesystem::create_directory(".//" + slidingType[m]);
        cout << slidingType[m] + "  dir created!" << endl;
        for (int k = 0; k < mm.size() - trainNum; k += testNum)
        {
            ofstream myfile;
            string file = "AAPL_" + slidingType[m] + "_" + to_string(delta) + "_" + date[mm[k]] + "_" + date[mm[k + trainNum] - 1] + ".csv";
            myfile.open(".//"+ slidingType[m] + "//" + file);
            for (int i = 0; i < 50; i++)
            {
                initial();
                for (int j = 0; j < 10000; j++)
                {
                    randomize();
                    calculate();
                    trade(j, i, date[mm[k]], date[mm[k + trainNum] - 1], k);
                    // trade(j, i, "2020-01-02", "2021-06-30", 0);
                }
                // cout << globalRecord[0] << "," << globalRecord[1] << "," << globalRecord[2] << "," << globalRecord[3] << endl;
                // cout << globalBest <<endl;
                if (globalBest > historyBest)
                {
                    historyBest = globalBest;
                    historyBestGen = bestGen;
                    historyBestExp = bestExp;
                    for (int i = 0; i < 4; i++)
                    {
                        historyRecord[i] = globalRecord[i];
                    }
                }
            }
            myfile << "algo"
                << ","
                << "GNQTS" << endl;
            myfile << "exp"
                << "," << 50 << endl;
            myfile << "gen"
                << "," << 10000 << endl;
            myfile << "p amount"
                << "," << 10 << endl;
            myfile << endl;
            myfile << "initial capital"
                << "," << 10000000 << endl;
            myfile << "BestExp"
                << "," << historyBestExp << endl;
            myfile << "BestGen"
                << "," << historyBestGen + 1 << endl;
            myfile << historyRecord[0] << "," << historyRecord[1] << "," << historyRecord[2] << "," << historyRecord[3] << endl;
            myfile << "Return :"
                << "," << historyBest << "%" << endl;
            // cout << historyRecord[0] << "," << historyRecord[1] << "," << historyRecord[2] << "," << historyRecord[3] << endl;
            // cout << "Return :" << "," << historyBest << "%" << endl;
            historyBest = 0, globalBest = 0, bestGen = 0, bestExp = 0;
        }
        mm.clear();
        srand(343);
    }
    return 0;
}

vector<string> _csv(string s)
{
    vector<string> arr;
    istringstream delim(s);
    string token;
    int c = 0;
    while (getline(delim, token, ','))
    {
        arr.push_back(token);
        c++;
    }
    return arr;
}
