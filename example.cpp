#include <iostream>
#include <algorithm>
#include <string>
#include "parser.hpp"

int main(int argc, char **argv)
{
	Parser parser;
	
	if( parser.add_opt("-out", 1) || \
		parser.add_opt("-in", 2) || \
		parser.add_opt("-aux", 1, 2, Parser::ARG_TYPE::OPTIONAL) == -1){
			return -1;
		}
	
	if( parser.parse(argv, argc) == -1){
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