#include <iostream>
#include <set>
#include <vector>
#include <utility>
#include <algorithm>
#include <cstring>
#include <string>

class Parser{
public:
	enum class ARG_TYPE{
		MANDATORY = 0,
		OPTIONAL = 1
	};
	void print_help(void){
		std::cout<<"Usage: "<< program_name <<" ";
		for(auto &pair : args_info){
			std::cout<< pair.first;
			if( (ARG_TYPE) pair.second.at(2) == ARG_TYPE::OPTIONAL ){
				std::cout<<"(optional)";
			}
			std::cout<<" args=[" << pair.second.at(0) << "," << pair.second.at(1)<< "] ";
		}
		std::cout<<std::endl;
	}

	int add_opt(std::string opt, int opt_arg_cnt_min, int opt_arg_cnt_max, ARG_TYPE argType = ARG_TYPE::MANDATORY){

		int opt_exist = 0;
		for_each(arg_vec.begin(), arg_vec.end(), [&opt_exist, &opt](auto &arg) \
			{if (arg.at(0) == opt) {opt_exist = 1; return; }});

		if( !opt_exist ){
			args.at(0) = opt;
			arg_vec.push_back(args);
			std::vector<int> info = {opt_arg_cnt_min, opt_arg_cnt_max, (int) argType};
			args_info.push_back(std::make_pair(opt, info));
			return 0;
		}else{
			std::cout<<"Parser error..."<<std::endl<< "Option [" << opt << "] already exist..."<<std::endl;
			return -1;
		}
	}

	int add_opt(std::string opt, int opt_arg_cnt, ARG_TYPE argType = ARG_TYPE::MANDATORY){

		return add_opt(opt, opt_arg_cnt, opt_arg_cnt, argType);
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
		std::string name( *(--args) ); args++;
		program_name = &name[name.find("/") + 1];
		char **arg_end = &args[arg_len - 1];
		for(int i = 0; i < arg_len; ++i){

			auto arg_vec_it = find_opt_it(*args);
			if( arg_vec_it == arg_vec.end()){
				std::cout<<"Argument error..."<<std::endl<<"Arg ["<< *args <<"] not in list.."<<std::endl;
				return -1;
			}

			args++;
			int _arg_cnt = get_arg_cnt(args, arg_len - i - 1);
			for(auto &pair : args_info){
				if( pair.first == arg_vec_it->at(0) ){
					if( (_arg_cnt > pair.second.at(1)) || (_arg_cnt < pair.second.at(0)) ){
						std::cerr<< "Argument error: "<< arg_vec_it->at(0) << std::endl;
						return -1;
					}
				}
			}
			for(;i < arg_len; ++i){
				if( *args[0] == '-' ){
					arg_vec_it->push_back(" ");
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
		for(auto &arg : arg_vec){
			if( arg.size() < 2){
				for(auto &pair : args_info){
					if( pair.first == arg.at(0) ){
						if( (ARG_TYPE) pair.second.at(2) == ARG_TYPE::MANDATORY ){
							std::cout<<"Argument error..."<< std::endl <<"No argument: "<< arg.at(0) <<std::endl;
							print_help();
							return -1;
						}
					}
				}
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

	std::string program_name;
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
	std::vector< std::pair<std::string, std::vector<int> >> args_info;
};

int main(int argc, char **argv)
{
	Parser parser;
	
	if( parser.add_opt("-out", 1, 2) || \
		parser.add_opt("-in", 2) || \
		parser.add_opt("-xxd", 1, 2, Parser::ARG_TYPE::OPTIONAL) == -1) \
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
		}else if( opt == "-xxd" ){
			
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