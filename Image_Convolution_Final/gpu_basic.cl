#pragma OPENCL EXTENTION cl_intel_printf : enable
__kernel void convolution(__read_only image2d_t input, const int imageWidth, const int imageHeight, __global int* inFilter, const int filterWidth, const float factor, __global float4* output)
{
	/****
		A sampler is used to set the formatting for how an Image2D memory object is read,
		disabling normalized coords allows us to access the pixels of the image by absolute
		x and y coordinates e.g. 852,436. Address clamping means that if we select a pixel
		outside the boundaries of the image the closest pixel inside the image will be selected
		instead.
	****/
	const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;
	
	//The global id's represents pixel coordinates in 2D space  
	int2 pos = {get_global_id(0),get_global_id(1)};	
	
	/****
		Because the number of enqueued work-items has to be a power of 2
		there will almost always be superfluous work-items. We therefore check
		that the current work-item "pos" actually represents a pixel in the image
		if it does not we immediately exit the kernel.
	****/
	if(pos.x < imageWidth && pos.y < imageHeight)
	{
		//Take 1D filter representation and convert to 2D for easier management
		
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
		
		/****
			With the "pos" pixel as the center we traverse the surrounding pixels 
			within the filter radius and calculate new RGB values for the "pos" pixel. 
		****/
		for(int filterX = 0; filterX < filterWidth; ++filterX)
		{
			for(int filterY = 0; filterY < filterWidth; ++filterY)
			{
				int imageX = (pos.x - filterWidth / 2 + filterX + imageWidth) % imageWidth;
				int imageY = (pos.y - filterWidth / 2 + filterY + imageHeight) % imageHeight;
				float4 currentPixel = read_imagef(input,smp, (int2)(imageX, imageY));
				
				
				red += currentPixel.x * filter[filterX][filterY]; 
				green += currentPixel.y * filter[filterX][filterY];
				blue += currentPixel.z * filter[filterX][filterY];
			}
		}
		
		//multiply the RGB values by the filter factor in order to maintain brightness consistency.
		red = red * factor + 0.00;
		green = green * factor + 0.00;
		blue = blue * factor + 0.00;
		
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