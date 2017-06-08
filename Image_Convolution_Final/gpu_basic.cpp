#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <CL/cl.hpp>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iterator>
#include "CImg.h"
#include "Filters.h"
#include <sstream>

#define cimg_use_magick
using namespace cimg_library;

int set_filter(int*&, const std::string, float&);
//for images that are to large enough to fit in a single Image2D memory object ( image resolution > 5K)
void large_convolution(CImg<double>&, Filter, cl::CommandQueue&, cl::Program&, cl::Context&); 
//for images that are small enough to fit inside a single Image2D memory object ( image resolution < 5K)
void normal_convolution(CImg<double>&, Filter, cl::CommandQueue&, cl::Program&, cl::Context&);
//single core cpu convolution
void cpu_convolution(CImg<double>&, Filter);

int main(int argc, char* argv[])
{
	//Timer from start of program
	std::chrono::high_resolution_clock::time_point WholeProgramStart = std::chrono::high_resolution_clock::now();
	
	
	//argument checks
	if(argc < 3)
	{
		std::cerr << argc-1 << " arguments provided, atleast 2 expected(relative path to image, GPU/CPU, filter=none, save name=none, display duration = 0, timeWholeProgram = 1)" << std::endl;
		return -1;
	}
		//OpenCL setup
	cl_int status;
	std::vector<cl::Platform> platforms;
	status = cl::Platform::get(&platforms);
	cl_context_properties cps[3] = {CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(),0};
	//cl::Context context(CL_DEVICE_TYPE_GPU, cps);
	cl::Context context;
	
	bool singleCoreCPU = false;
	if(std::string(argv[2]) == "GPU")
	{
		context = cl::Context(CL_DEVICE_TYPE_GPU, cps);
	}
	else if(std::string(argv[2]) == "CPU")
	{
		context = cl::Context(CL_DEVICE_TYPE_CPU, cps);
	}
	else if(std::string(argv[2]) == "CPU-single")
	{
		singleCoreCPU = true;
	}
	else
	{
		std::cerr << argv[2] << " is an incorrect device, accepted devices are GPU, CPU, CPU-single" << std::endl;
		return -1;
	}
	
	std::string imagePath = std::string(argv[1]);
	CImg<double> image(imagePath.c_str());
	//Filter to apply
	Filter filter("no filter");
	if(argc > 3)
	{
		filter.set_filter(argv[3]);
	}
	bool saveFilteredImage = false;
	if(argc > 4)
	{
		saveFilteredImage = true;
	}
	bool displayFilteredImage = false;
	int displayDuration = 0;
	if(argc > 5)
	{
		displayFilteredImage = true;
		displayDuration = std::stoi(std::string(argv[5]));
	}
	bool timeWholeProgram = true;
	if(argc > 6 && argv[6] != 0)	
	{
		timeWholeProgram = false;
	}
	
	
	
	/*****
		Images that are too large to fit in a single Image2D memory object are dividied into
		4 parts which are then independently filtered before being put back together
	*****/
	std::chrono::high_resolution_clock::time_point kernelOnlyStart;
	if(singleCoreCPU)
	{
		kernelOnlyStart = std::chrono::high_resolution_clock::now();
		//std::cout << "CPU-singlecore" << std::endl;
		cpu_convolution(image, filter);
	}
	else
	{
		std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
		cl::CommandQueue queue = cl::CommandQueue(context, devices[0]);
		std::string sourceCode;
		cl::Program::Sources source;
		
		//Load kernel code
		std::ifstream file("gpu_basic.cl");
		if(file.is_open())
		{
			sourceCode = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
			source = cl::Program::Sources(1, std::make_pair(sourceCode.c_str(), sourceCode.length()+1));
		}
		else
		{
			std::cerr << "Unable to load kernel source" << std::endl;
			return 1;
		}
		cl::Program program = cl::Program(context, source);
		program.build(devices);
	
		kernelOnlyStart = std::chrono::high_resolution_clock::now();
		if(image.width() > 4000)
		{
			//std::cout << "large-" << argv[2] << std::endl;
			large_convolution(image, filter, queue, program, context);
		}
		else
		{
			//std::cout << "accelerated-" << argv[2] << std::endl;
			normal_convolution(image, filter, queue, program, context);
		}
	}
	
	//convolution is done, report time
	std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> time_span;
	if(timeWholeProgram)
	{
		time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end - WholeProgramStart);
	}else
	{
		time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end - kernelOnlyStart);
	}
	std::cout << time_span.count() << std::endl; 
	
	if(saveFilteredImage)
	{
		image.save(argv[4]);//TODO: check for valid path
	}		
	if(displayFilteredImage)
	{
		std::stringstream timeConverter;
		timeConverter << time_span.count();
		
		//Display filtered baseImage
		CImgDisplay main_disp(image, std::string("Image: " + std::string(argv[1]) + "		Filter: " + filter.get_name() + "		Time: " + timeConverter.str()).c_str() ,0);
		main_disp.wait(displayDuration*1000);
	}
	//std::cout << std::endl << "CL MAX WIDTH: " << CL_DEVICE_IMAGE2D_MAX_WIDTH << "    CL MAX HEIGHT: " << CL_DEVICE_IMAGE2D_MAX_HEIGHT << std::endl;
	return 0;
}

void normal_convolution(CImg<double>& baseImage, Filter filter, cl::CommandQueue& queue, cl::Program& program, cl::Context& context)
{
	int totalPixels = baseImage.width()*baseImage.height();
	//baseImage as 1D array, RGBA
	cl_float4* imagePixels = new cl_float4[totalPixels]; 
	for(CImg<double>::iterator it = baseImage.begin(); it < baseImage.end()-totalPixels*2; ++it)
	{
		int i = std::distance(baseImage.begin(),it);
		imagePixels[i].s[0] = *it;
		imagePixels[i].s[1] = *(it+totalPixels);
		imagePixels[i].s[2] = *(it+totalPixels*2);
		imagePixels[i].s[3] = 255;
	}
	
	//Create Image2D buffer
	cl::ImageFormat inFormat;
	inFormat.image_channel_data_type = CL_FLOAT;
	inFormat.image_channel_order = CL_RGBA;
	cl::Image2D inputBuffer(context, CL_MEM_READ_ONLY, inFormat, baseImage.width(), baseImage.height());
	
	//Enqueue Image2D buffer
	cl::size_t<3> region;
	region[0] = baseImage.width();
	region[1] = baseImage.height();
	region[2] = 1;
	cl::size_t<3> origin;
	origin[0] = 0;
	origin[1] = 0;
	origin[2] = 0;
	queue.enqueueWriteImage(inputBuffer, CL_TRUE, origin, region, 0, 0, imagePixels, NULL, NULL); 
	
	//Enqueue filter
	cl::Buffer filterBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, filter.get_size()*sizeof(int));
	queue.enqueueWriteBuffer(filterBuffer, CL_TRUE, 0, filter.get_size()*sizeof(int), filter.get_filter());
	
	//Output buffer and array to store data read back from kernel
	cl::Buffer outputBuffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, totalPixels*sizeof(cl_float4));
	cl_float4* newImagePixels = new cl_float4[totalPixels]; 
	
	//Kernel setup
	cl::Kernel kernel(program, "convolution");
	kernel.setArg(0, inputBuffer);
	kernel.setArg(1, baseImage.width());
	kernel.setArg(2, baseImage.height());
	kernel.setArg(3, filterBuffer);
	kernel.setArg(4, filter.get_width());
	kernel.setArg(5, filter.get_factor());
	kernel.setArg(6, outputBuffer);
	
	//Find closest power of 2 for proper enqueuing of work-items
	int x = baseImage.width();
	int pow2X = 2;
	while (x >>= 1) pow2X <<= 1;
	int y = baseImage.height();
	int pow2Y = 2;
	while (y >>= 1) pow2Y <<= 1;
	cl::NDRange global(pow2X,pow2Y);
	cl::NDRange local(16,16);

	//std::cout << "POWX: " << pow2X << "        POWY: " << pow2Y << std::endl;
	
	//Enqueue kernel
	queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, local);
	//Read back filtered imaged from kernel as a 1D array
	queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, totalPixels*sizeof(cl_float4), newImagePixels);
	//Syncs
	queue.finish();
	
	//Recreate baseImage from 1D array to CImg baseImage, assumes new baseImage has same dimensions as the base baseImage
	for(CImg<double>::iterator it = baseImage.begin(); it < baseImage.end()-totalPixels*2; ++it)
	{
		int i = std::distance(baseImage.begin(), it);
		*(it) = newImagePixels[i].s[0];
		*(it+totalPixels) = newImagePixels[i].s[1];
		*(it+totalPixels*2) = newImagePixels[i].s[2];
	}
}


void large_convolution(CImg<double>& baseImage, Filter filter, cl::CommandQueue& queue, cl::Program& program, cl::Context& context)
{
	//upper left
	CImg<double> topLeft = baseImage.get_crop(0,0,baseImage.width()/2+100, baseImage.height()/2+100);
	int totalPixels1 = topLeft.width()*topLeft.height();
	cl_float4* imagePixels1 = new cl_float4[totalPixels1]; 
	for(CImg<double>::iterator it = topLeft.begin(); it < topLeft.end()-totalPixels1*2; ++it)
	{
		int i = std::distance(topLeft.begin(),it);
		imagePixels1[i].s[0] = *it;
		imagePixels1[i].s[1] = *(it+totalPixels1);
		imagePixels1[i].s[2] = *(it+totalPixels1*2);
		imagePixels1[i].s[3] = 255;
	}
	
	//upper right
	CImg<double> topRight = baseImage.get_crop(baseImage.width()/2-100, 0, baseImage.width(), baseImage.height()/2+100); 
	int totalPixels2 = topRight.width()*topRight.height();
	cl_float4* imagePixels2 = new cl_float4[totalPixels2]; 
	for(CImg<double>::iterator it = topRight.begin(); it < topRight.end()-totalPixels2*2; ++it)
	{
		int i = std::distance(topRight.begin(),it);
		imagePixels2[i].s[0] = *it;
		imagePixels2[i].s[1] = *(it+totalPixels2);
		imagePixels2[i].s[2] = *(it+totalPixels2*2);
		imagePixels2[i].s[3] = 255;
	}
	
	//bottom left
	CImg<double> bottomLeft = baseImage.get_crop(0,baseImage.height()/2-100, baseImage.width()/2+100, baseImage.height()); 
	int totalPixels3 = bottomLeft.width()*bottomLeft.height();
	cl_float4* imagePixels3 = new cl_float4[totalPixels3]; 
	for(CImg<double>::iterator it = bottomLeft.begin(); it < bottomLeft.end()-totalPixels3*2; ++it)
	{
		int i = std::distance(bottomLeft.begin(),it);
		imagePixels3[i].s[0] = *it;
		imagePixels3[i].s[1] = *(it+totalPixels3);
		imagePixels3[i].s[2] = *(it+totalPixels3*2);
		imagePixels3[i].s[3] = 255;
	}
	
	//bottom right
	CImg<double> bottomRight = baseImage.get_crop(baseImage.width()/2-100, baseImage.height()/2-100, baseImage.width(), baseImage.height()); 
	int totalPixels4 = bottomRight.width()*bottomRight.height();
	cl_float4* imagePixels4 = new cl_float4[totalPixels4]; 
	for(CImg<double>::iterator it = bottomRight.begin(); it < bottomRight.end()-totalPixels4*2; ++it)
	{
		int i = std::distance(bottomRight.begin(),it);
		imagePixels4[i].s[0] = *it;
		imagePixels4[i].s[1] = *(it+totalPixels4);
		imagePixels4[i].s[2] = *(it+totalPixels4*2);
		imagePixels4[i].s[3] = 255;
	}
	
	//Create Image2D buffers
	cl::ImageFormat inFormat;
	inFormat.image_channel_data_type = CL_FLOAT;
	inFormat.image_channel_order = CL_RGBA;
	cl::Image2D inputBuffer1(context, CL_MEM_READ_ONLY, inFormat, topLeft.width(), topLeft.height());
	cl::Image2D inputBuffer2(context, CL_MEM_READ_ONLY, inFormat, topRight.width(), topRight.height());
	cl::Image2D inputBuffer3(context, CL_MEM_READ_ONLY, inFormat, bottomLeft.width(), bottomLeft.height());
	cl::Image2D inputBuffer4(context, CL_MEM_READ_ONLY, inFormat, bottomRight.width(), bottomRight.height());

	//Enqueue Image2D buffers
	cl::size_t<3> region;
	region[0] = topLeft.width();
	region[1] = topLeft.height();
	region[2] = 1;
	
	cl::size_t<3> origin;
	origin[0] = 0;
	origin[1] = 0;
	origin[2] = 0;
	queue.enqueueWriteImage(inputBuffer1, CL_TRUE, origin, region, 0, 0, imagePixels1, NULL, NULL); 
	
	//Enqueue filter
	cl::Buffer filterBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, filter.get_size()*sizeof(int));
	queue.enqueueWriteBuffer(filterBuffer, CL_TRUE, 0, filter.get_size()*sizeof(int), filter.get_filter());
	
	//Find closest power of 2 for proper enqueuing of work-items
	int x = baseImage.width()/2;
	int pow2X = 2;
	while (x >>= 1) pow2X <<= 1;
	int y = baseImage.height()/2;
	int pow2Y = 2;
	while (y >>= 1) pow2Y <<= 1;
	cl::NDRange global(pow2X,pow2Y);
	cl::NDRange local(32,32);
	
	//Output buffer and array to store data read back from kernel
	cl::Buffer outputBuffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, totalPixels1*sizeof(cl_float4));
	
	//---firstHalf kernel computations
	//1D read back storage for firstHalf 
	cl_float4* newImagePixels1 = new cl_float4[totalPixels1]; 
	
	//Kernel setup
	cl::Kernel kernel(program, "convolution");
	kernel.setArg(0, inputBuffer1);
	kernel.setArg(1, topLeft.width());
	kernel.setArg(2, topLeft.height());
	kernel.setArg(3, filterBuffer);
	kernel.setArg(4, filter.get_width());
	kernel.setArg(5, filter.get_factor());
	kernel.setArg(6, outputBuffer);

	//Enqueue kernel
	queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, local);
	//Read back filtered imaged from kernel as a 1D array
	queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, totalPixels1*sizeof(cl_float4), newImagePixels1);
	//Syncs
	queue.finish();
//---

//---secondHalf kernel computations
	//1D read back storage for secondHalf
	cl_float4* newImagePixels2 = new cl_float4[totalPixels2]; 
	region[0] = topRight.width();
	region[1] = topRight.height();
	queue.enqueueWriteImage(inputBuffer2, CL_TRUE, origin, region, 0, 0, imagePixels2, NULL, NULL); 
	cl::Buffer outputBuffer2 = cl::Buffer(context, CL_MEM_WRITE_ONLY, totalPixels2*sizeof(cl_float4));
	//std::cout << (baseImage.width()/2) << " != " << topRight.width() << "    &    " << (baseImage.height()/2) << " != " << topRight.height() << std::endl; 	
	//Kernel setup
	kernel.setArg(0, inputBuffer2);

	//Enqueue kernel
	queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, local);
	//Read back filtered imaged from kernel as a 1D array
	queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, totalPixels2*sizeof(cl_float4), newImagePixels2);
	//Syncs
	queue.finish();
//---
	
//---
	cl_float4* newImagePixels3 = new cl_float4[totalPixels3]; 
	region[0] = bottomLeft.width();
	region[1] = bottomLeft.height();
	queue.enqueueWriteImage(inputBuffer3, CL_TRUE, origin, region, 0, 0, imagePixels3, NULL, NULL); 
	//Kernel setup
	kernel.setArg(0, inputBuffer3);

	//Enqueue kernel
	queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, local);
	//Read back filtered imaged from kernel as a 1D array
	queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, totalPixels3*sizeof(cl_float4), newImagePixels3);
	//Syncs
	queue.finish();
//--

//--
	cl_float4* newImagePixels4 = new cl_float4[totalPixels4]; 
	region[0] = bottomRight.width();
	region[1] = bottomRight.height();
	queue.enqueueWriteImage(inputBuffer4, CL_TRUE, origin, region, 0, 0, imagePixels4, NULL, NULL); 
	//Kernel setup
	kernel.setArg(0, inputBuffer4);

	//Enqueue kernel
	queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, local);
	//Read back filtered imaged from kernel as a 1D array
	queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, totalPixels4*sizeof(cl_float4), newImagePixels4);
	//Syncs
	queue.finish();
//--
	//Recreate image from 1D array to CImg image, assumes new image has same dimensions as the base image
	//TODO: sätt in pixlarna direkt i base image istället för firstHalf, secondHalf och sen slå ihop
	
	for(CImg<double>::iterator it = topLeft.begin(); it < topLeft.end()-totalPixels1*2; ++it)
	{
		int i = std::distance(topLeft.begin(), it);
		*(it) = newImagePixels1[i].s[0];
		*(it+totalPixels1) = newImagePixels1[i].s[1];
		*(it+totalPixels1*2) = newImagePixels1[i].s[2];
	}
	
	for(CImg<double>::iterator it = topRight.begin(); it < topRight.end()-totalPixels2*2; ++it)
	{
		int i = std::distance(topRight.begin(), it);
		*(it) = newImagePixels2[i].s[0];
		*(it+totalPixels2) = newImagePixels2[i].s[1];
		*(it+totalPixels2*2) = newImagePixels2[i].s[2];
	}
	
	for(CImg<double>::iterator it = bottomLeft.begin(); it < bottomLeft.end()-totalPixels3*2; ++it)
	{
		int i = std::distance(bottomLeft.begin(), it);
		*(it) = newImagePixels3[i].s[0];
		*(it+totalPixels3) = newImagePixels3[i].s[1];
		*(it+totalPixels3*2) = newImagePixels3[i].s[2];
	}
	
	for(CImg<double>::iterator it = bottomRight.begin(); it < bottomRight.end()-totalPixels4*2; ++it)
	{
		int i = std::distance(bottomRight.begin(), it);
		*(it) = newImagePixels4[i].s[0];
		*(it+totalPixels4) = newImagePixels4[i].s[1];
		*(it+totalPixels4*2) = newImagePixels4[i].s[2];
	}

	
	for(int i = 0, x = 0; i < baseImage.width()/2; ++i,++x)
	{
		for(int j = 0, y = 0; j < baseImage.height()/2; ++j, ++y)
		{
			for(int k = 0; k < 3; ++k)
			{
				baseImage(i,j,0,k) = topLeft(x,y,0,k);
			}
		}
	}
	for(int i = baseImage.width()/2, x = 100; i < baseImage.width(); ++i, ++x)
	{
		for(int j = 0, y = 0; j < baseImage.height()/2; ++j, ++y)
		{
			for(int k = 0; k < 3; ++k)
			{
				baseImage(i,j,0,k) = topRight(x,y,0,k);
			}
		}
	}
	for(int i = 0,x = 0; i < baseImage.width()/2; ++i, ++x)
	{
		for(int j = baseImage.height()/2, y = 100; j < baseImage.height(); ++j, ++y)
		{
			for(int k = 0; k < 3; ++k)
			{
				baseImage(i,j,0,k) = bottomLeft(x,y,0,k);
			}
		}
	}
	for(int i = baseImage.width()/2,x = 100; i < baseImage.width(); ++i, ++x)
	{
		for(int j = baseImage.height()/2, y = 100; j < baseImage.height(); ++j, ++y)
		{
			for(int k = 0; k < 3; ++k)
			{
				baseImage(i,j,0,k) = bottomRight(x,y,0,k);
			}
		}
	}
}

void cpu_convolution(CImg<double>& baseImage, Filter inFilter)
{
	double factor = inFilter.get_factor();
	int filterWidth = inFilter.get_width();
	int filterHeight = inFilter.get_width();
	int bias = 0;
	
	int filter[filterWidth][filterHeight];
	for(int x = 0, counter = 0; x < filterWidth; ++x)
	{
		for(int y = 0; y < filterWidth; ++y, ++counter)
		{
			filter[x][y] = inFilter.get_filter()[counter];
		}
	}
	
	
	
	CImg<double> filtered_image(baseImage.width(), baseImage.height(), 1,3,0);

	for(int x = 0; x < baseImage.width(); ++x)
	{
		for(int y = 0; y < baseImage.height(); ++y)
		{
			double red = 0.0;
			double green = 0.0;
			double blue = 0.0;
				
			for(int filterX = 0; filterX < filterWidth; ++filterX)
			{	
				for(int filterY = 0; filterY < filterHeight; ++filterY)
				{
					int imageX = (x - filterWidth / 2 + filterX + baseImage.width()) % baseImage.width();
					int imageY = (y - filterHeight / 2 + filterY + baseImage.height()) % baseImage.height();
					
					red += baseImage(imageX,imageY,0,0) * filter[filterX][filterY];
					green += baseImage(imageX,imageY,0,1) * filter[filterX][filterY];
					blue += baseImage(imageX,imageY,0,2) * filter[filterX][filterY];
				}
			}
			
			filtered_image(x,y,0,0) = std::min(std::max(int(factor * red + bias), 0), 255);
			filtered_image(x,y,0,1) = std::min(std::max(int(factor * green + bias), 0), 255);
			filtered_image(x,y,0,2) = std::min(std::max(int(factor * blue + bias), 0), 255);	
		}
	}	
	baseImage = filtered_image;
}


