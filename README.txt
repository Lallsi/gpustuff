Real-time GPU Ray Tracing 
	Implemented Real-time GPU Ray Tracer with OpenGL Compute Shader. 
	The tracing itself is fairly simple - 
	I just put together some sort of tracer to demonstrate the idea.

	Camera can be moved with WASD, QE and MLEFT + mouse movement.
	When the camera is still, the compute shader blends new frames with the old ones 
	so the longer you wait, the more samples get calculated and the image becomes less noisy.
	I added an option to change the amount of samples per frame with number keys which makes 
	the image less noisy when moving.

	The tracer uses area light for soft shadows
	and cosine weighted hemisphere sampling for diffuse reflection. 
	For each pixel we bounce a ray 3 times randomly(or perfect reflection for mirror ball) 
	in the scene. 

	Debug mode runs in 1280x720 window and Release in fullscreen.

	Requires OpenGL 4.5, glew and glm.
	Glew and glm are included and shouldn't require any installing.
	Built and tested on Windows 10/VS2015 so I can't be sure about other versions.