#include <iostream>
#include <set>
#include <vector>
#include <utility>
#include <algorithm>
#include <cstring>
#include <string>

class Parser{
public:

	int add_opt(std::string opt, int opt_arg_cnt_min, int opt_arg_cnt_max){

		int opt_exist = 0;
		for_each(arg_vec.begin(), arg_vec.end(), [&opt_exist, &opt](auto &arg) \
			{if (arg.at(0) == opt) {opt_exist = 1; return; }});

		if( !opt_exist ){
			args.at(0) = opt;
			arg_vec.push_back(args);
			std::vector<int> cnt = {opt_arg_cnt_min, opt_arg_cnt_max};
			args_cnt.push_back(std::make_pair(opt, cnt));
			return 0;
		}else{
			std::cout<<"Parser error..."<<std::endl<< "Option [" << opt << "] already exist..."<<std::endl;
			return -1;
		}
	}
	int add_opt(std::string opt, int opt_arg_cnt){

		return add_opt(opt, opt_arg_cnt, opt_arg_cnt);
	}
	
	int get_arg_cnt(char **args, int arg_len){
		for(int i = 0; i < arg_len; ++i){
			if( args[i][0] == '-'){
				return i;
			}
		}
		return arg_len;
	}
	int add_args(char **args, int arg_len){
		
		char **arg_end = &args[arg_len - 1];
		for(int i = 0; i < arg_len; ++i){

			auto arg_vec_it = find_opt_it(*args);
			if( arg_vec_it == arg_vec.end()){
				return -1;
			}
			args++;
			int _arg_cnt = get_arg_cnt(args, arg_len - i - 1);
			for(auto &pair : args_cnt){
				if( pair.first == arg_vec_it->at(0) ){
					if( (_arg_cnt > pair.second.at(1)) || (_arg_cnt < pair.second.at(0)) ){
						std::cerr<< "Argument error: "<< arg_vec_it->at(0) << std::endl << "Required: "\
						 	<< "[min,max] [" << pair.second.at(0) << "," << pair.second.at(1) << "]" <<std::endl;
						return -1;
					}
				}
			}
			for(;i < arg_len; ++i){
				if( *args[0] == '-' ){
					break;
				}
				arg_vec_it->push_back(*args);
				if( args == arg_end ){
					i++;
					break;
				}
				args++;
			}
		}
		return 0;
	}
	std::vector< std::vector<std::string> > & get_parser_ctx(){
		return arg_vec;
	}

	void print_args(){
		for(auto &arg: arg_vec){
			for(auto it = arg.begin(); it != arg.end(); ++it){
				std::cout<< *it <<" ";
			}
			std::cout<<std::endl;

		}
	}
private:
	typedef std::vector< std::vector<std::string> >::iterator pars_it;

	pars_it find_opt_it(char *args){
		for(auto it = arg_vec.begin(); it != arg_vec.end(); ++it){
			if( strcmp( it->at(0).c_str(), args ) == 0 ){
				return it;
			}
		}
		return arg_vec.end();
	}

	std::vector< std::vector<std::string> > arg_vec;
	std::vector< std::string > args = {""};
	std::vector< std::pair<std::string, std::vector<int> >> args_cnt;
};

int main(int argc, char **argv)
{
	Parser parser;
	
	if( parser.add_opt("-out", 1, 2) || \
		parser.add_opt("-out", 2) == -1) \
			return -1;
	
	if( parser.add_args(&argv[1], argc - 1) == -1){
		return -1;
	}

	std::string opt1_arg1, opt2_arg1, opt2_arg2;
	auto parser_process = [&opt1_arg1, &opt2_arg1, &opt2_arg2](auto ctx)
	{
		std::string opt = ctx.at(0);
		if( opt == "-out" ){
			opt1_arg1 = ctx.at(1);

		}else if( opt == "-in" ){
			opt2_arg1 = ctx.at(1);
			opt2_arg2 = ctx.at(2);
		}else{
			std::cout<<"Something gone wrong"<<std::endl;
		}

	 };

	auto parse_ctx = parser.get_parser_ctx();
	for_each(parse_ctx.begin(), parse_ctx.end(), parser_process);

	std::cout<<"-out: "<< opt1_arg1 <<std::endl;
	std::cout<<"-in: "<< opt2_arg1 <<" "<< opt2_arg2 <<std::endl;

return 0;
}