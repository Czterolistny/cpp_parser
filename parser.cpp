#include <iostream>
#include <utility>
#include <algorithm>
#include <cstring>
#include "parser.hpp"

int Parser::add_opt(std::string opt, int opt_arg_cnt, Parser::ARG_TYPE argType){
	return add_opt(opt, opt_arg_cnt, opt_arg_cnt, argType);
}

int Parser::add_opt(std::string opt, int opt_arg_cnt_min, int opt_arg_cnt_max, \
					Parser::ARG_TYPE argType)
{
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

int Parser::add_args(char **args, int arg_len){
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

int Parser::get_arg(const std::string opt, std::string &arg, int nmb){
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

int Parser::get_arg_cnt(std::string &opt){
	auto it = find_opt_it( opt.c_str() );
	return it->size() - 1;
}

int Parser::get_arg_cnt(char **args, int arg_len) const{
	for(int i = 0; i < arg_len; ++i){
		if( args[i][0] == '-'){
			return i;
		}
	}
	return arg_len;
}

Parser::pars_it Parser::find_opt_it(const char *args){
	for(auto it = arg_vec.begin(); it != arg_vec.end(); ++it){
		if( strcmp( it->at(0).c_str(), args ) == 0 ){
			return it;
		}
	}
	return arg_vec.end();
}

int Parser::check_args_cnt(pars_it it, args_info_type &args_info, int arg_cnt) const{
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

int Parser::check_all_mandatory_args() const{
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

std::vector< std::vector<std::string> > const & Parser::get_parser_ctx() const{
	return arg_vec;
}

void Parser::print_help(void) const{
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

void Parser::print_args() const{
	for(auto const &arg: arg_vec){
		for(auto it = arg.begin(); it != arg.end(); ++it){
			std::cout<< *it <<" ";
		}
		std::cout<<std::endl;
	}
}