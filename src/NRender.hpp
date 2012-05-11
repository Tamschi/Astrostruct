/**
* @file NRender.hpp
* @brief Handles most rendering calls, and exposes them to the rest of the engine.
* @author Dalton Nell
* @version 1.0
* @date 2012-05-02
*/
#ifndef NAELSTROF_RENDER
#define NAELSTROF_RENDER

class NCachedTexture
{
public:
	void SetFilter(GLuint);
	NCachedTexture(std::string i_Name, GLuint i_ID);
	~NCachedTexture();
	GLuint ID;
	std::string Name;
};
/**
* @brief Handles most rendering calls, and exposes them to the rest of the engine.
*/
class NRender
{
public:
	NRender();
	~NRender();
	/**
	* @brief Statically loads shaders into memory.
	*
	* @return False on failure, true on success.
	*/
	bool LoadShaders();
	/**
	* @brief Grabs the desired shader from memory.
	*
	* @param Name The name of the desired shader.
	*
	* @return A pointer to the shader.
	*/
	NShader* GetShader(std::string Name);
	/**
	* @brief Clears the color buffer, draws all objects in the current scene, then swaps the buffer.
	*/
	void Draw();
	/**
	* @brief Sets the camera to use to generate a view matrix from.
	*
	* @param Camera The desired camera to use.
	*/
	void SetCamera(NCamera* Camera);
	/**
	* @brief Returns the current camera in use, or NULL if none.
	*
	* @return A pointer to the current camera in use, or NULL if none.
	*/
	NCamera* GetCamera();
	/**
	* @brief Sets all active texture's texture filters. Useful for graphical settings.
	*
	* @param Param GL_LINEAR or GL_NEAREST
	*/
	void SetTextureFilter(GLuint Param);
	/**
	* @brief Returns what texture filter we're currently using.
	*
	* @return Either GL_LINEAR or GL_NEAREST.
	*/
	GLuint GetTextureFilter();
	/**
	* @brief Enables or disables VSync.
	*
	* @param i_VSync true for enable, false for disable.
	*/
	void SetVSync(bool i_VSync);
	/**
	* @brief Returns if VSync is enabled or not.
	*
	* @return true if enabled, false if it's disabled.
	*/
	bool GetVSync();
	/**
	* @brief Returns the amount of time in seconds it took to render the last frame.
	*
	* @return The amount of time in seconds it took to render the last frame.
	*/
	double GetFrameTime();
	/**
	* @brief Attempts to grab a texture already in memory and return an ID for it, if it doesn't exist it uses SOIL to load one into memory and returns that ID.
	*
	* @param Name The file path to the image/texture.
	*
	* @return A texture ID for the already loaded image.
	*/
	GLuint GetCachedTexture(std::string Name);
	/**
	* @brief Returns a Texture object from memory.
	*
	* @param Name The name of the texture object.
	*
	* @return A newly allocated unique texture object. It allocates a new texture to keep unique time locations in each texture for animations.
	*/
	NTexture* GetTexture(std::string Name);
	/**
	* @brief Adds a texture object into memory.
	*
	* @param Texture The desired texture.
	*/
	void AddTexture(NTexture* Texture);
	/**
	* @brief Adds a texture ID into memory, this is so global texture filters can be applied to dynamically created textures.
	*
	* @param ID The OpenGL texture ID.
	*/
	void AddCachedTexture(GLuint ID);
private:
	double FrameTime;
	bool VSync;
	GLuint TextureFilter;
	float MaxFPS;
	double LastTime;
	unsigned int FPS;
	std::vector<NShader*> Shaders;
	std::vector<NCachedTexture*> CachedTextures;
	std::vector<NTexture*> Textures;
	NCamera* Camera;
};

#endif
