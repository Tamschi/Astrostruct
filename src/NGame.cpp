#include "NEngine.hpp"

NGame::NGame()
{
	Width = 0;
	Height = 0;
	Run = true;
	Valid = false;
	WindowChanged = true;
}

NGame::~NGame()
{
	if (Valid)
	{
		delete Input;
		delete Scene;
		delete Render;
		delete SoundSystem;
		delete TextSystem;
		delete Config;
		delete Lua;
		glfwTerminate();
	}
}

bool NGame::Init(int i_Width, int i_Height, std::string Title)
{
	//Initialize everything we can
	Width = i_Width;
	Height = i_Height;
	if (!glfwInit())
	{
		SetColor(Red);
		std::cout << "ENGINE ERROR: ";
		ClearColor();
		std::cout << "GLFW failed to initialize!\n";
		return Fail;
	}
	Lua = new NLua();
	Config = new NConfig("data/config/init.lua");
	//Now lets load some data from our config interface
	NewWidth = Config->GetFloat("Width");
	NewHeight = Config->GetFloat("Height");
	if (NewWidth != 0)
	{
		Width = NewWidth;
	}
	if (NewHeight != 0)
	{
		Height = NewHeight;
	}
	int Check = 0;
	if (Config->GetBool("Fullscreen"))
	{
		Check = glfwOpenWindow(Width,Height,0,0,0,0,0,0,GLFW_FULLSCREEN);
	} else {
		Check = glfwOpenWindow(Width,Height,0,0,0,0,0,0,GLFW_WINDOW);
	}
	if (!Check)
	{
		SetColor(Red);
		std::cout << "ENGINE ERROR: ";
		ClearColor();
		std::cout << "GLFW failed to open a window!\n";
		glfwTerminate();
		return Fail;
	}
	glfwSetWindowTitle(Title.c_str());
	if (glewInit() != GLEW_OK)
	{
		SetColor(Red);
		std::cout << "ENGINE ERROR: ";
		ClearColor();
		std::cout << "GLEW failed to initialize!\n";
		glfwTerminate();
		return Fail;
	}
	glfwSetWindowSizeCallback(&ResizeWindow);
	Input = new NInput();
	Scene = new NScene();
	Render = new NRender();
	Render->LoadTextures();
	SoundSystem = new NSoundSystem();
	SoundSystem->LoadSounds();
	TextSystem = new NTextSystem();
	TextSystem->LoadFaces();
	Valid = true;
	return Success;
}

NInput* NGame::GetInput()
{
	return Input;
}

NScene* NGame::GetScene()
{
	return Scene;
}

NRender* NGame::GetRender()
{
	return Render;
}

bool NGame::Running()
{
	return Run;
}

void NGame::Close()
{
	Run = false;
}

int NGame::GetWindowWidth()
{
	return Width;
}

int NGame::GetWindowHeight()
{
	return Height;
}

glm::vec2 NGame::GetWindowSize()
{
	return glm::vec2(Width,Height);
}

void NGame::Poll()
{
	//If the window closes, end the game!
	if (!glfwGetWindowParam(GLFW_OPENED))
	{
		SetColor(Blue);
		std::cout << "ENGINE INFO: ";
		ClearColor();
		std::cout << "Game window was closed, exiting...\n" << std::flush;
		Close();
		return;
	}
	//If the window has changed, reset our opengl view matrix and context size.
	if (Width != NewWidth || Height != NewHeight)
	{
		Width = NewWidth;
		Height = NewHeight;
		WindowChanged = true;
		Render->SetSize(Width,Height);
	} else {
		WindowChanged = false;
	}
}

void ResizeWindow(int W, int H)
{
	GetGame()->NewWidth = W;
	GetGame()->NewHeight = H;
}

void NGame::SetWindowChanged(bool Changed)
{
	WindowChanged = Changed;
}

bool NGame::GetWindowChanged()
{
	return WindowChanged;
}

NLua* NGame::GetLua()
{
	return Lua;
}

NConfig* NGame::GetConfig()
{
	return Config;
}

void NGame::SetWindowSize(int W, int H)
{
	glfwSetWindowSize(W,H);
	NewWidth = W;
	NewHeight = H;
}

void NGame::CleanUp()
{
	delete this;
}

NSoundSystem* NGame::GetSoundSystem()
{
    return SoundSystem;
}

NTextSystem* NGame::GetTextSystem()
{
	return TextSystem;
}
