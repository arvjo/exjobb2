#include <CL/cl.hpp>
#include <iostream>
#include <vector>
#include <fstream>

enum DeviceType {GPU,CPU,HYBRID,OTHER};

class ComputeDevice
{
private:
	cl_int status;
	std::vector<cl::Platform> platforms;
	std::vector<cl::Device> devices;
	cl::Context context;
	cl::CommandQueue cpu_queue;
	cl::CommandQueue gpu_queue;
	cl::Program program;
	cl::Kernel kernel;
	
public:

	ComputeDevice(DeviceType device_type)
	{
		cl_context_properties cps[3];
		// Get all the platforms
		status = cl::Platform::get(&platforms);
				
		cps[0] = CL_CONTEXT_PLATFORM;
		cps[2] = 0;
		// If we were to need a hybrid solution. 
		std::vector<cl::Device> cpus;
		std::vector<cl::Device> gpus;
		
		switch(device_type)
		{	
			case GPU:
				
				for(int i = 0; i < platforms.size() && devices.size() == 0; ++i)
				{
					// Context specific properties
					cps[1] = (cl_context_properties)(platforms[i])();
					// Set up context 
					context = cl::Context(CL_DEVICE_TYPE_GPU, cps);
					// Get devices 
					devices = context.getInfo<CL_CONTEXT_DEVICES>();
				}
				gpu_queue = cl::CommandQueue(context, devices[0]);
			break;
			case CPU:
				for(int i = 0; i < platforms.size() && devices.size() == 0; ++i)
				{
					// Context specific properties
					cps[1] = (cl_context_properties)(platforms[i])();
					// Set up context 
					context = cl::Context(CL_DEVICE_TYPE_CPU, cps);
					// Get devices 
					devices = context.getInfo<CL_CONTEXT_DEVICES>();
				}
				cpu_queue = cl::CommandQueue(context, devices[0]);
			break;
			case HYBRID:
				for(int i = 0; i < platforms.size(); ++i)
				{
					// Context specific properties
					cps[1] = (cl_context_properties)(platforms[i])();
					// Set up context 
					context = cl::Context(CL_DEVICE_TYPE_ALL, cps);
					// Get devices 
					devices = context.getInfo<CL_CONTEXT_DEVICES>();
					// Check if we have multiple devices 
					if(devices.size() == 2)
					{
						if(devices[0].getInfo<CL_DEVICE_TYPE>() == CL_DEVICE_TYPE_CPU)
						{
							gpu_queue = cl::CommandQueue(context, devices[1]);
							cpu_queue = cl::CommandQueue(context, devices[0]);
						}
						else
						{
							gpu_queue = cl::CommandQueue(context, devices[0]);
							cpu_queue = cl::CommandQueue(context, devices[1]);
						}
						break;
					}
				}
			break;
			case OTHER:
				std::cout << "NOT IMPLEMENTED YET!!!" << std::endl;
			break;
			default:
				std::cout << "NOT A DEVICE_TYPE!!!" << std::endl;
		}
	}	
	
	void setKernelSourceFileAndBuild(const std::string path)
	{
		std::string sourceCode;
		cl::Program::Sources source;
		std::ifstream file(path);
		if(file.is_open())
		{
			sourceCode = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
			source = cl::Program::Sources(1, std::make_pair(sourceCode.c_str(), sourceCode.length()+1));
		}
		else
		{
			std::cerr << "Whoop can not haz reading from file!!!" << std::endl;
			return;
		}
		program = cl::Program(context, source);
		program.build(devices);
	}
	
	void setKernelFunction(const std::string &function)
	{
		kernel = cl::Kernel(program,function.c_str());
	}
	
	cl::Kernel& getKernel()
	{
		return kernel;
	}
	
	cl::Context getContext()
	{
		return context;
	}
	
	cl::CommandQueue& getGPUCommandQueue()
	{
		return gpu_queue;
	}
	
	cl::CommandQueue& getCPUCommandQueue()
	{
		return cpu_queue;
	}
};