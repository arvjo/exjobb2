#ifndef FILTERS_H
#define FILTERS_H
#include <string>

class Filter
{
	private:
	int* filter;
	int filterSize;
	int filterWidth;
	float factor;
	std::string name;
	
	public:
	Filter(std::string filterName);
	~Filter();
	void set_filter(std::string filterName);
	int get_size();
	int get_width();
	int*& get_filter();
	float get_factor();
	std::string get_name();
};
#endif