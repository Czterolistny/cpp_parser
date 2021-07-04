#ifndef __PARSER_H__
#define __PARSER_H__
#include <string>
#include <vector>

class Parser{
public:

	enum class ARG_TYPE{
		MANDATORY = 0,
		OPTIONAL = 1
	};
	
	void print_help(void) const;

	int add_opt(std::string opt, int opt_arg_cnt_min, int opt_arg_cnt_max, ARG_TYPE argType = ARG_TYPE::MANDATORY);

	int add_opt(std::string opt, int opt_arg_cnt, ARG_TYPE argType = ARG_TYPE::MANDATORY);
	
	int get_arg_cnt(char **args, int arg_len) const;

	int add_args(char **args, int arg_len);

	std::vector< std::vector<std::string> > const & get_parser_ctx() const;

	void print_args() const;

	int get_arg(const std::string opt, std::string &arg, int nmb);

	int get_arg_cnt(std::string &opt);

private:

	typedef std::vector< std::vector<std::string> >::iterator pars_it;
	typedef std::vector< std::pair<std::string, std::vector<int> >> args_info_type; 

	std::string program_name;
	std::vector< std::vector<std::string> > arg_vec;
	std::vector< std::string > args = {""};
	std::vector< std::pair<std::string, std::vector<int> >> args_info;

    pars_it find_opt_it(const char *args);

	int check_args_cnt(pars_it it, args_info_type &args_info, int arg_cnt) const;
    
	int check_all_mandatory_args() const;
};

#endif