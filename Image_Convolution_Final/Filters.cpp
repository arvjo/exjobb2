#include "Filters.h"

Filter::Filter(std::string filterName)
{
		set_filter(filterName);
}
Filter::~Filter()
{
	
}
void Filter::set_filter(std::string filterName)
{
	filterSize = 0;
	if(filterName == "box3x3")
	{
		filter = new int[9] { 1, 1, 1,
							  1, 1, 1,
							  1, 1, 1};
		filterWidth = 3;
		filterSize = 9;
		factor = 1.00/9.00 ;
		name = "Box blur 3x3";
	}
	else if(filterName == "box5x5")
	{
		filter = new int[25] { 1, 1, 1, 1, 1,
							  1, 1, 1, 1, 1,
							  1, 1, 1, 1, 1,
							  1, 1, 1, 1, 1,
							  1, 1, 1, 1, 1};
		filterWidth = 5;
		filterSize = 25;
		factor = 1.00/25.00 ;
		name = "Box blur 5x5";
	}
	else if(filterName == "box7x7")
	{
		filter = new int[49]{1, 1, 1, 1, 1, 1, 1,
							 1, 1, 1, 1, 1, 1, 1,
							 1, 1, 1, 1, 1, 1, 1,
							 1, 1, 1, 1, 1, 1, 1,
							 1, 1, 1, 1, 1, 1, 1,
							 1, 1, 1, 1, 1, 1, 1,
							 1, 1, 1, 1, 1, 1, 1};
		filterWidth = 7;
		filterSize = 49;
		factor = 1.00/49.00 ;
		name = "box blur 7x7";;
	}
	else if(filterName == "box9x9")
	{
		filter = new int[81]{1, 1, 1, 1, 1, 1, 1, 1, 1,
							 1, 1, 1, 1, 1, 1, 1, 1, 1,
							 1, 1, 1, 1, 1, 1, 1, 1, 1,
							 1, 1, 1, 1, 1, 1, 1, 1, 1,
							 1, 1, 1, 1, 1, 1, 1, 1, 1,
							 1, 1, 1, 1, 1, 1, 1, 1, 1,
							 1, 1, 1, 1, 1, 1, 1, 1, 1,
							 1, 1, 1, 1, 1, 1, 1, 1, 1,
							 1, 1, 1, 1, 1, 1, 1, 1, 1};
		filterWidth = 9; 
		filterSize = 81;
		factor = 1.00 / 81.00;
		name = "Box blur 9x9";
	}
	else if(filterName == "diag_motion3x3")
	{
		filter = new int[9] { 1, 0, 0,
							  0, 1, 0,
							  0, 0, 1};
		filterWidth = 3;
		filterSize = 9;
		factor = 1.00/3.00 ;
		name = "Diagonal motion blur 3x3";
	}
	else if(filterName == "diag_motion5x5")
	{
		filter = new int[25] {1, 0, 0, 0, 0,
							  0, 1, 0, 0, 0,
							  0, 0, 1, 0, 0,
							  0, 0, 0, 1, 0,
							  0, 0, 0, 0, 1};
		filterWidth = 5;
		filterSize = 25;
		factor = 1.00/5.00 ;
		name = "Diagonal motion blur 5x5";
	}
	else if(filterName == "diag_motion7x7")
	{
		filter = new int[49]{1, 0, 0, 0, 0, 0, 0,
							 0, 1, 0, 0, 0, 0, 0, 
							 0, 0, 1, 0, 0, 0, 0, 
							 0, 0, 0, 1, 0, 0, 0, 
							 0, 0, 0, 0, 1, 0, 0, 
							 0, 0, 0, 0, 0, 1, 0, 
							 0, 0, 0, 0, 0, 0, 1};
		filterWidth = 7;
		filterSize = 49;
		factor = 1.00/7.00 ;
		name = "Diagonal motion blur 7x7";;
	}
	else if(filterName == "sharpen3x3")
	{
		filter = new int[9] { -1,-1,-1,
							  -1, 9,-1,
							  -1,-1,-1};
		filterWidth = 3;
		filterSize = 9;
		factor = 1.00;
		name = "Sharpen 3x3";
	}
	else if(filterName == "sharpen5x5")
	{
		filter = new int[25] {-1,-1,-1,-1,-1,
							  -1,-1,-1,-1,-1,
							  -1,-1,25,-1,-1,
							  -1,-1,-1,-1,-1,
							  -1,-1,-1,-1,-1,};
		filterWidth = 5;
		filterSize = 25;
		factor = 1.00;
		name = "Sharpen 5x5";
	}
	else if(filterName == "sharpen7x7")
	{
		filter = new int[49]{-1,-1,-1,-1,-1,-1,-1,
							 -1,-1,-1,-1,-1,-1,-1,
							 -1,-1,-1,-1,-1,-1,-1,
							 -1,-1,-1,49,-1,-1,-1,
							 -1,-1,-1,-1,-1,-1,-1,
							 -1,-1,-1,-1,-1,-1,-1,
							 -1,-1,-1,-1,-1,-1,-1};
		filterWidth = 7;
		filterSize = 49;
		factor = 1.00;
		name = "sharpen 7x7";;
	}
	else if(filterName == "sharpen9x9")
	{
		filter = new int[81]{-1,-1,-1,-1,-1,-1,-1,-1,-1,
							 -1,-1,-1,-1,-1,-1,-1,-1,-1,
							 -1,-1,-1,-1,-1,-1,-1,-1,-1,
							 -1,-1,-1,-1,-1,-1,-1,-1,-1,
							 -1,-1,-1,-1,81,-1,-1,-1,-1,
							 -1,-1,-1,-1,-1,-1,-1,-1,-1,
							 -1,-1,-1,-1,-1,-1,-1,-1,-1,
							 -1,-1,-1,-1,-1,-1,-1,-1,-1,
							 -1,-1,-1,-1,-1,-1,-1,-1,-1};
		filterWidth = 9;
		filterSize = 81;
		factor = 1.00;
		name = "Sharpen 9x9";
	}
	else if(filterName == "sharpen11x11")
	{
		filter = new int[122]{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							 -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							 -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							 -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							 -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							 -1,-1,-1,-1,-1,122,-1,-1,-1,-1,-1,
							 -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							 -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							 -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							 -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							 -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
		filterWidth = 11;
		filterSize = 122;
		factor = 1.00;
		name = "sharpen 11x11";
	}
	else if(filterName == "sharpen13x13")
	{
		filter = new int[169]{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							  -1,-1,-1,-1,-1,-1,169,-1,-1,-1,-1,-1,-1,
							  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
		filterWidth = 13;
		filterSize = 169;
		factor = 1.00;
		name = "sharpen 13x13";
	}
	else if(filterName == "sharpen15x15")
	{
		filter = new int[225]{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							  -1,-1,-1,-1,-1,-1,-1,225,-1,-1,-1,-1,-1,-1,-1,
							  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1-1,-1};
		filterWidth = 15;
		filterSize = 225;
		factor = 1.00;
		name = "sharpen	15x15";
	}
	else if(filterName == "hor_edge_detect3x3")
	{
		filter = new int[9]{ 0, 0, 0, 
							-1, 2, -1, 
							 0, 0, 0};
		filterWidth = 3;
		filterSize = 9;
		factor = 1.00;
		name = "Horizontal edge detection 3x3";
	}
	else if(filterName == "hor_edge_detect5x5")
	{
		filter = new int[25]{
							 0, 0, 0, 0, 0,
							 0, 0, 0, 0, 0, 
							-1,-1, 2, 0, 0,
							 0, 0, 0, 0, 0,
							 0, 0, 0, 0, 0};
		filterWidth = 5;
		filterSize = 25;
		factor = 1.00;
		name = "Horizontal edge detection 5x5";
	}
	else if(filterName == "hor_edge_detect7x7")
	{
		filter = new int[49]{0, 0, 0, 0, 0, 0, 0,
							 0, 0, 0, 0, 0, 0, 0, 
							 0, 0, 0, 0, 0, 0, 0, 
							-1,-1,-1, 3, 0, 0, 0, 
							 0, 0, 0, 0, 0, 0, 0, 
							 0, 0, 0, 0, 0, 0, 0, 
							 0, 0, 0, 0, 0, 0, 0,};
		filterWidth = 7; 
		filterSize = 49;
		factor = 1.00;
		name = "Horizontal edge detection 7x7";
	}
	else if(filterName == "hor_edge_detect9x9")
	{
		filter = new int[81]{0, 0, 0, 0, 0, 0, 0, 0, 0,
							 0, 0, 0, 0, 0, 0, 0, 0, 0,
							 0, 0, 0, 0, 0, 0, 0, 0, 0,
							 0, 0, 0, 0, 0, 0, 0, 0, 0,
							-1,-1,-1,-1, 8, -1, -1, -1, -1,
							 0, 0, 0, 0, 0, 0, 0, 0, 0,
							 0, 0, 0, 0, 0, 0, 0, 0, 0,
							 0, 0, 0, 0, 0, 0, 0, 0, 0,
							 0, 0, 0, 0, 0, 0, 0, 0, 0};
		filterWidth = 9; 
		filterSize = 81;
		factor = 1.00;
		name = "Horizontal edge detection 9x9";
	}
	else
	{
		filter = new int[9] { 0, 0, 0,
							  0, 1, 0,
							  0, 0, 0};
		filterWidth = 3;
		filterSize = 9;
		factor = 1.00;; 
		name = "No Filter";
	}
}

int Filter::get_width()
{
	return filterWidth;
}
int Filter::get_size()
{
	return filterSize;
}
int*& Filter::get_filter()
{
	return filter;
}
float Filter::get_factor()
{
	return factor;
}
std::string Filter::get_name()
{
	return name;
}