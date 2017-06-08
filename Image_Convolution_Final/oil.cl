#pragma OPENCL EXTENTION cl_intel_printf : enable
__kernel void test(__read_only image2d_t input, const int imageWidth, const int imageHeight, __global int* inFilter, const int filterSize, const float factor, __global float4* output)
{
	const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
	
	//2d global id's represents pixel coordinates  
	int2 pos = {get_global_id(0),get_global_id(1)};	
	
	if(pos.x < imageWidth && pos.y < imageHeight)
	{
		float intensityLevel = 15;
		int intensityCount[257] = {0};
		int averageR[257] = {0};
		int averageB[257] = {0};
		int averageG[257] = {0};

		for(int filterX = 0; filterX < 7; ++filterX)
		{
			for(int filterY = 0; filterY < 7; ++filterY)
			{
				
				int imageX = (pos.x - 3 / 2 + filterX + imageWidth) % imageWidth;
				int imageY = (pos.y - 3 / 2 + filterY + imageHeight) % imageHeight;
				
				float4 currentPixel = read_imagef(input, smp, (int2)(imageX, imageY));
		
				int currentIntensity = (int)((float)((currentPixel.x+currentPixel.y+currentPixel.z)/3)*intensityLevel)/255.00;
				
				intensityCount[currentIntensity] +=1;
				averageR[currentIntensity] += currentPixel.x;
				averageG[currentIntensity] += currentPixel.y;
				averageB[currentIntensity] += currentPixel.z;
			}
		}
		
		int currentMax = 0;
		int maxIndex = 0;
		for(int i = 0; i < 256; ++i)
		{
			if(intensityCount[i] > currentMax)
			{
				currentMax = intensityCount[i];
				maxIndex = i;
			}
		}
		
		float4 FinalPixel = read_imagef(input, smp, pos);
		FinalPixel.x = averageR[maxIndex] / currentMax;
		FinalPixel.y = averageG[maxIndex] / currentMax;;
		FinalPixel.z = averageB[maxIndex] / currentMax;;
		
		output[imageWidth * pos.y + pos.x] = FinalPixel;
	}
}