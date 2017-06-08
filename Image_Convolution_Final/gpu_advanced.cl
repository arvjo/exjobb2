#pragma OPENCL EXTENTION cl_intel_printf : enable
__kernel void convolution(__read_only image2d_t input, const int imageWidth, const int imageHeight, __global int* inFilter, const int filterWidth, const float factor, __global float4* output)
{
	//The global id's represents pixel coordinates in 2D space  
	int2 pos = {get_global_id(0),get_global_id(1)};	
	__local float4 localImage4[32][32];
	int localX;
	int localY;
	if(pos.x < imageWidth && pos.y < imageHeight)
	{
		const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;

		
			
		localX = get_local_id(0);
		localY = get_local_id(1);
			
		localX*=2;
		localY*=2;
		int col = get_group_id(0)*get_local_size(0);
		int row = get_group_id(1)*get_local_size(1);
			
		int xPos = col + localX;
		int yPos = row + localY;
			 
		yPos -= 8;
		xPos -= 8;
		
		localImage4[localX][localY] = read_imagef(input, smp,(int2)(xPos,yPos));
		localImage4[localX+1][localY] = read_imagef(input, smp,(int2)(xPos+1,yPos));
		localImage4[localX][localY+1] = read_imagef(input, smp,(int2)(xPos,yPos+1));
		localImage4[localX+1][localY+1] = read_imagef(input, smp,(int2)(xPos+1,yPos+1));
		
		localX = get_local_id(0)+8;
		localY = get_local_id(1)+8;
	}
	
	barrier(CLK_LOCAL_MEM_FENCE);
		
	if(pos.x < imageWidth && pos.y < imageHeight)
	{
		int filter[3][3];
		for(int x = 0, counter = 0; x < filterWidth; ++x)
		{
			for(int y = 0; y < filterWidth; ++y, ++counter)
			{
				filter[x][y] = inFilter[counter];
			}
		}
		
		float red = 0.0;
		float green = 0.0;
		float blue = 0.0;
				
		for(int filterX = 0; filterX < filterWidth; ++filterX)
		{
		
			for(int filterY = 0; filterY < filterWidth; ++filterY)
			{
				
				int imageX = (localX - filterWidth / 2 + filterX + 32) % 32;
				int imageY = (localY - filterWidth / 2 + filterY + 32) % 32;
					
				if(imageX >= 0 && imageX < 32 && imageY >= 0 && imageY < 32)
				{
					float4 currentPixel = localImage4[imageX][imageY];
					red += currentPixel.x * filter[filterX][filterY]; 
					green += currentPixel.y * filter[filterX][filterY];
					blue += currentPixel.z * filter[filterX][filterY];	
				}
						
			}	
		}
			
		//multiply the RGB values by the filter factor in order to maintain brightness consistency.
		red = red * factor + 1.00;
		green = green * factor + 1.00;
		blue = blue * factor + 1.00;
			
		//clamp RGB values to the 0-255 range
		if(red < 0.00)
			red = 0.00;
		if(red > 255.00)
			red = 255.00;
				
		if(green < 0.00)
			green = 0.00;
		if(green > 255.00)
			green = 255.00;
			
		if(blue < 0.00)
			blue = 0.00;
		if(blue > 255.00)
			blue = 255.00;
			
		//Assign a pixel with the new RGB values to the output array.
		float4 modifiedPixel;
		modifiedPixel.x = red;
		modifiedPixel.y = green;
		modifiedPixel.z = blue;
			
		output[imageWidth * pos.y + pos.x] = modifiedPixel;	
	}
}