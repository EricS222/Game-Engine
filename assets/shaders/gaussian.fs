#version 330 core
out vec4 FragColor;

in vec2 uvCoord;

uniform sampler2D brightTexture;
uniform float kernelSize;

void main()
{
 
  vec3 hdrColor = texture(screenTexture, uvCoord).rgb;

  vec3 bloomColor = vec3(0.0);

  // Perform linearly separable gaussian blur on brightTexture
  vec2 dimensions = textureSize(brightTexture, 0);
  float xOffset = 1.0 / dimensions.x;
  float yOffset = 1.0 / dimensions.y;
  for(int i=1; i<kernelSize; i++) {

    bloomColor += texture(brightTexture, uvCoord + vec2(0, i * yOffset), 0).rgb;
    bloomColor += texture(brightTexture, uvCoord + vec2(0, -i * yOffset), 0).rgb;

  }

  for(int j=1; j<kernelSize; j++) {

    bloomColor += texture(brightTexture, uvCoord + vec2(j * xOffset, 0), 0).rgb;
    bloomColor += texture(brightTexture, uvCoord + vec2(-j * xOffset, 0), 0).rgb;
  
  }

  bloomColor /= kernelSize * kernelSize;
  FragColor = vec4(bloomColor, 1.0);
};