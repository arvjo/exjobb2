#include <iostream>
#include "CImg.h"
#include <algorithm>
#include <CL/cl.hpp>
#include "filters.h"
#include <vector>
#include <chrono>


#define cimg_use_magick

using namespace cimg_library;


CImg<double> apply_filter(CImg<double> image, const std::vector<std::vector<int>> filter, double bias = 0);

int main(int argc, char* argv[])
{	
	
	const std::vector<std::vector<int>> filter = horizontal_edge_detect;
	
	std::string folder = "8k/";
	CImg<double> image((folder+argv[1]).c_str());
	
	//CImgDisplay main_disp(image, "Base image",0);
	//CImgDisplay third_disp(apply_filter(image,blur9x9,0), "filtered image",0);
	
	
	std::chrono::high_resolution_clock::time_point start, end;
	std::chrono::duration<double> time_span;
	
	
	//8k
	folder = "8k/";
	image = CImg<double>((folder+argv[1]).c_str());
	start = std::chrono::high_resolution_clock::now();
	CImg<double> filtered8k(apply_filter(image, filter));
	end = std::chrono::high_resolution_clock::now();
	time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
	std::cout << "8k took: " << time_span.count() << "seconds" << std::endl;
	CImgDisplay main_disp(filtered8k, "Base image",0);
	
	/*
	//4k
	folder = "4k/";
	image = CImg<double>((folder+argv[1]).c_str());
	start = std::chrono::high_resolution_clock::now();
	CImg<double> filtered4k(apply_filter(image, filter));
	end = std::chrono::high_resolution_clock::now();
	time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
	std::cout << "4k took: " << time_span.count() << "seconds" << std::endl;
	CImgDisplay main_disp(filtered4k, " image",0);
	*/
	/*
	//1440p
	folder = "1440p/";
	image = CImg<double>((folder+argv[1]).c_str());
	start = std::chrono::system_clock::now();
	CImg<double> filtered4k = apply_filter(image, filter);
	end = std::chrono::system_clock::now();
	elapsed_time = end - start;
	std::cout << "1440p took: " << elapsed_time.count() << "seconds" << std::endl;
	*/
	/*
	//1080p
	folder = "1080p/";
	image = CImg<double>((folder+argv[1]).c_str());
	
	start = std::chrono::high_resolution_clock::now();
	CImg<double> filtered1080p = apply_filter(image, filter);
	end = std::chrono::high_resolution_clock::now();
	time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
	std::cout << "1080p took: " << time_span.count() << "seconds" << std::endl;
	CImgDisplay main_disp(filtered1080p, " image",0);
	/*
	//720p
	folder = "720p/";
	image = CImg<double>((folder+argv[1]).c_str());
	start = std::chrono::system_clock::now();
	CImg<double> filtered1080p = apply_filter(image, filter);
	end = std::chrono::system_clock::now();
	elapsed_time = end - start;
	std::cout << "720p took: " << elapsed_time.count() << "seconds" << std::endl;
	*/
	//480p
	/*folder = "480p/";
	image = CImg<double>((folder+argv[1]).c_str());
	start = std::chrono::system_clock::now();
	CImg<double> filtered480p = apply_filter(image, filter);
	end = std::chrono::system_clock::now();
	elapsed_time = end - start;
	std::cout << "480p took: " << elapsed_time.count() << "seconds" << std::endl;
	*/
	
	while(!main_disp.is_closed())
	{
		main_disp.wait();
	}
}

CImg<double> apply_filter(CImg<double> image, const std::vector<std::vector<int>> filter, double bias )
{
	double factor = 1.0;
	int filterWidth = filter[0].size();
	int filterHeight = filterWidth;
	
	double filterTotal = 0;
	for(int i = 0; i < filterWidth; ++i)
	{
		for(int j = 0; j < filterHeight; ++j)
		{
			filterTotal += filter[i][j];
		}
	}
	if(filterTotal > 1)
	{
		factor = factor / filterTotal;
	}
	
	CImg<double> filtered_image(image.width(), image.height(), 1,3,0);
	//CImgDisplay second_display(filtered_image,"Filtered image", 0);

	for(int x = 0; x < image.width(); ++x)
	{
		for(int y = 0; y < image.height(); ++y)
		{
			double red = 0.0;
			double green = 0.0;
			double blue = 0.0;
				
			for(int filterX = 0; filterX < filterWidth; ++filterX)
			{	
				for(int filterY = 0; filterY < filterHeight; ++filterY)
				{
					int imageX = (x - filterWidth / 2 + filterX + image.width()) % image.width();
					int imageY = (y - filterHeight / 2 + filterY + image.height()) % image.height();
					
					red += image(imageX,imageY,0,0) * filter[filterX][filterY];
					green += image(imageX,imageY,0,1) * filter[filterX][filterY];
					blue += image(imageX,imageY,0,2) * filter[filterX][filterY];
				}
			}
			
			filtered_image(x,y,0,0) = std::min(std::max(int(factor * red + bias), 0), 255);
			filtered_image(x,y,0,1) = std::min(std::max(int(factor * green + bias), 0), 255);
			filtered_image(x,y,0,2) = std::min(std::max(int(factor * blue + bias), 0), 255);	
	
		}
				//second_display.display(filtered_image);
	}	
	
	return filtered_image;
}

