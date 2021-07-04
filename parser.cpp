#include <iostream>
#include <set>
#include <vector>
#include <utility>
#include <algorithm>
#include <cstring>
#include <string>
#include <functional>

class Parser{
public:

	enum class ARG_TYPE{
		MANDATORY = 0,
		OPTIONAL = 1
	};
	
	void print_help(void) const{
		std::cout<<"Usage: "<< program_name <<" ";
		for(auto &pair : args_info){
			std::cout<< pair.first;
			if( (ARG_TYPE) pair.second.at(2) == ARG_TYPE::OPTIONAL ){
				std::cout<<"(optional)";
			}
			std::cout<<" args=[" << pair.second.at(0);
			if( pair.second.at(0) != pair.second.at(1) )
				std::cout << "," << pair.second.at(1);
			std::cout<< "] ";
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
			std::cerr<<"Parser error..."<<"  "<< "[" << opt << "] already exist"<<std::endl;
			return -1;
		}
	}

	int add_opt(std::string opt, int opt_arg_cnt, ARG_TYPE argType = ARG_TYPE::MANDATORY){

		return add_opt(opt, opt_arg_cnt, opt_arg_cnt, argType);
	}
	
	int get_arg_cnt(char **args, int arg_len) const{
		for(int i = 0; i < arg_len; ++i){
			if( args[i][0] == '-'){
				return i;
			}
		}
		return arg_len;
	}
	int add_args(char **args, int arg_len){
		std::string name( *(args++) );
		program_name = &name[name.find_last_of("/") + 1];
		char **arg_end = &args[arg_len - 1];
		for(int i = 0; i < arg_len; ++i){

			auto arg_vec_it = find_opt_it(*args);
			if( arg_vec_it == arg_vec.end()){
				std::cerr<<"Argument error..."<<"   "<<"["<< *args <<"] not in list"<<std::endl;
				print_help();
				return -1;
			}

			args++;
			int _arg_cnt = get_arg_cnt(args, arg_len - i - 1);
			if( check_args_cnt(arg_vec_it, args_info, _arg_cnt) == -1 ){
				print_help();
				return -1;
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
		if( check_all_mandatory_args() == -1){
			print_help();
			return -1;
		}
		return 0;
	}

	std::vector< std::vector<std::string> > const & get_parser_ctx() const{
		return arg_vec;
	}

	void print_args() const{
		for(auto const &arg: arg_vec){
			for(auto it = arg.begin(); it != arg.end(); ++it){
				std::cout<< *it <<" ";
			}
			std::cout<<std::endl;
		}
	}
	int get_arg(const std::string opt, std::string &arg, int nmb){
		if(nmb == 0){
			return -1;
		}
		auto it = find_opt_it(opt.c_str() );
		if( it->size() > nmb ){
			arg = it->at(nmb);
			return 0;
		}else{
			std::cerr<<"get_arg() error..."<<"   "<< opt <<"["<< nmb <<"] out of range"<<std::endl;
			return -1;
		}
	}
	int get_arg_cnt(std::string &opt){
		auto it = find_opt_it( opt.c_str() );
		return it->size() - 1;
	}
private:
	typedef std::vector< std::vector<std::string> >::iterator pars_it;
	typedef std::vector< std::pair<std::string, std::vector<int> >> args_info_type; 

	std::string program_name;
	pars_it find_opt_it(const char *args){
		for(auto it = arg_vec.begin(); it != arg_vec.end(); ++it){
			if( strcmp( it->at(0).c_str(), args ) == 0 ){
				return it;
			}
		}
		return arg_vec.end();
	}
	int check_args_cnt(pars_it it, args_info_type &args_info, int arg_cnt) const{
		for(auto const &pair : args_info){
			if( pair.first == it->at(0) ){
				if( (arg_cnt > pair.second.at(1)) || (arg_cnt < pair.second.at(0)) ){
					std::cerr<< "Argument error..."<<"  "<< it->at(0) \
						 << "["<< arg_cnt << "]" << std::endl;
					return -1;
				}
			}
		}
		return 0;
	}
	int check_all_mandatory_args() const{
		for(auto const &arg : arg_vec){
			if( arg.size() < 2){
				for(auto const &pair : args_info){
					if( pair.first == arg.at(0) ){
						if( (ARG_TYPE) pair.second.at(2) == ARG_TYPE::MANDATORY ){
							std::cerr<<"Argument error..."<<"  "<<"["<< arg.at(0) <<"]"<<" is not given" <<std::endl;
							return -1;
						}
					}
				}
			}
		}
		return 0;	
	}
	std::vector< std::vector<std::string> > arg_vec;
	std::vector< std::string > args = {""};
	std::vector< std::pair<std::string, std::vector<int> >> args_info;
};

int main(int argc, char **argv)
{
	Parser parser;
	
	if( parser.add_opt("-out", 1) || \
		parser.add_opt("-in", 2) || \
		parser.add_opt("-aux", 1, 2, Parser::ARG_TYPE::OPTIONAL) == -1){
			return -1;
		}
	
	if( parser.add_args(&argv[0], argc - 1) == -1){
		return -1;
	}

	std::string opt1_arg1, opt2_arg1, opt2_arg2;
	int arg_err = 0;
	auto parser_process = [&parser, &opt1_arg1, &opt2_arg1, &opt2_arg2, &arg_err](auto ctx)
	{
		std::string opt = ctx.at(0);
		if( opt == "-out" ){
			arg_err += parser.get_arg(opt, opt1_arg1, 1);
		}else if( opt == "-in" ){
			arg_err += parser.get_arg(opt, opt2_arg1, 1);
			arg_err += parser.get_arg(opt, opt2_arg2, 2);
		}else if( opt == "-aux" ){
			int cnt;
			if( (cnt = parser.get_arg_cnt(opt)) ){
				std::cout<< opt <<" arg cnt "<< cnt <<std::endl;
			}
		}else{
			std::cout<<"Something gone wrong"<<std::endl;
		}
	 };
	auto parse_ctx = parser.get_parser_ctx();
	for_each(parse_ctx.begin(), parse_ctx.end(), parser_process);
	if(arg_err < 0){
		return -1;
	}

	std::cout<<"-out: "<< opt1_arg1 <<std::endl;
	std::cout<<"-in: "<< opt2_arg1 <<" "<< opt2_arg2 <<std::endl;

return 0;
}