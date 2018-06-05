#include <iostream>
#include <cmath>
#include <vector>
#include <ctime>
#include <map>

int lower_b,upper_b;
///binary to  decimal
int decimal(std::string num){
    int decimal = 0;
//    printf("size = %d\n",num.size());
    for (int i = 0; i < num.size(); ++i) {
        int power = (int) pow(2, (num.size() - i - 1) );
        char c = num[i];
        ///这里就是char c = 1,2,3,4
        int base = c - 0;
        decimal += base * power;
    }
//    printf("decimal = %d\n",decimal);
    return decimal;

}
///decode chromosome
double decode(int low_bound,int upper_bound,std::string chromosome){
    int deno = static_cast<int>( pow( 2 ,chromosome.size() ) ) - 1;
    double decimal_num = static_cast<double>( decimal(chromosome) );
    double x = static_cast<double>(low_bound)
              + static_cast<double>(decimal_num  * (upper_bound - low_bound) / deno );
    return x;
}
///产生种群
void gene_chromosome(std::vector<std::string>& chromo_vec,int vec_count,int chromo_size){
    srand((unsigned)time(NULL));
    //产生一定数量的染色体
    for (int i = 0; i < vec_count; ++i) {
        std::string chromosome;
        //产生固定长度的染色体
        for (int j = 0; j < chromo_size; ++j) {
            char bit = static_cast<char> (rand() % 2);
            chromosome+=bit;
        }
        chromo_vec.push_back(chromosome);
    }
}
///接下来的演变包括三个部分，选择(select),交叉(crossover),变异(mutation)
///对种群中的染色体进行适应度函数计算，如果是计算函数，也就是让f(x)最大的值
double f_x(std::string chromosome){
    double decoded_number = decode(lower_b,upper_b,chromosome);
    double fx = decoded_number + 10*std::sin(5*decoded_number) + 7*std::cos(4*decoded_number);
    return fx;
}
///选择种群中表现好的,retain_rate，选择多大的适应性强的染色体进行保留.random_select_rate从原本应该淘汰的中选择的比例
void select(std::vector<std::string>& parents_chromo,std::vector<std::string>& chromo_vec,double remain_rate = 0.8,double random_select_rate = 0.5){
    ///传入最后一个参数让map按key降序排列
    std::map< double,std::string,std::greater<double> > chromo_map;
    for (int i = 0; i < chromo_vec.size(); ++i) {
        chromo_map.insert(std::make_pair( f_x(chromo_vec[i]),chromo_vec[i] ) );
    }
    ///只有前面的百分之remain_rate的染色体幸存下来
    int remain_length = static_cast<int>( chromo_map.size() * remain_rate );

    std::map< double,std::string,std::greater<double> >::iterator iter = chromo_map.begin();
    for (int j = 0; j < remain_length; ++j) {
        parents_chromo.push_back(iter->second);
        ++iter;
    }
    for (int k = remain_length ; k < chromo_map.size(); ++k) {
        if( rand() % 100 < (random_select_rate * 100) ){
            parents_chromo.push_back(iter->second);
        }
        ++iter;
    }

}
///对上一步中幸存的parent进行交叉，产生后代
void crossover(std::vector<std::string> parents_chromo,std::vector<std::string>& chromo_vec){
    int needed_size = chromo_vec.size() - parents_chromo.size();
    std::vector<std::string> need_generated;
    while(need_generated.size() < needed_size){
        int father_index = rand() % parents_chromo.size();
        int mother_index = rand() % parents_chromo.size();
        std::string need_chromo;
        if(father_index != mother_index){
            int random_pos = rand() % chromo_vec[0].size();
            need_chromo.insert(need_chromo.end(),parents_chromo[father_index].begin(),parents_chromo[father_index].end() - random_pos);
            need_chromo.insert(need_chromo.end(),parents_chromo[mother_index].begin() + (parents_chromo[mother_index].size() - random_pos),parents_chromo[mother_index].end());
        }
        need_generated.push_back(need_chromo);
    }
    chromo_vec.assign(parents_chromo.begin(),parents_chromo.end());
    chromo_vec.insert(chromo_vec.end(),need_generated.begin(),need_generated.end());
//    printf("generated size = %d\n",chromo_vec.size());
}
void mutation(std::vector<std::string>& chromo_vec,int mutation_rate = 0.01){
    for (int i = 0; i < chromo_vec.size(); ++i) {
        if( rand() % 100 < (mutation_rate * 100) ){
            int random_pos = rand() % chromo_vec[0].size();
            if( chromo_vec[i].at(random_pos) == '0')
                chromo_vec[i].at(random_pos) = '1';
            else if(chromo_vec[i].at(random_pos) == '1')
                chromo_vec[i].at(random_pos) = '0';
        }
    }
}
void evolve(std::vector<std::string>& chromo_vec){
    std::vector<std::string> parents_vec;
    select(parents_vec,chromo_vec);
    crossover(parents_vec,chromo_vec);
    mutation(chromo_vec);
}
double result(std::vector<std::string>& chromo_vec){
    std::map< double,std::string,std::greater<double> > chromo_map;
    for (int i = 0; i < chromo_vec.size(); ++i) {
        chromo_map.insert(std::make_pair( f_x(chromo_vec[i]),chromo_vec[i] ) );
    }
    printf("result = %.4f\n",decode(lower_b,upper_b,chromo_map.begin()->second));
    return decode(lower_b,upper_b,chromo_map.begin()->second);
}
int main() {
    ///例题 f(x) = x + 10*sin(5*x) + 7*cos(4*x)  在区间[0,9]的最大值，函数图像在附件
    ///[0,9] 我们假设求精度为0.001的解,则有 2^11<3000<2^12，所以每个染色体大小为12
    std::vector<std::string> chromo_vec;
    ///种群大小
    int population_count = 300;
    ///每个染色体大小
    int chromosome_size = 12;
    ///上界下界
    lower_b = 0;
    upper_b = 9;
    gene_chromosome(chromo_vec,population_count,chromosome_size);
    ///迭代次数
    int iteration_count = 100;
    for (int i = 0; i < iteration_count; ++i) {
        evolve(chromo_vec);
    }
    double res = result(chromo_vec);
    double fx = res + 10*std::sin(5*res) + 7*std::cos(4*res);
    printf("%.4f\n",fx);
    std::cout << "Hello, World!" << std::endl;
    return 0;
}