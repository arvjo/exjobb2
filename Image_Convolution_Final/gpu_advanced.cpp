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
std::chrono::duration<double> large_convolution_optimized(CImg<double>&, Filter, cl::CommandQueue&, cl::Program&, cl::Context&, bool); 
//for images that are small enough to fit inside a single Image2D memory object ( image resolution < 5K)
std::chrono::duration<double> normal_convolution_optimized(CImg<double>&, Filter, cl::CommandQueue&, cl::Program&, cl::Context&, bool);
//for images that are to large enough to fit in a single Image2D memory object ( image resolution > 5K)
std::chrono::duration<double> large_convolution_basic(CImg<double>&, Filter, cl::CommandQueue&, cl::Program&, cl::Context&, bool); 
//for images that are small enough to fit inside a single Image2D memory object ( image resolution < 5K)
std::chrono::duration<double> normal_convolution_basic(CImg<double>&, Filter, cl::CommandQueue&, cl::Program&, cl::Context&, bool);
//single core cpu convolution
std::chrono::duration<double> cpu_convolution(CImg<double>&, Filter);

int main(int argc, char* argv[])
{
	std::chrono::duration<double> setup;
	std::chrono::high_resolution_clock::time_point setupTimerStart = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point setupTimerEnd;
	//argument checks
	if(argc < 4)
	{
		std::cerr << argc-1 << " arguments provided, atleast 3 expected(relative path to image, GPU/CPU, Basic/Optimized, filter=none, save name=none, display duration = 0, timeWholeProgram = 1)" << std::endl;
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
	bool multiCoreCPU = false;
	if(std::string(argv[2]) == "GPU")
	{
		context = cl::Context(CL_DEVICE_TYPE_GPU, cps);
	}
	else if(std::string(argv[2]) == "CPU")
	{
		context = cl::Context(CL_DEVICE_TYPE_CPU, cps);
		multiCoreCPU = true;
	}
	else if(std::string(argv[2]) == "CPU-single")
	{
		singleCoreCPU = true;
	}
	else
	{
		std::cerr << argv[2] << " is an incorrect device, accepted devices are GPU, CPU, CPU-single" << std::endl;
		return -2;
	}
	
	bool optimized = false;
	if(std::string(argv[3]) == "Basic" || std::string(argv[3]) == "Optimized") 
	{
		if(std::string(argv[3]) == "Optimized")
			optimized = true;		
	}
	else
	{
		std::cerr << argv[3] << " is an invalid type, valid types are Basic or Optimized" << std::endl;
		return -3;
	}
	
	
	std::string imagePath = std::string(argv[1]);
	CImg<double> image(imagePath.c_str());
	//Filter to apply
	Filter filter("no filter");
	if(argc > 4)
	{
		filter.set_filter(argv[4]);
	}
	bool saveFilteredImage = false;
	if(argc > 5 && !(std::string(argv[5]) == "0"))
	{
		saveFilteredImage = true;
	}
	bool displayFilteredImage = false;
	int displayDuration = 0;
	if(argc > 6 && !(std::string(argv[6]) == "0"))
	{
		displayFilteredImage = true;
		displayDuration = std::stoi(std::string(argv[6]));
	}
	bool timeWholeProgram = true;
	if(argc > 7 && argv[7] != 0)	
	{
		timeWholeProgram = false;
	}
	
	
	
	/*****
		Images that are too large to fit in a single Image2D memory object are dividied into
		4 parts which are then independently filtered before being put back together
	*****/
	std::chrono::duration<double> benchTime;
	
	if(singleCoreCPU)
	{
		//std::cout << "\n running sequential cpu" << std::endl;
		benchTime = cpu_convolution(image, filter);
	}
	else
	{
		std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
		cl::CommandQueue queue = cl::CommandQueue(context, devices[0],CL_QUEUE_PROFILING_ENABLE); //La till profiling_enabled flagga
		std::string sourceCode;
		cl::Program::Sources source;
		
		std::ifstream file;
		if(optimized)
		{
			if(multiCoreCPU)
			{
				file.open("gpu_basic.cl");
			}
			else
			{
				file.open("gpu_advanced.cl");
			}
		}
		else
		{
				file.open("gpu_basic.cl");
		}
		
		//Load kernel code
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
		
			
			
		
		setupTimerEnd= std::chrono::high_resolution_clock::now();
		setup = std::chrono::duration_cast<std::chrono::duration<double>>(setupTimerEnd - setupTimerStart);
		std::cout << "setup :" << setup.count() << std::endl;
	
		if(optimized)
		{
			if(image.width() > 4000)
			{
				//std::cout << "\n running optimized large" << std::endl;
				benchTime = large_convolution_optimized(image, filter, queue, program, context, timeWholeProgram);
			}
			else
			{
				//std::cout << "\n running optimized normal" << std::endl;
				benchTime = normal_convolution_optimized(image, filter, queue, program, context, timeWholeProgram);
			}
		}
		else
		{
			if(image.width() > 4000)
			{
				//std::cout << "\n running basic large" << std::endl;
				benchTime = large_convolution_basic(image, filter, queue, program, context, timeWholeProgram);
			}
			else
			{
				//std::cout << "\n running basic normal" << std::endl;
				benchTime = normal_convolution_basic(image, filter, queue, program, context, timeWholeProgram);
			}
		}
		
	}
	
	if(saveFilteredImage)
	{
		image.save(argv[5]);//TODO: check for valid path
	}		
	if(displayFilteredImage)
	{
		//std::stringstream timeConverter;
		//timeConverter << time_span.count();
		
		//Display filtered baseImage
		CImgDisplay main_disp(image, std::string("Image: " + std::string(argv[1]) + "		Filter: " + filter.get_name()).c_str() ,0);
		main_disp.wait(displayDuration*1000);
	}
	
	std::cout << benchTime.count() << std::endl;
	//std::cout << std::endl << "CL MAX WIDTH: " << CL_DEVICE_IMAGE2D_MAX_WIDTH << "    CL MAX HEIGHT: " << CL_DEVICE_IMAGE2D_MAX_HEIGHT << std::endl;
	return 0;
}

std::chrono::duration<double> normal_convolution_optimized(CImg<double>& baseImage, Filter filter, cl::CommandQueue& queue, cl::Program& program, cl::Context& context, bool timeKernelOnly)
{
	std::chrono::high_resolution_clock::time_point benchTimerStart;
	std::chrono::high_resolution_clock::time_point benchTimerEnd;
	if(!timeKernelOnly)
	{
		benchTimerStart = std::chrono::high_resolution_clock::now();
	}
		
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
	
	
	cl_int error;
	//Create Image2D buffer
	cl::ImageFormat inFormat;
	inFormat.image_channel_data_type = CL_FLOAT;
	inFormat.image_channel_order = CL_RGBA;
	
	cl::Event write1, read1, unmap;
	cl_ulong write_start1, write_end1, read_start1, read_end1, unmap_start, unmap_end;
	
	//cl::Image2D inputBuffer(context, CL_MEM_READ_ONLY, inFormat, baseImage.width(), baseImage.height());
	cl::Image2D inputBuffer(context, CL_MEM_USE_HOST_PTR , inFormat, baseImage.width(), baseImage.height(),0,imagePixels, &error);

	//Enqueue Image2D buffer
	cl::size_t<3> region;
	region[0] = baseImage.width();
	region[1] = baseImage.height();
	region[2] = 1;
	cl::size_t<3> origin;
	origin[0] = 0;
	origin[1] = 0;
	origin[2] = 0;
	
	size_t tmp3 = totalPixels*sizeof(cl_float4);
	
	void* tmp = queue.enqueueMapImage(inputBuffer, CL_TRUE, CL_MAP_WRITE, origin, region,&tmp3 ,0,NULL,&write1, &error);
	error = queue.enqueueUnmapMemObject(inputBuffer, tmp, NULL, &unmap);
	
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

	if(timeKernelOnly)
	{
		benchTimerStart = std::chrono::high_resolution_clock::now();
	}
	cl::Event first;
	cl_ulong time_start, time_end;
	//Enqueue kernel
	queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, local, NULL, &first);
	//Read back filtered imaged from kernel as a 1D array
	queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, totalPixels*sizeof(cl_float4), newImagePixels, NULL, &read1);
	//Syncs
	queue.finish();


	if(timeKernelOnly)
	{
		benchTimerEnd = std::chrono::high_resolution_clock::now();
	}
	first.getProfilingInfo(CL_PROFILING_COMMAND_START,&time_start);
	first.getProfilingInfo(CL_PROFILING_COMMAND_END, &time_end);
	cl_ulong kernel_time1 = time_end - time_start;
	
	write1.getProfilingInfo(CL_PROFILING_COMMAND_START,&write_start1);
	write1.getProfilingInfo(CL_PROFILING_COMMAND_END, &write_end1);
	cl_ulong write_time1 = write_end1 - write_start1;
	
	read1.getProfilingInfo(CL_PROFILING_COMMAND_START,&read_start1);
	read1.getProfilingInfo(CL_PROFILING_COMMAND_END, &read_end1);
	cl_ulong read_time1 = read_end1 - read_start1;
	
	unmap.getProfilingInfo(CL_PROFILING_COMMAND_START,&unmap_start);
	unmap.getProfilingInfo(CL_PROFILING_COMMAND_END, &unmap_end);
	cl_ulong unmap_time = unmap_end - unmap_start;
	
	std::cout << "Kernel tid: " << (kernel_time1)/(float)10e8 << std::endl;
	std::cout << "Transfer tid: " << (write_time1 + read_time1)/(float)10e8 << std::endl;
	std::cout << "Unmap tid: " << (unmap_time)/(float)10e8 << std::endl;
	std::cout << "Total tid: " << (kernel_time1 + write_time1 + read_time1 + unmap_time)/(float)10e8 << std::endl;
	
	//Recreate baseImage from 1D array to CImg baseImage, assumes new baseImage has same dimensions as the base baseImage
	for(CImg<double>::iterator it = baseImage.begin(); it < baseImage.end()-totalPixels*2; ++it)
	{
		int i = std::distance(baseImage.begin(), it);
		*(it) = newImagePixels[i].s[0];
		*(it+totalPixels) = newImagePixels[i].s[1];
		*(it+totalPixels*2) = newImagePixels[i].s[2];
	}
	if(!timeKernelOnly)
	{
		benchTimerEnd = std::chrono::high_resolution_clock::now();
	}
	
	return std::chrono::duration_cast<std::chrono::duration<double>>(benchTimerEnd - benchTimerStart);
}

std::chrono::duration<double> large_convolution_optimized(CImg<double>& baseImage, Filter filter, cl::CommandQueue& queue, cl::Program& program, cl::Context& context, bool timeKernelOnly)
{
	std::chrono::high_resolution_clock::time_point benchTimerStart;
	std::chrono::high_resolution_clock::time_point benchTimerEnd;
	if(!timeKernelOnly)
	{
		benchTimerStart = std::chrono::high_resolution_clock::now();
	}
	
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
	cl::Image2D inputBuffer1(context, CL_MEM_USE_HOST_PTR, inFormat, topLeft.width(), topLeft.height(),0,imagePixels1);
	cl::Image2D inputBuffer2(context, CL_MEM_USE_HOST_PTR, inFormat, topRight.width(), topRight.height(),0,imagePixels2);
	cl::Image2D inputBuffer3(context, CL_MEM_USE_HOST_PTR, inFormat, bottomLeft.width(), bottomLeft.height(),0,imagePixels3);
	cl::Image2D inputBuffer4(context, CL_MEM_USE_HOST_PTR, inFormat, bottomRight.width(), bottomRight.height(),0,imagePixels4);

	//Enqueue Image2D buffers
	cl::size_t<3> region;
	region[0] = topLeft.width();
	region[1] = topLeft.height();
	region[2] = 1;
	
	cl::size_t<3> origin;
	origin[0] = 0;
	origin[1] = 0;
	origin[2] = 0;

	cl::Event write1, read1, write2, read2, write3, read3, write4, read4, unmap1, unmap2, unmap3, unmap4, first, second, third, fourth;
	cl_ulong write_start1, write_end1, write_start2, write_end2, write_start3, write_end3, write_start4, write_end4, read_start1, read_end1, read_start2, read_end2,read_start3, read_end3, read_start4, read_end4, unmap_start, unmap_end, unmap_start2, unmap_end2, unmap_start3, unmap_end3 ,unmap_start4, unmap_end4;
	cl_ulong time_start, time_end, time_start2, time_end2, time_start3, time_end3, time_start4, time_end4;
	
	size_t imageSize = totalPixels1*sizeof(cl_float4);
	void *tmpMap1 = queue.enqueueMapImage(inputBuffer1, CL_TRUE, CL_MAP_WRITE | CL_MAP_READ, origin, region, &imageSize,0,NULL, &write1);
	queue.enqueueUnmapMemObject(inputBuffer1, tmpMap1, NULL, &unmap1);
	
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
	cl::NDRange local(16,16);
	
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

	
	if(timeKernelOnly)
	{
		benchTimerEnd = std::chrono::high_resolution_clock::now();
	}
	
	//Enqueue kernel
	queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, local, NULL, &first);
	//Read back filtered imaged from kernel as a 1D array
	queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, totalPixels1*sizeof(cl_float4), newImagePixels1, NULL, &read1);
	//Syncs
	queue.finish();
	first.getProfilingInfo(CL_PROFILING_COMMAND_START,&time_start);
	first.getProfilingInfo(CL_PROFILING_COMMAND_END, &time_end);

	cl_ulong time1 = time_end - time_start;
	
	write1.getProfilingInfo(CL_PROFILING_COMMAND_START,&write_start1);
	write1.getProfilingInfo(CL_PROFILING_COMMAND_END, &write_end1);
	cl_ulong write_time1 = write_end1 - write_start1;	
	//std::cout << "read 1 : " << (write_time1)/(float)10e8 << std::endl;
	
	read1.getProfilingInfo(CL_PROFILING_COMMAND_START,&read_start1);
	read1.getProfilingInfo(CL_PROFILING_COMMAND_END, &read_end1);
	cl_ulong read_time1 = read_end1 - read_start1;
	//std::cout << "read 1 : " << (read_time1)/(float)10e8 << std::endl;
	
	unmap1.getProfilingInfo(CL_PROFILING_COMMAND_START,&unmap_start);
	unmap1.getProfilingInfo(CL_PROFILING_COMMAND_END, &unmap_end);
	cl_ulong unmap_time1 = unmap_end - unmap_start;
	
	
//---

//---secondHalf kernel computations
	//1D read back storage for secondHalf
	cl_float4* newImagePixels2 = new cl_float4[totalPixels2]; 
	region[0] = topRight.width();
	region[1] = topRight.height();
	
	imageSize = totalPixels2*sizeof(cl_float4);
	void *tmpMap2 = queue.enqueueMapImage(inputBuffer2, CL_TRUE, CL_MAP_WRITE | CL_MAP_READ, origin, region, &imageSize,0,NULL,&read2);
	queue.enqueueUnmapMemObject(inputBuffer2, tmpMap2, NULL, &unmap2);
	
	//queue.enqueueWriteImage(inputBuffer2, CL_TRUE, origin, region, 0, 0, imagePixels2, NULL, NULL); 
	cl::Buffer outputBuffer2 = cl::Buffer(context, CL_MEM_WRITE_ONLY, totalPixels2*sizeof(cl_float4));
	//std::cout << (baseImage.width()/2) << " != " << topRight.width() << "    &    " << (baseImage.height()/2) << " != " << topRight.height() << std::endl; 	
	//Kernel setup
	kernel.setArg(0, inputBuffer2);

	//Enqueue kernel
	queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, local, NULL, &second);
	//Read back filtered imaged from kernel as a 1D array
	queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, totalPixels2*sizeof(cl_float4), newImagePixels2,NULL, &read2);
	//Syncs
	queue.finish();
	second.getProfilingInfo(CL_PROFILING_COMMAND_START,&time_start2);
	second.getProfilingInfo(CL_PROFILING_COMMAND_END, &time_end2);

	cl_ulong time2 = time_end2 - time_start2;
	
	write2.getProfilingInfo(CL_PROFILING_COMMAND_START,&write_start2);
	write2.getProfilingInfo(CL_PROFILING_COMMAND_END, &write_end2);
	cl_ulong write_time2 = write_end2 - write_start2;	
	std::cout << "write 2: " << (write_start2)/(float)10e8 << std::endl;
	
	read2.getProfilingInfo(CL_PROFILING_COMMAND_START,&read_start2);
	read2.getProfilingInfo(CL_PROFILING_COMMAND_END, &read_end2);
	cl_ulong read_time2 = read_end2 - read_start2;
	//std::cout << "read 1 : " << (read_time1)/(float)10e8 << std::endl;
	
	unmap2.getProfilingInfo(CL_PROFILING_COMMAND_START,&unmap_start2);
	unmap2.getProfilingInfo(CL_PROFILING_COMMAND_END, &unmap_end2);
	cl_ulong unmap_time2 = unmap_end2 - unmap_start2;
//---
	
//---
	cl_float4* newImagePixels3 = new cl_float4[totalPixels3]; 
	region[0] = bottomLeft.width();
	region[1] = bottomLeft.height();
	
	imageSize = totalPixels3*sizeof(cl_float4);
	void *tmpMap3 = queue.enqueueMapImage(inputBuffer3, CL_TRUE, CL_MAP_WRITE | CL_MAP_READ, origin, region, &imageSize,0,NULL,&write3);
	queue.enqueueUnmapMemObject(inputBuffer3, tmpMap3, NULL, &unmap3);
	//queue.enqueueWriteImage(inputBuffer3, CL_TRUE, origin, region, 0, 0, imagePixels3, NULL, NULL); 
	//Kernel setup
	kernel.setArg(0, inputBuffer3);

	//Enqueue kernel
	queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, local, NULL, &third);
	//Read back filtered imaged from kernel as a 1D array
	queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, totalPixels3*sizeof(cl_float4), newImagePixels3, NULL, &read3);
	//Syncs
	queue.finish();
	third.getProfilingInfo(CL_PROFILING_COMMAND_START,&time_start3);
	third.getProfilingInfo(CL_PROFILING_COMMAND_END, &time_end3);

	cl_ulong time3 = time_end3 - time_start3;
	
	write3.getProfilingInfo(CL_PROFILING_COMMAND_START,&write_start3);
	write3.getProfilingInfo(CL_PROFILING_COMMAND_END, &write_end3);
	cl_ulong write_time3 = write_end3 - write_start3;	
	//std::cout << "write_time3 : " << (write_time3)/(float)10e8 << std::endl;
	
	read3.getProfilingInfo(CL_PROFILING_COMMAND_START,&read_start3);
	read3.getProfilingInfo(CL_PROFILING_COMMAND_END, &read_end3);
	cl_ulong read_time3 = read_end3 - read_start3;
	//std::cout << "read 1 : " << (read_time1)/(float)10e8 << std::endl;
	
	unmap3.getProfilingInfo(CL_PROFILING_COMMAND_START,&unmap_start3);
	unmap3.getProfilingInfo(CL_PROFILING_COMMAND_END, &unmap_end3);
	cl_ulong unmap_time3 = unmap_end3 - unmap_start3;
//--

//--
	cl_float4* newImagePixels4 = new cl_float4[totalPixels4]; 
	region[0] = bottomRight.width();
	region[1] = bottomRight.height();
	
	imageSize = totalPixels4*sizeof(cl_float4);
	void *tmpMap4 = queue.enqueueMapImage(inputBuffer4, CL_TRUE, CL_MAP_WRITE | CL_MAP_READ, origin, region, &imageSize,0,NULL, &write4);
	queue.enqueueUnmapMemObject(inputBuffer4, tmpMap4, NULL, &unmap4);
	//queue.enqueueWriteImage(inputBuffer4, CL_TRUE, origin, region, 0, 0, imagePixels4, NULL, NULL); 
	//Kernel setup
	kernel.setArg(0, inputBuffer4);

	//Enqueue kernel
	queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, local, NULL, &fourth);
	//Read back filtered imaged from kernel as a 1D array
	queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, totalPixels4*sizeof(cl_float4), newImagePixels4,NULL, &read4);
	//Syncs
	queue.finish();
	fourth.getProfilingInfo(CL_PROFILING_COMMAND_START,&time_start4);
	fourth.getProfilingInfo(CL_PROFILING_COMMAND_END, &time_end4);
	cl_ulong time4 = time_end4 - time_start4;
	
	write4.getProfilingInfo(CL_PROFILING_COMMAND_START,&write_start4);
	write4.getProfilingInfo(CL_PROFILING_COMMAND_END, &write_end4);
	cl_ulong write_time4 = write_end4 - write_start4;	
	//std::cout << "read 1 : " << (write_time1)/(float)10e8 << std::endl;
	
	read4.getProfilingInfo(CL_PROFILING_COMMAND_START,&read_start4);
	read4.getProfilingInfo(CL_PROFILING_COMMAND_END, &read_end4);
	cl_ulong read_time4 = read_end4 - read_start4;
	//std::cout << "read 1 : " << (read_time1)/(float)10e8 << std::endl;
	
	unmap2.getProfilingInfo(CL_PROFILING_COMMAND_START,&unmap_start4);
	unmap2.getProfilingInfo(CL_PROFILING_COMMAND_END, &unmap_end4);
	cl_ulong unmap_time4 = unmap_end4 - unmap_start4;
	
	//std::cout << "Kernel 1 : " << (time1)/(float)10e8 << std::endl;
	//std::cout << "Kernel 2 : " << (time2)/(float)10e8 << std::endl;
	//std::cout << "Kernel 3 : " << (time3)/(float)10e8 << std::endl;
	//std::cout << "Kernel 4 : " << (time4)/(float)10e8 << std::endl;
	cl_ulong total_kernel_time = time1 + time2 + time3 + time4;
	std::cout << "Total Kernel: " << (total_kernel_time)/(float)10e8 << std::endl;
	cl_ulong total_write_time = write_time1 + write_time2 +write_time3 +write_time4;
	//std::cout << "Total write: " << (total_write_time)/(float)10e8 << std::endl;
	cl_ulong total_read_time = read_time1 + read_time2 + read_time3 + read_time4;
	//std::cout << "Total read: " << (total_read_time)/(float)10e8 << std::endl;
	std::cout << "Total transfer: " << (total_read_time + total_write_time)/(float)10e8 << std::endl;
	cl_ulong total_unmap = unmap_time1 + unmap_time2 + unmap_time3 + unmap_time4;
	std::cout << "Total unmap time: " << (total_unmap)/(float)10e8 << std::endl;
	std::cout << "Total time: " << (total_read_time + total_write_time + total_kernel_time + total_unmap)/(float)10e8 << std::endl;
	
//--
	if(timeKernelOnly)
	{
		benchTimerEnd = std::chrono::high_resolution_clock::now();
	}


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
	
	if(!timeKernelOnly)
	{
		benchTimerEnd = std::chrono::high_resolution_clock::now();
	}
	
	return std::chrono::duration_cast<std::chrono::duration<double>>(benchTimerEnd - benchTimerStart);
}

std::chrono::duration<double> cpu_convolution(CImg<double>& baseImage, Filter inFilter)
{
	std::chrono::high_resolution_clock::time_point benchTimerStart;
	std::chrono::high_resolution_clock::time_point benchTimerEnd;
	benchTimerStart = std::chrono::high_resolution_clock::now();
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
	
	benchTimerEnd = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::duration<double>>(benchTimerEnd - benchTimerStart);
}

std::chrono::duration<double> normal_convolution_basic(CImg<double>& baseImage, Filter filter, cl::CommandQueue& queue, cl::Program& program, cl::Context& context, bool timeKernelOnly)
{
	std::chrono::high_resolution_clock::time_point benchTimerStart;
	std::chrono::high_resolution_clock::time_point benchTimerEnd;
	if(!timeKernelOnly)
	{
		benchTimerStart = std::chrono::high_resolution_clock::now();
	}
	
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
	cl::Image2D inputBuffer(context, CL_MEM_READ_ONLY , inFormat, baseImage.width(), baseImage.height());
	
	//Enqueue Image2D buffer
	cl::size_t<3> region;
	region[0] = baseImage.width();
	region[1] = baseImage.height();
	region[2] = 1;
	cl::size_t<3> origin;
	origin[0] = 0;
	origin[1] = 0;
	origin[2] = 0;
	cl::Event write1, read1;
	cl_ulong write_start1, write_end1, read_start1, read_end1, time_start, time_end;;
	queue.enqueueWriteImage(inputBuffer, CL_TRUE, origin, region, 0, 0, imagePixels, NULL, &write1); 

	//Enqueue filter
	cl::Buffer filterBuffer = cl::Buffer(context, CL_MEM_READ_ONLY , filter.get_size()*sizeof(int));
	
	queue.enqueueWriteBuffer(filterBuffer, CL_TRUE, 0, filter.get_size()*sizeof(int), filter.get_filter());
	
	//Output buffer and array to store data read back from kernel
	cl_float4* newImagePixels = new cl_float4[totalPixels]; 
	cl::Buffer outputBuffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, totalPixels*sizeof(cl_float4));
	
	
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

	if(timeKernelOnly)
	{
		benchTimerStart = std::chrono::high_resolution_clock::now();
	}
	
	cl::Event first;
	//Enqueue kernel
	queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, local,NULL, &first);
	//Read back filtered imaged from kernel as a 1D array
	queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, totalPixels*sizeof(cl_float4), newImagePixels, NULL, &read1);
	//Syncs
	queue.finish();
	first.getProfilingInfo(CL_PROFILING_COMMAND_START,&time_start);
	first.getProfilingInfo(CL_PROFILING_COMMAND_END, &time_end);
	cl_ulong kernel_tid = time_end - time_start;
	if(timeKernelOnly)
	{
		benchTimerEnd = std::chrono::high_resolution_clock::now();
	}
	
	
	std::cout << "Tid: " << (time_end - time_start)/(float)10e8 << std::endl;
	
	write1.getProfilingInfo(CL_PROFILING_COMMAND_START,&write_start1);
	write1.getProfilingInfo(CL_PROFILING_COMMAND_END, &write_end1);
	cl_ulong write_time12 = write_end1 - write_start1;
	
	read1.getProfilingInfo(CL_PROFILING_COMMAND_START,&read_start1);
	read1.getProfilingInfo(CL_PROFILING_COMMAND_END, &read_end1);
	cl_ulong read_time12 = read_end1 - read_start1;
	
	std::cout << "Transfer Tid: " << (write_time12 + read_time12)/(float)10e8 << std::endl;
	std::cout << "Total Tid: " << (kernel_tid + write_time12 + read_time12)/(float)10e8 << std::endl;
	
	
	//Recreate baseImage from 1D array to CImg baseImage, assumes new baseImage has same dimensions as the base baseImage
	for(CImg<double>::iterator it = baseImage.begin(); it < baseImage.end()-totalPixels*2; ++it)
	{
		int i = std::distance(baseImage.begin(), it);
		*(it) = newImagePixels[i].s[0];
		*(it+totalPixels) = newImagePixels[i].s[1];
		*(it+totalPixels*2) = newImagePixels[i].s[2];
	}
	if(!timeKernelOnly)
	{
		benchTimerEnd = std::chrono::high_resolution_clock::now();
	}
	
	return std::chrono::duration_cast<std::chrono::duration<double>>(benchTimerEnd - benchTimerStart);
}

std::chrono::duration<double> large_convolution_basic(CImg<double>& baseImage, Filter filter, cl::CommandQueue& queue, cl::Program& program, cl::Context& context, bool timeKernelOnly)
{
	std::chrono::high_resolution_clock::time_point benchTimerStart;
	std::chrono::high_resolution_clock::time_point benchTimerEnd;
	if(!timeKernelOnly)
	{
		benchTimerStart = std::chrono::high_resolution_clock::now();
	}
	
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
	
	cl::Event write1, read1;
	cl_ulong write_start1, write_end1, read_start1, read_end1;
	queue.enqueueWriteImage(inputBuffer1, CL_TRUE, origin, region, 0, 0, imagePixels1, NULL, &write1); 
	
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
	cl::NDRange local(16,16);
	
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

	
	if(timeKernelOnly)
	{
		benchTimerEnd = std::chrono::high_resolution_clock::now();
	}
	cl::Event first;
	cl_ulong time_start, time_end;
	
	//Enqueue kernel
	queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, local,NULL, &first);
	//Read back filtered imaged from kernel as a 1D array
	queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, totalPixels1*sizeof(cl_float4), newImagePixels1, NULL, &read1);
	//Syncs
	queue.finish();
	first.getProfilingInfo(CL_PROFILING_COMMAND_START,&time_start);
	first.getProfilingInfo(CL_PROFILING_COMMAND_END, &time_end);

	cl_ulong time1 = time_end - time_start;
	
	write1.getProfilingInfo(CL_PROFILING_COMMAND_START,&write_start1);
	write1.getProfilingInfo(CL_PROFILING_COMMAND_END, &write_end1);
	cl_ulong write_time1 = write_end1 - write_start1;	
	//std::cout << "read 1 : " << (write_time1)/(float)10e8 << std::endl;
	
	read1.getProfilingInfo(CL_PROFILING_COMMAND_START,&read_start1);
	read1.getProfilingInfo(CL_PROFILING_COMMAND_END, &read_end1);
	cl_ulong read_time1 = read_end1 - read_start1;
	//std::cout << "read 1 : " << (read_time1)/(float)10e8 << std::endl;
	
//---

//---secondHalf kernel computations
	//1D read back storage for secondHalf
	cl_float4* newImagePixels2 = new cl_float4[totalPixels2]; 
	region[0] = topRight.width();
	region[1] = topRight.height();
	cl::Event write2, read2;
	cl_ulong write_start2, write_end2, read_start2, read_end2;
	
	queue.enqueueWriteImage(inputBuffer2, CL_TRUE, origin, region, 0, 0, imagePixels2, NULL, &write2); 
	cl::Buffer outputBuffer2 = cl::Buffer(context, CL_MEM_WRITE_ONLY, totalPixels2*sizeof(cl_float4));
	//std::cout << (baseImage.width()/2) << " != " << topRight.width() << "    &    " << (baseImage.height()/2) << " != " << topRight.height() << std::endl; 	
	//Kernel setup
	kernel.setArg(0, inputBuffer2);
	cl::Event second;
	cl_ulong time_start2, time_end2;
	//Enqueue kernel
	queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, local, NULL, &second);
	//Read back filtered imaged from kernel as a 1D array
	queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, totalPixels2*sizeof(cl_float4), newImagePixels2, NULL, &read2);
	//Syncs
	queue.finish();
	second.getProfilingInfo(CL_PROFILING_COMMAND_START,&time_start2);
	second.getProfilingInfo(CL_PROFILING_COMMAND_END, &time_end2);
	cl_ulong time2 = time_end2 - time_start2;
	
	write2.getProfilingInfo(CL_PROFILING_COMMAND_START,&write_start2);
	write2.getProfilingInfo(CL_PROFILING_COMMAND_END, &write_end2);
	cl_ulong write_time2 = write_end2 - write_start2;
	//std::cout << "write 2 : " << (write_time2)/(float)10e8 << std::endl;
	
	read2.getProfilingInfo(CL_PROFILING_COMMAND_START,&read_start2);
	read2.getProfilingInfo(CL_PROFILING_COMMAND_END, &read_end2);
	cl_ulong read_time2 = read_end2 - read_start2;
	//std::cout << "read 2 : " << (read_time2)/(float)10e8 << std::endl;
//---
	
//---
	cl_float4* newImagePixels3 = new cl_float4[totalPixels3]; 
	region[0] = bottomLeft.width();
	region[1] = bottomLeft.height();
	cl::Event write3, read3;
	cl_ulong write_start3, write_end3, read_start3, read_end3;
	queue.enqueueWriteImage(inputBuffer3, CL_TRUE, origin, region, 0, 0, imagePixels3, NULL, &write3); 
	//Kernel setup
	kernel.setArg(0, inputBuffer3);

	cl::Event third;
	cl_ulong time_start3, time_end3;
	//Enqueue kernel
	queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, local, NULL, &third);
	//Read back filtered imaged from kernel as a 1D array
	queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, totalPixels3*sizeof(cl_float4), newImagePixels3, NULL, &read3 );
	//Syncs
	queue.finish();
	third.getProfilingInfo(CL_PROFILING_COMMAND_START,&time_start3);
	third.getProfilingInfo(CL_PROFILING_COMMAND_END, &time_end3);
	cl_ulong time3 = time_end3 - time_start3;
	
	write3.getProfilingInfo(CL_PROFILING_COMMAND_START,&write_start3);
	write3.getProfilingInfo(CL_PROFILING_COMMAND_END, &write_end3);
	cl_ulong write_time3 = write_end3 - write_start3;
	//std::cout << "write 3 : " << (write_time3)/(float)10e8 << std::endl;
	
	read3.getProfilingInfo(CL_PROFILING_COMMAND_START,&read_start3);
	read3.getProfilingInfo(CL_PROFILING_COMMAND_END, &read_end3);
	cl_ulong read_time3 = read_end3 - read_start3;
	//std::cout << "read 3 : " << (read_time3)/(float)10e8 << std::endl;
//--

//--
	cl_float4* newImagePixels4 = new cl_float4[totalPixels4]; 
	region[0] = bottomRight.width();
	region[1] = bottomRight.height();
	cl::Event write4, read4;
	cl_ulong write_start4, write_end4, read_start4, read_end4;
	queue.enqueueWriteImage(inputBuffer4, CL_TRUE, origin, region, 0, 0, imagePixels4, NULL, &write4); 
	//Kernel setup
	kernel.setArg(0, inputBuffer4);

	cl::Event fourth;
	cl_ulong time_start4, time_end4;
	//Enqueue kernel
	queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, local,NULL, &fourth);
	//Read back filtered imaged from kernel as a 1D array
	queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, totalPixels4*sizeof(cl_float4), newImagePixels4, NULL, &read4);
	//Syncs
	queue.finish();
	
//--
	if(timeKernelOnly)
	{
		benchTimerEnd = std::chrono::high_resolution_clock::now();
	}

	fourth.getProfilingInfo(CL_PROFILING_COMMAND_START,&time_start4);
	fourth.getProfilingInfo(CL_PROFILING_COMMAND_END, &time_end4);
	cl_ulong time4 = time_end4 - time_start4;
	std::cout << "Tid1: " << (time1)/(float)10e8 << std::endl;
	std::cout << "Tid2: " << (time2)/(float)10e8 << std::endl;
	std::cout << "Tid3: " << (time3)/(float)10e8 << std::endl;
	std::cout << "Tid4: " << (time4)/(float)10e8 << std::endl;
	cl_ulong total_kernel_time = time1 + time2 + time3 + time4;
	std::cout << "Total kernel Tid: " << (total_kernel_time)/(float)10e8 << std::endl;
	
	write4.getProfilingInfo(CL_PROFILING_COMMAND_START,&write_start4);
	write4.getProfilingInfo(CL_PROFILING_COMMAND_END, &write_end4);
	cl_ulong write_time4 = write_end4 - write_start4;
	//std::cout << "write 4 : " << (write_time4)/(float)10e8 << std::endl;
	
	read4.getProfilingInfo(CL_PROFILING_COMMAND_START,&read_start4);
	read4.getProfilingInfo(CL_PROFILING_COMMAND_END, &read_end4);
	cl_ulong read_time4 = read_end4 - read_start4;
	cl_ulong write_transfer_time = write_time1 + write_time2 + write_time3 + write_time4;
	cl_ulong read_transfer_time =  read_time1 + read_time2 + read_time3 + read_time4;
	//std::cout << "read 4 : " << (read_time4)/(float)10e8 << std::endl;
	std::cout << "transfer time : " << (read_transfer_time + write_transfer_time)/(float)10e8 << std::endl;
	std::cout << "Total time : " << (read_transfer_time + write_transfer_time + total_kernel_time)/(float)10e8 << std::endl;
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
	
	if(!timeKernelOnly)
	{
		benchTimerEnd = std::chrono::high_resolution_clock::now();
	}
	
	return std::chrono::duration_cast<std::chrono::duration<double>>(benchTimerEnd - benchTimerStart);
}

