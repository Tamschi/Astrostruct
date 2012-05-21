/**
* @file NScene.hpp
* @brief Handles active game objects.
* @author Dalton Nell
* @version 1.0
* @date 2012-05-02
*/
#ifndef NAELSTROF_SCENE
#define NAELSTROF_SCENE

/**
* @brief Class that holds arbitrary objects and handles Tick-ing and drawing them.
*/
class NScene
{
public:
	NScene();
	~NScene();
	/**
	* @brief Updates all objects in the scene.
	*/
	void Tick();
	/**
	* @brief Draws all objects in the scene.
	*
	* @param View The camera view matrix.
	*/
	void Draw(NCamera* View);
	/**
	* @brief Adds an object to the scene.
	*
	* @param Node The desired object to be added.
	*/
	void AddNode(NNode* Node);
	/**
	* @brief Adds a text object to the scene.
	*
	* @param Font The desired font of the object.
	* @param Data The desired text of the object.
	*
	* @return A pointer to the newly created object.
	*/
	NText* AddText(std::string Font, std::string Data);
	/**
	* @brief Swaps the depth of the specified object.
	*
	* @param Node The node we want to swap depths.
	* @param Depth The desired depth of the node, if a node exists at the desired depth they will swap depths.
	*/
	void SwapDepth(NNode* Node, unsigned int Depth);
	/**
	* @brief Returns the highest depth available.
	*
	* @return The highest depth to render to.
	*/
	unsigned int GetTopDepth();
	/**
	* @brief Returns a pointer to the text system, only useful for lua to be able to load fonts into it.
	*
	* @return A pointer to the font system.
	*/
	NTextSystem* GetTextSystem();
	/**
	 * @brief Adds a window to the scene.
	 *
	 * @return A pointer to the new window.
	 */
	NWindow* AddWindow();
	/**
	 * @brief Adds a button to the scene.
	 *
	 * @return A pointer to the new button.
	 */
	NButton* AddButton();
	/**
	 * @brief Adds a sound to the scene.
	 *
	 * @param Name The name of the sound we want to load.
	 *
	 * @return A pointer to the new sound.
	 */
	NSound* AddSound(std::string Name);
	NMap* AddMap(std::string TileSet);
private:
	bool LoadedText;
	NTextSystem* TextSystem;
	double LastTick;
	std::vector<NNode*> Nodes;
};

#endif
