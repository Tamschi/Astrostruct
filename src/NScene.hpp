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
	NText* AddText(std::string Font, std::wstring Data);
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
	/**
	 * @brief Adds a map to the scene.
	 *
	 * @param TileSet The name of the texture we want to use for the map.
	 *
	 * @return A pointer to the map created.
	 */
	NMap* AddMap(std::string TileSet);
	/**
	 * @brief Adds a camera to the scene.
	 *
	 * @return A pointer to the camera.
	 */
	NCamera* AddCamera();
	/**
	 * @brief Adds a player to the scene.
	 *
	 * @return A pointer to the player created.
	 */
	NPlayer* AddPlayer(std::wstring Name);
	/**
	 * @brief Adds a light to the scene.
	 *
	 * @param Texture The desired texture of the light.
	 *
	 * @return A pointer to the light created.
	 */
	NLight* AddLight(std::string Texture);
	NPlayer* AddPlayer(std::string Name);
	NCheckbox* AddCheckbox(std::string Texture);
	std::vector<NNode*>* GetWorld();
	NNode* GetNodeByID(unsigned int ID);
	void RemoveByID(unsigned int ID);
	void Remove(NNode* Node);
	void ToggleFullBright();
private:
	double LastTick;
	std::vector<NNode*> GUI;
	std::vector<NNode*> World;
	std::vector<NNode*> Lights;
	std::vector<NNode*> Garbage;
	bool ShuttingDown;
	bool FullBright;
};

#endif
